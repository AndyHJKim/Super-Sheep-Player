
// D3DRenderer.cpp : Direct3D ȭ�� ǥ��
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "D3DRenderer.h"



// D3D ������ ��ü ������
CD3DRenderer::CD3DRenderer()
	: m_pDirect3D9(NULL)
	, m_pDirect3DDevice(NULL)
	, m_pDirect3DSurfaceRender(NULL)
	, bitPerPixel(12)
{

}



// D3D ������ ��ü �Ҹ���
CD3DRenderer::~CD3DRenderer()
{

}



// D3D ��� �� ��ġ �ʱ�ȭ
HRESULT CD3DRenderer::D3DInitialize(HWND hWnd, ULONG pxWidth, ULONG pxHeight, RECT viewRect)
{
	HRESULT hr;

	m_hWnd = hWnd;
	pixelWidth = pxWidth;
	pixelHeight = pxHeight;
	aspRatio = (double)pxWidth / pxHeight;

	InitializeCriticalSection(&m_critial);
	D3DCleanup();

	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
		return E_FAIL;

	// ǥ���� ȭ�� ���� �ɼ� ����
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferWidth = pxWidth;
	d3dpp.BackBufferHeight = pxHeight;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// D3D ����̽� �ʱ�ȭ (�ϵ���� ��� �ִ�ȭ)
	hr = m_pDirect3D9->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pDirect3DDevice);
	if (FAILED(hr))
		return hr;

	// YUV420p(= YV12) ���˿� �´� ������ũ�� ����
	hr = m_pDirect3DDevice->CreateOffscreenPlainSurface(
		pxWidth, pxHeight,
		(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
		D3DPOOL_DEFAULT,
		&m_pDirect3DSurfaceRender,
		NULL);

	m_prevViewport.right = viewRect.right;
	m_prevViewport.bottom = viewRect.bottom;

	return hr;

}



// D3D ������ ����
HRESULT CD3DRenderer::D3DVideoRender(UINT8* buffer, CRect docRect)
{
	HRESULT	hr;

	// �ʱ�ȭ Ȯ��
	if (m_pDirect3DSurfaceRender == NULL)
		return E_FAIL;

	// ǥ���ϱ� ���� ȭ�� ������ ���� ��׶��忡�� ����
	D3DLOCKED_RECT d3dRect;
	hr = m_pDirect3DSurfaceRender->LockRect(&d3dRect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(hr))
		return hr;

	// ���� �ʱ� ����
	byte*	pSrc = buffer;
	byte*	pDest = (byte *)d3dRect.pBits;
	int		stride = d3dRect.Pitch;
	ULONG	i = 0;

	// YUV420p ���˿� �°� �����͸� ���ۿ� ����
	for (i = 0; i < pixelHeight; i++)
	{
		memcpy(pDest + i*stride,
			pSrc + i*pixelWidth,
			pixelWidth);
	}
	for (i = 0; i < pixelHeight / 2; i++)
	{
		memcpy(pDest + stride*pixelHeight + i*stride / 2,
			pSrc + pixelWidth*pixelHeight + pixelWidth*pixelHeight / 4 + i*pixelWidth / 2,
			pixelWidth / 2);
	}
	for (i = 0; i < pixelHeight / 2; i++)
	{
		memcpy(pDest + stride*pixelHeight + stride*pixelHeight / 4 + i*stride / 2,
			pSrc + pixelWidth*pixelHeight + i*pixelWidth / 2,
			pixelWidth / 2);
	}

	// ���� �� ���: ȭ�鿡 ǥ���� �غ� �Ϸ�
	hr = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(hr))
		return hr;

	if (m_pDirect3DDevice == NULL)
		return E_FAIL;

	// ȭ�� ���� ���� ����
	if (m_prevViewport.right != docRect.Width()
		|| m_prevViewport.bottom != docRect.Height())
	{
		m_prevViewport.right = docRect.Width();
		m_prevViewport.bottom = docRect.Height();
	}

	m_dstViewport.right = docRect.Width();
	m_dstViewport.bottom = docRect.Height();

	double scrRatio = (double)m_dstViewport.right / m_dstViewport.bottom;
	if (aspRatio > scrRatio)
	{
		int vHeight = docRect.Width() / aspRatio;
		m_dstViewport.top = (docRect.Height() - vHeight) / 2;
		m_dstViewport.left = 0;
		m_dstViewport.right = docRect.Width();
		m_dstViewport.bottom = (docRect.Height() - vHeight) / 2 + vHeight;
	}
	else if (aspRatio <= scrRatio)
	{
		int vWidth = docRect.Height() * aspRatio;
		m_dstViewport.top = 0;
		m_dstViewport.left = (docRect.Width() - vWidth) / 2;
		m_dstViewport.right = (docRect.Width() - vWidth) / 2 + vWidth;
		m_dstViewport.bottom = docRect.Height();
	}

	// ȭ�鿡 ǥ���ϱ� ���� ���� ����
	IDirect3DSurface9 * pBackBuffer = NULL;
	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(
		m_pDirect3DSurfaceRender, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, &m_dstViewport, NULL, NULL);

	pDest = nullptr;
	buffer = nullptr;

	return hr;
}



// D3D �۾� �� �ڿ� ����
void CD3DRenderer::D3DCleanup()
{
	EnterCriticalSection(&m_critial);
	if (m_pDirect3DSurfaceRender)
		m_pDirect3DSurfaceRender->Release();
	if (m_pDirect3DDevice)
		m_pDirect3DDevice->Release();
	if (m_pDirect3D9)
		m_pDirect3D9->Release();
	LeaveCriticalSection(&m_critial);
}


