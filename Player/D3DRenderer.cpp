
// D3DRenderer.cpp : Direct3D 화면 표시
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "D3DRenderer.h"



// D3D 렌더러 객체 생성자
CD3DRenderer::CD3DRenderer()
	: m_pDirect3D9(NULL)
	, m_pDirect3DDevice(NULL)
	, m_pDirect3DSurfaceRender(NULL)
	, bitPerPixel(12)
{

}



// D3D 렌더러 객체 소멸자
CD3DRenderer::~CD3DRenderer()
{

}



// D3D 사용 전 장치 초기화
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

	// 표시할 화면 영역 옵션 지정
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferWidth = pxWidth;
	d3dpp.BackBufferHeight = pxHeight;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// D3D 디바이스 초기화 (하드웨어 사용 최대화)
	hr = m_pDirect3D9->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pDirect3DDevice);
	if (FAILED(hr))
		return hr;

	// YUV420p(= YV12) 포맷에 맞는 오프스크린 설정
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



// D3D 렌더링 과정
HRESULT CD3DRenderer::D3DVideoRender(UINT8* buffer, CRect docRect)
{
	HRESULT	hr;

	// 초기화 확인
	if (m_pDirect3DSurfaceRender == NULL)
		return E_FAIL;

	// 표시하기 위한 화면 세팅을 위해 백그라운드에서 고정
	D3DLOCKED_RECT d3dRect;
	hr = m_pDirect3DSurfaceRender->LockRect(&d3dRect, NULL, D3DLOCK_DONOTWAIT);
	if (FAILED(hr))
		return hr;

	// 버퍼 초기 세팅
	byte*	pSrc = buffer;
	byte*	pDest = (byte *)d3dRect.pBits;
	int		stride = d3dRect.Pitch;
	ULONG	i = 0;

	// YUV420p 포맷에 맞게 데이터를 버퍼에 복사
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

	// 세팅 후 언락: 화면에 표시할 준비 완료
	hr = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(hr))
		return hr;

	if (m_pDirect3DDevice == NULL)
		return E_FAIL;

	// 화면 비율 유지 설정
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

	// 화면에 표시하기 위한 최종 과정
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



// D3D 작업 후 자원 정리
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


