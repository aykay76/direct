// Bullet.cpp: implementation of the Bullet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bullet.h"

#include <ddraw.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern IDirectDraw7 *g_lpDD;
extern IDirectDrawSurface7 *g_lpDDSBack;
extern IDirectDrawSurface7 *g_lpDDSBullet;

std::list<Bullet *> g_bullets;

Bullet::Bullet()
{

}

Bullet::Bullet(double dX, double dY, double dTrajectory, double dVelocity)
{
	m_dX = dX;
	m_dY = dY;
	m_dTrajectory = dTrajectory;
	m_dVelocity = dVelocity;
	m_dAcceleration = 0.0;
	m_dMaxVelocity = dVelocity;
}

Bullet::~Bullet()
{

}

bool Bullet::UpdatePosition(DWORD dwLastFrameTime)
{
	double dFractionOfSecond = dwLastFrameTime / 1000.0;

	m_dX = m_dX + (cos(m_dTrajectory) * m_dVelocity * dFractionOfSecond);
	m_dY = m_dY + (sin(m_dTrajectory) * m_dVelocity * dFractionOfSecond);

	if (m_dX < 0.0 || m_dX > 800.0 || m_dY < 0.0 || m_dY > 600.0) {
		return false;
	}

	// now update the velocity, based on the acceleration
	if (m_dAcceleration && m_dVelocity < m_dMaxVelocity) {
		m_dVelocity += m_dAcceleration * dFractionOfSecond;
	}

	return true;
}

void Bullet::Draw(HDC hdc)
{
	int x = (int)m_dX;
	int y = (int)m_dY;

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.bottom = 4;
	rc.right = 4;

	HRESULT hr;
	hr = g_lpDDSBack->BltFast(x - 2, y - 2, g_lpDDSBullet, &rc, DDBLTFAST_SRCCOLORKEY);
	if (hr != DD_OK) {
		if (hr == DDERR_SURFACELOST) {
			if (g_lpDD) g_lpDD->RestoreAllSurfaces();
		}
	}
}