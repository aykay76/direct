// Static.cpp: implementation of the Static class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "direct.h"
#include "Static.h"
#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Static::Static()
{
	m_frame = 0;
	m_dFrame = 0.0;
}

Static::~Static()
{

}

bool Static::Initialise(int x, int y, UINT uBitmap, int frames, int width, int height, int framerate)
{
	DDSURFACEDESC2 ddsd;

	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_framerate = framerate;
	m_frames = frames;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; 
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
	ddsd.dwWidth = width * frames; 
	ddsd.dwHeight = height;
	HRESULT hr = g_lpDD->CreateSurface(&ddsd, &m_lpSurface, NULL); 
	if (hr != DD_OK) { 
		char szMessage[256];
		sprintf(szMessage, "Failed to create surface for rocket: (%08x)", hr);
		MessageBox(GetForegroundWindow(), szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return -1; 
	} 
	// Load the bitmap resource. 
	HBITMAP hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(uBitmap), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hbm == NULL) {
		char szMessage[256];
		sprintf(szMessage, "Failed to load rocket bitmap: (%08x)", hr);
		MessageBox(GetForegroundWindow(), szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return FALSE; 
	}

	DDCOLORKEY ddck;
	ddck.dwColorSpaceHighValue = RGB(0, 0, 0);
	ddck.dwColorSpaceLowValue = RGB(0, 0, 0);
	m_lpSurface->SetColorKey(DDCKEY_SRCBLT, &ddck);

	if (FAILED(hr = DDUtil_CopyBitmap(m_lpSurface, hbm, 0, 0, width * frames, height))) {
		char szMessage[256];
		sprintf(szMessage, "Failed to copy rocket bitmap: (%08x)", hr);
		MessageBox(GetForegroundWindow(), szMessage, "Fatal error", MB_OK | MB_ICONHAND);
		return FALSE; 
	}
	DeleteObject(hbm);

	return true;
}

bool Static::Update(DWORD dwLastFrameTime)
{
	double dFractionOfSecond = dwLastFrameTime / 1000.0;

	m_dFrame += m_framerate * dFractionOfSecond;
	m_frame = (int)m_dFrame;

	if (m_frame == m_frames) {
		return false;
	}

	RECT rc;
	rc.top = 0;
	rc.left = m_width * m_frame;
	rc.bottom = m_height - 1;
	rc.right = m_width * m_frame + m_width - 1;

	char sz[128];
	sprintf(sz, "Blitting frame %d of %d from (%d, %d, %d, %d)\n", m_frame, m_frames, rc.left, rc.top, rc.right, rc.bottom);
	OutputDebugString(sz);

	HRESULT hr;
	hr = g_lpDDSBack->BltFast(m_x, m_y, m_lpSurface, &rc, DDBLTFAST_SRCCOLORKEY);
	if (hr != DD_OK) {
		if (hr == DDERR_SURFACELOST) {
			if (g_lpDD) g_lpDD->RestoreAllSurfaces();
		} else {
			char szMessage[128];
			sprintf(szMessage, "Failed to blit static: %08x @ (%d, %d, %d, %d) -> (%d, %d)\n", hr, rc.left, rc.top, rc.right, rc.bottom, m_x, m_y);
			OutputDebugString(szMessage);
		}
	}

	return true;
}
