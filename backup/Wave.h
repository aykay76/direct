// Wave.h: interface for the Wave class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVE_H__9B98C97B_1E90_4ED5_B750_AE4633EB3D6B__INCLUDED_)
#define AFX_WAVE_H__9B98C97B_1E90_4ED5_B750_AE4633EB3D6B__INCLUDED_

#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Wave  
{
public:
	Wave();
	virtual ~Wave();

	enum MovementType { MovementStraight, MovementSin, MovementGuard };

	class Baddy
	{
	public:
		Baddy() { }

		double x;
		double y;
		int score;
	};

public:
	void Create(int count);
	void UpdatePosition(DWORD dwLastFrameTime);
	bool Draw();

public:
	int m_nCount;
	double m_dVelocity;
	std::vector<Baddy> m_baddies;
	MovementType m_movement;
};

#endif // !defined(AFX_WAVE_H__9B98C97B_1E90_4ED5_B750_AE4633EB3D6B__INCLUDED_)
