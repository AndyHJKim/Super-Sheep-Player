
// MediaInfoDlg.cpp : ��� ���� �̵���� ������ �����ִ� ��ȭ����
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "MediaInfoDlg.h"
#include "afxdialogex.h"


// CMediaInfoDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMediaInfoDlg, CDialogEx)

CMediaInfoDlg::CMediaInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MEDIAINFO, pParent)
{

}

CMediaInfoDlg::~CMediaInfoDlg()
{
}

void CMediaInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMediaInfoDlg, CDialogEx)
END_MESSAGE_MAP()


// CMediaInfoDlg �޽��� ó�����Դϴ�.
