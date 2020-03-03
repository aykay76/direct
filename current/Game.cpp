// Game.cpp: implementation of the Game class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "direct.h"
#include "Game.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct ALIENWAVEINFO {
	DWORD dwTime;
} awi[] = {
	{ 1000 },
	{ 5000 },
	{ 6000 },
	{ 9000 },
	{ 10000 },
	{ 11000 },
	{ 12000 },
	{ 18000 },
};
int m_nWaves = sizeof(awi) / sizeof(ALIENWAVEINFO);
int m_nWave;

Game::Game()
{

}

Game::~Game()
{

}

BOOL Game::Initialise(HWND hWnd)
{
	HRESULT hr;

	m_hWnd = hWnd;

	hr = InitialiseDirectDraw();
	hr = InitialiseDirectInput();	
	hr = InitialiseDirectSound();

	m_Starfield.Initialise();

	m_dwStart = GetTickCount();

	return TRUE;
}

void Game::Frame()
{
	try {
		DWORD dwFrameStart = GetTickCount();
		Input();
		Process();
		Output();
		m_dwLastFrameTime = GetTickCount() - dwFrameStart;
//		frame++;
		if (GetTickCount() - m_dwStart >= 1000) {
//			g_nFPS = frame;
			m_dwStart = GetTickCount();
//			frame = 0;
		}
	} catch (char *str) {
		OutputDebugString("Exception caught: ");
		OutputDebugString(str);
	}
}

void Game::Cleanup()
{
	if (m_lpKeyboard) {
		m_lpKeyboard->Unacquire();
		m_lpKeyboard->Release();
	}

	if (m_lpMouse) {
		m_lpMouse->Unacquire();
		m_lpMouse->Release();
	}

	if (m_lpDDSBack) {
		m_lpDDSBack->Release();
	}
	if (m_lpDDSFront) {
		m_lpDDSBack->Release();
	}
	if (m_lpDD) {
		m_lpDD->Release();
		m_lpDD = NULL;
	}
}

HRESULT Game::InitialiseDirectDraw()
{
	HRESULT hr;
	
	hr = DirectDrawCreateEx(NULL, (void **)&m_lpDD, IID_IDirectDraw7, NULL);
	if (FAILED(hr)) return -1;

	hr = m_lpDD->SetCooperativeLevel(m_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	if (hr == DD_OK) {
	} else {
	}

	hr = m_lpDD->SetDisplayMode(800, 600, 32, 0, 0);
	if (hr == DD_OK) {
	} else {
	}

	DDSURFACEDESC2 ddsd;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;

	hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSFront, NULL);
	if (hr == DD_OK) {
	} else {
		char szMessage[256];
		sprintf(szMessage, "Failed to create primary surface: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER; 
	hr = m_lpDDSFront->GetAttachedSurface(&ddsd.ddsCaps, &m_lpDDSBack);
	if (hr == DD_OK) {
	} else {
		MessageBox(m_hWnd, "Failed to get back buffer", "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	// create a surface for our rocket sprite
	// Create an offscreen bitmap. 
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; 
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
	ddsd.dwWidth = 23; 
	ddsd.dwHeight = 9;
	hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSRocket, NULL); 
	if (hr != DD_OK) { 
		char szMessage[256];
		sprintf(szMessage, "Failed to create surface for rocket: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1; 
	} 
	// Load the bitmap resource. 
	HBITMAP hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_ROCKET), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hbm == NULL) {
		char szMessage[256];
		sprintf(szMessage, "Failed to load rocket bitmap: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return FALSE; 
	}

	DDCOLORKEY ddck;
	ddck.dwColorSpaceHighValue = RGB(0, 0, 0);
	ddck.dwColorSpaceLowValue = RGB(0, 0, 0);
	m_lpDDSRocket->SetColorKey(DDCKEY_SRCBLT, &ddck);

	if (FAILED(hr = DDUtil_CopyBitmap(m_lpDDSRocket, hbm, 0, 0, 23, 9))) {
		char szMessage[256];
		sprintf(szMessage, "Failed to copy rocket bitmap: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return FALSE; 
	}
	DeleteObject(hbm);

	/////////////////////////////////////////////////

	ddsd.dwWidth = 32;
	ddsd.dwHeight = 32;
	hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSShip, NULL);
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SHIP), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	m_lpDDSShip->SetColorKey(DDCKEY_SRCBLT, &ddck);

	if (FAILED(hr = DDUtil_CopyBitmap(m_lpDDSShip, hbm, 0, 0, 32, 32))) {
	}
	DeleteObject(hbm);

	//////////////////////////////////////////////////

	ddsd.dwWidth = 11;
	ddsd.dwHeight = 11;
	hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSSight, NULL);
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SIGHT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	m_lpDDSSight->SetColorKey(DDCKEY_SRCBLT, &ddck);
	if (FAILED(hr = DDUtil_CopyBitmap(m_lpDDSSight, hbm, 0, 0, 11, 11))) {
	}
	DeleteObject(hbm);

	///////////////////////////////////////////////////

	ddsd.dwWidth = 5;
	ddsd.dwHeight = 5;
	hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSBullet, NULL);
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BULLET), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	m_lpDDSBullet->SetColorKey(DDCKEY_SRCBLT, &ddck);
	if (FAILED(hr = DDUtil_CopyBitmap(m_lpDDSBullet, hbm, 0, 0, 5, 5))) {
	}
	DeleteObject(hbm);

	/////////////////////////////////////////////////////

	ddsd.dwWidth = 20;
	ddsd.dwHeight = 20;
	hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSAlien, NULL);
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_ALIEN), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	m_lpDDSAlien->SetColorKey(DDCKEY_SRCBLT, &ddck);
	if (FAILED(hr = DDUtil_CopyBitmap(m_lpDDSAlien, hbm, 0, 0, 20, 20))) {
		char szMessage[256];
		sprintf(szMessage, "Failed to copy alien bitmap: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
	}
	DeleteObject(hbm);

	return hr;
}

