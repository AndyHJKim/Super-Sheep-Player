
// FFmpeg.cpp : 디코딩까지의 과정 - 이후 렌더링
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "FFmpeg.h"



// static 변수 초기화
int CFFmpeg::videoWidth = 0;
int CFFmpeg::videoHeight = 0;
CRect CFFmpeg::viewRect = 0;



// CFFmpeg 클래스 생성자
CFFmpeg::CFFmpeg(const int type)
	: avFormatCtx(nullptr)
	, avAudioCodecCtx(nullptr)
	, avVideoCodecCtx(nullptr)
	, avAudioStream(nullptr)
	, avVideoStream(nullptr)
	, avFrame(nullptr)
	, m_nAudioStreamIndex(NULL)
	, m_nVideoStreamIndex(NULL)
	, m_pSwrCtx(nullptr)
	, m_pSwr_buf(nullptr)
	, m_swr_buf_len(NULL)
	, decodeType(type)
	, m_pVideo(nullptr)
	, m_pAudio(nullptr)
	, pictq_windex(0)
	, pictq_size(0)
	, pictq_rindex(0)
{
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
			for (int i = 0; i < VIDEO_PICTURE_QUEUE_SIZE; i++) {
				int ret = av_image_alloc(pictq[i].videoData, pictq[i].videoLinesize,
					videoWidth, videoHeight, pixelFormat, 1);
			}
			
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

	AfxGetMainWnd()->GetClientRect(viewRect);
	videoWidth = avVideoStream->codec->width;
	videoHeight = avVideoStream->codec->height;

	/*switch (decodeType) {
	case DECODE_AUDIO:*/
		m_pAudio = new AudioRenderer();
		//XAudio2 초기화
		m_pAudio->XAudio2Initialize(AfxGetMainWnd()->GetSafeHwnd(),
			avAudioCodecCtx->channels, avAudioCodecCtx->sample_rate);
		//break;
	//case DECODE_VIDEO:
		m_pVideo = new CD3DRenderer();
		// Direct3D 초기화
		m_pVideo->D3DInitialize(AfxGetMainWnd()->GetSafeHwnd(),
			videoWidth, videoHeight, viewRect);
		//break;
	//}
	


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

	// 패킷 초기화
	av_init_packet(&avPacket);
	avPacket.data = NULL;
	avPacket.size = 0;

	// 스트림 인덱스 초기화
	*streamIdx = streamIndex;


	packet_queue_init(&audioq);
	packet_queue_init(&videoq);

	return hr;
}



// 한 프레임 디코딩
int CFFmpeg::Decoder()
{
	int proc = -1;

	int64_t pts = 0;
	int ret = -1;
	int gotFrame;


	frame_timer = (double)av_gettime() / 1000000.0;
	frame_last_delay = 40e-3;
	avVideoCodecCtx->get_buffer2 = get_buffer;

	std::thread audioDecodeThread(threadAudio, this);
	//audioDecodeThread.join();
	std::thread videoDecodeThread(threadVideo, this);



	
		// 프레임 읽기
	while (1)
	{
		AVPacket originalPacket = avPacket;
		if (audioq.size > MAX_AUDIOQ_SIZE ) {
			Sleep(10);
			continue;
		}


		//do
		//{
		//	if (avPacket.stream_index == m_nAudioStreamIndex)
		//	{
		//		// 오디오 디코딩
	//			ret = DecodeAudioFrame(&gotFrame, 0, &pts);
		//	}
		//	else if (avPacket.stream_index == m_nVideoStreamIndex)
		//	{
		//		// 비디오 디코딩
		//		ret = DecodeVideoFrame(&gotFrame, 0);
		//	}

		//	if (ret < 0)
		//		break;
		//	avPacket.data += ret;
		//	avPacket.size -= ret;
		//} while (avPacket.size > 0);
		if ((proc = av_read_frame(avFormatCtx, &avPacket)) >= 0) {
			if (avPacket.stream_index == m_nVideoStreamIndex) {
				packet_queue_put(&videoq, &avPacket);
			}
			else if (avPacket.stream_index == m_nAudioStreamIndex) {
				packet_queue_put(&audioq, &avPacket);
			}
			else {
				av_free_packet(&avPacket);
			}
		}
		
		
// 		AVPacket * tmpPck = &avPacket;
// 		av_init_packet(&avPacket);
// 		avPacket.data = NULL;
// 		avPacket.size = 0;
// 		av_packet_unref(tmpPck);
// 
// 		AVFrame * tmpFrm = avFrame;
// 		avFrame = av_frame_alloc();
// 		av_frame_free(&tmpFrm);

// 		av_packet_unref(&avPacket);
// 		av_frame_free(&avFrame);
	}
	return proc;
}



