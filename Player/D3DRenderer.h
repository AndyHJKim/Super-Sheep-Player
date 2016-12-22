
// D3DRenderer.h : Direct3D ȭ�� ǥ�� = ���� + �ڸ�
//

#pragma once

#include <d3dx9core.h>


// D3D Renderer Ŭ���� > ȭ�� ��ü ��� �� ���� ��� ���
class CD3DRenderer
{
public:
	CD3DRenderer();
	~CD3DRenderer();

	HRESULT D3DInitialize(HWND hWnd, ULONG pxWidth, ULONG pxHeight, RECT viewRect);
	HRESULT D3DVideoRender(UINT8 * buffer, CRect docRect);
	void D3DCleanup();

	// �ڸ� ó��
	D3DCOLOR ConvertHextoRGB(CString input);
	void SubtitleProcess(int x, int y, CString * subtitle, CString * color);
	void DrawSubtitle(int x, int y, DWORD color, LPD3DXFONT g_pFont, LPCSTR str);

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

	double		aspRatio;
	const int	bitPerPixel;

	// �ڸ� ����� ���� ����
	LPD3DXFONT pFontOut;
	LPD3DXFONT pFont;

};

