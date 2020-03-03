// Game.h: interface for the Game class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAME_H__0BB5854D_A4FF_46C8_ADB3_0B01D7B6BC10__INCLUDED_)
#define AFX_GAME_H__0BB5854D_A4FF_46C8_ADB3_0B01D7B6BC10__INCLUDED_

#include "Ship.h"
#include "Starfield.h"
#include "Wave.h"
#include "Static.h"
#include "Bullet.h"
#include "Rocket.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define KEYDOWN(name, key) (name[key] & 0x80) 

class Game  
{
// Construction / Destruction
public:
	Game();
	virtual ~Game();

// Implementation
public:
	BOOL Initialise(HWND hWnd);
	void Frame();
	void Cleanup();

protected:
	HRESULT InitialiseDirectDraw();
	HRESULT InitialiseDirectInput();
	HRESULT InitialiseDirectSound();
	HRESULT Input();
	HRESULT Process();
	HRESULT Output();

// Data members
protected:
	IDirectDraw7 *m_lpDD;
	IDirectDrawSurface7 *m_lpDDSFront;
	IDirectDrawSurface7 *m_lpDDSBack;
	IDirectDrawSurface7 *m_lpDDSRocket;
	IDirectDrawSurface7 *m_lpDDSSight;
	IDirectDrawSurface7 *m_lpDDSShip;
	IDirectDrawSurface7 *m_lpDDSBullet;
	IDirectDrawSurface7 *m_lpDDSAlien;
	IDirectInputDevice8 *m_lpKeyboard;
	IDirectInputDevice8 *m_lpMouse;
	HANDLE m_hMouseEvent;
	IDirectSound8 *m_lpDS;
	Ship m_Ship;
	Starfield m_Starfield;
	HWND m_hWnd;
	DWORD m_dwLastFrameTime;
	BYTE m_bKeys;

	int m_nXOff, m_nYOff;
	int m_nFPS;

	bool m_fCtrlPress;
	bool m_fLeftClick;
	bool m_fFire;
	bool m_fRightClick;
	bool m_fFire2;
	DWORD m_dwFillColour;
	DWORD m_dwLastFire;
	DWORD m_dwFireRate;
	DWORD m_dwGameStart;
	std::list<Wave *> m_waves;
	std::list<Static *> m_statics;
	std::list<Bullet *> m_bullets;
	std::list<Rocket *> m_rockets;
	DWORD m_dwStart;
};

#endif // !defined(AFX_GAME_H__0BB5854D_A4FF_46C8_ADB3_0B01D7B6BC10__INCLUDED_)