// 오디오 프레임 디코딩
int CFFmpeg::DecodeAudioFrame()
{

	int ret = 0;
	int decoded = avPacket.size;
	int *gotFrame = nullptr;

	audioDecoded = false;

	while (audio_pkt.size > 0)
	{
		//avFrame = av_frame_alloc();
		int ret = 0;
		int decoded = avPacket.size;
		int gotFrame;

		audioDecoded = false;

		/*AVPacket avPacket;
		packet_queue_get(&audioq, &avPacket, 1);
		*/
		// 오디오 프레임 디코딩
		ret = avcodec_decode_audio4(avAudioCodecCtx, avFrame, &gotFrame, &audio_pkt);
		if (ret < 0)
		{
			AfxMessageBox(_T("ERROR: decoding audio frame"));
			return ret;
		}
		decoded = FFMIN(ret, audio_pkt.size);

		if (gotFrame)
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
				m_pSwr_buf = new unsigned char[buf_size];
				m_swr_buf_len = buf_size;
			}

			ret = swr_convert(m_pSwrCtx, &m_pSwr_buf, avFrame->nb_samples, (const uint8_t**)avFrame->extended_data, avFrame->nb_samples);
			if (ret < 0) {
				AfxMessageBox(_T("swr_convert error ret=%08x.\n"));
				return ret;
			}

			m_pAudio->XAudio2Render(m_pSwr_buf, m_swr_buf_len);
		}

		if (ret < 0)
			break;
		audio_pkt.data += ret;
		audio_pkt.size -= ret;

// 		pts = audio_clock;
// 		*pts_ptr = pts;
// 		n = 2 * is->audio_st->codec->channels;
// 		is->audio_clock += (double)data_size /
// 			(double)(n * is->audio_st->codec->sample_rate);

		//av_frame_free(&avFrame);
	}


	/*if (audio_pkt.data)
		av_free_packet(&audio_pkt);*/

	/* next packet */
	if (packet_queue_get(&audioq, &audio_pkt, 1) < 0) {
		return -1;
	}
	audio_pkt_data = audio_pkt.data;
	audio_pkt_size = audio_pkt.size;

	return ret;
}



