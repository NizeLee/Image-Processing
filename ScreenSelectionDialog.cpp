// ScreenSelectionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "ScreenSelectionDialog.h"


// CScreenSelectionDialog dialog

IMPLEMENT_DYNAMIC(CScreenSelectionDialog, CDialog)

CScreenSelectionDialog::CScreenSelectionDialog(CRect& rect)
   : CDialog(IDD_SCREEN_SELECTION), m_rect(rect)
{
   rect.SetRectEmpty();
}

CScreenSelectionDialog::~CScreenSelectionDialog()
{
}


BEGIN_MESSAGE_MAP(CScreenSelectionDialog, CDialog)
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CScreenSelectionDialog message handlers

BOOL CScreenSelectionDialog::OnInitDialog()
{
   CDialog::OnInitDialog();
   
// AfxGetMainWnd()->ShowWindow(SW_HIDE);
   ::Sleep(333);
   if(!m_image.CaptureScreen())
   {
      EndDialog(IDCANCEL);
   }
   const int cx = ::GetSystemMetrics(SM_CXSCREEN);
   const int cy = ::GetSystemMetrics(SM_CYSCREEN);
   SetWindowPos(&CWnd::wndTopMost, 0, 0, cx, cy, SWP_SHOWWINDOW);

   return TRUE;
}

BOOL CScreenSelectionDialog::OnEraseBkgnd(CDC* pDC)
{
   return TRUE;
}

void CScreenSelectionDialog::OnPaint()
{
   CPaintDC dc(this);
   m_image.Draw(dc.GetSafeHdc(), 0, 0);
   m_tracker.Draw(&dc);
}

void CScreenSelectionDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
   m_tracker.TrackRubberBand(this, point);
   m_tracker.GetTrueRect(m_rect);
   EndDialog(IDOK);
}
