// MySliderCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Player.h"
#include "MySliderCtrl.h"
#include "PlayerDlg.h"
// CMySliderCtrl

IMPLEMENT_DYNAMIC(CMySliderCtrl, CSliderCtrl)

CMySliderCtrl::CMySliderCtrl()
	: m_bClicked(false)
{
	
}

CMySliderCtrl::~CMySliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CMySliderCtrl, CSliderCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CMySliderCtrl �޽��� ó�����Դϴ�.




void CMySliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (m_bClicked) {
		CRect   rectClient, rectChannel;
		GetClientRect(rectClient);
		GetChannelRect(rectChannel);
		double   nPos =
			(GetRangeMax() - GetRangeMin())*(double)(point.x - rectClient.left - rectChannel.left) / (double)(rectChannel.right - rectChannel.left);
		CPlayerDlg* pMain = (CPlayerDlg*)AfxGetMainWnd();
		SetPos(nPos);
		double move;
	/*	move = nPos / 1000 - pMain->m_pCFFmpeg->audio_clock;*/
		move = nPos / 1000;
		pMain->m_pCFFmpeg->stream_seek(move, 1);

	}


	CSliderCtrl::OnMouseMove(nFlags, point);
}


void CMySliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_bClicked = TRUE;
	CRect  rectChannel;
	GetChannelRect(rectChannel);
	double   nPos =
		(GetRangeMax() - GetRangeMin())*(double)(point.x - rectChannel.left) 
		/ (double)(rectChannel.right - rectChannel.left);
	CPlayerDlg* pMain = (CPlayerDlg*)AfxGetMainWnd();
	pMain->m_bSeek = TRUE;
	SetPos(nPos);
	double move;
	move = nPos / 1000 - pMain->m_pCFFmpeg->audio_clock;
	pMain->m_pCFFmpeg->stream_seek(move, 0);

	CSliderCtrl::OnLButtonDown(nFlags, point);
}


void CMySliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_bClicked = FALSE;
	CPlayerDlg* pMain = (CPlayerDlg*)AfxGetMainWnd();
	pMain->m_bSeek = FALSE;

	CSliderCtrl::OnLButtonUp(nFlags, point);
}
