// Static.h: interface for the Static class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATIC_H__E1B79764_DC23_44C6_A19B_CF7CFA9F3BC2__INCLUDED_)
#define AFX_STATIC_H__E1B79764_DC23_44C6_A19B_CF7CFA9F3BC2__INCLUDED_

#include <ddraw.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Static  
{
public:
	Static();
	virtual ~Static();

	bool Initialise(int x, int y, UINT uBitmap, int frames, int width, int height, int framerate = 30);
	bool Update(DWORD dwLastFrameTime);

	int m_x;
	int m_y;
	int m_framerate;
	int m_frames;
	int m_frame;
	int m_width;
	int m_height;
	double m_dFrame;
	IDirectDrawSurface7 *m_lpSurface;
};

#endif // !defined(AFX_STATIC_H__E1B79764_DC23_44C6_A19B_CF7CFA9F3BC2__INCLUDED_)
