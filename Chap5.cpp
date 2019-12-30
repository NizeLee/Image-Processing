// Chap5.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "Chap5.h"
#include "MainFrm.h"

#include "Image.h"		
#include "Image4Win.h"

#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChap5 dialog


CChap5::CChap5(CWnd* pParent /*=NULL*/)
	: CDialog(CChap5::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChap5)
	m_ThreRMSE = 15.0;
	//}}AFX_DATA_INIT

	m_bCreated = FALSE;
}


void CChap5::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChap5)
	DDX_Control(pDX, IDC_SHAPE, m_ShapeCtrl);
	DDX_Control(pDX, IDC_UP_DOWN, m_UpDownCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChap5, CDialog)
	//{{AFX_MSG_MAP(CChap5)
	ON_BN_CLICKED(IDC_THRESHOLDING_PTILE, OnThresholdingPtile)
	ON_BN_CLICKED(IDC_THRESHOLDING_VALLEY, OnThresholdingValley)
	ON_BN_CLICKED(IDC_THRESHOLDING_ITERATE, OnThresholdingIterate)
	ON_BN_CLICKED(IDC_HOUGH, OnHough)
	ON_BN_CLICKED(IDC_TURTLE, OnTurtle)
	ON_BN_CLICKED(IDC_K_COSINE, OnKCosine)
	ON_BN_CLICKED(IDC_DILATION, OnDilation)
	ON_BN_CLICKED(IDC_EROSION, OnErosion)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_LABELING, OnLabeling)
	ON_BN_CLICKED(IDC_THINNING, OnThinning)
	ON_BN_CLICKED(IDC_LABELING_MOMENT, OnLabelingMoment)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChap5 message handlers

BOOL CChap5::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_UpDownCtrl.InsertString(0, _T("Upper"));
	m_UpDownCtrl.InsertString(1, _T("Lower"));
	m_UpDownCtrl.SetCurSel(0);

	SetDlgItemInt(IDC_PERCENT, 10);
	SetDlgItemInt(IDC_K, 15);

	CheckDlgButton(IDC_BIN, 1);
	SetDlgItemInt(IDC_ELE_SIZE, 3);

	m_ShapeCtrl.InsertString(0, _T("Square"));
	m_ShapeCtrl.InsertString(1, _T("-"));
	m_ShapeCtrl.InsertString(2, _T("|"));
	m_ShapeCtrl.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChap5::OnCancel() 
{
	DestroyWindow();	
	m_bCreated = FALSE;
}

void CChap5::OnThresholdingPtile() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	int nPercent = GetDlgItemInt(IDC_PERCENT);
	int nType = m_UpDownCtrl.GetCurSel();

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	pTileThresholding(ImageGray, OutputGray, nW, nH, nPercent/100., nType);
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnThresholdingValley() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	ValleyThresholding(ImageGray, OutputGray, nW, nH);
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnThresholdingIterate() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	IterateThresholding(ImageGray, OutputGray, nW, nH);
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnHough() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray, **OutputGray;
	double **OutputDouble;
	int nSW, nSH;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	nSW = 180;
	nSH = (int)(sqrt(double(nW*nW+nH*nH))/2.)*2;
	OutputDouble = dmatrix(nSH, nSW);

	GetCurrentImageGray(ImageGray);

	HoughTransform(ImageGray, OutputDouble, nW, nH);
	
	DisplayDimage2D(OutputDouble, nSW, nSH, nPosX+nW, nPosY);

	int x, y;
	int r, theta;
	int dr, dt;
	bool line;
	double m;
	const double Pi = acos(-1.);

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			OutputGray[y][x] = 0;

	for(r = 0 ; r < nSH ; r++)
		for(theta = 0 ; theta < nSW ; theta++)
		{
			if(OutputDouble[r][theta] > 150)
			{
				line = true;
				for(dr = -1 ; dr <= 1 ; dr++)
					for(dt = -1 ; dt <= 1 ; dt++)
					{
						if(r+dr < 0 || theta+dt < 0 ||
							r+dr >= nSH || theta+dt >= nSW || 
							(dt == 0 && dr == 0))
							continue;
						if(OutputDouble[r][theta] < OutputDouble[r+dr][theta+dt])
							line = false;
					}
				
				if(line)
				{
					m = -cos(theta*Pi/180)/sin(theta*Pi/180);

					if(fabs(m) > 1)
					{
						for(y = 0 ; y < nH ; y++)
						{
							x = (int)(((r-nSH/2)*2-y*sin(theta*Pi/180))/cos(theta*Pi/180));
							if(x >= 0 && x < nW)
								OutputGray[y][x] = GRAY_CNT-1;
						}
					}
					else
					{
						for(x = 0 ; x < nW ; x++)
						{
							y = (int)(((r-nSH/2)*2-x*cos(theta*Pi/180))/sin(theta*Pi/180));
							if(y >= 0 && y < nH)
								OutputGray[y][x] = GRAY_CNT-1;
						}
					}
				}
			}
		}

	DisplayCimage2D(OutputGray, nW, nH, nPosX, nPosY+nH);

	free_cmatrix(ImageGray, nH, nW);
	free_cmatrix(OutputGray, nH, nW);
	free_dmatrix(OutputDouble, nSH, nSW);	
}

