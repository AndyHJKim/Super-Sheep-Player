/* XAudio2 오디오 렌더러 클래스 */
#ifndef __XAUDIO2Renderer_H__
#define __XAUDIO2Renderer_H__
#include <windows.h>
#include <xaudio2.h>

#pragma warning(disable : 4996)
#pragma warning(disable : 4819)

#pragma comment(lib, "xaudio2.lib")


class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	HANDLE event;
	VoiceCallback() : event(CreateEvent(NULL, FALSE, FALSE, NULL)) {}
	~VoiceCallback() { CloseHandle(event); }
	void STDMETHODCALLTYPE OnStreamEnd() {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 samples) {}
	void STDMETHODCALLTYPE OnBufferEnd(void * context) { SetEvent(event); }
	void STDMETHODCALLTYPE OnBufferStart(void * context) {}
	void STDMETHODCALLTYPE OnLoopEnd(void * context) {}
	void STDMETHODCALLTYPE OnVoiceError(void * context, HRESULT Error) {}
	
};


class AudioRenderer {
public:		//	public 함수


	AudioRenderer();
	~AudioRenderer();
	bool XAudio2Initialize(HWND hwnd, int channels, int sample_rate);

	int XAudio2Render(unsigned char *srcBuffer, DWORD bufSize);
	void XAudio2Pause();
	void XAudio2Resume();
	void XAudio2Cleanup();

private:	//	private 변수
	IXAudio2 *m_pAudio;
	IXAudio2SourceVoice *m_pVoice;
	VoiceCallback callback;

	bool m_first_submit;
	unsigned char** m_dpBuf;
	int m_buf_cnt;

	int channels, sample_rate;
};


#endif

