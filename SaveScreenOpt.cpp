// SaveScreenOpt.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "SaveScreenOpt.h"

#include "ScreenSelectionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveScreenOpt dialog


CSaveScreenOpt::CSaveScreenOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveScreenOpt::IDD, pParent)
	, m_nFPS(30)
	, m_bSaveView(TRUE)
	, m_nNotViewOpt(0)
{
	//{{AFX_DATA_INIT(CSaveScreenOpt)
	m_nW = 640;
	m_nH = 480;
	//}}AFX_DATA_INIT
}


void CSaveScreenOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveScreenOpt)
	DDX_Text(pDX, IDC_H, m_nH);
	DDX_Text(pDX, IDC_W, m_nW);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FPS, m_nFPS);
	DDX_Check(pDX, IDC_SAVE_VIEW, m_bSaveView);
	DDX_Radio(pDX, IDC_NOT_VIEW_OPT, m_nNotViewOpt);
}


BEGIN_MESSAGE_MAP(CSaveScreenOpt, CDialog)
	//{{AFX_MSG_MAP(CSaveScreenOpt)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SEL_SAVE_RECT, &CSaveScreenOpt::OnBnClickedSelSaveRect)
	ON_BN_CLICKED(IDC_NOT_VIEW_OPT3, &CSaveScreenOpt::OnBnClickedNotViewOpt3)
	ON_BN_CLICKED(IDC_NOT_VIEW_OPT, &CSaveScreenOpt::OnBnClickedNotViewOpt)
	ON_BN_CLICKED(IDC_NOT_VIEW_OPT2, &CSaveScreenOpt::OnBnClickedNotViewOpt2)
	ON_BN_CLICKED(IDC_SAVE_VIEW, &CSaveScreenOpt::OnBnClickedSaveView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveScreenOpt message handlers


void CSaveScreenOpt::OnBnClickedSelSaveRect()
{
	CScreenSelectionDialog dlg(m_rtClientRect);
	dlg.DoModal();

	CString Buf;
	Buf.Format(_T("Lefttop(%d, %d), Rightbottom(%d, %d)"), m_rtClientRect.left, m_rtClientRect.top, m_rtClientRect.right, m_rtClientRect.bottom);

	SetDlgItemText(IDC_VIEW_INFO, Buf);
//	AfxMessageBox(Buf);
   
//	m_image.CaptureRect(rect);
	
//	AfxGetMainWnd()->ShowWindow(SW_SHOW);
//	UpdateAllViews(NULL);
}

void CSaveScreenOpt::OnBnClickedNotViewOpt()
{
	UpdateData(TRUE);

	m_rtClientRect = CRect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));

	CString Buf;
	Buf.Format(_T("Lefttop(%d, %d), Rightbottom(%d, %d)"), m_rtClientRect.left, m_rtClientRect.top, m_rtClientRect.right, m_rtClientRect.bottom);

	SetDlgItemText(IDC_VIEW_INFO, Buf);
}

void CSaveScreenOpt::OnBnClickedNotViewOpt2()
{
	UpdateData(TRUE);

	AfxGetMainWnd()->GetWindowRect(m_rtClientRect);

	if(m_rtClientRect.Width() % 4 != 0)
		m_rtClientRect.right += 4 - m_rtClientRect.Width() % 4;

	if(m_rtClientRect.Height() % 2 != 0)
		m_rtClientRect.top += 2 - m_rtClientRect.Height() % 2;

	//m_rtClientRect.left = 100;
	//m_rtClientRect.right = 100+640;

	CString Buf;
	Buf.Format(_T("Lefttop(%d, %d), Rightbottom(%d, %d)"), m_rtClientRect.left, m_rtClientRect.top, m_rtClientRect.right, m_rtClientRect.bottom);

	SetDlgItemText(IDC_VIEW_INFO, Buf);
}

void CSaveScreenOpt::OnBnClickedNotViewOpt3()
{
	UpdateData(TRUE);

	CScreenSelectionDialog dlg(m_rtClientRect);
	dlg.DoModal();

	if(m_rtClientRect.Width() % 4 != 0)
		m_rtClientRect.right += 4 - m_rtClientRect.Width() % 4;

	if(m_rtClientRect.Height() % 2 != 0)
		m_rtClientRect.top += 2 - m_rtClientRect.Height() % 2;

	CString Buf;
	Buf.Format(_T("Lefttop(%d, %d), Rightbottom(%d, %d)"), m_rtClientRect.left, m_rtClientRect.top, m_rtClientRect.right, m_rtClientRect.bottom);

	SetDlgItemText(IDC_VIEW_INFO, Buf);
//	AfxMessageBox(Buf);
   
//	m_image.CaptureRect(rect);
	
//	AfxGetMainWnd()->ShowWindow(SW_SHOW);
//	UpdateAllViews(NULL);
}

void CSaveScreenOpt::OnBnClickedSaveView()
{
	UpdateData(TRUE);
}


BOOL CSaveScreenOpt::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_rtClientRect = CRect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));

	CString Buf;
	Buf.Format(_T("Lefttop(%d, %d), Rightbottom(%d, %d)"), m_rtClientRect.left, m_rtClientRect.top, m_rtClientRect.right, m_rtClientRect.bottom);

	SetDlgItemText(IDC_VIEW_INFO, Buf);

	return TRUE;  // return TRUE unless you set the focus to a control
}
