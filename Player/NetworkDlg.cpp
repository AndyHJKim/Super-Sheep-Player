// NetworkDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Player.h"
#include "NetworkDlg.h"
#include "afxdialogex.h"


// CNetworkDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CNetworkDlg, CDialogEx)

CNetworkDlg::CNetworkDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_NETWORK, pParent)
	, m_strAddress(_T(""))
{

}

CNetworkDlg::~CNetworkDlg()
{
}

void CNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NETWORK, m_strAddress);
}


BEGIN_MESSAGE_MAP(CNetworkDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CNetworkDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNetworkDlg �޽��� ó�����Դϴ�.


void CNetworkDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	UpdateData(FALSE);

	CDialogEx::OnOK();
}
