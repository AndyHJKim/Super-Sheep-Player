
// FFmpeg.cpp : 디코딩까지의 과정 - 이후 렌더링
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "FFmpeg.h"
#include "D3DRenderer.h"



// static 변수 초기화
int CFFmpeg::videoWidth = 0;
int CFFmpeg::videoHeight = 0;
CRect CFFmpeg::viewRect = 0;



// CFFmpeg 클래스 생성자
CFFmpeg::CFFmpeg(int type)
	: avFormatCtx(nullptr)
	, avAudioCodecCtx(nullptr)
	, avVideoCodecCtx(nullptr)
	, avAudioStream(nullptr)
	, avVideoStream(nullptr)
	, avFrame(nullptr)
	, m_nAudioStreamIndex(NULL)
	, m_nVideoStreamIndex(NULL)
	, m_pSwrCtx(NULL)
	, m_resampledBuffer(NULL)
	, threadType(type)
	, m_pSwr_buf(NULL)	
{
	m_AudioRender = new AudioRenderer();
	av_register_all();
}



// CFFmpeg 클래스 소멸자
CFFmpeg::~CFFmpeg()
{
	av_packet_unref(&avPacket);
	av_frame_free(&avFrame);
	avcodec_free_context(&avAudioCodecCtx);		// Audio Codec Context 반환
	avcodec_free_context(&avVideoCodecCtx);		// Video Codec Context 반환
	avformat_close_input(&avFormatCtx);			// 열린 스트림 닫기
}



// 미디어 소스 열기
HRESULT CFFmpeg::OpenMediaSource(CString & filePath)
{
	HRESULT hr = S_OK;

	// AVFormatContext 메모리 할당
	if (SUCCEEDED(hr))
	{
		avFormatCtx = avformat_alloc_context();
		if (avFormatCtx == nullptr)
		{
			AfxMessageBox(_T("ERROR: allocating AVFormat context"));
			hr = E_OUTOFMEMORY; // 할당 실패
		}
	}

	// 입력 스트림 하나를 열고 헤더 읽기
	if (SUCCEEDED(hr))
	{
		if (avformat_open_input(&avFormatCtx, (CStringA)filePath, NULL, NULL) < 0)
		{
			AfxMessageBox(_T("ERROR: opening input stream"));
			hr = E_FAIL; // 파일 열기 실패
		}
	}

	// 입력 스트림 정보 받아오기
	if (SUCCEEDED(hr))
	{
		if (avformat_find_stream_info(avFormatCtx, NULL) < 0)
		{
			AfxMessageBox(_T("ERROR: finding stream information"));
			hr = E_FAIL; // 파일 정보 받아오기 실패
		}
	}

	// 오디오 코덱 컨텍스트 받아오기
	if (SUCCEEDED(hr))
	{
		int ret = InitCodecContext(&m_nAudioStreamIndex, &avAudioCodecCtx,
			avFormatCtx, AVMEDIA_TYPE_AUDIO);
		if (ret >= 0)
			avAudioStream = avFormatCtx->streams[m_nAudioStreamIndex];
	}

	// 비디오 코덱 컨텍스트 받아오기
	if (SUCCEEDED(hr))
	{
		int ret = InitCodecContext(&m_nVideoStreamIndex, &avVideoCodecCtx,
			avFormatCtx, AVMEDIA_TYPE_VIDEO);
		if (ret >= 0)
		{
			avVideoStream = avFormatCtx->streams[m_nVideoStreamIndex];

			// 영상 타입 raw video buffer 할당
			videoWidth = avVideoCodecCtx->width;
			videoHeight = avVideoCodecCtx->height;
			pixelFormat = avVideoCodecCtx->pix_fmt;
			int ret = av_image_alloc(videoData, videoLinesize,
				videoWidth, videoHeight, pixelFormat, 1);
			if (ret < 0)
			{
				AfxMessageBox(_T("ERROR: allocating raw video buffer"));
				hr = E_FAIL;
			}
			videoBuffersize = ret;
		}
	}

	// 프레임 할당
	if (SUCCEEDED(hr))
	{
		avFrame = av_frame_alloc();
		if (!avFrame)
		{
			AfxMessageBox(_T("ERROR: allocating frame"));
			hr = E_OUTOFMEMORY; // 할당 실패

		}
	}

	videoWidth = avVideoStream->codec->width;
	videoHeight = avVideoStream->codec->height;
	// Direct3D 초기화
	D3DInitialize(AfxGetMainWnd()->GetSafeHwnd(), videoWidth, videoHeight);

	return hr;
}



