
// PlayerDlg.h : �÷��̾� ���� ���
//

#pragma once

#include "FFmpeg.h"
#include "D3DRenderer.h"


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

	//CFFmpeg *		m_pDecoder;		// FFmpeg ���ڴ�&��Լ� ��ü
	CFFmpeg *		m_pAudio;		// FFmpeg ����� ���ڵ� ��ü
	CFFmpeg *		m_pVideo;		// FFmpeg ���� ���ڵ� ��ü

//	CWinThread *	m_pDThread;		// ���ڴ�&��Լ� ������ ��ü
	CWinThread *	m_pARThread;	// ����� ������ ������ ��ü
	CWinThread *	m_pVRThread;	// ���� ������ ������ ��ü

	CRect	m_rectPrevWindow;		// ������ ���� ��ǥ
	int		m_nWindowded;			// ������ ���� ��Ÿ��
	bool	m_bIsFullScreen;		// ��üȭ�� üũ
	CMenu *	m_dlgMenu;				// ��üȭ�� ��ȯ�� �޴� ��ü ������ ��

	// ������ �޽��� �� �Լ�
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
