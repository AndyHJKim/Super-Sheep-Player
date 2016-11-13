
// FFmpeg.cpp : ���ڵ������� ���� - ���� ������
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "FFmpeg.h"
#include "D3DRenderer.h"



// static ���� �ʱ�ȭ
int CFFmpeg::videoWidth = 0;
int CFFmpeg::videoHeight = 0;
CRect CFFmpeg::viewRect = 0;



// CFFmpeg Ŭ���� ������
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
{
	m_AudioRender = new AudioRenderer();
	av_register_all();
}



// CFFmpeg Ŭ���� �Ҹ���
CFFmpeg::~CFFmpeg()
{
	av_packet_unref(&avPacket);
	av_frame_free(&avFrame);
	avcodec_free_context(&avAudioCodecCtx);		// Audio Codec Context ��ȯ
	avcodec_free_context(&avVideoCodecCtx);		// Video Codec Context ��ȯ
	avformat_close_input(&avFormatCtx);			// ���� ��Ʈ�� �ݱ�
}



// �̵�� �ҽ� ����
HRESULT CFFmpeg::OpenMediaSource(CString & filePath)
{
	HRESULT hr = S_OK;

	// AVFormatContext �޸� �Ҵ�
	if (SUCCEEDED(hr))
	{
		avFormatCtx = avformat_alloc_context();
		if (avFormatCtx == nullptr)
		{
			AfxMessageBox(_T("ERROR: allocating AVFormat context"));
			hr = E_OUTOFMEMORY; // �Ҵ� ����
		}
	}

	// �Է� ��Ʈ�� �ϳ��� ���� ��� �б�
	if (SUCCEEDED(hr))
	{
		if (avformat_open_input(&avFormatCtx, (CStringA)filePath, NULL, NULL) < 0)
		{
			AfxMessageBox(_T("ERROR: opening input stream"));
			hr = E_FAIL; // ���� ���� ����
		}
	}

	// �Է� ��Ʈ�� ���� �޾ƿ���
	if (SUCCEEDED(hr))
	{
		if (avformat_find_stream_info(avFormatCtx, NULL) < 0)
		{
			AfxMessageBox(_T("ERROR: finding stream information"));
			hr = E_FAIL; // ���� ���� �޾ƿ��� ����
		}
	}

	// ����� �ڵ� ���ؽ�Ʈ �޾ƿ���
	if (SUCCEEDED(hr))
	{
		int ret = InitCodecContext(&m_nAudioStreamIndex, &avAudioCodecCtx,
			avFormatCtx, AVMEDIA_TYPE_AUDIO);
		if (ret >= 0)
			avAudioStream = avFormatCtx->streams[m_nAudioStreamIndex];
	}

	// ���� �ڵ� ���ؽ�Ʈ �޾ƿ���
	if (SUCCEEDED(hr))
	{
		int ret = InitCodecContext(&m_nVideoStreamIndex, &avVideoCodecCtx,
			avFormatCtx, AVMEDIA_TYPE_VIDEO);
		if (ret >= 0)
		{
			avVideoStream = avFormatCtx->streams[m_nVideoStreamIndex];

			// ���� Ÿ�� raw video buffer �Ҵ�
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

	// ������ �Ҵ�
	if (SUCCEEDED(hr))
	{
		avFrame = av_frame_alloc();
		if (!avFrame)
		{
			AfxMessageBox(_T("ERROR: allocating frame"));
			hr = E_OUTOFMEMORY; // �Ҵ� ����

		}
	}

	videoWidth = avVideoStream->codec->width;
	videoHeight = avVideoStream->codec->height;
	// Direct3D �ʱ�ȭ
	D3DInitialize(AfxGetMainWnd()->GetSafeHwnd(), videoWidth, videoHeight);

	return hr;
}



// �ڵ� ���ؽ�Ʈ �ʱ�ȭ
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

	// �Էµ� ��Ʈ�� ã��
	if (SUCCEEDED(hr))
	{
		streamIndex = av_find_best_stream(formatCtx, mediaType, -1, -1, NULL, 0);
		if (streamIndex < 0)
		{
			AfxMessageBox(_T("ERROR: finding stream"));
			hr = E_FAIL; // ��Ʈ�� ã�� ����
		}
	}

	// ��Ʈ���� �´� ���ڴ� ã��
	if (SUCCEEDED(hr))
	{
		stream = formatCtx->streams[streamIndex];
		decoder = avcodec_find_decoder(stream->codecpar->codec_id);
		if (!decoder)
		{
			AfxMessageBox(_T("ERROR: finding decoder"));
			hr = E_FAIL; // ���ڴ� ã�� ����
		}
	}

	// ���ڴ��� �� �ڵ� ���ؽ�Ʈ �Ҵ�
	if (SUCCEEDED(hr))
	{
		*decoderCtx = avcodec_alloc_context3(decoder);
		if (!*decoderCtx)
		{
			AfxMessageBox(_T("ERROR: allocating codec context"));
			hr = E_OUTOFMEMORY; // �Ҵ� ����
		}
	}

	// �Է� ��Ʈ������ ��� �ڵ� ���ؽ�Ʈ�� �ڵ� �Ķ���͸� ����
	if (SUCCEEDED(hr))
	{
		if (avcodec_parameters_to_context(*decoderCtx, stream->codecpar) < 0)
		{
			AfxMessageBox(_T("ERROR: coping codec context"));
			hr = E_FAIL; // ���� ����
		}
	}

	// ���ڴ��� ���� ���� ���ؽ�Ʈ �ʱ�ȭ
	if (SUCCEEDED(hr))
	{
		if (avcodec_open2(*decoderCtx, decoder, &options) < 0)
		{
			AfxMessageBox(_T("ERROR: initializing codec context"));
			hr = E_FAIL; // �ʱ�ȭ ����
		}
	}

	// ��Ʈ�� �ε��� �ʱ�ȭ
	*streamIdx = streamIndex;
	return hr;
}



// �� ���ڴ� �Լ�
HRESULT CFFmpeg::Decoder()
{
	HRESULT hr = S_OK;

	// ��Ŷ �ʱ�ȭ
	av_init_packet(&avPacket);
	avPacket.data = NULL;
	avPacket.size = 0;

	//DirectSound �ʱ�ȭ
	m_AudioRender->DSoundInitialize(AfxGetMainWnd()->GetSafeHwnd(), avAudioCodecCtx->channels,
		avAudioCodecCtx->sample_rate, avAudioCodecCtx->bits_per_coded_sample);

	m_pSwrCtx = swr_alloc();
	av_opt_set_int(m_pSwrCtx, "in_channel_layout", avAudioCodecCtx->channel_layout, 0);
	av_opt_set_int(m_pSwrCtx, "in_sample_rate", avAudioCodecCtx->sample_rate, 0);
	av_opt_set_sample_fmt(m_pSwrCtx, "in_sample_fmt", avAudioCodecCtx->sample_fmt, 0);
	av_opt_set_int(m_pSwrCtx, "out_channel_layout", avAudioCodecCtx->channel_layout, 0);
	av_opt_set_int(m_pSwrCtx, "out_sample_rate", avAudioCodecCtx->sample_rate, 0);
	av_opt_set_sample_fmt(m_pSwrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
	swr_init(m_pSwrCtx);


	int64_t pts = 0;

	int ret = -1;
	int gotFrame;

	// ������ �б�
	while (av_read_frame(avFormatCtx, &avPacket) >= 0)
	{
		AVPacket originalPacket = avPacket;
		do
		{
			if (threadType == DECODE_AUDIO &&
				avPacket.stream_index == m_nAudioStreamIndex)
			{
				// ����� ���ڵ�
				ret = DecodeAudioFrame(&gotFrame, 0, &pts);
			}
			else if (threadType == DECODE_VIDEO &&
				avPacket.stream_index == m_nVideoStreamIndex)
			{
				// ���� ���ڵ�
				ret = DecodeVideoFrame(&gotFrame, 0);
			}

			if (ret < 0)
				break;
			avPacket.data += ret;
			avPacket.size -= ret;
		} while (avPacket.size > 0);
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

	av_packet_unref(&avPacket);
	av_frame_free(&avFrame);

	return hr;
}



// ����� ������ ���ڵ�
int CFFmpeg::DecodeAudioFrame(int * gotFrame, int cached, int64_t *pts)
{
	int ret = 0;
	int decoded = avPacket.size;

	// ����� ������ ���ڵ�
	ret = avcodec_decode_audio4(avAudioCodecCtx, avFrame, gotFrame, &avPacket);
	if (ret < 0)
	{
		AfxMessageBox(_T("ERROR: decoding audio frame"));
		return ret;
	}
	decoded = FFMIN(ret, avPacket.size);

	if (*gotFrame)
	{
		int numChannels = av_get_channel_layout_nb_channels(avAudioCodecCtx->channel_layout);
		// ��ȯ �� ���� �� ���.
		int numSamples = av_rescale_rnd(
			swr_get_delay(m_pSwrCtx, avAudioCodecCtx->sample_rate) + avFrame->nb_samples,
			avAudioCodecCtx->sample_rate, avAudioCodecCtx->sample_rate, AV_ROUND_UP);

		if (m_resampledBuffer)
		{
			if (m_resampleMaxCount < numSamples)
			{
				av_freep(m_resampledBuffer[0]);
				av_samples_alloc(m_resampledBuffer, &m_resampleLineSize, numChannels,
					numSamples, AV_SAMPLE_FMT_S16, 1);
				m_resampleMaxCount = numSamples;
			}
		}
		else
		{
			av_samples_alloc_array_and_samples(&m_resampledBuffer, &m_resampleLineSize,
				numChannels, numSamples, AV_SAMPLE_FMT_S16, 1);
			m_resampleMaxCount = numSamples;
		}

		*pts = swr_next_pts(m_pSwrCtx, *pts);
		int ret = swr_convert(m_pSwrCtx, m_resampledBuffer, numSamples, (const uint8_t**)avFrame->extended_data, avFrame->nb_samples);
		//length = double(ret) / double(this->frequency);

		int bufSize = av_samples_get_buffer_size(&m_resampleLineSize, numChannels, ret,
			AV_SAMPLE_FMT_S16, 1);
		if (bufSize > 0)
			m_AudioRender->DSoundRender(m_resampledBuffer[0], bufSize);

		


// 		size_t unpadded_linesize =
// 			avFrame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)avFrame->format);

		// raw audio �����͸� ������
		/* Write the raw audio data samples of the first plane. */
		//fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);

		// ������� fps ��� - ����� ��� Ÿ�ֿ̹� ����
		//double fps = av_q2d(avFormatCtx->streams[m_nAudioStreamIndex]->r_frame_rate) / 100000;
		double fps = av_q2d(avFormatCtx->streams[m_nVideoStreamIndex]->r_frame_rate) - 0.3;
		Sleep(700 / fps - 1);
	}

	return decoded;
}



// ���� ������ ���ڵ�
int CFFmpeg::DecodeVideoFrame(int * gotFrame, int cached)
{
	HRESULT hr = S_OK;
	int ret = 0;
	int decoded = avPacket.size;

	// ���� ������ ���ڵ�
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

		// �ӽ� ���ۿ� raw video �����͸� ����
		av_image_copy(videoData, videoLinesize,
			(const uint8_t **)(avFrame->data), avFrame->linesize,
			pixelFormat, videoWidth, videoHeight);

		// ȭ�� ����
		AfxGetMainWnd()->GetClientRect(viewRect);
		D3DVideoRender(*videoData, viewRect);

		// ������ fps ��� - ȭ�� ǥ�� Ÿ�ֿ̹� ����
		double fps = av_q2d(avFormatCtx->streams[m_nVideoStreamIndex]->r_frame_rate) - 0.3;
		Sleep(700 / fps - 1);
	}

	return decoded;
}


