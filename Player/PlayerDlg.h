
// PlayerDlg.h : 플레이어 메인 헤더
//

#pragma once

#include "FFmpeg.h"
#include "D3DRenderer.h"


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

	//CFFmpeg *		m_pDecoder;		// FFmpeg 디코더&디먹서 객체
	CFFmpeg *		m_pAudio;		// FFmpeg 오디오 디코딩 객체
	CFFmpeg *		m_pVideo;		// FFmpeg 비디오 디코딩 객체

//	CWinThread *	m_pDThread;		// 디코더&디먹서 스레드 객체
	CWinThread *	m_pARThread;	// 오디오 렌더러 스레드 객체
	CWinThread *	m_pVRThread;	// 비디오 렌더러 스레드 객체

	CRect	m_rectPrevWindow;		// 윈도우 상태 좌표
	int		m_nWindowded;			// 윈도우 상태 스타일
	bool	m_bIsFullScreen;		// 전체화면 체크
	CMenu *	m_dlgMenu;				// 전체화면 전환시 메뉴 객체 저장해 둠

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	static	UINT FFmpegDecoderThread(LPVOID _method);

	void DrawBlackScreen();

	afx_msg void OnOpenFile();
	afx_msg void OnClose();
	afx_msg void OnQuit();
	afx_msg void OnPlayPause();
	afx_msg void OnFullscreen();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
