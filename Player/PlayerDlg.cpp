
// PlayerDlg.cpp : �÷��̾� ���� �ڵ�
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "MediaInfoDlg.h"
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


RENDER_STATE CPlayerDlg::eVideo = RENDER_STATE_STOPPED;


CPlayerDlg::CPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLAYER_DIALOG, pParent)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bSeek = false;
	seekTick = 0;
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
	DDX_Control(pDX, IDC_STATIC_PLAYTIME, m_sPlaytime);
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
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CPlayerDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CPlayerDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CPlayerDlg::OnBnClickedButtonStop)
	ON_WM_HSCROLL()
//	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SEEK, &CPlayerDlg::OnReleasedcaptureSliderSeek)
ON_COMMAND(IDM_REPORT, &CPlayerDlg::OnReport)
ON_WM_ACTIVATE()
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
	MoveWindow(NULL, NULL, 960, 640);
	GdiplusStartup(&m_GdiplusToken, &m_GdiplusStartupInput, NULL);

	//GetDlgItem(IDC_STATIC_FRAME);
	GetDlgItem(IDC_SLIDER_SEEK)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_SLIDER_VOLUME)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_BUTTON_PLAY)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_BUTTON_PAUSE)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_BUTTON_STOP)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_STATIC_PLAYTIME)->SendMessage(WM_KILLFOCUS, NULL);
	m_sliderVolume.SetRange(0, 100);
	m_sliderVolume.SetPos(100);

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


	// ȭ�� ũ�� ����� ��� ���� ��� �� ���͹ڽ� ó��
	CDC * pDC = AfxGetMainWnd()->GetDC();
	if (m_bIsFullScreen)
	{
		CRect fullRect = NULL;
		GetWindowRect(fullRect);
		pDC->FillSolidRect(fullRect, RGB(0, 0, 0));

		GetDlgItem(IDC_STATIC_FRAME)->ShowWindow(FALSE);
		GetDlgItem(IDC_SLIDER_SEEK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SLIDER_VOLUME)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PLAY)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PAUSE)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_PLAYTIME)->ShowWindow(FALSE);
		return;
	}

	GetDlgItem(IDC_STATIC_FRAME)->ShowWindow(TRUE);
	GetDlgItem(IDC_SLIDER_SEEK)->ShowWindow(TRUE);
	GetDlgItem(IDC_SLIDER_VOLUME)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PLAY)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PAUSE)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_PLAYTIME)->ShowWindow(TRUE);


	OnPlayPause();
	pDC->FillSolidRect(picRect, RGB(0, 0, 0));
	if (eVideo == RENDER_STATE_STOPPED)
	{
		Graphics graphics(this->m_hWnd);
		Image image(L"res\\SuperSheepFlying.png");
		graphics.DrawImage(&image, (picRect.right / 2 - 128), (picRect.bottom / 2 - 128));
	}
	pDC->FillSolidRect(toolbarRect, RGB(240, 240, 240));
	AfxGetMainWnd()->ReleaseDC(pDC);
	OnPlayPause();

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
	// Ű �̺�Ʈ ó��
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
			if (eVideo == RENDER_STATE_STARTED)
			{
				eVideo = RENDER_STATE_PAUSED;
				m_pCFFmpeg->m_pAudio->XAudio2Pause();
				KillTimer(0);
				SuspendThread(m_pCFFmpeg->audioDecodeThread.native_handle());
				SuspendThread(m_pCFFmpeg->videoDecodeThread.native_handle());
				SuspendThread(m_pPlayThread->m_hThread);
				progTick = currTick;
			}
			else if (eVideo == RENDER_STATE_PAUSED)
			{
				strtTick = clock();
				eVideo = RENDER_STATE_STARTED;
				m_pCFFmpeg->m_pAudio->XAudio2Resume();
				m_pCFFmpeg->frame_timer = av_gettime() / 1000000.0;
				SetTimer(0, 1, NULL);
				ResumeThread(m_pCFFmpeg->audioDecodeThread.native_handle());
				ResumeThread(m_pCFFmpeg->videoDecodeThread.native_handle());
				ResumeThread(m_pPlayThread->m_hThread);
			}
			break;

		case VK_LEFT:
			m_pCFFmpeg->stream_seek(-5);
			break;
		case VK_RIGHT:
			m_pCFFmpeg->stream_seek(+5);
			break;
		case VK_UP:
			m_sliderVolume.SetPos(m_sliderVolume.GetPos() + 10);
			break;
		case VK_DOWN:
			m_sliderVolume.SetPos(m_sliderVolume.GetPos() - 10);
			break;

		default:
			break;
		}
	// Ű �̺�Ʈ ó�� ��

	// ���콺 �̺�Ʈ ó��
	case WM_MOUSEFIRST:
		// ��Ʈ�� ��Ŀ�� ����
		GetDlgItem(IDC_SLIDER_SEEK)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_SLIDER_VOLUME)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_PLAY)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_PAUSE)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_STOP)->SendMessage(WM_KILLFOCUS, NULL);
		break;
	//���콺 �̺�Ʈ ó�� ��

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
		m_pCFFmpeg	= new CFFmpeg(DECODE_VIDEO);

		if (FAILED(m_pCFFmpeg->OpenMediaSource(filePath)))
			AfxMessageBox(_T("ERROR: OpenMediaSource function call"));
		m_pCFFmpeg->m_pAudio->XAudio2SetVolume(((float)m_sliderVolume.GetPos()) / 100);
		
		// ��� ����
		eVideo = RENDER_STATE_STARTED;
		m_btnPause.EnableWindow(TRUE);
		m_btnStop.EnableWindow(TRUE);
		m_sliderSeek.EnableWindow(TRUE);

		m_pPlayThread = AfxBeginThread(FFmpegDecoderThread, m_pCFFmpeg);
		SetTimer(0, 40, NULL);

		/*m_dVideoDuration =
			av_q2d(m_pCFFmpeg->avFormatCtx->streams[m_pCFFmpeg->m_nVideoStreamIndex]->time_base)
			* m_pCFFmpeg->avFormatCtx->streams[m_pCFFmpeg->m_nVideoStreamIndex]->duration;*/
		m_dVideoDuration = m_pCFFmpeg->avFormatCtx->duration / AV_TIME_BASE;

		CString strDur;
		strDur.Format(_T("00:00:00 / %02d:%02d:%02d"),
			(int)m_dVideoDuration/3600, (int)m_dVideoDuration/60 % 60,
			(int)m_dVideoDuration%60);
		m_sPlaytime.SetWindowText((LPCTSTR)strDur);
		//m_sliderSeek.SetPageSize(m_dVideoDuration);
		m_sliderSeek.SetPos(0);
		m_sliderSeek.SetRange(0, (int)m_dVideoDuration*1000);
		strtTick = clock();
		SetTimer(1, 250, NULL);
	}

}



