
// D3DRenderer.h : Direct3D 화면 표시
//

#pragma once

#include <stdio.h>
#include <d3d9.h>


HRESULT D3DInitialize(HWND hWnd, ULONG pxWidth, ULONG pxHeight);
HRESULT D3DVideoRender(UINT8* buffer, CRect docRect);
void D3DCleanup();
void D3DReset(int width, int height);


// D3D Renderer 클래스
class CD3DRenderer
{
public:
// 	CD3DRenderer();
// 	~CD3DRenderer();

public:
};

