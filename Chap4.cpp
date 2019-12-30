// Chap4.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "Chap4.h"

#include "Image.h"	
#include "Image4Win.h"

#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChap4 dialog


CChap4::CChap4(CWnd* pParent /*=NULL*/)
	: CDialog(CChap4::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChap4)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bCreated = FALSE;
}


void CChap4::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChap4)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChap4, CDialog)
	//{{AFX_MSG_MAP(CChap4)
	ON_BN_CLICKED(IDC_DCT, OnDct)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_COMPRESSION, OnCompression)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChap4 message handlers

BOOL CChap4::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemInt(IDC_BLOCK_SIZE, 8);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChap4::OnCancel() 
{
	DestroyWindow();	
	m_bCreated = FALSE;
}


void CChap4::OnDct() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	double **OutputDouble;

	int nBlockSize = GetDlgItemInt(IDC_BLOCK_SIZE);

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputDouble = dmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	DCT2D(ImageGray, OutputDouble, nW, nH, nBlockSize);

	int u, v;
	for(v = 0 ; v < nH ; v++)
		for(u = 0 ; u < nW ; u++)
		{
			OutputDouble[v][u]
				= log(fabs(OutputDouble[v][u])+1.);
		}
	
	DisplayDimage2D(OutputDouble, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_dmatrix(OutputDouble, nH, nW);	
}

void CChap4::OnCompression() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	int nBlockSize = GetDlgItemInt(IDC_BLOCK_SIZE);

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	Compression(ImageGray, OutputGray, nW, nH);
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
	
}
