// Ship.h: interface for the Ship class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIP_H__2031CF1F_C14D_45C5_9D60_BDF8FB7C6BDC__INCLUDED_)
#define AFX_SHIP_H__2031CF1F_C14D_45C5_9D60_BDF8FB7C6BDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Ship  
{
public:
	Ship();
	virtual ~Ship();

public:
	void UpdatePosition(BYTE bKeys, DWORD dwLastFrameTime);
	void UpdateAim(int nXOff, int nYOff);
	void Draw(HDC hdc);

public:
	double m_dX;
	double m_dY;
	double m_dAim;
	int m_nSightX;
	int m_nSightY;
	double m_dVelocity;
	double m_dAcceleration;
	double m_dMaxVelocity;
};

#endif // !defined(AFX_SHIP_H__2031CF1F_C14D_45C5_9D60_BDF8FB7C6BDC__INCLUDED_)
