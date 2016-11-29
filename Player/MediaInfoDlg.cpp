
// MediaInfoDlg.cpp : 재생 중인 미디어의 정보를 보여주는 대화상자
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "MediaInfoDlg.h"
#include "afxdialogex.h"


// CMediaInfoDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMediaInfoDlg, CDialogEx)

CMediaInfoDlg::CMediaInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MEDIAINFO, pParent)
{

}

CMediaInfoDlg::~CMediaInfoDlg()
{
}

void CMediaInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMediaInfoDlg, CDialogEx)
END_MESSAGE_MAP()


// CMediaInfoDlg 메시지 처리기입니다.


BOOL CMediaInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	CPlayerDlg * pDlg = (CPlayerDlg *)::AfxGetMainWnd();
	if (pDlg->m_pCFFmpeg != nullptr)
	{
		CString str;

		// 일반 재생 정보
		str = pDlg->m_pCFFmpeg->avFormatCtx->filename;
		GetDlgItem(IDC_EDIT_FULLNAME)->SetWindowTextW(str);
		
		str = pDlg->m_pCFFmpeg->avFormatCtx->iformat->long_name;
		if (str == "QuickTime / MOV")
			str = "MPEG - 4";
		GetDlgItem(IDC_STATIC_CONTAINER)->SetWindowTextW(str);
		
		long dur = pDlg->m_pCFFmpeg->avFormatCtx->duration / AV_TIME_BASE;
		str.Format(_T("%02d:%02d:%02d"), dur/3600, dur/60 % 60, dur % 60);
		GetDlgItem(IDC_STATIC_DURATION)->SetWindowTextW(str);

		// 비디오 재생 정보
		str = pDlg->m_pCFFmpeg->avVideoCodecCtx->codec->next->long_name;
		GetDlgItem(IDC_STATIC_VIDEO_FORMAT)->SetWindowTextW(str);
		
		str = pDlg->m_pCFFmpeg->avVideoCodecCtx->codec->next->name;
		GetDlgItem(IDC_STATIC_VIDEO_INPUT)->SetWindowTextW(str);
		
		str.Format(_T("%d pixel"), pDlg->m_pCFFmpeg->avVideoCodecCtx->width);
		GetDlgItem(IDC_STATIC_VIDEO_WIDTH)->SetWindowTextW(str);
		
		str.Format(_T("%d pixel"), pDlg->m_pCFFmpeg->avVideoCodecCtx->height);
		GetDlgItem(IDC_STATIC_VIDEO_HEIGHT)->SetWindowTextW(str);

		double bitRate = pDlg->m_pCFFmpeg->avVideoCodecCtx->bit_rate;
		if (bitRate == 0)
		{
			bitRate = pDlg->m_pCFFmpeg->avFormatCtx->bit_rate;
			bitRate -= pDlg->m_pCFFmpeg->avAudioCodecCtx->bit_rate;
			str.Format(_T("%0.1f kbps (가변)"), (float)bitRate / 1000);
		}
		else str.Format(_T("%0.1f kbps"), (float)bitRate / 1000);
		GetDlgItem(IDC_STATIC_VIDEO_BITRATE)->SetWindowTextW(str);
		
		str.Format(_T("%.2f FPS"), (float)pDlg->m_pCFFmpeg->avVideoStream->r_frame_rate.num
			/ pDlg->m_pCFFmpeg->avVideoStream->r_frame_rate.den);
		GetDlgItem(IDC_STATIC_VIDEO_FRAMERATE)->SetWindowTextW(str);
		
		switch (pDlg->m_pCFFmpeg->avVideoFrame->colorspace)
		{
		case AVCOL_SPC_RGB:
			str.Format(_T("RGB"));
			break;
		case AVCOL_SPC_BT709:
			str.Format(_T("BT709"));
			break;
		case AVCOL_SPC_FCC:
			str.Format(_T("FCC"));
			break;
		case AVCOL_SPC_BT470BG:
			str.Format(_T("BT470BG"));
			break;
		case AVCOL_SPC_SMPTE170M:
			str.Format(_T("SMPTE170M"));
			break;
		case AVCOL_SPC_SMPTE240M:
			str.Format(_T("SMPTE240M"));
			break;
		case AVCOL_SPC_YCOCG:
			str.Format(_T("RGB"));
			break;
		case AVCOL_SPC_BT2020_NCL:
			str.Format(_T("BT2020 non-constant"));
			break;
		case AVCOL_SPC_BT2020_CL:
			str.Format(_T("BT2020 constant"));
			break;
		default:
			str.Format(_T("YUV"));
		}
		GetDlgItem(IDC_STATIC_VIDEO_COLORSPACE)->SetWindowTextW(str);

		// 오디오 재생 정보
		str = pDlg->m_pCFFmpeg->avAudioCodecCtx->codec->long_name;
		GetDlgItem(IDC_STATIC_AUDIO_FORMAT)->SetWindowTextW(str);
		
		str.Format(_T("%d"), pDlg->m_pCFFmpeg->avAudioCodecCtx->channels);
		GetDlgItem(IDC_STATIC_AUDIO_CHANNEL)->SetWindowTextW(str);
		
		str.Format(_T("%0.1f kbps"), (float)pDlg->m_pCFFmpeg->avAudioCodecCtx->bit_rate / 1000);
		GetDlgItem(IDC_STATIC_AUDIO_BITRATE)->SetWindowTextW(str);
		
		str.Format(_T("%0.1f kHz"), (float)pDlg->m_pCFFmpeg->avAudioCodecCtx->sample_rate / 1000);
		GetDlgItem(IDC_STATIC_AUDIO_SAMPLERATE)->SetWindowTextW(str);

	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
