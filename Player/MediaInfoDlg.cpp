
// MediaInfoDlg.cpp : 재생 중인 미디어의 정보를 보여주는 대화상자
//

#include "stdafx.h"
#include "Player.h"
#include "PlayerDlg.h"
#include "MediaInfoDlg.h"
#include "afxdialogex.h"


// CMediaInfoDlg 대화 상자입니다.

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


// CMediaInfoDlg 메시지 처리기입니다.
