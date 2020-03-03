// stdafx.cpp : source file that includes just the standard includes
//	direct.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

HRESULT DDUtil_CopyBitmap(IDirectDrawSurface7* pdds, HBITMAP hbm, int x, int y, int dx, int dy)
{
    HDC           hdcImage;
    HDC           hdc;
    BITMAP        bm;
    DDSURFACEDESC2 ddsd;
    HRESULT       hr;

    if( hbm == NULL || pdds == NULL )
        return E_FAIL;

    // Make sure this surface is restored.
    pdds->Restore();

    // Select bitmap into a memoryDC so we can use it.
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage) {
        OutputDebugString( TEXT("CreateCompatibleDC() failed\n") );
        return E_FAIL;
    }
    SelectObject(hdcImage, hbm);

    // Get size of the bitmap
    GetObject(hbm, sizeof(bm), &bm);    // get size of bitmap
    dx = dx == 0 ? bm.bmWidth  : dx;    // use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;

    // Gt size of surface.
    ddsd.dwSize  = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if (SUCCEEDED(hr = pdds->GetDC(&hdc))) {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    } else {
        OutputDebugString( TEXT("GetDC() failed\n") );
	}

    DeleteDC(hdcImage);

    return hr;
}