HRESULT Game::InitialiseDirectInput()
{
	HRESULT hr;
	IDirectInput8 *lpDI;

	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&lpDI, NULL);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to create DirectInput object: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = lpDI->CreateDevice(GUID_SysKeyboard, &m_lpKeyboard, NULL);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to create keyboard device object: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = m_lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set keyboard data format: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = m_lpKeyboard->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set keyboard behaviour: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	if (m_lpKeyboard) m_lpKeyboard->Acquire();

	// now the mouse
	hr = lpDI->CreateDevice(GUID_SysMouse, &m_lpMouse, NULL);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to create mouse device object: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = m_lpMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set mouse data format: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = m_lpMouse->SetCooperativeLevel(m_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set keyboard behaviour: (%08x)", hr);
		MessageBox(m_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	m_hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hMouseEvent == NULL) {
		return FALSE;
	}
 
	hr = m_lpMouse->SetEventNotification(m_hMouseEvent);
	if (FAILED(hr)) {
		return FALSE;
	}

	#define SAMPLE_BUFFER_SIZE  16
 
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = SAMPLE_BUFFER_SIZE;

	hr = m_lpMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) {
		return FALSE;
	}
 
	if (m_lpMouse) m_lpMouse->Acquire();

	lpDI->Release();

	return DI_OK;
}

HRESULT Game::InitialiseDirectSound()
{
	HRESULT hr;

    if (FAILED(hr = DirectSoundCreate8(NULL, &m_lpDS, NULL)))
        return hr;

    // Set DirectSound coop level 
    if (FAILED(hr = m_lpDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY)))
        return hr;
    
    // Set primary buffer format
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if (m_lpDS == NULL)
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if (FAILED(hr = m_lpDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL)))
        return hr;

    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = (WORD) 2;
    wfx.nSamplesPerSec  = 22050;
    wfx.wBitsPerSample  = (WORD) 16;
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if (FAILED(hr = pDSBPrimary->SetFormat(&wfx)))
        return hr;

    pDSBPrimary->Release();

    return S_OK;
}

