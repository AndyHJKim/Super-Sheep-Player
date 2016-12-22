
// D3DRenderer.cpp : Direct3D ȭ�� ǥ�� = ���� + �ڸ�
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "D3DRenderer.h"



/////////////////////////////////////////////////////////////////////////////////////////

// D3D �������� ���� CD3DRenderer Ŭ����

/////////////////////////////////////////////////////////////////////////////////////////


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

	m_prevViewport.right = viewRect.right - 1;
	m_prevViewport.bottom = viewRect.bottom - 1;

	/// �ڸ� ����� ���� D3DXFONT �ʱ�ȭ ����
	D3DXCreateFontW(m_pDirect3DDevice, (int)(pxHeight*0.07), 0, FW_DEMIBOLD, 1, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"���� ���", &pFontOut);
	D3DXCreateFontW(m_pDirect3DDevice, (int)(pxHeight*0.07), 0, FW_DEMIBOLD, 1, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"���� ���", &pFont);
	/// �ڸ� ����� ���� D3DXFONT �ʱ�ȭ ��

	return hr;
}



// D3D ������ ����
HRESULT CD3DRenderer::D3DVideoRender(UINT8 * buffer, CRect docRect)
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
	m_dstViewport.right = docRect.Width() - 1;
	m_dstViewport.bottom = docRect.Height() - 1;

	double scrRatio = (double)m_dstViewport.right / m_dstViewport.bottom;

	if (aspRatio > scrRatio)			// ���� �ڽ� ����
	{
		int vHeight = (int)(docRect.Width() / aspRatio);
		m_dstViewport.top = (docRect.Height() - vHeight) / 2;
		m_dstViewport.left = 1;
		m_dstViewport.right = docRect.Width() - 1;
		m_dstViewport.bottom = (docRect.Height() - vHeight) / 2 + vHeight;
	}
	else if (aspRatio <= scrRatio)		// �ʷ� �ڽ� ����
	{
		int vWidth = (int)(docRect.Height() * aspRatio);
		m_dstViewport.top = 1;
		m_dstViewport.left = (docRect.Width() - vWidth) / 2;
		m_dstViewport.right = (docRect.Width() - vWidth) / 2 + vWidth;
		m_dstViewport.bottom = docRect.Height() - 1;
	}

	int textX = (int)(pixelWidth / 2);
	int textY = (int)(pixelHeight - 40);

	// ȭ�鿡 ǥ���ϱ� ���� ���� ����
	IDirect3DSurface9 * pBackBuffer = NULL;
	m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(
		m_pDirect3DSurfaceRender, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR);
	/// �ڸ� ó�� ����
	CPlayerDlg * pDlg = (CPlayerDlg*)::AfxGetMainWnd();
	if (pDlg->eSubtitle == _EXISTS && pDlg->m_pCFFmpeg->currSubIndex != -1)
	{
		SubtitleProcess(textX, textY,
			pDlg->subtitleSet[pDlg->m_pCFFmpeg->currSubIndex]->lpContext,
			pDlg->subtitleSet[pDlg->m_pCFFmpeg->currSubIndex]->strColor);
	}
	/// �ڸ� ó�� ��
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






/////////////////////////////////////////////////////////////////////////////////////////

// �ڸ� ���� �Լ�

/////////////////////////////////////////////////////////////////////////////////////////


// �ڸ� ���Ͽ��� �Ľ��� ���� ������ RGB�� �ٲٴ� ����
// input�� #00ff00 �������� HTML���� ����ϴ� ����
D3DCOLOR CD3DRenderer::ConvertHextoRGB(CString input)
{
	CString strColor;
	int nColor[3];

	for (int i = 0; i < 3; i++)
	{
		// string���� ���� ���� 2�ڸ��� �̾Ƴ�
		strColor = input.Mid(i * 2 + 1, 2);
		// �̾ƿ� ��Ʈ���� 16���� ���̹Ƿ� �̸� 10������ ����
		nColor[i] = (int)wcstol(strColor, NULL, 16);
	}

	D3DCOLOR output = D3DCOLOR_ARGB(255, nColor[0], nColor[1], nColor[2]);
	return output;
}


// �ڸ� ��� ó�� ���� �Լ�
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


// �ڸ� ��� �Լ�
void CD3DRenderer::DrawSubtitle(int x, int y, DWORD color, LPD3DXFONT g_pFont, LPCSTR str)
{
	RECT FontPos = { x, y, x, y };
	g_pFont->DrawTextA(NULL, str, -1, &FontPos, DT_CENTER | DT_VCENTER | DT_NOCLIP, color);
}


