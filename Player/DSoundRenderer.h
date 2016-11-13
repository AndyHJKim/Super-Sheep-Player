
// D3DRenderer.h : Direct3D 화면 표시
//
#ifndef __DSoundRenderer_H_
#define __DSoundRenderer_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <mutex>
#include <queue>





class AudioRenderer {
public:		//	public 함수


	AudioRenderer();
	~AudioRenderer();
	bool DSoundInitialize(HWND hwnd, int channels, int sample_rate, int bits_per_sample);
	
	int DSoundRender(UINT8 *srcBuffer, DWORD buf);
	void DSoundCleanup();

private:	//	private 변수
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer;
	int channels, sample_rate, bits_per_sample;
};

#endif