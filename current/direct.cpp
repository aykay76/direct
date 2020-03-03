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
#include <dsound.h>

#include "Game.h"
#include "Starfield.h"
#include "Ship.h"
#include "Bullet.h"
#include "Rocket.h"
#include "Wave.h"
#include "static.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;													// current instance
TCHAR szTitle[MAX_LOADSTRING];										// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
HWND g_hWnd;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HRESULT				InitialiseDirectDraw();
HRESULT				InitialiseDirectInput();
HRESULT				InitialiseDirectSound();
HRESULT				Input();
HRESULT				Process();
HRESULT				Output();
bool g_fActive = false;
Game theGame;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	HWND hWnd;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DIRECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!(hWnd = InitInstance (hInstance, nCmdShow))) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DIRECT);

	theGame.Initialise(hWnd);

	// Main message loop:
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
			theGame.Frame();
        }
    }

	theGame.Cleanup();

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
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable
	
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUPWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (!hWnd) {
		return NULL;
	}
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	return hWnd;
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
			PostQuitMessage(0);
			break;

		case WM_ACTIVATEAPP:
			if (wParam == TRUE) {
				g_fActive = true;
//				if (g_lpMouse) g_lpMouse->Acquire();
//				if (g_lpKeyboard) g_lpKeyboard->Acquire();
			} else {
				g_fActive = false;
//				if (g_lpMouse) g_lpMouse->Unacquire();
//				if (g_lpKeyboard) g_lpKeyboard->Unacquire();
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}

