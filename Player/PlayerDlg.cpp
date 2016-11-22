
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

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_sFrame);
	DDX_Control(pDX, IDC_SLIDER_SEEK, m_sliderSeek);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_sliderVolume);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_btnPause);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
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
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CPlayerDlg �޽��� ó����

BOOL CPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	

	// �ʱ�ȭ �۾�
	MoveWindow(NULL, NULL, 976, 599);
	GdiplusStartup(&m_GdiplusToken, &m_GdiplusStartupInput, NULL);



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

	CDC * pDC = AfxGetMainWnd()->GetDC();
	pDC->FillSolidRect(picRect, RGB(0, 0, 0));

	Graphics graphics(this->m_hWnd);
	Image image(L"res\\SuperSheepFlying.png");
	graphics.DrawImage(&image, (picRect.right / 2 - 128), (picRect.bottom / 2 - 128));

	pDC->FillSolidRect(toolbarRect, RGB(240, 240, 240));
	AfxGetMainWnd()->ReleaseDC(pDC);

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
				m_bIsFullScreen = false;
				SetMenu(m_dlgMenu);
				SetWindowLong(this->m_hWnd, GWL_STYLE, m_nWindowded);
				SetWindowPos(GetParent(), m_rectPrevWindow.left, m_rectPrevWindow.top,
					m_rectPrevWindow.right - m_rectPrevWindow.left,
					m_rectPrevWindow.bottom - m_rectPrevWindow.top, NULL);
			}
			else
			{
				m_bIsFullScreen = true;
				m_nWindowded = GetWindowLong(this->m_hWnd, GWL_STYLE);
				GetWindowRect(m_rectPrevWindow);
				SetWindowLong(this->GetSafeHwnd(), GWL_STYLE,
					GetWindowLong(this->m_hWnd, GWL_STYLE) & ~(WS_DLGFRAME | WS_OVERLAPPEDWINDOW));
				m_dlgMenu = GetMenu();
				SetMenu(NULL);
				ShowWindow(SW_SHOWMAXIMIZED);
			}
		
			return TRUE;

		// SPACEBAR : ���/�Ͻ����� ���
		case VK_SPACE:
			if (m_pVDecoder->m_pVideo != NULL)
			{
				switch (m_pVDecoder->m_pVideo->renderState)
				{
				case RENDER_STATE_STARTED:
 					m_pVDecoder->m_pVideo->renderState = RENDER_STATE_PAUSED;
					m_pADecoder->m_pAudio->XAudio2Pause();
					m_pADThread->SuspendThread();
					m_pVDThread->SuspendThread();
					break;
				case RENDER_STATE_PAUSED:
 					m_pVDecoder->m_pVideo->renderState = RENDER_STATE_STARTED;
					m_pADThread->ResumeThread();
					m_pVDThread->ResumeThread();
					m_pADecoder->m_pAudio->XAudio2Resume();
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
	CPlayerDlg * pDlg = (CPlayerDlg *)AfxGetApp()->m_pMainWnd;

	while (m_pDecoder->Decoder() >= 0)
	{

	}
	pDlg->Invalidate();
	
	return 0;
}

UINT CPlayerDlg::FFmpegAudioDecodeThread(LPVOID _method) {
	CFFmpeg * m_pDecoder = (CFFmpeg *)_method;
	//while (m_pDecoder->DecodeAudioFrame()>=0)
	//{
	m_pDecoder->DecodeAudioFrame();
	//}

	return 0;
}


// ������ ��� �׸���
void CPlayerDlg::DrawBlackScreen()
{
	CDC * pDC = AfxGetMainWnd()->GetDC();
 	AfxGetMainWnd()->GetClientRect(m_pVDecoder->viewRect);
	pDC->FillSolidRect(m_pVDecoder->viewRect, RGB(0, 0, 0));
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

		// �̵�� �ҽ� ���� + �ʱ�ȭ
		OnClose();
		//m_pADecoder	= new CFFmpeg(DECODE_AUDIO);
		m_pVDecoder	= new CFFmpeg(DECODE_VIDEO);
		

		/*	if (FAILED(m_pADecoder->OpenMediaSource(filePath)))
				AfxMessageBox(_T("ERROR: OpenMediaSource function call"));*/
		if (FAILED(m_pVDecoder->OpenMediaSource(filePath)))
			AfxMessageBox(_T("ERROR: OpenMediaSource function call"));

		

		// Direct3D �ʱ�ȭ
//		m_pVDecoder->m_pVideo->renderState = RENDER_STATE_STARTED;
		
		// ������ ����
		//m_pADThread = AfxBeginThread(FFmpegDecoderThread, m_pADecoder);
		m_pVDThread = AfxBeginThread(FFmpegDecoderThread, m_pVDecoder);
		m_pAudioDecodeThread = AfxBeginThread(FFmpegAudioDecodeThread, m_pVDecoder);
		SetTimer(0, 40, NULL);
 	
	}

}



