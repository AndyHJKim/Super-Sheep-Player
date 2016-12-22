
// PlayerDlg.h : 플레이어 메인 헤더
//

extern "C" {
#include <libavutil/time.h>
}
#pragma once
#include <queue>
#include "FFmpeg.h"
#include "XAudio2Renderer.h"
#include "D3DRenderer.h"
#include "afxwin.h"
#include "afxcmn.h"


// 플레이어 재생 상태
enum RENDER_STATE {
	RENDER_STATE_STARTED = 1,
	RENDER_STATE_STOPPED,
	RENDER_STATE_PAUSED,
	//RENDER_STATE_SHUTDOWN
};


// 자막 파일 유무
enum SUBTITLE_STATE {
	_EXISTS = 1,
	_NONE,
};


// CPlayerDlg 대화 상자
class CPlayerDlg : public CDialogEx
{
// 생성입니다.
public:
	CPlayerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	

	CWinThread *	m_pDecodeThread;

	CRect		m_rectPrevWindow;	// 윈도우 상태 좌표
	int			m_nWindowded;		// 윈도우 상태 스타일
	bool		m_bIsFullScreen;	// 전체화면 체크
	CMenu *		m_dlgMenu;			// 전체화면 전환시 메뉴 객체 저장해 둠
	
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	static	UINT FFmpegDecoderThread(LPVOID _method);

	bool IsFullScreen() { return m_bIsFullScreen; }

	afx_msg void OnOpenFile();
	afx_msg void OnClose();
	afx_msg void OnReport();
	afx_msg void OnQuit();
	afx_msg void OnPlayPause();
	afx_msg void OnFullscreen();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonStop();

	CFFmpeg *		m_pCFFmpeg;	// FFmpeg 비디오 디코더&디먹서 객체

	CRect picRect;		// 영상 영역
	CRect toolbarRect;	// 툴바 영역

	GdiplusStartupInput m_GdiplusStartupInput;	// 리소스 출력에 사용
	ULONG_PTR			m_GdiplusToken;			// 리소스 출력에 사용

	CStatic m_sFrame;
	CSliderCtrl m_sliderSeek;
	CSliderCtrl m_sliderVolume;
	CButton m_btnPlay;
	CButton m_btnPause;
	CButton m_btnStop;

	static RENDER_STATE eVideo;
	clock_t strtTick;
	clock_t currTick;
	clock_t progTick;
	clock_t seekTick;

	double m_dVideoDuration;
	double progTime;
	CStatic m_sPlaytime;
	bool m_bSeek;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//	afx_msg void OnReleasedcaptureSliderSeek(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	/// 자막 데이터가 저장되는 구조체 하나의 단위
	struct ParsedSMI {
		int		nSync;
		CString strColor[10];
		CString lpContext[10];
	};

	/// 자막 세팅 변수 및 함수
	static SUBTITLE_STATE eSubtitle;
	ParsedSMI * subtitleSet[512];
	FILE *		pSubFd;
	int  subtitleCount;

	char * UTF8toANSI(char * pszCode);
	int SetSubtitleData(ParsedSMI * data[], CString subFilePath);

};

