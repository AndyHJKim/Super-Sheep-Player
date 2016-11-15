
// FFmpeg.h : ���ڵ������� ���� - ���� ������
//

#pragma once

#pragma warning(disable : 4996)
#pragma warning(disable : 4819)


// C++���� FFmpeg
extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}


// ���ڴ� ������ Ÿ��
#define DECODE_AUDIO 0
#define DECODE_VIDEO 1


// FFmpeg Ŭ����
class CFFmpeg
{
public:
// 	CFFmpeg();
	CFFmpeg(const int type);
	~CFFmpeg();

	HRESULT OpenMediaSource(CString & filePath);
	HRESULT InitCodecContext(int * streamIdx, AVCodecContext ** decoderCtx, AVFormatContext * formatCtx, enum AVMediaType mediaType);
	int Decoder();

	// ������ Ÿ�Կ� ���� ���ڵ�
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

	const int decodeType;

	int m_nAudioStreamIndex;
	int m_nVideoStreamIndex;

	bool audioDecoded;
	bool videoDecoded;

	enum AVPixelFormat pixelFormat;

	UINT8 *	videoData[4] = { NULL };
	int		videoLinesize[4];
	int		videoBuffersize;

	static int videoWidth;
	static int videoHeight;
	static CRect viewRect;

	SwrContext *	m_pSwrCtx;
	unsigned char * m_pSwr_buf;
	int				m_swr_buf_len;

};

