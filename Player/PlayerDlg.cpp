
// PlayerDlg.cpp : 플레이어 메인 코드
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CPlayerDlg 대화 상자



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


// CPlayerDlg 메시지 처리기

BOOL CPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	

	// 초기화 작업
	MoveWindow(NULL, NULL, 976, 599);
	GdiplusStartup(&m_GdiplusToken, &m_GdiplusStartupInput, NULL);



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
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

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 메시지 처리
BOOL CPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	// 키 이벤트
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		// ESC
		case VK_ESCAPE:
			return TRUE;

		// ENTER : 전체 화면/창모드 토글
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

		// SPACEBAR : 재생/일시정지 토글
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



// FFmpeg 디코딩 스레드
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


// 검은색 배경 그리기
void CPlayerDlg::DrawBlackScreen()
{
	CDC * pDC = AfxGetMainWnd()->GetDC();
 	AfxGetMainWnd()->GetClientRect(m_pVDecoder->viewRect);
	pDC->FillSolidRect(m_pVDecoder->viewRect, RGB(0, 0, 0));
	AfxGetMainWnd()->ReleaseDC(pDC);
}



// 파일 → 파일 열기 메뉴
void CPlayerDlg::OnOpenFile()
{
	HRESULT hr = S_OK;
	CString filePath = nullptr;

	CFileDialog pDlg(
		TRUE,
		_T("모든 동영상 파일(*.mp4, *.wmv, *.asf, *.avi, *.mkv)"), _T("*.mp4;*.wmv;*.asf;*.avi;*.mkv;"),
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		_T("mp4 파일(*.mp4)|*.mp4|\
			wmv파일(*.wmv)|*.wmv|\
			asf 파일(*.asf)|*.asf|\
			avi 파일(*.avi)|*.avi|\
			mkv 파일(*.mkv)|*.mkv|\
			모든 동영상 파일(*.mp4, *.wmv, *.asf, *.avi, *.mkv)|*.mp4;*.wmv;*.asf;*.avi;*.mkv;|")
		, NULL, NULL, TRUE
	);

	if (pDlg.DoModal() == IDOK)
	{
		filePath = pDlg.GetPathName();

		// 미디어 소스 열기 + 초기화
		OnClose();
		//m_pADecoder	= new CFFmpeg(DECODE_AUDIO);
		m_pVDecoder	= new CFFmpeg(DECODE_VIDEO);
		

		/*	if (FAILED(m_pADecoder->OpenMediaSource(filePath)))
				AfxMessageBox(_T("ERROR: OpenMediaSource function call"));*/
		if (FAILED(m_pVDecoder->OpenMediaSource(filePath)))
			AfxMessageBox(_T("ERROR: OpenMediaSource function call"));

		

		// Direct3D 초기화
//		m_pVDecoder->m_pVideo->renderState = RENDER_STATE_STARTED;
		
		// 스레드 시작
		//m_pADThread = AfxBeginThread(FFmpegDecoderThread, m_pADecoder);
		m_pVDThread = AfxBeginThread(FFmpegDecoderThread, m_pVDecoder);
		m_pAudioDecodeThread = AfxBeginThread(FFmpegAudioDecodeThread, m_pVDecoder);
		SetTimer(0, 40, NULL);
 	
	}

}



// 파일 → 파일 닫기 메뉴
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



// 파일 → 종료 메뉴
void CPlayerDlg::OnQuit()
{
	PostQuitMessage(WM_QUIT);
}



// 재생 → 재생/일시정지 메뉴
void CPlayerDlg::OnPlayPause()
{
	PostMessage(WM_KEYDOWN, VK_SPACE);
}



// 화면크기 → 전체 화면 메뉴
void CPlayerDlg::OnFullscreen()
{
	PostMessage(WM_KEYDOWN, VK_RETURN);
}



// 화면 크기 조절시 처리 - 화면 영역 및 컨트롤 크기 조절 처리
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



// 오디오-비디오 싱크를 위한 타이머 설정
void CPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pVDecoder->video_refresh_timer();


	CDialogEx::OnTimer(nIDEvent);
}



// 다이얼로그 최소 크기 설정
void CPlayerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 300;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}