HRESULT Game::Input()
{
	char buffer[256];
	HRESULT hr;

	m_bKeys = 0;

	if (m_lpKeyboard) {
		m_lpKeyboard->GetDeviceState(sizeof(buffer), (LPVOID)&buffer);
		if (KEYDOWN(buffer, DIK_UP)) {
			m_bKeys |= 8;
		}
		if (KEYDOWN(buffer, DIK_DOWN)) {
			m_bKeys |= 4;
		}
		if (KEYDOWN(buffer, DIK_LEFT)) {
			m_bKeys |= 2;
		}
		if (KEYDOWN(buffer, DIK_RIGHT)) {
			m_bKeys |= 1;
		}
		if (KEYDOWN(buffer, DIK_LCONTROL)) {
			if (m_fCtrlPress) {
				if (GetTickCount() - m_dwLastFire >= m_dwFireRate) {
					m_fFire = true;
					m_dwLastFire = GetTickCount();
				}
			} else {
				m_fFire = true;
				m_dwLastFire = GetTickCount();
			}
			m_fCtrlPress = true;
		} else {
			m_fCtrlPress = false;
		}
	}

	BOOL bDone = FALSE;
	m_nXOff = 0;
	m_nYOff = 0;
	// Check the mouse...
	if (WaitForSingleObject(m_hMouseEvent, 0) == WAIT_OBJECT_0) {
		while (!bDone) {
			DIDEVICEOBJECTDATA od;
			DWORD dwElements = 1;   // number of items to be retrieved

 			hr = m_lpMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
			if (hr == DIERR_INPUTLOST) {
				m_lpMouse->Acquire();
				break;
			}

			/* Unable to read data or no data available */
			if (FAILED(hr) || dwElements == 0) {
				break;
			}

			/* Look at the element to see what occurred */
			switch (od.dwOfs) {
				// Mouse horizontal motion
				case DIMOFS_X: 
					m_nXOff = od.dwData;
					break;
				// Mouse vertical motion
				case DIMOFS_Y: 
					m_nYOff = od.dwData;
					break; 
				case DIMOFS_Z:
					break;
				// DIMOFS_BUTTON0: Left button pressed or released 
				case DIMOFS_BUTTON0:
					if (od.dwData & 0x80) {
						if (m_fLeftClick == false) {
							m_fFire = true;
							m_dwLastFire = GetTickCount();
						}
						m_fLeftClick = true;
					} else {
						m_fLeftClick = false;
					}
					break;
				// DIMOFS_BUTTON1: Right button pressed or released 
				case DIMOFS_BUTTON1:
					if (od.dwData & 0x80) {
						if (m_fRightClick == false) {
							m_fFire2 = true;
						}
						m_fRightClick = true;
					} else {
						m_fRightClick = false;
					}
					break;
				case DIMOFS_BUTTON2:
					if (od.dwData & 0x80) {
					} else {
					}
					break;
				case DIMOFS_BUTTON3:
					if (od.dwData & 0x80) {
					} else {
					}
					break;
				case DIMOFS_BUTTON4:
					if (od.dwData & 0x80) {
					} else {
					}
					break;
				case DIMOFS_BUTTON5:
					if (od.dwData & 0x80) {
					} else {
					}
					break;
				case DIMOFS_BUTTON6:
					if (od.dwData & 0x80) {
					} else {
					}
					break;
				case DIMOFS_BUTTON7:
					if (od.dwData & 0x80) {
					} else {
					}
					break;
			}
		}
	} else {
		// nothing actually happened to change the state of the mouse but it could well be that
		// a button is being held down.

		// naturally if the mouse button was clicked at some point the flag will be set, and will
		// continue to be set until the button is released.
		if (m_fLeftClick) {
			// here's how to do rapid fire...
			if (GetTickCount() - m_dwLastFire >= m_dwFireRate) {
				m_fFire = true;
				m_dwLastFire = GetTickCount();
			}
		}
	}

	return hr;
}

