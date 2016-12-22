
// D3DRenderer.cpp : Direct3D 화면 표시 = 영상 + 자막
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "D3DRenderer.h"



/////////////////////////////////////////////////////////////////////////////////////////

// D3D 렌더링을 위한 CD3DRenderer 클래스

/////////////////////////////////////////////////////////////////////////////////////////


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

	m_prevViewport.right = viewRect.right - 1;
	m_prevViewport.bottom = viewRect.bottom - 1;

	/// 자막 출력을 위한 D3DXFONT 초기화 시작
	D3DXCreateFontW(m_pDirect3DDevice, (int)(pxHeight*0.07), 0, FW_DEMIBOLD, 1, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"맑은 고딕", &pFontOut);
	D3DXCreateFontW(m_pDirect3DDevice, (int)(pxHeight*0.07), 0, FW_DEMIBOLD, 1, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"맑은 고딕", &pFont);
	/// 자막 출력을 위한 D3DXFONT 초기화 끝

	return hr;
}



// D3D 렌더링 과정
HRESULT CD3DRenderer::D3DVideoRender(UINT8 * buffer, CRect docRect)
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
	m_dstViewport.right = docRect.Width() - 1;
	m_dstViewport.bottom = docRect.Height() - 1;

	double scrRatio = (double)m_dstViewport.right / m_dstViewport.bottom;

	if (aspRatio > scrRatio)			// 레터 박스 조건
	{
		int vHeight = (int)(docRect.Width() / aspRatio);
		m_dstViewport.top = (docRect.Height() - vHeight) / 2;
		m_dstViewport.left = 1;
		m_dstViewport.right = docRect.Width() - 1;
		m_dstViewport.bottom = (docRect.Height() - vHeight) / 2 + vHeight;
	}
	else if (aspRatio <= scrRatio)		// 필러 박스 조건
	{
		int vWidth = (int)(docRect.Height() * aspRatio);
		m_dstViewport.top = 1;
		m_dstViewport.left = (docRect.Width() - vWidth) / 2;
		m_dstViewport.right = (docRect.Width() - vWidth) / 2 + vWidth;
		m_dstViewport.bottom = docRect.Height() - 1;
	}

	int textX = (int)(pixelWidth / 2);
	int textY = (int)(pixelHeight - 40);

	// 화면에 표시하기 위한 최종 과정
	IDirect3DSurface9 * pBackBuffer = NULL;
	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(
		m_pDirect3DSurfaceRender, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR);
	/// 자막 처리 시작
	CPlayerDlg * pDlg = (CPlayerDlg*)::AfxGetMainWnd();
	if (pDlg->eSubtitle == _EXISTS && pDlg->m_pCFFmpeg->currSubIndex != -1)
	{
		SubtitleProcess(textX, textY,
			pDlg->subtitleSet[pDlg->m_pCFFmpeg->currSubIndex]->lpContext,
			pDlg->subtitleSet[pDlg->m_pCFFmpeg->currSubIndex]->strColor);
	}
	/// 자막 처리 끝
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






/////////////////////////////////////////////////////////////////////////////////////////

// 자막 관련 함수

/////////////////////////////////////////////////////////////////////////////////////////


// 자막 파일에서 파싱한 색상 정보를 RGB로 바꾸는 과정
// input은 #00ff00 형식으로 HTML에서 사용하는 형식
D3DCOLOR CD3DRenderer::ConvertHextoRGB(CString input)
{
	CString strColor;
	int nColor[3];

	for (int i = 0; i < 3; i++)
	{
		// string에서 값을 얻어올 2자리씩 뽑아냄
		strColor = input.Mid(i * 2 + 1, 2);
		// 뽑아온 스트링은 16진수 값이므로 이를 10진수로 변경
		nColor[i] = (int)wcstol(strColor, NULL, 16);
	}

	D3DCOLOR output = D3DCOLOR_ARGB(255, nColor[0], nColor[1], nColor[2]);
	return output;
}


