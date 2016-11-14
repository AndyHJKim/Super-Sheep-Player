
// PlayerDlg.cpp : �÷��̾� ���� �ڵ�
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPlayerDlg ��ȭ ����



CPlayerDlg::CPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLAYER_DIALOG, pParent)
	, m_pAudio(nullptr)
	, m_pVideo(nullptr)
	, m_pARThread(nullptr)
	, m_pVRThread(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDM_OPEN_FILE, &CPlayerDlg::OnOpenFile)
	ON_COMMAND(IDM_CLOSE, &CPlayerDlg::OnClose)
	ON_COMMAND(IDM_QUIT, &CPlayerDlg::OnQuit)
	ON_COMMAND(IDM_PLAY_PAUSE, &CPlayerDlg::OnPlayPause)
	ON_COMMAND(IDM_FULLSCREEN, &CPlayerDlg::OnFullscreen)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPlayerDlg �޽��� ó����

BOOL CPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ���� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// �޽��� ó��
BOOL CPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
		// Ű �̺�Ʈ
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
			// ESC
		case VK_ESCAPE:
			return TRUE;

			// ENTER : ��ü ȭ��/â��� ���
		case VK_RETURN:
			if (m_bIsFullScreen)
			{
				SetMenu(m_dlgMenu);
				SetWindowLong(this->m_hWnd, GWL_STYLE, m_nWindowded);
				SetWindowPos(GetParent(), m_rectPrevWindow.left, m_rectPrevWindow.top,
					m_rectPrevWindow.right - m_rectPrevWindow.left,
					m_rectPrevWindow.bottom - m_rectPrevWindow.top, NULL);
				m_bIsFullScreen = false;
			}
			else
			{
				m_nWindowded = GetWindowLong(this->m_hWnd, GWL_STYLE);
				GetWindowRect(m_rectPrevWindow);
				SetWindowLong(this->GetSafeHwnd(), GWL_STYLE,
					GetWindowLong(this->m_hWnd, GWL_STYLE) & ~(WS_DLGFRAME/* | WS_OVERLAPPEDWINDOW*/));
				SetWindowPos(GetParent(), 0, 0,
					GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL);
				m_dlgMenu = GetMenu();
				SetMenu(NULL);
				ShowWindow(SW_SHOWMAXIMIZED);
				m_bIsFullScreen = true;
			}
			return TRUE;

			// SPACEBAR : ���/�Ͻ����� ���
		case VK_SPACE:
			if (m_pVideo != NULL)
			{
				switch (m_pVideo->renderState)
				{
				case RENDER_STATE_STARTED:
					m_pVideo->renderState = RENDER_STATE_PAUSED;
					//m_pDThread->SuspendThread();
					m_pARThread->SuspendThread();
					m_pVRThread->SuspendThread();
					break;
				case RENDER_STATE_PAUSED:
					m_pVideo->renderState = RENDER_STATE_STARTED;
					//m_pDThread->ResumeThread();
					m_pARThread->ResumeThread();
					m_pVRThread->ResumeThread();
					break;
				}
			}
			break;

		default:
			break;
		}

	default:
		break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}



// FFmpeg ���ڵ� ������
UINT CPlayerDlg::FFmpegDecoderThread(LPVOID _method)
{
	CFFmpeg * m_pDecoder = (CFFmpeg *)_method;

	m_pDecoder->renderState = RENDER_STATE_STARTED;
	m_pDecoder->Decoder();

	// ��� ��ġ�� ������ ��� �׸���
	CDC * pDC = AfxGetMainWnd()->GetDC();
	AfxGetMainWnd()->GetClientRect(m_pDecoder->viewRect);
	pDC->FillSolidRect(m_pDecoder->viewRect, RGB(0, 0, 0));
	AfxGetMainWnd()->ReleaseDC(pDC);

	return 0;
}



