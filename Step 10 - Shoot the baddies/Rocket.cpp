// Rocket.cpp: implementation of the Rocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Rocket.h"

#include <ddraw.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::list<Rocket *> g_rockets;
extern IDirectDrawSurface7 *g_lpDDSBack;
extern IDirectDrawSurface7 *g_lpDDSRocket;
extern IDirectDraw7 *g_lpDD;

Rocket::Rocket()
{

}

Rocket::Rocket(double dX, double dY, double dTrajectory, double dVelocity, double dAcceleration, double dMaxVelocity)
{
	m_dX = dX;
	m_dY = dY;
	m_dTrajectory = dTrajectory;
	m_dVelocity = dVelocity;
	m_dAcceleration = dAcceleration;
	m_dMaxVelocity = dMaxVelocity;
}

Rocket::~Rocket()
{

}

bool Rocket::UpdatePosition(DWORD dwLastFrameTime)
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

void Rocket::Draw()
{
	int x = (int)m_dX;
	int y = (int)m_dY;

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.bottom = 8;
	rc.right = 22;

	HRESULT hr;
	hr = g_lpDDSBack->BltFast(x, y, g_lpDDSRocket, &rc, DDBLTFAST_SRCCOLORKEY);
	if (hr != DD_OK) {
		if (hr == DDERR_SURFACELOST) {
			if (g_lpDD) g_lpDD->RestoreAllSurfaces();
		} else {
			char szMessage[128];
			sprintf(szMessage, "Failed to blit rocket: %08x @ (%d, %d, %d, %d) -> (%d, %d)\n", hr, rc.left, rc.top, rc.right, rc.bottom, x, y);
			OutputDebugString(szMessage);
		}
	}
}