// ���� �� ���� �ݱ� �޴�
void CPlayerDlg::OnClose()
{
	eVideo = RENDER_STATE_STOPPED;
	if (m_pPlayThread != nullptr)
	{
		m_pCFFmpeg->m_pAudio->XAudio2Pause();
		KillTimer(0);
		SuspendThread(m_pCFFmpeg->audioDecodeThread.native_handle());
		SuspendThread(m_pCFFmpeg->videoDecodeThread.native_handle());
		SuspendThread(m_pPlayThread->m_hThread);

		TerminateThread(m_pCFFmpeg->audioDecodeThread.native_handle(), 0);
		TerminateThread(m_pCFFmpeg->videoDecodeThread.native_handle(), 0);
		TerminateThread(m_pPlayThread->m_hThread, 0);
		m_pPlayThread = nullptr;
	}

	if (m_pCFFmpeg != nullptr)
	{
		m_pCFFmpeg->m_pAudio->XAudio2Cleanup();
		m_pCFFmpeg->m_pVideo->D3DCleanup();
// 		delete m_pCFFmpeg->m_pAudio;
// 		delete m_pCFFmpeg->m_pVideo;
// 		delete m_pCFFmpeg;
		m_pCFFmpeg = nullptr;

		m_btnPause.EnableWindow(FALSE);
		m_btnStop.EnableWindow(FALSE);
		m_sliderSeek.EnableWindow(FALSE);
		m_sliderSeek.SetPos(0);
		m_sPlaytime.SetWindowTextW(_T("00:00:00 / 00:00:00"));

		progTick = currTick = 0;
		KillTimer(1);

		Invalidate();
	}
}



