// direct.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <stdio.h>

#include <ddraw.h>
#include <dinput.h>

#define MAX_LOADSTRING 100

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

bool g_fActive = false;

IDirectDraw7 *g_lpDD = NULL;
IDirectDrawSurface7 *g_lpDDSFront = NULL;
IDirectDrawSurface7 *g_lpDDSBack = NULL;
IDirectInputDevice8 *g_lpKeyboard = NULL;
IDirectInputDevice8 *g_lpMouse = NULL;
HANDLE g_hMouseEvent;

int shipx = 400, shipy = 300;
int aimx = 400, aimy = 300;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
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

	HRESULT hr;
	
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

	/////////////////////////////////////////////////////////////////////////////
	//
	// Now initialise the DirectInput
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
		// the header
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		// the data
		dipdw.dwData            = SAMPLE_BUFFER_SIZE;
 

	hr = g_lpMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (FAILED(hr)) {
		return FALSE;
	}
 
	if (g_lpMouse) g_lpMouse->Acquire();

	lpDI->Release();
	
	// Main message loop:
	IDirectDrawSurface7 *pSurface = g_lpDDSBack;
	int x = 0;
	int frame = 0;

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
#define KEYDOWN(name, key) (name[key] & 0x80) 

			DWORD dwFillColour = 0;
			char buffer[256];
			if (g_lpKeyboard) {
				g_lpKeyboard->GetDeviceState(sizeof(buffer), (LPVOID)&buffer);
				if (KEYDOWN(buffer, DIK_DOWN)) {
//					dwFillColour |= 0x00ff0000;
					shipy += 5;
				}
				if (KEYDOWN(buffer, DIK_UP)) {
//					dwFillColour |= 0x0000ff00;
					shipy -= 5;
				}
				if (KEYDOWN(buffer, DIK_LEFT)) {
//					dwFillColour |= 0x000000ff;
					shipx -= 5;
				}
				if (KEYDOWN(buffer, DIK_RIGHT)) {
//					dwFillColour |= 0x00336699;
					shipx += 5;
				}
			}

			BOOL bDone = FALSE;
			char szMessage[128];
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
							sprintf(szMessage, "X %ld", od.dwData);
							aimx += od.dwData;
							if (aimx < 0) aimx = 0;
							if (aimx > 800) aimx = 800;
							break;
						// Mouse vertical motion
						case DIMOFS_Y: 
							sprintf(szMessage, "Y %ld", od.dwData);
							aimy += od.dwData;
							if (aimy < 0) aimy = 0;
							if (aimy > 600) aimy = 600;
							break; 
						case DIMOFS_Z:
							sprintf(szMessage, "Z %ld", od.dwData);
							break;
						// DIMOFS_BUTTON0: Right button pressed or released 
						case DIMOFS_BUTTON0:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 0 - clicked");
							} else {
								strcpy(szMessage, "Button 0 - released");
							}
							break;
						// DIMOFS_BUTTON1: Left button pressed or released 
						case DIMOFS_BUTTON1:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 1 - clicked");
							} else {
								strcpy(szMessage, "Button 1 - released");
							}
							break;
						case DIMOFS_BUTTON2:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 2 - clicked");
							} else {
								strcpy(szMessage, "Button 2 - released");
							}
							break;
						case DIMOFS_BUTTON3:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 3 - clicked");
							} else {
								strcpy(szMessage, "Button 3 - released");
							}
							break;
						case DIMOFS_BUTTON4:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 4 - clicked");
							} else {
								strcpy(szMessage, "Button 4 - released");
							}
							break;
						case DIMOFS_BUTTON5:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 5 - clicked");
							} else {
								strcpy(szMessage, "Button 5 - released");
							}
							break;
						case DIMOFS_BUTTON6:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 6 - clicked");
							} else {
								strcpy(szMessage, "Button 6 - released");
							}
							break;
						case DIMOFS_BUTTON7:
							if (od.dwData & 0x80) {
								strcpy(szMessage, "Button 7 - clicked");
							} else {
								strcpy(szMessage, "Button 7 - released");
							}
							break;
					}
				}
			}

			// Erase the background
			DDBLTFX ddbltfx;
			ZeroMemory(&ddbltfx, sizeof(ddbltfx));
			ddbltfx.dwSize      = sizeof(ddbltfx);
			ddbltfx.dwFillColor = dwFillColour;
			g_lpDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);

			HDC hdc;
			g_lpDDSBack->GetDC(&hdc);
			SelectObject(hdc, GetStockObject(WHITE_PEN));
			frame++;
			if (GetTickCount() - dwStart >= 1000) {
				char szMessage[128];
				sprintf(szMessage, "%ld fps", frame);
				TextOut(hdc, 0, 0, szMessage, strlen(szMessage));
				frame = 0;
				dwStart = GetTickCount();
			}
			Rectangle(hdc, shipx - 16, shipy - 16, shipx + 16, shipy + 16);
			MoveToEx(hdc, aimx, aimy - 8, NULL);
			LineTo(hdc, aimx, aimy + 8);
			MoveToEx(hdc, aimx - 8, aimy, NULL);
			LineTo(hdc, aimx + 8, aimy);
			g_lpDDSBack->ReleaseDC(hdc);

			hr = g_lpDDSFront->Flip(NULL, DDFLIP_WAIT);
			if (hr == DD_OK) {

			} else {
				if (hr != DDERR_SURFACELOST)
					return hr;

				// The surfaces were lost so restore them 
				if (g_lpDD) {
					g_lpDD->RestoreAllSurfaces();
				}
			}

			if (pSurface == g_lpDDSFront) {
				pSurface = g_lpDDSBack;
			} else {
				pSurface = g_lpDDSFront;
			}

//			if (30 - (GetTickCount() - dwFrameStart) > 0) {
//				Sleep(30 - (GetTickCount() - dwFrameStart));
//			}
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
			
		case WM_CREATE:
			{
			}
			break;
		case WM_LBUTTONDOWN:
			g_lpDDSFront->Flip(NULL, DDFLIP_WAIT);
			break;
			
		case WM_ACTIVATEAPP:
			g_fActive = (BOOL)wParam;
			if (g_fActive) {
				if (g_lpMouse) g_lpMouse->Acquire();
			} else {
				if (g_lpMouse) g_lpMouse->Unacquire();
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}
