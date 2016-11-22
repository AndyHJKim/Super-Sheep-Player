
// D3DRenderer.h : Direct3D 화면 표시
//

#pragma once

#include <d3d9.h>


// D3D Renderer 클래스
class CD3DRenderer
{
public:
	CD3DRenderer();
	~CD3DRenderer();

	HRESULT D3DInitialize(HWND hWnd, ULONG pxWidth, ULONG pxHeight, RECT viewRect);
	HRESULT D3DVideoRender(UINT8* buffer, CRect docRect);
	void D3DCleanup();

	enum RENDER_STATE renderState;

private:
	CRITICAL_SECTION	m_critial;

	IDirect3D9 *		m_pDirect3D9;
	IDirect3DDevice9 *	m_pDirect3DDevice;
	IDirect3DSurface9 *	m_pDirect3DSurfaceRender;

	HWND	m_hWnd;
	RECT	m_dstViewport;
	RECT	m_prevViewport;

	ULONG	pixelWidth;
	ULONG	pixelHeight;

	double	aspRatio;
	const int bitPerPixel;

};

