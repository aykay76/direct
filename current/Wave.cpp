// Wave.cpp: implementation of the Wave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "direct.h"
#include "Wave.h"

#include <ddraw.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Wave::Wave()
{
	m_dVelocity = 15.0;
}

Wave::~Wave()
{

}

void Wave::Create(int count)
{
	m_nCount = count;

	int y = rand() * 400 / RAND_MAX;

	for (int i = 0; i < count; i++) {
		Baddy *baddy = new Baddy();

		baddy->x = 800 + 40 * i;
		baddy->y = y;
		baddy->score = 10;
		baddy->hitpoints = 10;

		m_baddies.push_back(baddy);
	}

	m_movement = MovementStraight;
}

void Wave::UpdatePosition(DWORD dwLastFrameTime)
{
	double dFractionOfSecond = dwLastFrameTime / 100.0;

	std::list<Baddy *>::iterator it = m_baddies.begin();
	std::list<Baddy *>::iterator end = m_baddies.end();
	while (it != end) {
		(*it)->x -= m_dVelocity * dFractionOfSecond;
		if ((*it)->x + 10 < 0) {
			std::list<Baddy *>::iterator del = it;

			++it;
			delete (*del);
			m_baddies.erase(del);
			m_nCount--;
		} else {
			++it;
		}
	}
}

bool Wave::Draw()
{
	if (m_nCount == 0) {
		return false;
	}

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.bottom = 19;
	rc.right = 19;

	std::list<Baddy *>::iterator it = m_baddies.begin();
	std::list<Baddy *>::iterator end = m_baddies.end();
	while (it != end) {
		int x = (int)(*it)->x;
		int y = (int)(*it)->y;

		HRESULT hr;
		hr = g_lpDDSBack->BltFast(x - 10, y - 10, g_lpDDSAlien, &rc, DDBLTFAST_SRCCOLORKEY);
		if (hr != DD_OK) {
			if (hr == DDERR_SURFACELOST) {
				if (g_lpDD) g_lpDD->RestoreAllSurfaces();
			}
		}

		++it;
	}

	return true;
}