// 비디오 프레임 디코딩
int CFFmpeg::DecodeVideoFrame()
{
	HRESULT hr = S_OK;
	int ret = 0;
	int decoded = avPacket.size;
	int gotFrame = 0;
	AVPacket video_pkt;
	double pts;

	videoDecoded = false;

	//// 비디오 프레임 디코딩
	//ret = avcodec_decode_video2(avVideoCodecCtx, avFrame, &gotFrame, &avPacket);
	//if (ret < 0)
	//{
	//	AfxMessageBox(_T("ERROR: decoding video frame"));
	//	return ret;
	//}

	//if (gotFrame)
	//{
	//	if (avFrame->width != videoWidth || avFrame->height != videoHeight ||
	//		avFrame->format != pixelFormat) {
	//		AfxMessageBox(_T("ERROR: irregular input video frame size"));
	//		return -1;
	//	}
	//	
	//	// 임시 버퍼에 raw video 데이터를 저장
	//	av_image_copy(videoData, videoLinesize,
	//		(const uint8_t **)(avFrame->data), avFrame->linesize,
	//		pixelFormat, videoWidth, videoHeight);

	//	// 화면 세팅
 //		AfxGetMainWnd()->GetClientRect(viewRect);

	//	m_pVideo->D3DVideoRender(*(videoData), viewRect);

	//	// 비디오의 fps 계산 - 화면 표시 타이밍에 영향
 //		double fps = av_q2d(avFormatCtx->streams[m_nVideoStreamIndex]->r_frame_rate) - 0.5;
 //		Sleep(850 / fps - 1);
	//}
	
	while (1) {
		if (packet_queue_get(&videoq, &video_pkt, 1) < 0) {
			// means we quit getting packets
			break;
		}
		pts = 0;

		global_video_pkt_pts = video_pkt.pts;
		ret = avcodec_decode_video2(avVideoCodecCtx, avFrame, &gotFrame, &video_pkt);
		if (ret < 0)
		{
			AfxMessageBox(_T("ERROR: decoding video frame"));
			return ret;
		}

		if (video_pkt.dts == AV_NOPTS_VALUE
			&& avFrame->opaque && *(uint64_t*)avFrame->opaque != AV_NOPTS_VALUE) {
			pts = *(uint64_t *)avFrame->opaque;
		}
		else if (video_pkt.dts != AV_NOPTS_VALUE) {
			pts = video_pkt.dts;
		}
		else {
			pts = 0;
		}
		pts *= av_q2d(avVideoStream->time_base);



		if (gotFrame)
		{
			pts = synchronize_video(avFrame, pts);
			if (avFrame->width != videoWidth || avFrame->height != videoHeight ||
				avFrame->format != pixelFormat) {
				AfxMessageBox(_T("ERROR: irregular input video frame size"));
				return -1;
			}

			std::unique_lock<std::mutex> lock(pictq_mutex);
			while (pictq_size >= VIDEO_PICTURE_QUEUE_SIZE) {
				pictq_cond.wait(lock);
			}
			//lock.unlock();
			// 임시 버퍼에 raw video 데이터를 저장
			av_image_copy(pictq[pictq_windex].videoData, pictq[pictq_windex].videoLinesize,
				(const uint8_t **)(avFrame->data), avFrame->linesize,
				pixelFormat, videoWidth, videoHeight);
			pictq[pictq_windex].pts = pts;
			//lock.lock();
			if (++pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) {
				pictq_windex = 0;
			}

			pictq_size++;
			lock.unlock();

			//// 화면 세팅
			//AfxGetMainWnd()->GetClientRect(viewRect);

			//m_pVideo->D3DVideoRender(*(videoData), viewRect);

			//// 비디오의 fps 계산 - 화면 표시 타이밍에 영향
			//double fps = av_q2d(avFormatCtx->streams[m_nVideoStreamIndex]->r_frame_rate) - 0.5;
			//Sleep(850 / fps - 1);
		}
		av_free_packet(&video_pkt);
	}
	videoDecoded = true;
	av_free(avFrame);
	return decoded;
}

