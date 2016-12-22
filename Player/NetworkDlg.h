#pragma once


// CNetworkDlg 대화 상자입니다.

class CNetworkDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNetworkDlg)

public:
	CNetworkDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNetworkDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NETWORK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strAddress;
	afx_msg void OnBnClickedOk();
};