// 코덱 컨텍스트 초기화
HRESULT CFFmpeg::InitCodecContext(
	int * streamIdx,
	AVCodecContext ** decoderCtx,
	AVFormatContext * formatCtx,
	enum AVMediaType mediaType)
{
	int				streamIndex;
	AVStream *		stream = nullptr;
	AVCodec *		decoder = nullptr;
	AVDictionary *	options = nullptr;
	HRESULT			hr = S_OK;

	// 입력된 스트림 찾기
	if (SUCCEEDED(hr))
	{
		streamIndex = av_find_best_stream(formatCtx, mediaType, -1, -1, NULL, 0);
		if (streamIndex < 0)
		{
			AfxMessageBox(_T("ERROR: finding stream"));
			hr = E_FAIL; // 스트림 찾기 실패
		}
	}

	// 스트림에 맞는 디코더 찾기
	if (SUCCEEDED(hr))
	{
		stream = formatCtx->streams[streamIndex];
		decoder = avcodec_find_decoder(stream->codecpar->codec_id);
		if (!decoder)
		{
			AfxMessageBox(_T("ERROR: finding decoder"));
			hr = E_FAIL; // 디코더 찾기 실패
		}
	}

	// 디코더가 쓸 코덱 컨텍스트 할당
	if (SUCCEEDED(hr))
	{
		*decoderCtx = avcodec_alloc_context3(decoder);
		if (!*decoderCtx)
		{
			AfxMessageBox(_T("ERROR: allocating codec context"));
			hr = E_OUTOFMEMORY; // 할당 실패
		}
	}

	// 입력 스트림에서 출력 코덱 컨텍스트로 코덱 파라미터를 복사
	if (SUCCEEDED(hr))
	{
		if (avcodec_parameters_to_context(*decoderCtx, stream->codecpar) < 0)
		{
			AfxMessageBox(_T("ERROR: coping codec context"));
			hr = E_FAIL; // 복사 실패
		}
	}

	// 디코더를 쓰기 위한 컨텍스트 초기화
	if (SUCCEEDED(hr))
	{
		if (avcodec_open2(*decoderCtx, decoder, &options) < 0)
		{
			AfxMessageBox(_T("ERROR: initializing codec context"));
			hr = E_FAIL; // 초기화 실패
		}
	}

	// 스트림 인덱스 초기화
	*streamIdx = streamIndex;
	return hr;
}



// 새 디코더 함수
HRESULT CFFmpeg::Decoder()
{
	HRESULT hr = S_OK;

	// 패킷 초기화
	av_init_packet(&avPacket);
	avPacket.data = NULL;
	avPacket.size = 0;

	//DirectSound 초기화
	m_AudioRender->XAudio2Initialize(AfxGetMainWnd()->GetSafeHwnd(), avAudioCodecCtx->channels,
		avAudioCodecCtx->sample_rate);

	
	int64_t pts = 0;

	int ret = -1;
	int gotFrame;

	// 프레임 읽기
	while (av_read_frame(avFormatCtx, &avPacket) >= 0)
	{
		AVPacket originalPacket = avPacket;
		do
		{
			if (threadType == DECODE_AUDIO &&
				avPacket.stream_index == m_nAudioStreamIndex)
			{
				// 오디오 디코딩
				ret = DecodeAudioFrame(&gotFrame, 0, &pts);
			}
			else if (threadType == DECODE_VIDEO &&
				avPacket.stream_index == m_nVideoStreamIndex)
			{
				// 비디오 디코딩
				ret = DecodeVideoFrame(&gotFrame, 0);
			}

			if (ret < 0)
				break;
			avPacket.data += ret;
			avPacket.size -= ret;
		} while (avPacket.size > 0);
		av_packet_unref(&avPacket);
	}

	avPacket.data = NULL;
	avPacket.size = 0;
	do
	{
		if (avPacket.stream_index == m_nAudioStreamIndex)
			DecodeAudioFrame(&gotFrame, 1, &pts);
		else if (avPacket.stream_index == m_nVideoStreamIndex)
			DecodeVideoFrame(&gotFrame, 1);
	} while (gotFrame);

	m_AudioRender->XAudio2Cleanup();

	//av_packet_unref(&avPacket);
	av_frame_free(&avFrame);

	return hr;
}