// ���� �� ��� ���� �޴�
void CPlayerDlg::OnReport()
{
	CMediaInfoDlg infoDlg;
	infoDlg.DoModal();
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
		OnPlayPause();
		picRect.right = cx;
		picRect.bottom = cy - 80;
		m_sFrame.MoveWindow(picRect);

		toolbarRect.top = picRect.bottom;
		toolbarRect.left = 0;
		toolbarRect.right = cx;
		toolbarRect.bottom = cy;
		OnPlayPause();
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

	if (IsWindow(m_sPlaytime.m_hWnd))
	{
		GetDlgItem(IDC_STATIC_PLAYTIME)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_PLAYTIME)->SetWindowPos(NULL, 120, picRect.bottom + 42, 120, 15, 0);
		GetDlgItem(IDC_STATIC_PLAYTIME)->ShowWindow(TRUE);
	}

	InvalidateRect(picRect);
}



// �����-���� ��ũ�� ���� Ÿ�̸� ���� �� Ž�� �����̴� ó��
void CPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
		m_pCFFmpeg->video_refresh_timer();
	}
	else if (eVideo == RENDER_STATE_STARTED && nIDEvent == 1)
	{	
		currTick = clock() - strtTick + progTick;
		m_sliderSeek.SetPos(currTick);
		CString strDur;
		strDur.Format(_T("%02d:%02d:%02d / %02d:%02d:%02d"),
			currTick/3600000000, currTick/60000 % 60, currTick/1000 % 60,
			(int)m_dVideoDuration/3600, (int)m_dVideoDuration/60 % 60,
			(int)m_dVideoDuration%60);
		m_sPlaytime.SetWindowText((LPCTSTR)strDur);
		
		if (currTick / 3600000000 == (int)m_dVideoDuration / 3600
			&& currTick / 60000 % 60 == (int)m_dVideoDuration / 60 % 60
			&& currTick / 1000 % 60 == (int)m_dVideoDuration % 60)
			OnClose();
	}

	CDialogEx::OnTimer(nIDEvent);
}



// ���̾�α� �ּ� ũ�� ����
void CPlayerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 400;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}



// �׺���̼� ��� ��ư
void CPlayerDlg::OnBnClickedButtonPlay()
{
	if (m_pCFFmpeg == nullptr)
		OnOpenFile();

	if (eVideo == RENDER_STATE_PAUSED)
	{
		PostMessage(WM_KEYDOWN, VK_SPACE);

		GetDlgItem(IDC_SLIDER_SEEK)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_SLIDER_VOLUME)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_PLAY)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_PAUSE)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_STOP)->SendMessage(WM_KILLFOCUS, NULL);
	}
}



// �׺���̼� �Ͻ����� ��ư
void CPlayerDlg::OnBnClickedButtonPause()
{
	if (eVideo == RENDER_STATE_STARTED)
	{
		PostMessage(WM_KEYDOWN, VK_SPACE);

		GetDlgItem(IDC_SLIDER_SEEK)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_SLIDER_VOLUME)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_PLAY)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_PAUSE)->SendMessage(WM_KILLFOCUS, NULL);
		GetDlgItem(IDC_BUTTON_STOP)->SendMessage(WM_KILLFOCUS, NULL);
	}
}



// �׺���̼� ���� ��ư
void CPlayerDlg::OnBnClickedButtonStop()
{
	OnClose();

	GetDlgItem(IDC_SLIDER_SEEK)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_SLIDER_VOLUME)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_BUTTON_PLAY)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_BUTTON_PAUSE)->SendMessage(WM_KILLFOCUS, NULL);
	GetDlgItem(IDC_BUTTON_STOP)->SendMessage(WM_KILLFOCUS, NULL);
}

void CPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (pScrollBar)
	{
		// � �����̴����� �˻�
		if (pScrollBar == (CScrollBar*)&m_sliderVolume){
			float volume = m_sliderVolume.GetPos();
			if (m_pCFFmpeg)
				m_pCFFmpeg->m_pAudio->XAudio2SetVolume(volume / 100);
		}
		if (pScrollBar == (CScrollBar *)&m_sliderSeek) {
			progTick = m_sliderSeek.GetPos();
			strtTick = clock();
			m_pCFFmpeg->stream_seek(progTick / 1000);
		}
	}


	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}





//void CPlayerDlg::OnReleasedcaptureSliderSeek(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	progTick = m_sliderSeek.GetPos();
//	strtTick = clock();
//	m_pCFFmpeg->stream_seek(progTick / 1000);
//
//	*pResult = 0;
//}


void CPlayerDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE)
	{
		SetFocus();
	}
}
