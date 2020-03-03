// Bullet.h: interface for the Bullet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BULLET_H__4EC8F70F_B729_4800_9657_15B8577970EC__INCLUDED_)
#define AFX_BULLET_H__4EC8F70F_B729_4800_9657_15B8577970EC__INCLUDED_

#include <list>
#include "Wave.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Bullet  
{
public:
	Bullet();
	Bullet(double dX, double dY, double dTrajectory, double dVelocity, int nDamage = 10);
	virtual ~Bullet();

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
	double m_dMaxVelocity;
	int m_nDamage;
};

#endif // !defined(AFX_BULLET_H__4EC8F70F_B729_4800_9657_15B8577970EC__INCLUDED_)
