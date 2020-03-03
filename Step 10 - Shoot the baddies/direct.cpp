// direct.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <stdio.h>
#include <math.h>
#include <list>
#include <mmsystem.h>

#include <ddraw.h>
#include <dinput.h>

#include "Starfield.h"
#include "Ship.h"
#include "Bullet.h"
#include "Rocket.h"
#include "Wave.h"
#include "static.h"

#define MAX_LOADSTRING 100
#define KEYDOWN(name, key) (name[key] & 0x80) 

// Global Variables:
HINSTANCE hInst;													// current instance
TCHAR szTitle[MAX_LOADSTRING];										// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
HWND g_hWnd;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HRESULT				InitialiseDirectDraw();
HRESULT				InitialiseDirectInput();
HRESULT				Input();
HRESULT				Process();
HRESULT				Output();

bool g_fActive = false;

IDirectDraw7 *g_lpDD = NULL;
IDirectDrawSurface7 *g_lpDDSFront = NULL;
IDirectDrawSurface7 *g_lpDDSBack = NULL;
IDirectDrawSurface7 *g_lpDDSRocket = NULL;
IDirectDrawSurface7 *g_lpDDSSight = NULL;
IDirectDrawSurface7 *g_lpDDSShip = NULL;
IDirectDrawSurface7 *g_lpDDSBullet = NULL;
IDirectDrawSurface7 *g_lpDDSAlien = NULL;
IDirectInputDevice8 *g_lpKeyboard = NULL;
IDirectInputDevice8 *g_lpMouse = NULL;
HANDLE g_hMouseEvent;

Ship g_Ship;
Starfield g_Starfield;
int g_nXOff = 0, g_nYOff = 0;
int g_nFPS = 0;

