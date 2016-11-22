
// PlayerDlg.h : �÷��̾� ���� ���
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


// CPlayerDlg ��ȭ ����
class CPlayerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CPlayerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;
	

	CWinThread *	m_pPlayThread;

	CRect		m_rectPrevWindow;	// ������ ���� ��ǥ
	int			m_nWindowded;		// ������ ���� ��Ÿ��
	bool		m_bIsFullScreen;	// ��üȭ�� üũ
	CMenu *		m_dlgMenu;			// ��üȭ�� ��ȯ�� �޴� ��ü ������ ��
	
	// ������ �޽��� �� �Լ�
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
	afx_msg void OnQuit();
	afx_msg void OnPlayPause();
	afx_msg void OnFullscreen();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	CFFmpeg *		m_pCFFmpeg;	// FFmpeg ���� ���ڴ�&��Լ� ��ü

	CRect picRect;		// ���� ����
	CRect toolbarRect;	// ���� ����

	GdiplusStartupInput m_GdiplusStartupInput;	// ���ҽ� ��¿� ���
	ULONG_PTR			m_GdiplusToken;			// ���ҽ� ��¿� ���

	CStatic m_sFrame;
	CSliderCtrl m_sliderSeek;
	CSliderCtrl m_sliderVolume;
	CButton m_btnPlay;
	CButton m_btnPause;
	CButton m_btnStop;

	static RENDER_STATE video;
};


// �÷��̾� ��� ����
enum RENDER_STATE {
	RENDER_STATE_STARTED = 1,
	RENDER_STATE_STOPPED,
	RENDER_STATE_PAUSED,
	//RENDER_STATE_SHUTDOWN
};

