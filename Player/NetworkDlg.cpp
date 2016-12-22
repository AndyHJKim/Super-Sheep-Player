// NetworkDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Player.h"
#include "NetworkDlg.h"
#include "afxdialogex.h"


// CNetworkDlg 대화 상자입니다.

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


// CNetworkDlg 메시지 처리기입니다.


void CNetworkDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	UpdateData(FALSE);

	CDialogEx::OnOK();
}
