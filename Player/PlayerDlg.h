
// PlayerDlg.h : �÷��̾� ���� ���
//

#pragma once
#include <queue>
#include "FFmpeg.h"
#include "XAudio2Renderer.h"
#include "D3DRenderer.h"

typedef struct AudioQueue {
	unsigned char* audioBuf;
	DWORD bufSize;
};
typedef std::queue<AudioQueue> Audio_Queue;



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

	CFFmpeg *		m_pADecoder;	// FFmpeg ����� ���ڴ�&��Լ� ��ü
	CFFmpeg *		m_pVDecoder;	// FFmpeg ���� ���ڴ�&��Լ� ��ü


	CWinThread *	m_pADThread;	// ����� ���ڴ�&��Լ� ������ ��ü
	CWinThread *	m_pVDThread;	// ���� ���ڴ�&��Լ� ������ ��ü

	CRect		m_rectPrevWindow;	// ������ ���� ��ǥ
	int			m_nWindowded;		// ������ ���� ��Ÿ��
	bool		m_bIsFullScreen;	// ��üȭ�� üũ
	CMenu *		m_dlgMenu;			// ��üȭ�� ��ȯ�� �޴� ��ü ������ ��
	
	CToolBar	m_dlgToolBar;		// ������ �׺���̼� ����
	CRect		m_rectOldSize;		// �׺���̼� ���� ��ġ ����



	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	static	UINT FFmpegDecoderThread(LPVOID _method);
	static	UINT AudioRendererThread(LPVOID _method);
	static	UINT D3DVideoRendererThread(LPVOID _method);

	bool IsFullScreen() { return m_bIsFullScreen; }
	void DrawBlackScreen();

	afx_msg void OnOpenFile();
	afx_msg void OnClose();
	afx_msg void OnQuit();
	afx_msg void OnPlayPause();
	afx_msg void OnFullscreen();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void InitToolbar();
};
