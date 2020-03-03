// Ship.cpp: implementation of the Ship class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ship.h"

#include <ddraw.h>
#include <math.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ship::Ship()
{
	m_dX = 50;
	m_dY = 300;
	m_dAim = 0;
	m_nSightX = 150;
	m_nSightY = 300;
	m_dVelocity = 300.0;
	m_dAcceleration = 1000.0;
	m_dMaxVelocity = 600.0;
}

Ship::~Ship()
{

}

// bKeys is a bit mask:
// +-+-+-+-+-+-+-+-+
// | | | | |U|D|L|R|
// +-+-+-+-+-+-+-+-+
void Ship::UpdatePosition(BYTE bKeys, DWORD dwLastFrameTime)
{
	double dFractionOfSecond = dwLastFrameTime / 1000.0;
	double dTrajectory = 0.0;
	bool fMove = true;

	// There must be a better way of doing this?!
	switch (bKeys) {
	case 0: // nothing
		fMove = false;
		// TODO: Reset velocity in here when keys are released, then ship will accelerate again when key is pressed
		m_dVelocity = 400.0;
		break;
	case 1:	// right
		dTrajectory = 0.0;
		break;
	case 2: // left
		dTrajectory = 3.141592;
		break;
	case 4: // down
		dTrajectory = 3.141592 / 2.;
		break;
	case 8: // up
		dTrajectory = -3.141592 / 2.;
		break;
	case 5: // down right
		dTrajectory = 3.141592 / 4.;
		break;
	case 6: // down left
		dTrajectory = 3.141592 / 2. + (3.141592 / 4.);
		break;
	case 9: // up right
		dTrajectory = -3.141592 / 4.;
		break;
	case 10: // up left
		dTrajectory = -3.141592 / 2. + (-3.141592 / 4.);
		break;
	case 12: // up and down
		fMove = false;
		break;
	case 3: // left and right
		fMove = false;
		break;
	}

	if (fMove) {
		m_dX = m_dX + (cos(dTrajectory) * m_dVelocity * dFractionOfSecond);
		if (m_dX < 16.) m_dX = 16.;
		if (m_dX > 784.) m_dX = 784.;

		m_dY = m_dY + (sin(dTrajectory) * m_dVelocity * dFractionOfSecond);	
		if (m_dY < 16.) m_dY = 16.;
		if (m_dY > 584.) m_dY = 584.;

		m_nSightX = m_dX + (int)(cos(m_dAim) * 100.0);
		m_nSightY = m_dY + (int)(sin(m_dAim) * 100.0);

		// TODO: If we want to implement acceleration, simply increase the velocity here
		if (m_dAcceleration && m_dVelocity < m_dMaxVelocity) {
			m_dVelocity += m_dAcceleration * dFractionOfSecond;
		}
	}
}

void Ship::UpdateAim(int nXOff, int nYOff)
{
	if (nXOff || nYOff) {
		m_nSightX += nXOff;
		m_nSightY += nYOff;

		m_dAim = atan2(m_nSightY - m_dY, m_nSightX - m_dX);
		m_nSightX = m_dX + (int)(cos(m_dAim) * 100.0);
		m_nSightY = m_dY + (int)(sin(m_dAim) * 100.0);
	}
}

void Ship::Draw()
{
	int x = (int)m_dX;
	int y = (int)m_dY;

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.bottom = 31;
	rc.right = 31;

	HRESULT hr;
	hr = g_lpDDSBack->BltFast(x - 16, y - 16, g_lpDDSShip, &rc, DDBLTFAST_SRCCOLORKEY);
	if (hr != DD_OK) {
		if (hr == DDERR_SURFACELOST) {
			if (g_lpDD) g_lpDD->RestoreAllSurfaces();
		} else {
			char szMessage[128];
			sprintf(szMessage, "Failed to blit ship: %08x @ (%d, %d, %d, %d) -> (%d, %d)\n", hr, rc.left, rc.top, rc.right, rc.bottom, x, y);
			OutputDebugString(szMessage);
		}
	}

	rc.right = 10;
	rc.bottom = 10;

	hr = g_lpDDSBack->BltFast(m_nSightX - 5, m_nSightY - 5, g_lpDDSSight, &rc, DDBLTFAST_SRCCOLORKEY);
	if (hr != DD_OK) {
		if (hr == DDERR_SURFACELOST) {
			if (g_lpDD) g_lpDD->RestoreAllSurfaces();
		} else {
			char szMessage[128];
			sprintf(szMessage, "Failed to blit ship: %08x @ (%d, %d, %d, %d) -> (%d, %d)\n", hr, rc.left, rc.top, rc.right, rc.bottom, x, y);
			OutputDebugString(szMessage);
		}
	}
}