// 자막 출력 처리 관리 함수
void CD3DRenderer::SubtitleProcess(int x, int y, CString * subtitle, CString * color)
{
	if (!subtitle[1].IsEmpty())
	{
		int subColorCount = 1;
		for (int i = 0; !subtitle[i].IsEmpty(); i++)
			subColorCount++;

		for (int i = 0; i < subColorCount; i++)
		{

			int brCount = 0;
			CString str = subtitle[i].Mid(0, subtitle[i].GetLength());
			while (!str.IsEmpty())
			{
				int x = str.Find(L"<br> ");
				if (x >= 0)
				{
					str.Delete(0, x + 5);
					brCount++;
				}
				else
					break;
			}

			int idx = -1;
			CString subtmp = *new CString(subtitle[i]);
			if ((idx = subtmp.Find(L"<br> ")) > 0)
			{
				CStringA strOut = (CStringA)subtmp.Mid(0, idx);
				while (!subtmp.IsEmpty())
				{
					D3DCOLOR colorARGB = ConvertHextoRGB(color[i]);
					int corY = y - (pixelHeight*0.07 + 10) * brCount--;
					DrawSubtitle(x - 2, corY - 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
					DrawSubtitle(x + 2, corY - 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
					DrawSubtitle(x - 2, corY + 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
					DrawSubtitle(x + 2, corY + 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
					DrawSubtitle(x + 2, corY, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
					DrawSubtitle(x - 2, corY, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
					DrawSubtitle(x, corY, colorARGB, pFont, (CStringA)strOut);
					strOut = subtmp.Mid(idx + 5, subtmp.GetLength());
					subtmp.Delete(0, idx + 5);
				}
			}



		}

	}
	else
	{
		int brCount = 0;
		CString str = subtitle[0].Mid(0, subtitle[0].GetLength());
		while (!str.IsEmpty())
		{
			int i = str.Find(L"<br> ");
			if (i >= 0)
			{
				str.Delete(0, i + 5);
				brCount++;
			}
			else
				break;
		}

		int idx = -1;
		CString subtmp = *new CString(subtitle[0]);
		if ((idx = subtmp.Find(L"<br> ")) > 0)
		{
			CStringA strOut = (CStringA)subtmp.Mid(0, idx);
			while (!subtmp.IsEmpty())
			{
				D3DCOLOR colorARGB = ConvertHextoRGB(color[0]);
				int corY = y - (pixelHeight*0.07 + 10) * brCount--;
				DrawSubtitle(x - 2, corY - 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
				DrawSubtitle(x + 2, corY - 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
				DrawSubtitle(x - 2, corY + 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
				DrawSubtitle(x + 2, corY + 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
				DrawSubtitle(x + 2, corY, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
				DrawSubtitle(x - 2, corY, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)strOut);
				DrawSubtitle(x, corY, colorARGB, pFont, (CStringA)strOut);
				strOut = subtmp.Mid(idx + 5, subtmp.GetLength());
				subtmp.Delete(0, idx + 5);
			}
		}
		else
		{
			D3DCOLOR colorARGB = ConvertHextoRGB(color[0]);
			DrawSubtitle(x - 2, y - 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)subtitle[0]);
			DrawSubtitle(x + 2, y - 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)subtitle[0]);
			DrawSubtitle(x - 2, y + 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)subtitle[0]);
			DrawSubtitle(x + 2, y + 2, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)subtitle[0]);
			DrawSubtitle(x + 2, y, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)subtitle[0]);
			DrawSubtitle(x - 2, y, D3DCOLOR_ARGB(90, 0, 0, 0), pFontOut, (CStringA)subtitle[0]);
			DrawSubtitle(x, y, colorARGB, pFont, (CStringA)subtitle[0]);
		}
	}
}


// 자막 출력 함수
void CD3DRenderer::DrawSubtitle(int x, int y, DWORD color, LPD3DXFONT g_pFont, LPCSTR str)
{
	RECT FontPos = { x, y, x, y };
	g_pFont->DrawTextA(NULL, str, -1, &FontPos, DT_CENTER | DT_VCENTER | DT_NOCLIP, color);
}