void CChap5::OnTurtle() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	CIpPoint *Boundary;

	Boundary = new CIpPoint[nW*nH];
	int nCnt, i;
	int x, y;

	Turtle(ImageGray, nW, nH, Boundary, &nCnt);
	
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			OutputGray[y][x] = 0;

	for(i = 0 ; i < nCnt ; i++)
		OutputGray[Boundary[i].y][Boundary[i].x] = GRAY_CNT-1;

	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	delete [] Boundary;

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnKCosine() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	CIpPoint *Boundary;
	double *kCosine;

	Boundary = new CIpPoint[nW*nH];
	
	int nCnt, i;
	int x, y;

	Turtle(ImageGray, nW, nH, Boundary, &nCnt);
	
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			OutputGray[y][x] = 0;

	for(i = 0 ; i < nCnt ; i++)
		OutputGray[Boundary[i].y][Boundary[i].x] = GRAY_CNT-1;

	int nK = GetDlgItemInt(IDC_K);
	kCosine = new double[nCnt];
	ComputekCosine(Boundary, nCnt, nK, kCosine);

	BYTE **kCosineImage;
	int nCH = 100+1;
	kCosineImage = cmatrix(nCH, nCnt);

	for(y = 0 ; y < nCH ; y++)
		for(x = 0 ; x < nCnt ; x++)
			kCosineImage[y][x] = 0;

	for(i = 1 ; i < nCnt ; i++)
		DrawLine(kCosineImage, nCnt, nCH, i-1, nCH-(int)((kCosine[i-1]+1)*50)-1,
			i, nCH-(int)((kCosine[i]+1)*50)-1, GRAY_CNT-1);

	for(i = 0 ; i < nCnt ; i++)
	{
		if(kCosine[i] >= kCosine[(i-1+nCnt)%nCnt] && kCosine[i] >= kCosine[(i+1)%nCnt]
			&& kCosine[i] > -0.75)
		{
			Dot(OutputGray, nW, nH, Boundary[i].x, Boundary[i].y);
			Dot(kCosineImage, nCnt, nCH, i, nCH-(int)((kCosine[i]+1)*50)-1);
		}
	}

	DisplayCimage2D(kCosineImage, nCnt, nCH, nPosX, nPosY+nH);
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);


	free_cmatrix(kCosineImage, nCH, nCnt);

	delete [] Boundary;
	delete [] kCosine;

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnDilation() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	int nSize = GetDlgItemInt(IDC_ELE_SIZE);
	int nShape = m_ShapeCtrl.GetCurSel();

	if(IsDlgButtonChecked(IDC_BIN))
		DilationBinary(ImageGray, OutputGray, nW, nH, nShape, nSize);
	else
		DilationGray(ImageGray, OutputGray, nW, nH, nShape, nSize);
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnErosion() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	int nSize = GetDlgItemInt(IDC_ELE_SIZE);
	int nShape = m_ShapeCtrl.GetCurSel();

	if(IsDlgButtonChecked(IDC_BIN))
		ErosionBinary(ImageGray, OutputGray, nW, nH, nShape, nSize);
	else
		ErosionGray(ImageGray, OutputGray, nW, nH, nShape, nSize);
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnClose() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray, **TempGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);
	TempGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	int nSize = GetDlgItemInt(IDC_ELE_SIZE);
	int nShape = m_ShapeCtrl.GetCurSel();

	if(IsDlgButtonChecked(IDC_BIN))
	{
		DilationBinary(ImageGray, TempGray, nW, nH, nShape, nSize);
		ErosionBinary(TempGray, OutputGray, nW, nH, nShape, nSize);
	}
	else
	{
		DilationGray(ImageGray, TempGray, nW, nH, nShape, nSize);
		ErosionGray(TempGray, OutputGray, nW, nH, nShape, nSize);
	}
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
	free_cmatrix(TempGray, nH, nW);
}

void CChap5::OnOpen() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	BYTE **OutputGray, **TempGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);
	TempGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	int nSize = GetDlgItemInt(IDC_ELE_SIZE);
	int nShape = m_ShapeCtrl.GetCurSel();

	if(IsDlgButtonChecked(IDC_BIN))
	{
		ErosionBinary(ImageGray, TempGray, nW, nH, nShape, nSize);
		DilationBinary(TempGray, OutputGray, nW, nH, nShape, nSize);
	}
	else
	{
		ErosionGray(ImageGray, TempGray, nW, nH, nShape, nSize);
		DilationGray(TempGray, OutputGray, nW, nH, nShape, nSize);
	}
	
	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
	free_cmatrix(TempGray, nH, nW);	
}

void CChap5::OnLabeling() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	int **OutputLabel;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputLabel = imatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	Labeling(ImageGray, OutputLabel, nW, nH, 100);

	DisplayIimage2D(OutputLabel, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_imatrix(OutputLabel, nH, nW);
}

