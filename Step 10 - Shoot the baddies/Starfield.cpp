// Starfield.cpp: implementation of the Starfield class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "direct.h"
#include "Starfield.h"

#include <ddraw.h>

extern IDirectDrawSurface7 *g_lpDDSBack;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Starfield::Starfield()
{
	m_dVelocity = 25.0;
}

Starfield::~Starfield()
{

}

void Starfield::Initialise()
{
	// create a nice scattering of stars, some big some small
	int i;

	srand(GetTickCount());

	for (i = 0; i < 200; i++) {
		Starfield::Star s;
		s.x = rand() * 800 / RAND_MAX;
		s.y = rand() * 600 / RAND_MAX;
		if (i % 15 == 0) {
			s.size = 2;
		} else {
			s.size = 1;
		}

		m_stars.push_back(s);
	}

	m_i = 0;
}

void Starfield::UpdatePosition(DWORD dwLastFrameTime)
{
	double dFractionOfSecond = dwLastFrameTime / 100.0;

	std::list<Star>::iterator it = m_stars.begin();
	std::list<Star>::iterator end = m_stars.end();

	while (it != end) {
		it->x -= m_dVelocity * dFractionOfSecond * it->size;

		if (it->x < 0) {
			// the star has gone, delete it
			std::list<Star>::iterator del = it;
			++it;
			m_stars.erase(del);

			// and create a new one in its place
			Star s;
			s.x = 800;//rand() * 800 / RAND_MAX;
			s.y = rand() * 600 / RAND_MAX;
			if (m_i % 15 == 0) {
				s.size = 2;
				m_i = 0;
			} else {
				s.size = 1;
			}
			m_i++;

			m_stars.push_back(s);
		} else {
			++it;
		}
	}
}

void Starfield::Draw()
{
	std::list<Star>::iterator it = m_stars.begin();
	std::list<Star>::iterator end = m_stars.end();

	HDC hdc;
	g_lpDDSBack->GetDC(&hdc);

	while (it != end) {
		Rectangle(hdc, it->x, it->y, it->x + it->size + 2, it->y + it->size + 2);
		++it;
	}

	g_lpDDSBack->ReleaseDC(hdc);
}
