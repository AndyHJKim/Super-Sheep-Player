
// FFmpeg.h : 디코딩까지의 과정 - 이후 렌더링
//

#pragma once

#pragma warning(disable : 4996)
#pragma warning(disable : 4819)

#include <thread>
#include <mutex>
#include <condition_variable>

// C++에서 FFmpeg
extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include "XAudio2Renderer.h"
#include "D3DRenderer.h"


// 디코더 스레드 타입
#define DECODE_AUDIO 0
#define DECODE_VIDEO 1

#define MAX_AUDIOQ_SIZE 1024 * 16 * 10
#define MAX_VIDEOQ_SIZE 1024 * 256 * 10

#define VIDEO_PICTURE_QUEUE_SIZE 5

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0


typedef struct PacketQueue {
	AVPacketList *first_pkt, *last_pkt;
	int nb_packets;
	int size;
	std::mutex *mutex;
	std::condition_variable *cond;
} PacketQueue;


typedef struct VideoPicture {
	UINT8 *	videoData[4] = { NULL };
	int		videoLinesize[4];

	double pts;
} VideoPicture;



// FFmpeg 클래스
class CFFmpeg
{
public:
// 	CFFmpeg();
	CFFmpeg(const int type);
// 	~CFFmpeg();
	void cleanUp();

	HRESULT OpenMediaSource(CString & filePath);
	HRESULT InitCodecContext(int * streamIdx, AVCodecContext ** decoderCtx, AVFormatContext * formatCtx, enum AVMediaType mediaType);
	int Decoder();

	// 프레임 타입에 따른 디코딩
	int DecodeAudioFrame();
	int DecodeVideoFrame();

	//패킷 큐 관련 함수
	int packet_queue_put(PacketQueue *q, AVPacket *pkt);
	void packet_queue_init(PacketQueue *q);
	int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block);

	//오디오 디코더/비디오 디코더 스레드
	static int threadAudio(CFFmpeg *ffmpeg);
	static int threadVideo(CFFmpeg * ffmpeg);

	//비디오 타이머 갱신
	void video_refresh_timer();

	//오디오/비디오 클럭 관련 함수
	static int get_buffer(struct AVCodecContext *c, AVFrame *pic, int flags);
	double synchronize_video(AVFrame *src_frame, double pts);
	double get_audio_clock();


public:
	AVFormatContext *	avFormatCtx;
	AVCodecContext *	avAudioCodecCtx;
	AVCodecContext *	avVideoCodecCtx;

	AVStream *	avAudioStream;
	AVStream *	avVideoStream;
	AVFrame *	avVideoFrame;
	AVFrame *   avAudioFrame;
	AVPacket	avPacket;

	int m_nAudioStreamIndex;
	int m_nVideoStreamIndex;

	enum AVPixelFormat pixelFormat;

	static int		videoWidth;
	static int		videoHeight;
	static CRect	viewRect;

	SwrContext *	m_pSwrCtx;
	unsigned char *	m_pSwr_buf;
	int				m_swr_buf_len;

	AudioRenderer *	m_pAudio;
	CD3DRenderer *	m_pVideo;

	
	double			audio_clock;
	PacketQueue		audioq;
	AVPacket		audio_pkt;
	uint8_t *		audio_pkt_data;
	int				audio_pkt_size;
	double			frame_timer;
	double			frame_last_pts;
	double			frame_last_delay;
	double			video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame
	PacketQueue		videoq;

	VideoPicture	pictq[VIDEO_PICTURE_QUEUE_SIZE];
	int				pictq_size, pictq_rindex, pictq_windex;
	std::mutex					pictq_mutex;
	std::condition_variable		pictq_cond;

	std::thread audioDecodeThread;
	std::thread videoDecodeThread;

	uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;
	
};