bool g_fCtrlPress = false;
bool g_fLeftClick = false;
bool g_fFire = false;
bool g_fRightClick = false;
bool g_fFire2 = false;
DWORD g_dwFillColour = 0;
DWORD g_dwLastFrameTime = 0;
BYTE g_bKeys = 0;
DWORD g_dwLastFire;
DWORD g_dwFireRate = 500;
DWORD g_dwGameStart;
std::list<Wave *> g_waves;
std::list<Static *> g_statics;

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
int g_nWaves = sizeof(awi) / sizeof(ALIENWAVEINFO);
int g_nWave = 0;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DIRECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DIRECT);

	HRESULT hr = NULL;

	hr = InitialiseDirectDraw();
	hr = InitialiseDirectInput();	

	g_Starfield.Initialise();

	// Main message loop:
	int x = 0;
	int frame = 0;
	g_dwGameStart = GetTickCount();

    BOOL bGotMsg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
	DWORD dwStart = GetTickCount();
    while (msg.message != WM_QUIT) {
		DWORD dwFrameStart = GetTickCount();
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.
        if (g_fActive) {
            bGotMsg = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        } else {
            bGotMsg = GetMessage(&msg, NULL, 0, 0);
		}

        if (bGotMsg) {
            // Translate and dispatch the message
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        } else {
            // Render a frame during idle time (no messages are waiting)

			// Game loop goes something like this:
			//
			//	Check input (keyboard, mouse, network)
			//  Update positions and generate frame
			//  Render back buffer
			//  Display back buffer

			// the simplest model for any machine
			try {
				DWORD dwFrameStart = GetTickCount();
				Input();
				Process();
				Output();
				g_dwLastFrameTime = GetTickCount() - dwFrameStart;
				frame++;
			} catch (char *str) {
				OutputDebugString("Exception caught: ");
				OutputDebugString(str);
			}

			if (GetTickCount() - dwStart >= 1000) {
				g_nFPS = frame;
				dwStart = GetTickCount();
				frame = 0;
			}
        }
    }

	if (g_lpKeyboard) {
		g_lpKeyboard->Unacquire();
		g_lpKeyboard->Release();
	}

	if (g_lpMouse) {
		g_lpMouse->Unacquire();
		g_lpMouse->Release();
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DIRECT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	
	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUPWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (!g_hWnd) {
		return FALSE;
	}
	
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	
	switch (message) {
		case WM_DESTROY:
			if (g_lpDDSBack) {
				g_lpDDSBack->Release();
			}
			if (g_lpDDSFront) {
				g_lpDDSBack->Release();
			}
			if (g_lpDD) {
				g_lpDD->Release();
				g_lpDD = NULL;
			}

			PostQuitMessage(0);
			break;

		case WM_ACTIVATEAPP:
			if (wParam == TRUE) {
				g_fActive = true;
				if (g_lpMouse) g_lpMouse->Acquire();
				if (g_lpKeyboard) g_lpKeyboard->Acquire();
			} else {
				g_fActive = false;
				if (g_lpMouse) g_lpMouse->Unacquire();
				if (g_lpKeyboard) g_lpKeyboard->Unacquire();
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}

HRESULT InitialiseDirectDraw()
{
	HRESULT hr = NULL;
	
	hr = DirectDrawCreateEx(NULL, (void **)&g_lpDD, IID_IDirectDraw7, NULL);
	if (FAILED(hr)) return -1;

	hr = g_lpDD->SetCooperativeLevel(g_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	if (hr == DD_OK) {
	} else {
	}

	hr = g_lpDD->SetDisplayMode(800, 600, 32, 0, 0);
	if (hr == DD_OK) {
	} else {
	}

	DDSURFACEDESC2 ddsd;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;

	hr = g_lpDD->CreateSurface(&ddsd, &g_lpDDSFront, NULL);
	if (hr == DD_OK) {
	} else {
		char szMessage[256];
		sprintf(szMessage, "Failed to create primary surface: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER; 
	hr = g_lpDDSFront->GetAttachedSurface(&ddsd.ddsCaps, &g_lpDDSBack);
	if (hr == DD_OK) {
	} else {
		MessageBox(g_hWnd, "Failed to get back buffer", "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	// create a surface for our rocket sprite
	// Create an offscreen bitmap. 
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; 
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
	ddsd.dwWidth = 23; 
	ddsd.dwHeight = 9;
	hr = g_lpDD->CreateSurface(&ddsd, &g_lpDDSRocket, NULL); 
	if (hr != DD_OK) { 
		char szMessage[256];
		sprintf(szMessage, "Failed to create surface for rocket: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1; 
	} 
	// Load the bitmap resource. 
	HBITMAP hbm = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ROCKET), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hbm == NULL) {
		char szMessage[256];
		sprintf(szMessage, "Failed to load rocket bitmap: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return FALSE; 
	}

	DDCOLORKEY ddck;
	ddck.dwColorSpaceHighValue = RGB(0, 0, 0);
	ddck.dwColorSpaceLowValue = RGB(0, 0, 0);
	g_lpDDSRocket->SetColorKey(DDCKEY_SRCBLT, &ddck);

	if (FAILED(hr = DDUtil_CopyBitmap(g_lpDDSRocket, hbm, 0, 0, 23, 9))) {
		char szMessage[256];
		sprintf(szMessage, "Failed to copy rocket bitmap: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return FALSE; 
	}
	DeleteObject(hbm);

	/////////////////////////////////////////////////

	ddsd.dwWidth = 32;
	ddsd.dwHeight = 32;
	hr = g_lpDD->CreateSurface(&ddsd, &g_lpDDSShip, NULL);
	hbm = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_SHIP), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	g_lpDDSShip->SetColorKey(DDCKEY_SRCBLT, &ddck);

	if (FAILED(hr = DDUtil_CopyBitmap(g_lpDDSShip, hbm, 0, 0, 32, 32))) {
	}
	DeleteObject(hbm);

	//////////////////////////////////////////////////

	ddsd.dwWidth = 11;
	ddsd.dwHeight = 11;
	hr = g_lpDD->CreateSurface(&ddsd, &g_lpDDSSight, NULL);
	hbm = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_SIGHT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	g_lpDDSSight->SetColorKey(DDCKEY_SRCBLT, &ddck);
	if (FAILED(hr = DDUtil_CopyBitmap(g_lpDDSSight, hbm, 0, 0, 11, 11))) {
	}
	DeleteObject(hbm);

	///////////////////////////////////////////////////

	ddsd.dwWidth = 5;
	ddsd.dwHeight = 5;
	hr = g_lpDD->CreateSurface(&ddsd, &g_lpDDSBullet, NULL);
	hbm = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BULLET), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	g_lpDDSBullet->SetColorKey(DDCKEY_SRCBLT, &ddck);
	if (FAILED(hr = DDUtil_CopyBitmap(g_lpDDSBullet, hbm, 0, 0, 5, 5))) {
	}
	DeleteObject(hbm);

	/////////////////////////////////////////////////////

	ddsd.dwWidth = 20;
	ddsd.dwHeight = 20;
	hr = g_lpDD->CreateSurface(&ddsd, &g_lpDDSAlien, NULL);
	hbm = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_ALIEN), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	g_lpDDSAlien->SetColorKey(DDCKEY_SRCBLT, &ddck);
	if (FAILED(hr = DDUtil_CopyBitmap(g_lpDDSAlien, hbm, 0, 0, 20, 20))) {
		char szMessage[256];
		sprintf(szMessage, "Failed to copy alien bitmap: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
	}
	DeleteObject(hbm);

	return hr;
}

HRESULT InitialiseDirectInput()
{
	HRESULT hr = NULL;
	IDirectInput8 *lpDI;

	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&lpDI, NULL);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to create DirectInput object: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = lpDI->CreateDevice(GUID_SysKeyboard, &g_lpKeyboard, NULL);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to create keyboard device object: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = g_lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set keyboard data format: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = g_lpKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set keyboard behaviour: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	if (g_lpKeyboard) g_lpKeyboard->Acquire();

	// now the mouse
	hr = lpDI->CreateDevice(GUID_SysMouse, &g_lpMouse, NULL);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to create mouse device object: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = g_lpMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set mouse data format: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	hr = g_lpMouse->SetCooperativeLevel(g_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		char szMessage[256];
		sprintf(szMessage, "Failed to set keyboard behaviour: (%08x)", hr);
		MessageBox(g_hWnd, szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1;
	}

	g_hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_hMouseEvent == NULL) {
		return FALSE;
	}
 
	hr = g_lpMouse->SetEventNotification(g_hMouseEvent);
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

	hr = g_lpMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) {
		return FALSE;
	}
 
	if (g_lpMouse) g_lpMouse->Acquire();

	lpDI->Release();

	return DI_OK;
}

HRESULT Input()
{
	char buffer[256];
	HRESULT hr = NULL;

	g_bKeys = 0;

	if (g_lpKeyboard) {
		g_lpKeyboard->GetDeviceState(sizeof(buffer), (LPVOID)&buffer);
		if (KEYDOWN(buffer, DIK_UP)) {
			g_bKeys |= 8;
		}
		if (KEYDOWN(buffer, DIK_DOWN)) {
			g_bKeys |= 4;
		}
		if (KEYDOWN(buffer, DIK_LEFT)) {
			g_bKeys |= 2;
		}
		if (KEYDOWN(buffer, DIK_RIGHT)) {
			g_bKeys |= 1;
		}
		if (KEYDOWN(buffer, DIK_LCONTROL)) {
			if (g_fCtrlPress) {
				if (GetTickCount() - g_dwLastFire >= g_dwFireRate) {
					g_fFire = true;
					g_dwLastFire = GetTickCount();
				}
			} else {
				g_fFire = true;
				g_dwLastFire = GetTickCount();
			}
			g_fCtrlPress = true;
		} else {
			g_fCtrlPress = false;
		}
	}

	BOOL bDone = FALSE;
	g_nXOff = 0;
	g_nYOff = 0;
	// Check the mouse...
	if (WaitForSingleObject(g_hMouseEvent, 0) == WAIT_OBJECT_0) {
		while (!bDone) {
			DIDEVICEOBJECTDATA od;
			DWORD dwElements = 1;   // number of items to be retrieved

 			hr = g_lpMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
			if (hr == DIERR_INPUTLOST) {
				g_lpMouse->Acquire();
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
					g_nXOff = od.dwData;
					break;
				// Mouse vertical motion
				case DIMOFS_Y: 
					g_nYOff = od.dwData;
					break; 
				case DIMOFS_Z:
					break;
				// DIMOFS_BUTTON0: Left button pressed or released 
				case DIMOFS_BUTTON0:
					if (od.dwData & 0x80) {
						if (g_fLeftClick == false) {
							g_fFire = true;
							g_dwLastFire = GetTickCount();
						}
						g_fLeftClick = true;
					} else {
						g_fLeftClick = false;
					}
					break;
				// DIMOFS_BUTTON1: Right button pressed or released 
				case DIMOFS_BUTTON1:
					if (od.dwData & 0x80) {
						if (g_fRightClick == false) {
							g_fFire2 = true;
						}
						g_fRightClick = true;
					} else {
						g_fRightClick = false;
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
		if (g_fLeftClick) {
			// here's how to do rapid fire...
			if (GetTickCount() - g_dwLastFire >= g_dwFireRate) {
				g_fFire = true;
				g_dwLastFire = GetTickCount();
			}
		}
	}

	return hr;
}

HRESULT Process()
{
	// now calculate the angle of the aim, and readjust the coordinates so that the crosshair stays
	// in a circle around the ship
	g_Ship.UpdateAim(g_nXOff, g_nYOff);
	g_Ship.UpdatePosition(g_bKeys, g_dwLastFrameTime);

	g_Starfield.UpdatePosition(g_dwLastFrameTime);

	// update the positions of all the bullets
	{
		std::list<Bullet *>::iterator it = g_bullets.begin();
		std::list<Bullet *>::iterator end = g_bullets.end();
		while (it != end) {
			if ((*it)->UpdatePosition(g_dwLastFrameTime) == false) {
				std::list<Bullet *>::iterator del = it;

				++it;
				delete (*del);
				g_bullets.erase(del);
			} else {
				if ((*it)->CheckBaddies() == true) {
					// alien was destroyed, delete the bullet
					std::list<Bullet *>::iterator del = it;
					++it;
					delete (*del);

					g_bullets.erase(del);
				} else {
					++it;
				}
			}
		}
	}
	{
		std::list<Rocket *>::iterator it = g_rockets.begin();
		std::list<Rocket *>::iterator end = g_rockets.end();
		while (it != end) {
			if ((*it)->UpdatePosition(g_dwLastFrameTime) == false) {
				std::list<Rocket *>::iterator del = it;
				++it;
				delete (*del);
				g_rockets.erase(del);
			} else {
				++it;
			}
		}
	}
	{
		std::list<Wave *>::iterator it = g_waves.begin();
		std::list<Wave *>::iterator end = g_waves.end();
		while (it != end) {
			(*it)->UpdatePosition(g_dwLastFrameTime);
			++it;
		}
	}

	if (g_fFire) {
		// create new bullet object
		Bullet *pBullet = new Bullet(g_Ship.m_dX, g_Ship.m_dY, g_Ship.m_dAim, 800.0);
		g_bullets.push_back(pBullet);
		g_fFire = false;
		PlaySound("bfire.wav", NULL, SND_ASYNC);
	}

	if (g_fFire2) {
		Rocket *pRocket = new Rocket(g_Ship.m_dX, g_Ship.m_dY, /*g_Ship.m_dAim*/0.0, -100.0);
		g_rockets.push_back(pRocket);
		g_fFire2 = false;
	}

	/////////////////////////////////////////////////////

	if (g_nWave < g_nWaves) {
		if (GetTickCount() - g_dwGameStart >= awi[g_nWave].dwTime) {
			Wave *wave = new Wave();
			wave->Create(5);
			g_waves.push_back(wave);
			g_nWave++;
		}
	}

	return DD_OK;
}

HRESULT Output()
{
	HRESULT hr = NULL;

	// Erase the background
	DDBLTFX ddbltfx;
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize      = sizeof(ddbltfx);
	ddbltfx.dwFillColor = g_dwFillColour;
	g_lpDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);

	// draw some primitive shapes using GDI calls (this will change to sprites!)
	g_Starfield.Draw();
	{
		std::list<Bullet *>::iterator it = g_bullets.begin();
		std::list<Bullet *>::iterator end = g_bullets.end();
		while (it != end) {
			(*it)->Draw();
			++it;
		}
	}
	{
		std::list<Rocket *>::iterator it = g_rockets.begin();
		std::list<Rocket *>::iterator end = g_rockets.end();
		while (it != end) {
			(*it)->Draw();
			++it;
		}
	}
	{
		std::list<Wave *>::iterator it = g_waves.begin();
		std::list<Wave *>::iterator end = g_waves.end();
		while (it != end) {
			if ((*it)->Draw() == false) {
				std::list<Wave *>::iterator del = it;
				++it;
				delete (*del);
				g_waves.erase(del);
			} else {
				++it;
			}
		}
	}
	{
		std::list<Static *>::iterator it = g_statics.begin();
		std::list<Static *>::iterator end = g_statics.end();
		while (it != end) {
			if ((*it)->Update(g_dwLastFrameTime) == false) {
				std::list<Static *>::iterator del = it;
				++it;
				delete (*del);
				g_statics.erase(del);
			} else {
				++it;
			}
		}
	}
	g_Ship.Draw();

	HDC hdc;
	g_lpDDSBack->GetDC(&hdc);
	char sz[128];
	sprintf(sz, "%d fps", g_nFPS);
	TextOut(hdc, 0, 0, sz, strlen(sz));
	g_lpDDSBack->ReleaseDC(hdc);

	hr = g_lpDDSFront->Flip(NULL, DDFLIP_WAIT);
	if (hr != DD_OK) {
		if (hr == DDERR_SURFACELOST) {
			if (g_lpDD) {
				g_lpDD->RestoreAllSurfaces();
			}
		}
	}

	return hr;
}
