
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

// 	InitToolbar();

// 	m_dlgToolBar.Create(this);
// 	m_dlgToolBar.LoadToolBar(IDR_TOOLBAR1);
// 	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

// 	if (!m_dlgToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_LIST, WS_CHILD | WS_VISIBLE)
// 		|| !m_dlgToolBar.LoadToolBar(IDR_TOOLBAR1))
// 	{
// 		TRACE0("Failed to create toolbar\n");
// 		return -1;
// 	}
// 	m_dlgToolBar.SetBarStyle(m_dlgToolBar.GetBarStyle()
// 		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_GRIPPER | CBRS_FLYBY | CBRS_BOTTOM);
// 	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
// 
// 	CToolBarCtrl& ctrbar = m_dlgToolBar.GetToolBarCtrl();
// 	ctrbar.AddBitmap(1, IDB_PRINT);
// 	m_dlgToolBar.SetButtonText(1, "�μ�");
// 
// 	ctrbar.AddBitmap(2, IDB_DOWN);
// 	m_dlgToolBar.SetButtonText(2, "�����ⵥ���� �б�");
// 
// 	ctrbar.AddBitmap(3, IDB_UP); // 
// 	m_dlgToolBar.SetButtonText(3, "������ ���");


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
			if (m_pVRThread != nullptr)
				m_pVRThread->SuspendThread();
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
			if (m_pVRThread != nullptr)
				m_pVRThread->ResumeThread();
			return TRUE;

		// SPACEBAR : ���/�Ͻ����� ���
		case VK_SPACE:
			if (m_pVideo != NULL)
			{
				switch (m_pVideo->renderState)
				{
				case RENDER_STATE_STARTED:
 					m_pVideo->renderState = RENDER_STATE_PAUSED;
					m_pADThread->SuspendThread();
					m_pVDThread->SuspendThread();
					m_pARThread->SuspendThread();
					m_pVRThread->SuspendThread();
					break;
				case RENDER_STATE_PAUSED:
 					m_pVideo->renderState = RENDER_STATE_STARTED;
					m_pADThread->ResumeThread();
					m_pVDThread->ResumeThread();
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
	CPlayerDlg * pDlg = (CPlayerDlg *)AfxGetApp()->m_pMainWnd;

	while (m_pDecoder->Decoder() >= 0)
	{
		if (m_pDecoder->decodeType == DECODE_AUDIO)
		{
			SetEvent(pDlg->m_hADEvent);
			WaitForSingleObject(pDlg->m_hAREvent, INFINITE);
		}
		else if (m_pDecoder->decodeType == DECODE_VIDEO)
		{
			SetEvent(pDlg->m_hVDEvent);
			WaitForSingleObject(pDlg->m_hVREvent, INFINITE);
		}
	}

	if(m_pDecoder->decodeType == DECODE_AUDIO)
		CloseHandle(pDlg->m_hADEvent);
	else if (m_pDecoder->decodeType == DECODE_VIDEO)
		CloseHandle(pDlg->m_hVDEvent);
	return 0;
}



// XAudio2 ����� ������ ������
UINT CPlayerDlg::AudioRendererThread(LPVOID _method)
{
	AudioRenderer * m_pAudio = (AudioRenderer *)_method;
	CPlayerDlg * pDlg = (CPlayerDlg *)AfxGetApp()->m_pMainWnd;

	while (WaitForSingleObject(pDlg->m_hADEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (pDlg->m_pADecoder->avPacket.stream_index == pDlg->m_pADecoder->m_nAudioStreamIndex)
			m_pAudio->XAudio2Render(pDlg->m_pADecoder->m_pSwr_buf, pDlg->m_pADecoder->m_swr_buf_len);
		SetEvent(pDlg->m_hAREvent);
	}

	CloseHandle(pDlg->m_hAREvent);
	return 0;
}



// D3D ���� ������ ������
UINT CPlayerDlg::D3DVideoRendererThread(LPVOID _method)
{
	CD3DRenderer * m_pVideo = (CD3DRenderer *)_method;
	CPlayerDlg * pDlg = (CPlayerDlg *)AfxGetApp()->m_pMainWnd;

	while (WaitForSingleObject(pDlg->m_hVDEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (pDlg->m_pVDecoder->avPacket.stream_index == pDlg->m_pVDecoder->m_nVideoStreamIndex)
			m_pVideo->D3DVideoRender(*(pDlg->m_pVDecoder->videoData), pDlg->m_pVDecoder->viewRect);
		SetEvent(pDlg->m_hVREvent);
	}

	CloseHandle(pDlg->m_hVREvent);
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
		m_pADecoder	= new CFFmpeg(DECODE_AUDIO);
		m_pVDecoder	= new CFFmpeg(DECODE_VIDEO);
		m_pAudio	= new AudioRenderer();
		m_pVideo	= new CD3DRenderer();

		if (FAILED(m_pADecoder->OpenMediaSource(filePath)))
			AfxMessageBox(_T("ERROR: OpenMediaSource function call"));
		if (FAILED(m_pVDecoder->OpenMediaSource(filePath)))
			AfxMessageBox(_T("ERROR: OpenMediaSource function call"));

		//DirectSound �ʱ�ȭ
		m_pAudio->XAudio2Initialize(this->m_hWnd,
			m_pADecoder->avAudioCodecCtx->channels, m_pADecoder->avAudioCodecCtx->sample_rate);

		// Direct3D �ʱ�ȭ
		m_pVideo->renderState = RENDER_STATE_STARTED;
		m_pVideo->D3DInitialize(this->m_hWnd,
			m_pVDecoder->videoWidth, m_pVDecoder->videoHeight, m_pVDecoder->viewRect);

		// ������ �̺�Ʈ �ڵ� �ʱ�ȭ
		m_hADEvent	= CreateEvent(NULL, FALSE, TRUE, NULL);
		m_hVDEvent	= CreateEvent(NULL, FALSE, TRUE, NULL);
		m_hAREvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hVREvent	= CreateEvent(NULL, FALSE, FALSE, NULL);

		// ������ ����
		m_pADThread = AfxBeginThread(FFmpegDecoderThread, m_pADecoder);
		m_pVDThread = AfxBeginThread(FFmpegDecoderThread, m_pVDecoder);
 		m_pARThread	= AfxBeginThread(AudioRendererThread, m_pAudio);
		m_pVRThread	= AfxBeginThread(D3DVideoRendererThread, m_pVideo);
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
	if (m_pARThread != nullptr)
	{
		m_pARThread->SuspendThread();
		HANDLE hThread = m_pARThread->m_hThread;
		TerminateThread(hThread, 0);
	}
	if (m_pVRThread != nullptr)
	{
		m_pVRThread->SuspendThread();
		HANDLE hThread = m_pVRThread->m_hThread;
		TerminateThread(hThread, 0);
	}
	if (m_pADecoder != nullptr)
	{
		delete m_pADecoder;
		m_pADecoder = nullptr;
	}
	if (m_pVDecoder != nullptr)
	{
		delete m_pVDecoder;
		m_pVDecoder = nullptr;
	}
	if (m_pAudio != nullptr)
	{
		m_pAudio->XAudio2Cleanup();
		delete m_pAudio;
		m_pAudio = nullptr;
	}
	if (m_pVideo != nullptr)
	{
		m_pVideo->D3DCleanup();
		delete m_pVideo;
		m_pVideo = nullptr;
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



// ȭ�� ũ�� ������ ������ ����
void CPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
// 	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}



// void CPlayerDlg::InitToolbar()
// {
// 	if (!m_dlgToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
// 		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
// 		!m_dlgToolBar.LoadToolBar(IDR_TOOLBAR1))
// 	{
// 		TRACE0("Failed to Create Dialog Toolbar\n");
// 		EndDialog(IDCANCEL);
// 	}
// 	CRect    rcClientNew; // New Client Rect with Tollbar Added
// 	GetClientRect(m_rectOldSize); // Retrive the Old Client WindowSize
// 
// 								// Called to reposition and resize control bars in the client area of a window
// 								// The reposQuery FLAG does not really traw the Toolbar.  It only does the calculations.
// 								// And puts the new ClientRect values in rcClientNew so we can do the rest of the Math.
// 	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNew);
// 
// 	// All of the Child Windows (Controls) now need to be moved so the Tollbar does not cover them up.
// 	// Offest to move all child controls after adding Tollbar
// 	CPoint ptOffset(rcClientNew.left - m_rectOldSize.left, rcClientNew.top - m_rectOldSize.top);
// 
// 	CRect    rcChild;
// 	CWnd*    pwndChild = GetWindow(GW_CHILD);  //Handle to the Dialog Controls
// 
// 	while (pwndChild) // Cycle through all child controls
// 	{
// 
// 		pwndChild->GetWindowRect(rcChild); //  Get the child control RECT
// 		ScreenToClient(rcChild);
// 		rcChild.OffsetRect(ptOffset); // Changes the Child Rect by the values of the claculated offset
// 		pwndChild->MoveWindow(rcChild, FALSE); // Move the Child Control
// 		pwndChild = pwndChild->GetNextWindow();
// 
// 	}
// 
// 	CRect    rcWindow;
// 	GetWindowRect(rcWindow); // Get the RECT of the Dialog
// 	rcWindow.right += m_rectOldSize.Width() - rcClientNew.Width(); // Increase width to new Client Width
// 	rcWindow.bottom += m_rectOldSize.Height() - rcClientNew.Height(); // Increase height to new Client Height
// 	MoveWindow(rcWindow, FALSE); // Redraw Window
// 
// 	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
// 
// 
// }
