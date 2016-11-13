
// D3DRenderer.h : Direct3D ȭ�� ǥ��
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
public:		//	public �Լ�


	AudioRenderer();
	~AudioRenderer();
	bool DSoundInitialize(HWND hwnd, int channels, int sample_rate, int bits_per_sample);
	
	int DSoundRender(UINT8 *srcBuffer, DWORD buf);
	void DSoundCleanup();

private:	//	private ����
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer;
	int channels, sample_rate, bits_per_sample;
};

#endif