int CFFmpeg::packet_queue_put(PacketQueue *q, AVPacket *pkt) {

	AVPacketList *pkt1 = nullptr;
	if (av_dup_packet(pkt) < 0) {
		return -1;
	}
	pkt1 = (AVPacketList *)av_malloc(sizeof(AVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;

	
	std::unique_lock<std::mutex> lock(*(q->mutex));

	if (!q->last_pkt)
		q->first_pkt = pkt1;
	else
		q->last_pkt->next = pkt1;
	q->last_pkt = pkt1;
	q->nb_packets++;
	q->size += pkt1->pkt.size;
	q->cond->notify_one();
	//q->mutex->unlock();
	
	return 0;
}

int CFFmpeg::packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
	AVPacketList *pkt1;
	int ret;

	//q->mutex->lock();
	std::unique_lock<std::mutex> lock(*q->mutex);


	for (;;) {

		/*if (global_video_state->quit) {
			ret = -1;
			break;
		}*/

		pkt1 = q->first_pkt;
		if (pkt1) {
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			q->size -= pkt1->pkt.size;
			*pkt = pkt1->pkt;
			av_free(pkt1);
			ret = 1;
			break;
		}
		else if (!block) {
			ret = 0;
			break;
		}
		else {
			//xtime time = { 1, 1000 };
			q->cond->wait(lock);
			//continue;
		}
	}

	return ret;
}

void CFFmpeg::packet_queue_init(PacketQueue *q) {
	memset(q, 0, sizeof(PacketQueue));
	q->mutex = new std::mutex();
	q->cond = new std::condition_variable();
}

int CFFmpeg::threadAudio(CFFmpeg *ffmpeg) {
	while (1) {
		ffmpeg->DecodeAudioFrame();
	}
	return 1;
}

int CFFmpeg::threadVideo(CFFmpeg * ffmpeg) {

	while (1) {
		ffmpeg->DecodeVideoFrame();
	}

	return 1;
}

void CFFmpeg::video_refresh_timer() {

	
	VideoPicture *vp;
	double actual_delay, delay, sync_threshold, ref_clock, diff;

	if (avVideoStream) {
		if (pictq_size == 0) {
			AfxGetMainWnd()->SetTimer(0, 1, NULL);
		}
		else {
			vp = &pictq[pictq_rindex];

			delay = vp->pts - frame_last_pts; /* the pts from last time */
			if (delay <= 0 || delay >= 1.0) {
				/* if incorrect delay, use previous one */
				delay = frame_last_delay;
			}
			/* save for next time */
			frame_last_delay = delay;
			frame_last_pts = vp->pts;

			/* update delay to sync to audio */
			ref_clock = get_audio_clock();
			diff = vp->pts - ref_clock;

			/* Skip or repeat the frame. Take delay into account
			FFPlay still doesn't "know if this is the best guess." */
			sync_threshold = (delay > AV_SYNC_THRESHOLD) ? delay : AV_SYNC_THRESHOLD;
			if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
				if (diff <= -sync_threshold) {
					delay = 0;
				}
				else if (diff >= sync_threshold) {
					delay = 2 * delay;
				}
			}
			frame_timer += delay;
			/* computer the REAL delay */
			actual_delay = frame_timer - (av_gettime() / 1000000.0);
			if (actual_delay < 0.010) {
				/* Really it should skip the picture instead */
				actual_delay = 0.010;
			}
			AfxGetMainWnd()->SetTimer(0, (int)(actual_delay * 1000 + 0.5), NULL);
			/* show the picture! */
			AfxGetMainWnd()->GetClientRect(viewRect);

			m_pVideo->D3DVideoRender(*(videoData), viewRect);

			/* update queue for next picture! */
			if (++pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) {
				pictq_rindex = 0;
			}
			std::unique_lock<std::mutex>lock(pictq_mutex);
			pictq_size--;
			pictq_cond.notify_one();
			lock.unlock();
		}
	}
	else {
		AfxGetMainWnd()->SetTimer(0, 100, NULL);
	}
}


int CFFmpeg::get_buffer(struct AVCodecContext *c, AVFrame *pic, int flag) {
	int ret = avcodec_default_get_buffer2(c, pic, NULL);
	uint64_t *pts = (uint64_t *)av_malloc(sizeof(uint64_t));
	CPlayerDlg * pDlg = (CPlayerDlg *)AfxGetApp()->m_pMainWnd;
	*pts = pDlg->m_pVDecoder->global_video_pkt_pts;
	pic->opaque = pts;
	return ret;
}

double CFFmpeg::synchronize_video(AVFrame *src_frame, double pts) {
	double frame_delay;

	if (pts != 0) {
		/* if we have pts, set video clock to it */
		video_clock = pts;
	}
	else {
		/* if we aren't given a pts, set it to the clock */
		pts = video_clock;
	}
	/* update the video clock */
	frame_delay = av_q2d(avVideoStream->codec->time_base);
	/* if we are repeating a frame, adjust clock accordingly */
	frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
	video_clock += frame_delay;
	return pts;
}

double CFFmpeg::get_audio_clock() {
	double pts;
	int hw_buf_size, bytes_per_sec, n;

	pts = audio_clock; /* maintained in the audio thread */
	hw_buf_size = audio_buf_size - audio_buf_index;
	bytes_per_sec = 0;
	n = avAudioStream->codec->channels * 2;
	if (avAudioStream) {
		bytes_per_sec =avAudioStream->codec->sample_rate * n;
	}
	if (bytes_per_sec) {
		pts -= (double)hw_buf_size / bytes_per_sec;
	}
	return pts;
}