// 오디오 프레임 디코딩
int CFFmpeg::DecodeAudioFrame(int * gotFrame, int cached, int64_t *pts)
{
	int ret = 0;
	int decoded = avPacket.size;

	// 오디오 프레임 디코딩
	ret = avcodec_decode_audio4(avAudioCodecCtx, avFrame, gotFrame, &avPacket);
	if (ret < 0)
	{
		AfxMessageBox(_T("ERROR: decoding audio frame"));
		return ret;
	}
	decoded = FFMIN(ret, avPacket.size);

	if (*gotFrame)
	{
		if (m_pSwrCtx == NULL) {
			m_pSwrCtx = swr_alloc();
			if (m_pSwrCtx == NULL) {
				AfxMessageBox(_T("swr_alloc error.\n"));
				return -1;
			}
			av_opt_set_int(m_pSwrCtx, "in_channel_layout", avFrame->channel_layout, 0);
			av_opt_set_int(m_pSwrCtx, "out_channel_layout", avFrame->channel_layout, 0);
			av_opt_set_int(m_pSwrCtx, "in_sample_rate", avFrame->sample_rate, 0);
			av_opt_set_int(m_pSwrCtx, "out_sample_rate", avFrame->sample_rate, 0);
			av_opt_set_sample_fmt(m_pSwrCtx, "in_sample_fmt", (AVSampleFormat)avFrame->format, 0);
			av_opt_set_sample_fmt(m_pSwrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
			ret = swr_init(m_pSwrCtx);
			if (ret < 0) {
				AfxMessageBox(_T("swr_init error ret=%08x.\n"));
				return ret;
			}
			int buf_size = avFrame->nb_samples*avFrame->channels * 2; /* the 2 means S16 */
			m_pSwr_buf = new BYTE[buf_size];
			m_swr_buf_len = buf_size;
		}

		ret = swr_convert(m_pSwrCtx, &m_pSwr_buf, avFrame->nb_samples, (const uint8_t**)avFrame->extended_data, avFrame->nb_samples);
		if (ret < 0) {
			AfxMessageBox(_T("swr_convert error ret=%08x.\n"));
			return ret;
		}

		m_AudioRender->XAudio2Render(m_pSwr_buf, m_swr_buf_len);

		
	}

	return decoded;
}



// 비디오 프레임 디코딩
int CFFmpeg::DecodeVideoFrame(int * gotFrame, int cached)
{
	HRESULT hr = S_OK;
	int ret = 0;
	int decoded = avPacket.size;

	// 비디오 프레임 디코딩
	ret = avcodec_decode_video2(avVideoCodecCtx, avFrame, gotFrame, &avPacket);
	if (ret < 0)
	{
		AfxMessageBox(_T("ERROR: decoding video frame"));
		return ret;
	}

	if (*gotFrame)
	{
		if (avFrame->width != videoWidth || avFrame->height != videoHeight ||
			avFrame->format != pixelFormat) {
			AfxMessageBox(_T("ERROR: irregular input video frame size"));
			return -1;
		}

		// 임시 버퍼에 raw video 데이터를 저장
		av_image_copy(videoData, videoLinesize,
			(const uint8_t **)(avFrame->data), avFrame->linesize,
			pixelFormat, videoWidth, videoHeight);

		// 화면 세팅
		AfxGetMainWnd()->GetClientRect(viewRect);
		D3DVideoRender(*videoData, viewRect);

		// 비디오의 fps 계산 - 화면 표시 타이밍에 영향
		double fps = av_q2d(avFormatCtx->streams[m_nVideoStreamIndex]->r_frame_rate) - 0.3;
		Sleep(950 / fps - 1);
	}

	return decoded;
}


