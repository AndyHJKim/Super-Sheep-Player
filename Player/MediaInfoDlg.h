
// MediaInfoDlg.h : 재생 중인 미디어의 정보를 보여주는 대화상자
//

#pragma once


// CMediaInfoDlg 대화 상자입니다.

class CMediaInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMediaInfoDlg)

public:
	CMediaInfoDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMediaInfoDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MEDIAINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