HRESULT Game::Process()
{
	// now calculate the angle of the aim, and readjust the coordinates so that the crosshair stays
	// in a circle around the ship
	m_Ship.UpdateAim(m_nXOff, m_nYOff);
	m_Ship.UpdatePosition(m_bKeys, m_dwLastFrameTime);

	m_Starfield.UpdatePosition(m_dwLastFrameTime);

	// update the positions of all the bullets
	{
		std::list<Bullet *>::iterator it = m_bullets.begin();
		std::list<Bullet *>::iterator end = m_bullets.end();
		while (it != end) {
			if ((*it)->UpdatePosition(m_dwLastFrameTime) == false) {
				std::list<Bullet *>::iterator del = it;

				++it;
				delete (*del);
				m_bullets.erase(del);
			} else {
				if ((*it)->CheckBaddies() == true) {
					// alien was destroyed, delete the bullet
					std::list<Bullet *>::iterator del = it;
					++it;
					delete (*del);

					m_bullets.erase(del);
				} else {
					++it;
				}
			}
		}
	}
	{
		std::list<Rocket *>::iterator it = m_rockets.begin();
		std::list<Rocket *>::iterator end = m_rockets.end();
		while (it != end) {
			if ((*it)->UpdatePosition(m_dwLastFrameTime) == false) {
				std::list<Rocket *>::iterator del = it;
				++it;
				delete (*del);
				m_rockets.erase(del);
			} else {
				if ((*it)->CheckBaddies() == true) {
					// alien was destroyed, delete the rocket
					std::list<Rocket *>::iterator del = it;
					++it;
					delete (*del);

					m_rockets.erase(del);
				} else {
					++it;
				}
			}
		}
	}
	{
		std::list<Wave *>::iterator it = m_waves.begin();
		std::list<Wave *>::iterator end = m_waves.end();
		while (it != end) {
			(*it)->UpdatePosition(m_dwLastFrameTime);
			++it;
		}
	}

	if (m_fFire) {
		// create new bullet object
		Bullet *pBullet = new Bullet(m_Ship.m_dX, m_Ship.m_dY, m_Ship.m_dAim, 800.0);
		m_bullets.push_back(pBullet);
		m_fFire = false;
		PlaySound("bfire.wav", NULL, SND_ASYNC);
	}

	if (m_fFire2) {
		Rocket *pRocket = new Rocket(m_Ship.m_dX, m_Ship.m_dY, /*m_Ship.m_dAim*/0.0, -100.0);
		m_rockets.push_back(pRocket);
		m_fFire2 = false;
	}

	/////////////////////////////////////////////////////

	if (m_nWave < m_nWaves) {
		if (GetTickCount() - m_dwGameStart >= awi[m_nWave].dwTime) {
			Wave *wave = new Wave();
			wave->Create(5);
			m_waves.push_back(wave);
			m_nWave++;
		}
	}

	return DD_OK;
}

HRESULT Game::Output()
{
	HRESULT hr;

	// Erase the background
	DDBLTFX ddbltfx;
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize      = sizeof(ddbltfx);
	ddbltfx.dwFillColor = m_dwFillColour;
	m_lpDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);

	// draw some primitive shapes using GDI calls (this will change to sprites!)
	m_Starfield.Draw();
	{
		std::list<Bullet *>::iterator it = m_bullets.begin();
		std::list<Bullet *>::iterator end = m_bullets.end();
		while (it != end) {
			(*it)->Draw();
			++it;
		}
	}
	{
		std::list<Rocket *>::iterator it = m_rockets.begin();
		std::list<Rocket *>::iterator end = m_rockets.end();
		while (it != end) {
			(*it)->Draw();
			++it;
		}
	}
	{
		std::list<Wave *>::iterator it = m_waves.begin();
		std::list<Wave *>::iterator end = m_waves.end();
		while (it != end) {
			if ((*it)->Draw() == false) {
				std::list<Wave *>::iterator del = it;
				++it;
				delete (*del);
				m_waves.erase(del);
			} else {
				++it;
			}
		}
	}
	{
		std::list<Static *>::iterator it = m_statics.begin();
		std::list<Static *>::iterator end = m_statics.end();
		while (it != end) {
			if ((*it)->Update(m_dwLastFrameTime) == false) {
				std::list<Static *>::iterator del = it;
				++it;
				delete (*del);
				m_statics.erase(del);
			} else {
				++it;
			}
		}
	}
	m_Ship.Draw();

	HDC hdc;
	m_lpDDSBack->GetDC(&hdc);
	char sz[128];
	sprintf(sz, "%d fps", m_nFPS);
	TextOut(hdc, 0, 0, sz, strlen(sz));
	m_lpDDSBack->ReleaseDC(hdc);

	hr = m_lpDDSFront->Flip(NULL, DDFLIP_WAIT);
	if (hr != DD_OK) {
		if (hr == DDERR_SURFACELOST) {
			if (m_lpDD) {
				m_lpDD->RestoreAllSurfaces();
			}
		}
	}

	return hr;
}