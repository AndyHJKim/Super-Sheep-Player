
// FFmpeg.h : 디코딩까지의 과정 - 이후 렌더링
//

#pragma once

#pragma warning(disable : 4996)
#pragma warning(disable : 4819)


// C++에서 FFmpeg
extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
#include "DSoundRenderer.h"
#pragma comment(lib, "swresample.lib")


// 플레이어 재생 상태
enum RENDER_STATE {
	RENDER_STATE_STARTED = 1,
	RENDER_STATE_STOPPED,
	RENDER_STATE_PAUSED,
	//RENDER_STATE_SHUTDOWN
};


// 스레드 타입
#define DECODE_AUDIO 1
#define DECODE_VIDEO 2


// FFmpeg 클래스
class CFFmpeg
{
public:
	CFFmpeg(int type);
	~CFFmpeg();

	HRESULT OpenMediaSource(CString & filePath);
	HRESULT InitCodecContext(int * streamIdx, AVCodecContext ** decoderCtx, AVFormatContext * formatCtx, enum AVMediaType mediaType);
	HRESULT Decoder();

	// 프레임 타입에 따른 디코딩
	int DecodeAudioFrame(int * gotFrame, int cached, int64_t *pts);
	int DecodeVideoFrame(int * gotFrame, int cached);

public:
	AVFormatContext *	avFormatCtx;
	AVCodecContext *	avAudioCodecCtx;
	AVCodecContext *	avVideoCodecCtx;

	AVStream *	avAudioStream;
	AVStream *	avVideoStream;
	AVFrame *	avFrame;
	AVPacket	avPacket;

	int m_nAudioStreamIndex;
	int m_nVideoStreamIndex;

	enum AVPixelFormat pixelFormat;

	UINT8 *	videoData[4] = { NULL };
	int		videoLinesize[4];
	int		videoBuffersize;

	enum RENDER_STATE renderState;

	static int videoWidth;
	static int videoHeight;
	static CRect viewRect;

	AudioRenderer *m_AudioRender;
	SwrContext *m_pSwrCtx;
	uint8_t	**m_resampledBuffer;
	int m_resampleMaxCount;
	int m_resampleLineSize;

	int threadType;
};