void CChap5::OnThinning() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray, **OutputGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	Thinning(ImageGray, OutputGray, nW, nH);

	DisplayCimage2D(OutputGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);	
	free_cmatrix(OutputGray, nH, nW);	
}

void CChap5::OnLabelingMoment() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;
	int **OutputLabel;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	OutputLabel = imatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	int LabelCnt = Labeling(ImageGray, OutputLabel, nW, nH, 100);

	DisplayIimage2D(OutputLabel, nW, nH, nPosX+nW, nPosY);

	double *m2, *m3;
	double *mu00, *mu20, *mu02, *mu11, *mu30, *mu03, *mu21, *mu12, r;
	CPoint *Center;
	int *Cnt;
	int x, y, i;

	m2 = new double[LabelCnt];
	m3 = new double[LabelCnt];

	Center = new CPoint[LabelCnt];
	Cnt = new int[LabelCnt];

	mu00 = new double[LabelCnt];
	mu20 = new double[LabelCnt];
	mu02 = new double[LabelCnt];
	mu11 = new double[LabelCnt];
	mu30 = new double[LabelCnt];
	mu03 = new double[LabelCnt];
	mu12 = new double[LabelCnt];
	mu21 = new double[LabelCnt];

	for(i = 0 ; i < LabelCnt ; i++)
	{
		Cnt[i] = 0;
		Center[i].x = 0;
		Center[i].y = 0;

		mu00[i] = 0;
		mu20[i] = 0;
		mu02[i] = 0;
		mu11[i] = 0;
		mu30[i] = 0;
		mu03[i] = 0;
		mu12[i] = 0;
		mu21[i] = 0;
	}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			i = OutputLabel[y][x];

			if(i < 0) continue;

			Cnt[i]++;
			Center[i].x += x;
			Center[i].y += y;
		}

	for(i = 0 ; i < LabelCnt ; i++)
	{
		Center[i].x /= Cnt[i];
		Center[i].y /= Cnt[i];
	}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			i = OutputLabel[y][x];

			if(i < 0) continue;
			
			mu00[i] += pow((double)(x-Center[i].x), 0)*pow((double)(y-Center[i].y), 0);
			mu20[i] += pow((double)(x-Center[i].x), 2)*pow((double)(y-Center[i].y), 0);
			mu02[i] += pow((double)(x-Center[i].x), 0)*pow((double)(y-Center[i].y), 2);
			mu11[i] += pow((double)(x-Center[i].x), 1)*pow((double)(y-Center[i].y), 1);
			mu30[i] += pow((double)(x-Center[i].x), 3)*pow((double)(y-Center[i].y), 0);
			mu03[i] += pow((double)(x-Center[i].x), 0)*pow((double)(y-Center[i].y), 3);
			mu21[i] += pow((double)(x-Center[i].x), 2)*pow((double)(y-Center[i].y), 1);
			mu12[i] += pow((double)(x-Center[i].x), 1)*pow((double)(y-Center[i].y), 2);
		}

	for(i = 0 ; i < LabelCnt ; i++)
	{
		mu20[i] /= Cnt[i];
		mu02[i] /= Cnt[i];
		mu11[i] /= Cnt[i];
		mu30[i] /= Cnt[i];
		mu03[i] /= Cnt[i];
		mu12[i] /= Cnt[i];
		mu21[i] /= Cnt[i];

		r = pow(mu20[i]+mu02[i], 0.5);
		m2[i] = (pow(mu20[i]-mu02[i], 2) + 4*mu11[i]*mu11[i])/pow(r, 4);
		m3[i] = (pow(mu30[i]-3*mu12[i], 2) + pow(3*mu21[i]-mu03[i], 2))/pow(r, 6);
	}

	BYTE **Image;
	Image = cmatrix(GRAY_CNT, GRAY_CNT);
	for(y = 0 ; y < GRAY_CNT ; y++)
		for(x = 0 ; x < GRAY_CNT ; x++)
			Image[y][x] = 0;

	for(i = 0 ; i < LabelCnt ; i++)
	{
		x = (int)(m2[i]*GRAY_CNT);
		y = (int)(m3[i]*GRAY_CNT);
		if(x < GRAY_CNT && y < GRAY_CNT)
			Image[y][x] = GRAY_CNT-1;
	}

	DisplayCimage2D(Image, GRAY_CNT, GRAY_CNT, nPosX, nPosY+nH);
	free_cmatrix(Image, GRAY_CNT, GRAY_CNT);

	delete [] m2;
	delete [] m3;

	delete [] Center;
	delete [] Cnt;

	delete [] mu00;
	delete [] mu20;
	delete [] mu02;
	delete [] mu11;
	delete [] mu30;
	delete [] mu03;
	delete [] mu21;
	delete [] mu12;


	free_cmatrix(ImageGray, nH, nW);	
	free_imatrix(OutputLabel, nH, nW);
}


static int MinValue(int x, int y) {
	return x>y? y : x;
}

static int MaxValue(int x, int y) {
	return x>y? x : y;
}

