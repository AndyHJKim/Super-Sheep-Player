#include "stdafx.h"
#include "XAudio2Renderer.h"


AudioRenderer::AudioRenderer() {
	m_pAudio = NULL;
	m_pVoice = NULL;
	
	m_dpBuf = NULL;
	m_first_submit = true;
	m_buf_cnt = 0;
}

AudioRenderer::~AudioRenderer()
{
}

bool AudioRenderer::XAudio2Initialize(HWND hwnd, int channels, int sample_rate) {
	HRESULT ret;

	ret = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(ret))
	{
		return false;
	}

	ret = XAudio2Create(&m_pAudio);
	if (FAILED(ret))
	{
		return false;
	}

	IXAudio2MasteringVoice *master = NULL;
	ret = m_pAudio->CreateMasteringVoice(&master);
	if (FAILED(ret))
	{
		return false;
	}
	

	this->channels = channels;
	this->sample_rate = sample_rate;

	return true;
}

int AudioRenderer::XAudio2Render(unsigned char *srcBuffer, DWORD bufSize) {
	int ret;

	if (m_pVoice == NULL) {
		WAVEFORMATEX format = { 0 };
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = channels;
		format.wBitsPerSample = 16;
		format.nSamplesPerSec = sample_rate;
		format.nBlockAlign = format.wBitsPerSample / 8 * format.nChannels;
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
		ret = m_pAudio->CreateSourceVoice(
			&m_pVoice,
			&format,
			0,                          
			XAUDIO2_DEFAULT_FREQ_RATIO,
			&callback                  
			);
		if (FAILED(ret)) {
			printf("error CreateSourceVoice ret=%d\n", ret);
		}
		m_pVoice->Start();
	}
	
	if (m_dpBuf == NULL) {
		m_dpBuf = new unsigned char*[2];
		m_dpBuf[0] = new unsigned char[bufSize];
		m_dpBuf[1] = new unsigned char[bufSize];
	}

	memcpy(m_dpBuf[m_buf_cnt], srcBuffer, bufSize);

	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = bufSize;
	buffer.pAudioData = m_dpBuf[m_buf_cnt];
	ret = m_pVoice->SubmitSourceBuffer(&buffer);

	if (FAILED(ret)) {
		printf("error SubmitSourceBuffer ret=%d\n", ret);
	}
	
	if (m_first_submit) {
		m_first_submit = false;
	}
	else {
		if (WaitForSingleObject(callback.event, INFINITE) != WAIT_OBJECT_0) {
			printf("error WaitForSingleObject\n");
		}
	}
	if (2 <= ++m_buf_cnt)
		m_buf_cnt = 0;

	return true;
}

void AudioRenderer::XAudio2Cleanup() {
	m_pVoice->Stop();
	CoUninitialize();
}

void AudioRenderer::XAudio2Pause() {
	m_pVoice->Stop();
}

void AudioRenderer::XAudio2Resume() {
	m_pVoice->Start();
}
