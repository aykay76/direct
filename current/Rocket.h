// Rocket.h: interface for the Rocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROCKET_H__741C862A_2CBB_4F21_BA92_CA85AE4B2BC8__INCLUDED_)
#define AFX_ROCKET_H__741C862A_2CBB_4F21_BA92_CA85AE4B2BC8__INCLUDED_

#include <list>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Rocket  
{
public:
	Rocket();
	Rocket(double dX, double dY, double dTrajectory, double dVelocity, double dAcceleration = 1500.0, double dMaxVelocity = 800.0);
	virtual ~Rocket();

public:
	bool UpdatePosition(DWORD dwLastFrameTime);
	bool CheckBaddies();
	void Draw();

public:
	double m_dX;
	double m_dY;
	double m_dTrajectory;
	double m_dVelocity;					// velocity is measured in pixels per second (instead of metres)
	double m_dAcceleration;				// acceleration is measured in pixels per second per second
	double m_dMaxVelocity;				// don't just accelerate forever!
	int m_nDamage;
};

#endif // !defined(AFX_ROCKET_H__741C862A_2CBB_4F21_BA92_CA85AE4B2BC8__INCLUDED_)
