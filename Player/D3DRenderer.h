
// D3DRenderer.h : Direct3D ȭ�� ǥ��
//

#pragma once

#include <stdio.h>
#include <d3d9.h>


HRESULT D3DInitialize(HWND hWnd, ULONG pxWidth, ULONG pxHeight);
HRESULT D3DVideoRender(UINT8* buffer, CRect docRect);
void D3DCleanup();
void D3DReset(int width, int height);


// D3D Renderer Ŭ����
class CD3DRenderer
{
public:
// 	CD3DRenderer();
// 	~CD3DRenderer();

public:
};

