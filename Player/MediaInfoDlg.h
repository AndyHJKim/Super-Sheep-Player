
// MediaInfoDlg.h : ��� ���� �̵���� ������ �����ִ� ��ȭ����
//

#pragma once


// CMediaInfoDlg ��ȭ �����Դϴ�.

class CMediaInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMediaInfoDlg)

public:
	CMediaInfoDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMediaInfoDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MEDIAINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
