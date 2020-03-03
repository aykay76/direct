// Wave.cpp: implementation of the Wave class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "direct.h"
#include "Wave.h"

#include <ddraw.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern IDirectDraw7 *g_lpDD;
extern IDirectDrawSurface7 *g_lpDDSBack;
extern IDirectDrawSurface7 *g_lpDDSAlien;
extern IDirectDrawSurface7 *g_lpDDSRocket;

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

	for (int i = 0; i < count; i++) {
		Baddy baddy;
		baddy.x = 500 + 30 * i;
		baddy.y = 400;
		baddy.score = 10;

		m_baddies.push_back(baddy);
	}

	m_movement = MovementStraight;
}

void Wave::UpdatePosition(DWORD dwLastFrameTime)
{
	double dFractionOfSecond = dwLastFrameTime / 100.0;

	std::vector<Baddy>::iterator it = m_baddies.begin();
	std::vector<Baddy>::iterator end = m_baddies.end();
	while (it != end) {
		it->x -= m_dVelocity * dFractionOfSecond;
		if (it->x < 0) {
			std::vector<Baddy>::iterator del = it;
			++it;
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

	char sz[128];
	sprintf(sz, "Drawing %d baddies\n", m_nCount);
	OutputDebugString(sz);

	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.bottom = 8;
	rc.right = 22;

	std::vector<Baddy>::iterator it = m_baddies.begin();
	std::vector<Baddy>::iterator end = m_baddies.end();
	while (it != end) {
		int x = (int)it->x;
		int y = (int)it->y;

		HRESULT hr;
		hr = g_lpDDSBack->BltFast(x - 10, y - 10, g_lpDDSRocket, &rc, DDBLTFAST_SRCCOLORKEY);
		if (hr != DD_OK) {
			if (hr == DDERR_SURFACELOST) {
				if (g_lpDD) g_lpDD->RestoreAllSurfaces();
			}
			char szMessage[128];
			sprintf(szMessage, "Failed to blit alien: %08x @ (%d, %d, %d, %d) -> (%d, %d): surface pointer: %p\n", hr, rc.left, rc.top, rc.right, rc.bottom, x, y, g_lpDDSAlien);
			OutputDebugString(szMessage);
		}

		++it;
	}

	return true;
}