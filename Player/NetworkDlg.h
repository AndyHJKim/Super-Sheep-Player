#pragma once


// CNetworkDlg ��ȭ �����Դϴ�.

class CNetworkDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNetworkDlg)

public:
	CNetworkDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNetworkDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NETWORK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strAddress;
	afx_msg void OnBnClickedOk();
};