// ���� �� ���� �ݱ� �޴�
void CPlayerDlg::OnClose()
{
	if (m_pADThread != nullptr)
	{
		m_pADThread->SuspendThread();
		HANDLE hThread = m_pADThread->m_hThread;
		TerminateThread(hThread, 0);
	}
	if (m_pVDThread != nullptr)
	{
		m_pVDThread->SuspendThread();
		HANDLE hThread = m_pVDThread->m_hThread;
		TerminateThread(hThread, 0);
	}
	
	if (m_pADecoder != nullptr)
	{
		m_pADecoder->m_pAudio->XAudio2Cleanup();
		delete m_pADecoder->m_pAudio;
		delete m_pADecoder;
		m_pADecoder = nullptr;
	}
	if (m_pVDecoder != nullptr)
	{
		m_pVDecoder->m_pVideo->D3DCleanup();
		delete m_pVDecoder->m_pVideo;
		delete m_pVDecoder;
		m_pVDecoder = nullptr;
	}
	
	Invalidate();
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



// ȭ�� ũ�� ������ ó�� - ȭ�� ���� �� ��Ʈ�� ũ�� ���� ó��
void CPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	if (IsWindow(m_sFrame.m_hWnd))
	{
		picRect.right = cx;
		picRect.bottom = cy - 80;
		m_sFrame.MoveWindow(picRect);

		toolbarRect.top = picRect.bottom;
		toolbarRect.left = 0;
		toolbarRect.right = cx;
		toolbarRect.bottom = cy;
	}

	if (IsWindow(m_sliderSeek.m_hWnd))
	{
		GetDlgItem(IDC_SLIDER_SEEK)->SetWindowPos(NULL, 10, picRect.bottom + 5, cx - 20, 30, 0);
		GetDlgItem(IDC_SLIDER_SEEK)->SendMessage(WM_KILLFOCUS, NULL);
	}

	if (IsWindow(m_sliderVolume.m_hWnd))
	{
		GetDlgItem(IDC_SLIDER_VOLUME)->SetWindowPos(NULL, cx - 140, picRect.bottom + 35, 130, 30, 0);
		GetDlgItem(IDC_SLIDER_VOLUME)->SendMessage(WM_KILLFOCUS, NULL);
	}

	if (IsWindow(m_btnPlay.m_hWnd))
	{
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowPos(NULL, 15, picRect.bottom + 35, 30, 30, 0);
		GetDlgItem(IDC_BUTTON_PLAY)->SendMessage(WM_KILLFOCUS, NULL);
	}

	if (IsWindow(m_btnPause.m_hWnd))
	{
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowPos(NULL, 45, picRect.bottom + 35, 30, 30, 0);
		GetDlgItem(IDC_BUTTON_PAUSE)->SendMessage(WM_KILLFOCUS, NULL);
	}

	if (IsWindow(m_btnStop.m_hWnd))
	{
		GetDlgItem(IDC_BUTTON_STOP)->SetWindowPos(NULL, 75, picRect.bottom + 35, 30, 30, 0);
		GetDlgItem(IDC_BUTTON_STOP)->SendMessage(WM_KILLFOCUS, NULL);
	}

	Invalidate();
}



// �����-���� ��ũ�� ���� Ÿ�̸� ����
void CPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_pVDecoder->video_refresh_timer();


	CDialogEx::OnTimer(nIDEvent);
}



// ���̾�α� �ּ� ũ�� ����
void CPlayerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 300;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}
