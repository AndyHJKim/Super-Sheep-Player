#include "stdafx.h"
#include "DSoundRenderer.h"

AudioRenderer::AudioRenderer() {
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer = 0;
}

AudioRenderer::~AudioRenderer()
{
}

bool AudioRenderer::DSoundInitialize(HWND hwnd, int channels, int sample_rate, int bits_per_sample)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;
	

	// 다이렉트 사운드 디바이스 생성
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//하나의 장치를 여러 프로그램이 사용하기때문에 협력 레벨 사용
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	//// 첫번째 버퍼 설정
	//bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	//bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	//bufferDesc.dwBufferBytes = 0;
	//bufferDesc.dwReserved = 0;
	//bufferDesc.lpwfxFormat = NULL;
	//bufferDesc.guid3DAlgorithm = GUID_NULL;

	//// 버퍼 생성
	//result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	//// 첫번째 버퍼의 출력 포맷 설정
	//waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	//waveFormat.nSamplesPerSec = sample_rate;
	//waveFormat.wBitsPerSample = bits_per_sample;
	//waveFormat.nChannels = channels;
	//waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;		//		모노 1 스테레오2
	//waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	//waveFormat.cbSize = 0;
	//	 
	//result = m_primaryBuffer->SetFormat(&waveFormat);
	//if (FAILED(result))
	//{
	//	return false;
	//}
	this->channels = channels;
	this->bits_per_sample = bits_per_sample;
	this->sample_rate = sample_rate;

	
	

	return true;
}

int AudioRenderer::DSoundRender(UINT8 *srcBuffer, DWORD bufSize) {
	HRESULT result;
	unsigned char *bufferPtr;
	DSBUFFERDESC bufferDesc;


	long bufferSize;
	unsigned char *bufferPtr2;
	long bufferSize2;
	WAVEFORMATEX waveFormat;
	if (m_secondaryBuffer == nullptr) {
		//두번째 버퍼의 출력 포맷 설정
		IDirectSoundBuffer* tempBuffer;
		

		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = sample_rate;
		waveFormat.wBitsPerSample = bits_per_sample;
		waveFormat.nChannels = channels;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;		//		모노 1 스테레오2
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		//waveFormat.nAvgBytesPerSec = (waveFormat.wBitsPerSample / 8) * waveFormat.nSamplesPerSec;
		waveFormat.cbSize = 0;

		// 두번째 버퍼 세팅
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME;
		//bufferDesc.dwBufferBytes = 3 * waveFormat.nAvgBytesPerSec;
		//bufferDesc.dwBufferBytes = unpadded_linesize * 3;
		bufferDesc.dwBufferBytes = 3*bufSize;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = &waveFormat;
		bufferDesc.guid3DAlgorithm = GUID_NULL;

		// 세컨 버퍼 임시 생성
		result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// 세컨 버퍼 생성
		result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8,
			(void **)&m_secondaryBuffer);
		if (FAILED(result))
		{
			return false;
		}
		// 임시 세컨 버퍼 해제
		tempBuffer->Release();
		tempBuffer = 0;
	}

	result = m_secondaryBuffer->Lock(0, bufSize, (void **)&bufferPtr, (DWORD *)&bufferSize,
		(void **)&bufferPtr2, (DWORD*)&bufferSize2, 0);
	if (FAILED(result))
	{
		return false;
	}
	memset(bufferPtr, 0x00, bufferSize);
	//memcpy_s(bufferPtr, bufferSize, srcBuffer,bufferSize);
	memcpy_s(bufferPtr, bufferSize, srcBuffer, bufferSize);
	
	result = m_secondaryBuffer->Unlock(bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}
	


	//플레이
	// Set position at the beginning of the sound buffer.
	result = m_secondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}

	//	볼륨세팅
	result = m_secondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}
	
	// Play the contents of the secondary sound buffer.
	result = m_secondaryBuffer->Play(0, 0, 0);
	if (FAILED(result))
	{
		return false;
	}


	return true;
	
}