// ������ ��� �׸���
void CPlayerDlg::DrawBlackScreen()
{
	CDC * pDC = AfxGetMainWnd()->GetDC();
	AfxGetMainWnd()->GetClientRect(m_pVideo->viewRect);
	pDC->FillSolidRect(m_pVideo->viewRect, RGB(0, 0, 0));
	AfxGetMainWnd()->ReleaseDC(pDC);
}



// ���� �� ���� ���� �޴�
void CPlayerDlg::OnOpenFile()
{
	HRESULT hr = S_OK;
	CString filePath = nullptr;

	CFileDialog pDlg(
		TRUE,
		_T("��� ������ ����(*.mp4, *.wmv, *.asf, *.avi, *.mkv)"), _T("*.mp4;*.wmv;*.asf;*.avi;*.mkv;"),
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		_T("mp4 ����(*.mp4)|*.mp4|\
			wmv����(*.wmv)|*.wmv|\
			asf ����(*.asf)|*.asf|\
			avi ����(*.avi)|*.avi|\
			mkv ����(*.mkv)|*.mkv|\
			��� ������ ����(*.mp4, *.wmv, *.asf, *.avi, *.mkv)|*.mp4;*.wmv;*.asf;*.avi;*.mkv;|")
		, NULL, NULL, TRUE
	);

	if (pDlg.DoModal() == IDOK)
	{
		filePath = pDlg.GetPathName();

		// �����ִ� �����尡 ������ ó��
		if (m_pARThread != nullptr)
		{
			HANDLE hThread = m_pARThread->m_hThread;
			TerminateThread(hThread, 1);
		}
		if (m_pVRThread != nullptr)
		{
			HANDLE hThread = m_pVRThread->m_hThread;
			TerminateThread(hThread, 1);
		}

		// ��� �ʱ� ������ ��� �׸���
		DrawBlackScreen();

		// �̵�� �ҽ� ���� + �ʱ�ȭ
		m_pAudio = new CFFmpeg(DECODE_AUDIO);
		m_pVideo = new CFFmpeg(DECODE_VIDEO);
		if (FAILED(m_pAudio->OpenMediaSource(filePath)))
			AfxMessageBox(_T("ERROR: OpenMediaSource function call"));
		if (FAILED(m_pVideo->OpenMediaSource(filePath)))
			AfxMessageBox(_T("ERROR: OpenMediaSource function call"));

		// ����� ������ ������
		m_pARThread = AfxBeginThread(FFmpegDecoderThread, m_pAudio);

		// ���� ������ ������
		m_pVRThread = AfxBeginThread(FFmpegDecoderThread, m_pVideo);
	}

}



// ���� �� ���� �ݱ� �޴�
void CPlayerDlg::OnClose()
{
	if (m_pARThread != nullptr)
	{
		HANDLE hThread = m_pARThread->m_hThread;
		TerminateThread(hThread, 1);
	}
	if (m_pVRThread != nullptr)
	{
		HANDLE hThread = m_pVRThread->m_hThread;
		TerminateThread(hThread, 1);
	}
	if (m_pAudio != nullptr)
	{
		m_pAudio->~CFFmpeg();
	}
	if (m_pVideo != nullptr)
	{
		m_pVideo->~CFFmpeg();
	}
	DrawBlackScreen();
}



// ���� �� ���� �޴�
void CPlayerDlg::OnQuit()
{
	PostQuitMessage(WM_QUIT);
}



// ��� �� ���/�Ͻ����� �޴�
void CPlayerDlg::OnPlayPause()
{
	PostMessage(WM_KEYDOWN, VK_SPACE);
}



// ȭ��ũ�� �� ��ü ȭ�� �޴�
void CPlayerDlg::OnFullscreen()
{
	PostMessage(WM_KEYDOWN, VK_RETURN);
}



// ȭ�� ũ�� ������ ���͹ڽ� ó��
void CPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_pVRThread != nullptr)
	{
		m_pVRThread->SuspendThread();
		DrawBlackScreen();
		m_pVRThread->ResumeThread();
	}
}


