// Starfield.h: interface for the Starfield class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STARFIELD_H__75D0D54D_E8A6_4825_8953_9B88820BAB8B__INCLUDED_)
#define AFX_STARFIELD_H__75D0D54D_E8A6_4825_8953_9B88820BAB8B__INCLUDED_

#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Starfield  
{
public:
	Starfield();
	virtual ~Starfield();

	struct Star
	{
		double x;
		double y;
		int size;
	};

	void Initialise();
	void UpdatePosition(DWORD dwLastFrameTime);
	void Draw();

	double m_dVelocity;
	std::vector<Star> m_stars;
	int m_i;
};

#endif // !defined(AFX_STARFIELD_H__75D0D54D_E8A6_4825_8953_9B88820BAB8B__INCLUDED_)
