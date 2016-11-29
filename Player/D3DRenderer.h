
// D3DRenderer.h : Direct3D 화면 표시
//

#pragma once

#include <d3dx9core.h>



// 자막 클래스

class cText
{
public:
	~cText(void);

	void Initialize(void);
	void TextPresent(LPCWSTR cStr, int nX = 0, int nY = 0, D3DXCOLOR ARGB = 0xFFFFFFFF);
	static cText * GetInstance(void);
	void FreeInstance(void);

private:
	cText(void);

	static cText * TextInstance;
	ID3DXFont * m_pFont;
	int m_nMax_X;
	int m_nMax_Y;
};



// 자막 존재 여부
enum SUBTITLE_CHECK {
	_EXIST = 1,
	_NONE,
};



// D3D Renderer 클래스
class CD3DRenderer
{
public:
	CD3DRenderer();
	~CD3DRenderer();

	HRESULT D3DInitialize(HWND hWnd, ULONG pxWidth, ULONG pxHeight, RECT viewRect);
	HRESULT D3DVideoRender(UINT8* buffer, CRect docRect);
	void D3DCleanup();
	
	IDirect3DDevice9 * GetDevice() { return m_pDirect3DDevice; }
	RECT GetRect() { return m_dstViewport; }

	enum RENDER_STATE	renderState;
	enum SUBTITLE_CHECK subtitleState;

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

	ID3DXFont *		Font;
	ID3DXSprite *	Sprite;

};


