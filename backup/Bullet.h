// Bullet.h: interface for the Bullet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BULLET_H__4EC8F70F_B729_4800_9657_15B8577970EC__INCLUDED_)
#define AFX_BULLET_H__4EC8F70F_B729_4800_9657_15B8577970EC__INCLUDED_

#include <list>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Bullet  
{
public:
	Bullet();
	Bullet(double dX, double dY, double dTrajectory, double dVelocity);
	virtual ~Bullet();

public:
	bool UpdatePosition(DWORD dwLastFrameTime);
	void Draw(HDC hdc);

public:
	double m_dX;
	double m_dY;
	double m_dTrajectory;
	double m_dVelocity;					// velocity is measured in pixels per second (instead of metres)
	double m_dAcceleration;				// acceleration is measured in pixels per second per second
	double m_dMaxVelocity;
};

extern std::list<Bullet *> g_bullets;

#endif // !defined(AFX_BULLET_H__4EC8F70F_B729_4800_9657_15B8577970EC__INCLUDED_)
