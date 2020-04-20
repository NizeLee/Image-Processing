// ImageProcessingView.cpp : implementation of the CImageProcessingView class
//

#include "stdafx.h"
#include "ImageProcessing.h"

#include "ImageProcessingDoc.h"
#include "ImageProcessingView.h"

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
// CImageProcessingView

IMPLEMENT_DYNCREATE(CImageProcessingView, CScrollView)

BEGIN_MESSAGE_MAP(CImageProcessingView, CScrollView)
	//{{AFX_MSG_MAP(CImageProcessingView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDM_GRAY_DUPLICATE, OnGrayDuplicate)
	ON_COMMAND(IDM_COLOR_DUPLICATE, OnColorDuplicate)
	ON_COMMAND(IDM_CHAP2, OnChap2)
	ON_COMMAND(IDM_CHAP3, OnChap3)
	ON_COMMAND(IDM_SCALE_X2, OnScaleX2)
	ON_COMMAND(IDM_SCALE_HALF, OnScaleHalf)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_CHAP4, OnChap4)
	ON_COMMAND(IDM_CHAP5, OnChap5)
	ON_COMMAND(IDM_APPLICATION, OnApplication)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_SEL_REGION, OnSelRegion)
	ON_COMMAND(IDM_SEL_REGION_ZOOM, OnSelRegionZoom)
	ON_COMMAND(IDM_SUM, OnSum)
	ON_COMMAND(IDM_SUBTRACT, OnSubtract)
	ON_COMMAND(IDM_DIFFERENCE, OnDifference)
	ON_COMMAND(IDM_EDIT_REGION, OnEditRegion)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_REGION, OnUpdateEditRegion)
	ON_COMMAND(IDM_DEL_REGION, OnDelRegion)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(IDM_IMAGE_INFO, &CImageProcessingView::OnImageInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingView construction/destruction

CImageProcessingView::CImageProcessingView()
{
	m_BmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BmiHeader.biPlanes = 1;
	m_BmiHeader.biBitCount = 24;
	m_BmiHeader.biCompression = BI_RGB;
	m_BmiHeader.biXPelsPerMeter = 1000;
	m_BmiHeader.biYPelsPerMeter = 1000;
	m_BmiHeader.biClrUsed = 0;
	m_BmiHeader.biClrImportant = 0;

	m_nCurrentImage = -1;

	m_nSelectRegion = 0;

	m_nDrawRegion = NOT_DRAW;
	m_nViewRegionSelected = -1;

	m_nPtCntOfCurrRegoin = 4;

	m_bEditRegionState = false;
	m_nRadiusForSel = 10;
}

CImageProcessingView::~CImageProcessingView()
{
}

BOOL CImageProcessingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingView drawing

void CImageProcessingView::OnDraw(CDC* pDC)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int i;
	for(i = 0 ; i < pDoc->m_nImageCnt ; i++)
	{
		m_BmiHeader.biWidth = pDoc->m_Image[i].nW;
		m_BmiHeader.biHeight = pDoc->m_Image[i].nH;

		m_BmiHeader.biSizeImage = GetBmp24Size(pDoc->m_Image[i].nW, pDoc->m_Image[i].nH);

		StretchDIBits(pDC->m_hDC,
			pDoc->m_Image[i].nPosX, pDoc->m_Image[i].nPosY,
			m_BmiHeader.biWidth,
			m_BmiHeader.biHeight,
			0, 0,
			m_BmiHeader.biWidth,
			m_BmiHeader.biHeight,
			pDoc->m_Image[i].Image1D,
			(LPBITMAPINFO)&m_BmiHeader,
			DIB_RGB_COLORS,
			SRCCOPY);
	}

	CRect rt;
	CFont font;

	GetClientRect(&rt);
	font.CreatePointFont(200, _T("Arial"));
	pDC->SelectObject(&font);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 0, 255));
	pDC->DrawText(m_ViewMsg, rt, DT_LEFT);
	
	pDC->SelectStockObject(NULL_BRUSH);

	CPen PenLine(PS_SOLID, 2, RGB(0,255,0));
	CPen PenCenter(PS_SOLID, 2, RGB(255,0,0));

	for(i = 0 ; i < m_ViewRegion.nCnt ; i++)
	{
		CPoint pt[MAX_REGION_POINT], Center(0, 0);
		double theta = m_ViewRegion.Rotate[i];

		int k;
		for(k = 0 ; k < m_ViewRegion.nPtCnt[i] ; k++)
		{
			Center.x += m_ViewRegion.Pt[i][k].x;
			Center.y += m_ViewRegion.Pt[i][k].y;
		}

		Center.x /= m_ViewRegion.nPtCnt[i];
		Center.y /= m_ViewRegion.nPtCnt[i];

		if(m_bEditRegionState)
		{
			pDC->SelectObject(&PenCenter);
			CRect rtCenter(Center.x-m_nRadiusForSel, Center.y-m_nRadiusForSel, Center.x+m_nRadiusForSel, Center.y+m_nRadiusForSel);

			pDC->Ellipse(rtCenter);
		}

		pDC->SelectObject(&PenLine);

		if(m_ViewRegion.nType[i] == RT_RECT)
		{
			pt[0].x = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][0].y - Center.y)*sin(theta) 
				+ Center.x);
			pt[0].y = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][0].y - Center.y)*cos(theta)
				+ Center.y);

			pt[1].x = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][0].y - Center.y)*sin(theta)
				+ Center.x);
			pt[1].y = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][0].y - Center.y)*cos(theta)
				+ Center.y);

			pt[2].x = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][1].y - Center.y)*sin(theta)
				+ Center.x);
			pt[2].y = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][1].y - Center.y)*cos(theta)
				+ Center.y);

			pt[3].x = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][1].y - Center.y)*sin(theta)
				+ Center.x);
			pt[3].y = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][1].y - Center.y)*cos(theta)
				+ Center.y);

			pDC->MoveTo(pt[3]);
			for(k = 0 ; k < 4 ; k++)
				pDC->LineTo(pt[k]);

			CPoint TopDir;
			TopDir.x = (LONG)((0)*cos(theta) - (-m_nRadiusForSel * 2)*sin(theta)
				+ Center.x);
			TopDir.y = (LONG)((0)*sin(theta) + (-m_nRadiusForSel * 2)*cos(theta)
				+ Center.y);
			
			//pDC->MoveTo(Center);
			//pDC->LineTo(TopDir);

			DrawArrow(pDC, Center.x, Center.y, TopDir.x, TopDir.y, RGB(255,0,0), 3);
		}
		else
		{
			for(k = 0 ; k < m_ViewRegion.nPtCnt[i] ; k++)
			{
				pt[k].x = (LONG)((m_ViewRegion.Pt[i][k].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][k].y - Center.y)*sin(theta) + Center.x);
				pt[k].y = (LONG)((m_ViewRegion.Pt[i][k].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][k].y - Center.y)*cos(theta) + Center.y);
			}

			pDC->MoveTo(pt[m_ViewRegion.nPtCnt[i]-1]);
			for(k = 0 ; k < m_ViewRegion.nPtCnt[i] ; k++)
				pDC->LineTo(pt[k]);
		}
	}
}

void CImageProcessingView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	
	sizeTotal.cx = 1024*20;
	sizeTotal.cy = 768*20;
	SetScrollSizes(MM_TEXT, sizeTotal);

	m_PastePoint= CPoint(-1, -1);
}

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingView printing

BOOL CImageProcessingView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CImageProcessingView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CImageProcessingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingView diagnostics

#ifdef _DEBUG
void CImageProcessingView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CImageProcessingView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CImageProcessingDoc* CImageProcessingView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageProcessingDoc)));
	return (CImageProcessingDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingView message handlers

int CImageProcessingView::GetCurrentImageRect(CPoint point)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPoint CurrentPoint= GetScrollPosition() + point;

	for(int i = pDoc->m_nImageCnt-1 ; i >= 0 ; i--)
	{
		CRect rt(pDoc->m_Image[i].nPosX, pDoc->m_Image[i].nPosY, 
			pDoc->m_Image[i].nPosX + pDoc->m_Image[i].nW, pDoc->m_Image[i].nPosY+pDoc->m_Image[i].nH);

		if(rt.PtInRect(CurrentPoint)) 
		{
			ImageInfo SaveImage = pDoc->m_Image[i];
			for(int k = i ; k < pDoc->m_nImageCnt - 1 ; k++)
				pDoc->m_Image[k] = pDoc->m_Image[k+1];

			pDoc->m_Image[pDoc->m_nImageCnt-1] = SaveImage;

			return pDoc->m_nImageCnt-1;
		}
	}

	return -1;
}

int CImageProcessingView::GetCurrentImagePos(CPoint point)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPoint CurrentPoint= GetScrollPosition() + point;

	for(int i = pDoc->m_nImageCnt-1 ; i >= 0 ; i--)
	{
		CPoint pt(pDoc->m_Image[i].nPosX, pDoc->m_Image[i].nPosY);

		if(CurrentPoint == pt) 
		{
			ImageInfo SaveImage = pDoc->m_Image[i];
			for(int k = i ; k < pDoc->m_nImageCnt - 1 ; k++)
				pDoc->m_Image[k] = pDoc->m_Image[k+1];

			pDoc->m_Image[pDoc->m_nImageCnt-1] = SaveImage;

			return pDoc->m_nImageCnt-1;
		}
	}

	return -1;
}

bool CImageProcessingView::GetCurrentImageInfo(int *pW, int *pH, int *pPosX, int *pPosY, int nIndex) 
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int nCurrentImage = pDoc->m_nImageCnt-1-nIndex;	

	if(nCurrentImage < 0) return false;

	*pW = pDoc->m_Image[nCurrentImage].nW;
	*pH = pDoc->m_Image[nCurrentImage].nH;
	*pPosX = pDoc->m_Image[nCurrentImage].nPosX;
	*pPosY = pDoc->m_Image[nCurrentImage].nPosY;

	return true;
}

bool CImageProcessingView::GetCurrentImageGray(BYTE **ImageGray, int nIndex)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int nCurrentImage = pDoc->m_nImageCnt-1-nIndex;	

	if(nCurrentImage < 0) return false;

	int x, y, Pos;

	for(y = 0 ; y < pDoc->m_Image[nCurrentImage].nH ; y++)
		for(x = 0 ; x < pDoc->m_Image[nCurrentImage].nW ; x++)
		{
			Pos = GetBmp24Pos(pDoc->m_Image[nCurrentImage].nW, pDoc->m_Image[nCurrentImage].nH, x, y);
			ImageGray[y][x] = (pDoc->m_Image[nCurrentImage].Image1D[Pos]+pDoc->m_Image[nCurrentImage].Image1D[Pos+1]+pDoc->m_Image[nCurrentImage].Image1D[Pos+2])/3;
		}

	return true;
}

bool CImageProcessingView::DisplayCimage2D(BYTE **ImageGray, int nW, int nH, int nPosX, int nPosY, 
	bool bErase, bool bDelete, int Rate)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(bDelete)
	{
		int nCurrentImage = GetCurrentImagePos(CPoint(nPosX, nPosY)-GetScrollPosition());
		if(nCurrentImage >= 0)
		{
			delete [] pDoc->m_Image[nCurrentImage].Image1D;

			pDoc->m_nImageCnt--;
		}
	}

	int nCurrentImage = pDoc->m_nImageCnt;	

	if(nCurrentImage >= MAX_IMAGE) return false;

	int x, y, Pos;

	pDoc->m_Image[nCurrentImage].nW = nW*Rate/100;
	pDoc->m_Image[nCurrentImage].nH = nH*Rate/100;
	pDoc->m_Image[nCurrentImage].nPosX = nPosX;
	pDoc->m_Image[nCurrentImage].nPosY = nPosY;

	pDoc->m_Image[nCurrentImage].Image1D = new BYTE[GetBmp24Size(nW*Rate/100, nH*Rate/100)];

	for(y = 0 ; y < nH*Rate/100 ; y++)
		for(x = 0 ; x < nW*Rate/100 ; x++)
		{
			Pos = GetBmp24Pos(nW*Rate/100, nH*Rate/100, x, y);

			pDoc->m_Image[nCurrentImage].Image1D[Pos++] = ImageGray[y*100/Rate][x*100/Rate];
			pDoc->m_Image[nCurrentImage].Image1D[Pos++] = ImageGray[y*100/Rate][x*100/Rate];
			pDoc->m_Image[nCurrentImage].Image1D[Pos] = ImageGray[y*100/Rate][x*100/Rate];
		}

	pDoc->m_nImageCnt++;
	Invalidate(bErase);
	UpdateWindow();

	return true;
}

bool CImageProcessingView::GetCurrentImageColor(BYTE **ImageRed, BYTE **ImageGreen, BYTE **ImageBlue, int nIndex)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int nCurrentImage = pDoc->m_nImageCnt-1-nIndex;	

	if(nCurrentImage < 0) return false;

	int x, y, Pos;

	for(y = 0 ; y < pDoc->m_Image[nCurrentImage].nH ; y++)
		for(x = 0 ; x < pDoc->m_Image[nCurrentImage].nW ; x++)
		{
			Pos = GetBmp24Pos(pDoc->m_Image[nCurrentImage].nW, pDoc->m_Image[nCurrentImage].nH, x, y);

			ImageBlue[y][x] = pDoc->m_Image[nCurrentImage].Image1D[Pos++];
			ImageGreen[y][x] = pDoc->m_Image[nCurrentImage].Image1D[Pos++];
			ImageRed[y][x] = pDoc->m_Image[nCurrentImage].Image1D[Pos];
		}

	return true;
}

bool CImageProcessingView::GetCurrentImage1DColor(BYTE *Image1D, int nIndex)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int nCurrentImage = pDoc->m_nImageCnt-1-nIndex;	

	if(nCurrentImage < 0) return false;

	memcpy(Image1D, pDoc->m_Image[nCurrentImage].Image1D, GetBmp24Size(pDoc->m_Image[nCurrentImage].nW, pDoc->m_Image[nCurrentImage].nH));

	return true;
}

bool CImageProcessingView::DisplayCimage1D(BYTE *Image1D, int nW, int nH, 
	int nPosX, int nPosY, bool bErase, bool bDelete, int Rate)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(bDelete)
	{
		int nCurrentImage = GetCurrentImagePos(CPoint(nPosX, nPosY)-GetScrollPosition());
		if(nCurrentImage >= 0)
		{
			delete [] pDoc->m_Image[nCurrentImage].Image1D;

			pDoc->m_nImageCnt--;
		}
	}

	int nCurrentImage = pDoc->m_nImageCnt;

	if(nCurrentImage >= MAX_IMAGE) return false;

	pDoc->m_Image[nCurrentImage].nW = nW*Rate/100;
	pDoc->m_Image[nCurrentImage].nH = nH*Rate/100;
	pDoc->m_Image[nCurrentImage].nPosX = nPosX;
	pDoc->m_Image[nCurrentImage].nPosY = nPosY;

	pDoc->m_Image[nCurrentImage].Image1D = new BYTE[GetBmp24Size(nW*Rate/100, nH*Rate/100)];

	if(Rate == 100)
		memcpy(pDoc->m_Image[nCurrentImage].Image1D, Image1D, GetBmp24Size(nW, nH));
	else
	{
		int x, y;
		int Pos, SrcPos;
		for(y = 0 ; y < nH*Rate/100 ; y++)
			for(x = 0 ; x < nW*Rate/100 ; x++)
			{
				Pos = GetBmp24Pos(nW*Rate/100, nH*Rate/100, x, y);
				SrcPos = GetBmp24Pos(nW, nH, x*100/Rate, y*100/Rate);

				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = Image1D[SrcPos++];
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = Image1D[SrcPos++];
				pDoc->m_Image[nCurrentImage].Image1D[Pos] = Image1D[SrcPos];
			}
	}

	pDoc->m_nImageCnt++;
	Invalidate(bErase);

	return true;
}
	
bool CImageProcessingView::DisplayCimage2DColor(BYTE **ImageRed, BYTE **ImageGreen, BYTE **ImageBlue, int nW, int nH, 
	int nPosX, int nPosY, bool bErase, bool bDelete, int Rate)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(bDelete)
	{
		int nCurrentImage = GetCurrentImagePos(CPoint(nPosX, nPosY)-GetScrollPosition());
		if(nCurrentImage >= 0)
		{
			delete [] pDoc->m_Image[nCurrentImage].Image1D;

			pDoc->m_nImageCnt--;
		}
	}

	int nCurrentImage = pDoc->m_nImageCnt;

	if(nCurrentImage >= MAX_IMAGE) return false;

	int x, y, Pos;

	pDoc->m_Image[nCurrentImage].nW = nW*Rate/100;
	pDoc->m_Image[nCurrentImage].nH = nH*Rate/100;
	pDoc->m_Image[nCurrentImage].nPosX = nPosX;
	pDoc->m_Image[nCurrentImage].nPosY = nPosY;

	pDoc->m_Image[nCurrentImage].Image1D = new BYTE[GetBmp24Size(nW*Rate/100, nH*Rate/100)];

	for(y = 0 ; y < nH*Rate/100 ; y++)
		for(x = 0 ; x < nW*Rate/100 ; x++)
		{
			Pos = GetBmp24Pos(nW*Rate/100, nH*Rate/100, x, y);

			pDoc->m_Image[nCurrentImage].Image1D[Pos++] = ImageBlue[y*100/Rate][x*100/Rate];
			pDoc->m_Image[nCurrentImage].Image1D[Pos++] = ImageGreen[y*100/Rate][x*100/Rate];
			pDoc->m_Image[nCurrentImage].Image1D[Pos] = ImageRed[y*100/Rate][x*100/Rate];
		}

	pDoc->m_nImageCnt++;
	Invalidate(bErase);
	UpdateWindow();

	return true;
}

bool CImageProcessingView::DisplayIimage2D(int **ImageInt, int nW, int nH, int nPosX, int nPosY, 
	bool bErase, bool bDelete)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(bDelete)
	{
		int nCurrentImage = GetCurrentImagePos(CPoint(nPosX, nPosY)-GetScrollPosition());
		if(nCurrentImage >= 0)
		{
			delete [] pDoc->m_Image[nCurrentImage].Image1D;

			pDoc->m_nImageCnt--;
		}
	}

	int nCurrentImage = pDoc->m_nImageCnt;		

	if(nCurrentImage >= MAX_IMAGE) return false;

	int x, y, Pos;

	pDoc->m_Image[nCurrentImage].nW = nW;
	pDoc->m_Image[nCurrentImage].nH = nH;
	pDoc->m_Image[nCurrentImage].nPosX = nPosX;
	pDoc->m_Image[nCurrentImage].nPosY = nPosY;

	pDoc->m_Image[nCurrentImage].Image1D = new BYTE[GetBmp24Size(nW, nH)];

	int nMax = ImageInt[0][0];
	int nMin = ImageInt[0][0];
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(ImageInt[y][x] > nMax) nMax = ImageInt[y][x];
			if(ImageInt[y][x] < nMin) nMin = ImageInt[y][x];
		}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			Pos = GetBmp24Pos(nW, nH, x, y);

			if(nMin == nMax)
			{
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = 0;
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = 0;
				pDoc->m_Image[nCurrentImage].Image1D[Pos] = 0;
			}
			else
			{
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = (BYTE)((ImageInt[y][x]-nMin)*(GRAY_CNT-1)/(nMax-nMin));
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = (BYTE)((ImageInt[y][x]-nMin)*(GRAY_CNT-1)/(nMax-nMin));
				pDoc->m_Image[nCurrentImage].Image1D[Pos] = (BYTE)((ImageInt[y][x]-nMin)*(GRAY_CNT-1)/(nMax-nMin));
			}
		}

	pDoc->m_nImageCnt++;
	Invalidate(bErase);

	return true;
}

bool CImageProcessingView::DisplayDimage2D(double **ImageDouble, int nW, int nH, int nPosX, int nPosY, 
	bool bErase, bool bDelete)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(bDelete)
	{
		int nCurrentImage = GetCurrentImagePos(CPoint(nPosX, nPosY)-GetScrollPosition());
		if(nCurrentImage >= 0)
		{
			delete [] pDoc->m_Image[nCurrentImage].Image1D;

			pDoc->m_nImageCnt--;
		}
	}

	int nCurrentImage = pDoc->m_nImageCnt;		

	if(nCurrentImage >= MAX_IMAGE) return false;

	int x, y, Pos;

	pDoc->m_Image[nCurrentImage].nW = nW;
	pDoc->m_Image[nCurrentImage].nH = nH;
	pDoc->m_Image[nCurrentImage].nPosX = nPosX;
	pDoc->m_Image[nCurrentImage].nPosY = nPosY;

	pDoc->m_Image[nCurrentImage].Image1D = new BYTE[GetBmp24Size(nW, nH)];

	double dMax = ImageDouble[0][0];
	double dMin = ImageDouble[0][0];
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(ImageDouble[y][x] > dMax) dMax = ImageDouble[y][x];
			if(ImageDouble[y][x] < dMin) dMin = ImageDouble[y][x];
		}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			Pos = GetBmp24Pos(nW, nH, x, y);

			if(dMin == dMax)
			{
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = 0;
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = 0;
				pDoc->m_Image[nCurrentImage].Image1D[Pos] = 0;
			}
			else
			{
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = (BYTE)((ImageDouble[y][x]-dMin)*(GRAY_CNT-1)/(dMax-dMin));
				pDoc->m_Image[nCurrentImage].Image1D[Pos++] = (BYTE)((ImageDouble[y][x]-dMin)*(GRAY_CNT-1)/(dMax-dMin));
				pDoc->m_Image[nCurrentImage].Image1D[Pos] = (BYTE)((ImageDouble[y][x]-dMin)*(GRAY_CNT-1)/(dMax-dMin));
			}
		}

	pDoc->m_nImageCnt++;
	Invalidate(bErase);

	return true;
}

void CImageProcessingView::GetViewRegionSelected(UINT nFlags, CPoint point)
{
	CPoint CurrentScrollPoint= GetScrollPosition();

	int i;

	if(m_bEditRegionState)
	{
		double MinDist;

		for(i = 0 ; i < m_ViewRegion.nCnt ; i++)
		{
			CRgn RgnRegion, InnerRgnRegion;
			CPoint pt[MAX_REGION_POINT], InnerPt[MAX_REGION_POINT], Center(0, 0);
			double theta = m_ViewRegion.Rotate[i];

			int k;
			for(k = 0 ; k < m_ViewRegion.nPtCnt[i] ; k++)
			{
				Center.x += m_ViewRegion.Pt[i][k].x;
				Center.y += m_ViewRegion.Pt[i][k].y;
			}

			Center.x /= m_ViewRegion.nPtCnt[i];
			Center.y /= m_ViewRegion.nPtCnt[i];

			if(m_ViewRegion.nType[i] == RT_RECT)
			{
				pt[0].x = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][0].y - Center.y)*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[0].y = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][0].y - Center.y)*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				pt[1].x = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][0].y - Center.y)*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[1].y = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][0].y - Center.y)*cos(theta) 
					+ Center.y - CurrentScrollPoint.y);

				pt[2].x = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][1].y - Center.y)*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[2].y = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][1].y - Center.y)*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				pt[3].x = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][1].y - Center.y)*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[3].y = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][1].y - Center.y)*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				InnerPt[0].x = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*0.9*cos(theta) - (m_ViewRegion.Pt[i][0].y - Center.y)*0.9*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				InnerPt[0].y = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*0.9*sin(theta) + (m_ViewRegion.Pt[i][0].y - Center.y)*0.9*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				InnerPt[1].x = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*0.9*cos(theta) - (m_ViewRegion.Pt[i][0].y - Center.y)*0.9*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				InnerPt[1].y = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*0.9*sin(theta) + (m_ViewRegion.Pt[i][0].y - Center.y)*0.9*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				InnerPt[2].x = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*0.9*cos(theta) - (m_ViewRegion.Pt[i][1].y - Center.y)*0.9*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				InnerPt[2].y = (LONG)((m_ViewRegion.Pt[i][1].x - Center.x)*0.9*sin(theta) + (m_ViewRegion.Pt[i][1].y - Center.y)*0.9*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				InnerPt[3].x = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*0.9*cos(theta) - (m_ViewRegion.Pt[i][1].y - Center.y)*0.9*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				InnerPt[3].y = (LONG)((m_ViewRegion.Pt[i][0].x - Center.x)*0.9*sin(theta) + (m_ViewRegion.Pt[i][1].y - Center.y)*0.9*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				MinDist = Dist(pt[0].x, pt[0].y, point.x, point.y);
				m_nMinDistIndexOfViewRegion = 0;
				for(k = 1 ; k < 4 ; k++)
				{
					double CurrDist = Dist(pt[k].x, pt[k].y, point.x, point.y);
					if(CurrDist < MinDist) {
						MinDist = CurrDist;
						m_nMinDistIndexOfViewRegion = k;
					}
				}

				RgnRegion.CreatePolygonRgn(pt, 4, WINDING);
				InnerRgnRegion.CreatePolygonRgn(InnerPt, 4, WINDING); 
			}
			else
			{
				for(k = 0 ; k < m_ViewRegion.nPtCnt[i] ; k++)
				{
					pt[k].x = (LONG)((m_ViewRegion.Pt[i][k].x - Center.x)*cos(theta) - (m_ViewRegion.Pt[i][k].y - Center.y)*sin(theta) + Center.x);
					pt[k].y = (LONG)((m_ViewRegion.Pt[i][k].x - Center.x)*sin(theta) + (m_ViewRegion.Pt[i][k].y - Center.y)*cos(theta) + Center.y);

					InnerPt[k].x = (LONG)((m_ViewRegion.Pt[i][k].x - Center.x)*0.9*cos(theta) - (m_ViewRegion.Pt[i][k].y - Center.y)*0.9*sin(theta) + Center.x);
					InnerPt[k].y = (LONG)((m_ViewRegion.Pt[i][k].x - Center.x)*0.9*sin(theta) + (m_ViewRegion.Pt[i][k].y - Center.y)*0.9*cos(theta) + Center.y);
				}

				MinDist = Dist(pt[0].x, pt[0].y, point.x, point.y);
				m_nMinDistIndexOfViewRegion = 0;
				for(k = 1 ; k < m_ViewRegion.nPtCnt[i] ; k++)
				{
					double CurrDist = Dist(pt[k].x, pt[k].y, point.x, point.y);
					if(CurrDist < MinDist) {
						MinDist = CurrDist;
						m_nMinDistIndexOfViewRegion = k;
					}
				}

				RgnRegion.CreatePolygonRgn(pt, m_ViewRegion.nPtCnt[i], WINDING);
				InnerRgnRegion.CreatePolygonRgn(InnerPt, m_ViewRegion.nPtCnt[i], WINDING);
			}
		
			if(Dist(Center.x-CurrentScrollPoint.x, Center.y-CurrentScrollPoint.y, point.x, point.y) < m_nRadiusForSel)
			{
				m_nViewRegionSelected = i;
				m_nEditRegionMode = ERM_ROTATE;
				break;
			}

			if(InnerRgnRegion.PtInRegion(point))
			{
				m_nViewRegionSelected = i;
				m_nEditRegionMode = ERM_MOVE;
				break;
			}
	
			if(RgnRegion.PtInRegion(point))
			{
				m_nViewRegionSelected = i;

				if(MinDist < m_nRadiusForSel*2)
					m_nEditRegionMode = ERM_PT_MOVE;
				else
					m_nEditRegionMode = ERM_SCALE;
				
				break;
			}
		}
	}
}

void CImageProcessingView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint CurrentScrollPoint= GetScrollPosition();

	m_nViewRegionSelected = -1;

	GetViewRegionSelected(nFlags, point);

	m_nCurrentImage = GetCurrentImageRect(point);

	if((m_nDrawRegion == RECT_REGION_DRAW || m_nDrawRegion == REGION_DRAW) && m_ViewRegion.nCnt < MAX_REGION)
	{
		if(m_nDrawnPointCnt == 0) 
		{
			if(m_nDrawRegion == RECT_REGION_DRAW)
			{
				m_ViewRegion.nType[m_ViewRegion.nCnt] = RT_RECT;
				m_ViewRegion.nPtCnt[m_ViewRegion.nCnt] = 2;
			}
			else
			{
				m_ViewRegion.nType[m_ViewRegion.nCnt] = RT_PT_REGION;
				m_ViewRegion.nPtCnt[m_ViewRegion.nCnt] = m_nPtCntOfCurrRegoin;
			}

			m_ViewRegion.Pt[m_ViewRegion.nCnt][0] = point + CurrentScrollPoint;

			m_PrevPoint[0] = point;
			m_PrevPoint[1] = point;

			if(m_nDrawRegion == RECT_REGION_DRAW)
				m_nDrawnPointCnt = 1;

			SetCapture();
		}
	}
	else if(m_nViewRegionSelected >= 0)
	{
		m_PrevViewRegionRotate = m_ViewRegion.Rotate[m_nViewRegionSelected];
		m_PrevMove = 0;
		m_GrabPoint = point;
		m_PrevScale = 1;
		m_PrevPtMove = 0;

		SetCapture();
	}
	else if(m_nSelectRegion > 0)
	{
		m_rtSelectRegion.left = point.x;
		m_rtSelectRegion.top = point.y;
		m_PrevRect = CRect(0, 0, 0, 0);
		SetCapture();	
	}
	else if(m_nCurrentImage > -1)
	{
		m_SaveStartPoint = point;
		m_PrevRect = CRect(0, 0, 0, 0);
		SetCapture();	
	}
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CImageProcessingView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPoint CurrentScrollPoint= GetScrollPosition();

	if((m_nDrawRegion == RECT_REGION_DRAW || m_nDrawRegion == REGION_DRAW) && m_ViewRegion.nCnt < MAX_REGION)
	{
		CPoint CurrentScrollPoint = GetScrollPosition();

		m_ViewRegion.Pt[m_ViewRegion.nCnt][m_nDrawnPointCnt].x = point.x + CurrentScrollPoint.x;
		m_ViewRegion.Pt[m_ViewRegion.nCnt][m_nDrawnPointCnt].y = point.y + CurrentScrollPoint.y;

		m_PrevPoint[m_nDrawnPointCnt] = point;
		if(m_nDrawnPointCnt < MAX_REGION_POINT-1) m_PrevPoint[m_nDrawnPointCnt+1] = point;

		m_nDrawnPointCnt++;

		if(m_nDrawnPointCnt == m_ViewRegion.nPtCnt[m_ViewRegion.nCnt])
		{
			m_ViewRegion.Rotate[m_ViewRegion.nCnt] = 0;

			if(m_ViewRegion.nType[m_ViewRegion.nCnt] == RT_RECT)
			{
				if(m_ViewRegion.Pt[m_ViewRegion.nCnt][0].x > m_ViewRegion.Pt[m_ViewRegion.nCnt][1].x) {
					int temp = m_ViewRegion.Pt[m_ViewRegion.nCnt][0].x;
					m_ViewRegion.Pt[m_ViewRegion.nCnt][0].x = m_ViewRegion.Pt[m_ViewRegion.nCnt][1].x;
					m_ViewRegion.Pt[m_ViewRegion.nCnt][1].x = temp;
				}

				if(m_ViewRegion.Pt[m_ViewRegion.nCnt][0].y > m_ViewRegion.Pt[m_ViewRegion.nCnt][1].y) {
					int temp = m_ViewRegion.Pt[m_ViewRegion.nCnt][0].y;
					m_ViewRegion.Pt[m_ViewRegion.nCnt][0].y = m_ViewRegion.Pt[m_ViewRegion.nCnt][1].y;
					m_ViewRegion.Pt[m_ViewRegion.nCnt][1].y = temp;
				}
		}

			m_ViewRegion.nCnt++;
			m_nDrawRegion = NOT_DRAW;
		}

		ReleaseCapture();
		Invalidate();
	}
	else if(m_bEditRegionState && m_nViewRegionSelected >= 0)
	{
		if(m_nEditRegionMode == ERM_ROTATE)
		{
			CPoint Center(0, 0);

			int k;
			for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
			{
				Center.x += m_ViewRegion.Pt[m_nViewRegionSelected][k].x;
				Center.y += m_ViewRegion.Pt[m_nViewRegionSelected][k].y;
			}

			Center.x /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];
			Center.y /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];

			Center -= CurrentScrollPoint;

			m_ViewRegion.Rotate[m_nViewRegionSelected] = atan2((double)point.y - Center.y, (double)point.x - Center.x);
		}
		else if(m_nEditRegionMode == ERM_MOVE)
		{
			int k;
			for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
				m_ViewRegion.Pt[m_nViewRegionSelected][k] += (point-m_GrabPoint);
		}
		else if(m_nEditRegionMode == ERM_SCALE)
		{
			CPoint Center(0, 0);

			int k;
			for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
			{
				Center.x += m_ViewRegion.Pt[m_nViewRegionSelected][k].x;
				Center.y += m_ViewRegion.Pt[m_nViewRegionSelected][k].y;
			}

			Center.x /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];
			Center.y /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];

			Center -= CurrentScrollPoint;

			double Scale;
			if(m_GrabPoint == Center)
				Scale = Dist(point.x, point.y, Center.x, Center.y);
			else
				Scale = Dist(point.x, point.y, Center.x, Center.y)/Dist(m_GrabPoint.x, m_GrabPoint.y, Center.x, Center.y);

			for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
			{
				m_ViewRegion.Pt[m_nViewRegionSelected][k].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*Scale + Center.x);
				m_ViewRegion.Pt[m_nViewRegionSelected][k].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*Scale + Center.y);
			}
		}
		else if(m_nEditRegionMode == ERM_PT_MOVE)
		{
			CPoint Center(0, 0);

			int k;
			for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
			{
				Center.x += m_ViewRegion.Pt[m_nViewRegionSelected][k].x;
				Center.y += m_ViewRegion.Pt[m_nViewRegionSelected][k].y;
			}

			Center.x /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];
			Center.y /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];

			Center -= CurrentScrollPoint;

			CPoint Extend;
			Extend.x = (LONG)((point.x - m_GrabPoint.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) +
				(point.y-m_GrabPoint.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]));
			Extend.y = (LONG)(-(point.x - m_GrabPoint.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) +
				(point.y-m_GrabPoint.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]));

			if(m_ViewRegion.nType[m_nViewRegionSelected] == RT_RECT)
			{
				int SignX = 1, SignY = 1;
				if(m_nMinDistIndexOfViewRegion == 0)
				{
					if(m_GrabPoint.x > Center.x) SignX = -1;
					if(m_GrabPoint.y > Center.y) SignY = -1;
				}
				if(m_nMinDistIndexOfViewRegion == 1)
				{
					if(m_GrabPoint.x < Center.x) SignX = -1;
					if(m_GrabPoint.y > Center.y) SignY = -1;
				}
				if(m_nMinDistIndexOfViewRegion == 2)
				{
					if(m_GrabPoint.x < Center.x) SignX = -1;
					if(m_GrabPoint.y < Center.y) SignY = -1;
				}
				if(m_nMinDistIndexOfViewRegion == 3)
				{
					if(m_GrabPoint.x > Center.x) SignX = -1;
					if(m_GrabPoint.y < Center.y) SignY = -1;
				}
					
//				m_ViewRegion.Pt[m_nViewRegionSelected][0].x += SignX*(Extend.x)/2;
//				m_ViewRegion.Pt[m_nViewRegionSelected][0].y += SignY*(Extend.y)/2;
//				m_ViewRegion.Pt[m_nViewRegionSelected][1].x += SignX*(Extend.x)/2;
//				m_ViewRegion.Pt[m_nViewRegionSelected][1].y += SignY*(Extend.y)/2;

				switch(m_nMinDistIndexOfViewRegion)
				{

					case 0:
						m_ViewRegion.Pt[m_nViewRegionSelected][0].x += (SignX*Extend.x + Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][0].y += (SignY*Extend.y + Extend.y)/2;

						m_ViewRegion.Pt[m_nViewRegionSelected][1].x += (SignX*Extend.x - Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][1].y += (SignY*Extend.y - Extend.y)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][0].x += (Extend.x)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][0].y += (Extend.y)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][1].x -= (Extend.x)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][1].y -= (Extend.y)/2;
						break;
					case 1:
						m_ViewRegion.Pt[m_nViewRegionSelected][0].x += (SignX*Extend.x - Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][0].y += (SignY*Extend.y + Extend.y)/2;

						m_ViewRegion.Pt[m_nViewRegionSelected][1].x += (SignX*Extend.x + Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][1].y += (SignY*Extend.y - Extend.y)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][0].y += (Extend.y)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][1].x += (Extend.x)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][0].x -= (Extend.x)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][1].y -= (Extend.y)/2;
						break;
					case 2:
						m_ViewRegion.Pt[m_nViewRegionSelected][0].x += (SignX*Extend.x - Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][0].y += (SignY*Extend.y - Extend.y)/2;

						m_ViewRegion.Pt[m_nViewRegionSelected][1].x += (SignX*Extend.x + Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][1].y += (SignY*Extend.y + Extend.y)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][1].x += (Extend.x)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][1].y += (Extend.y)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][0].x -= (Extend.x)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][0].y -= (Extend.y)/2;
	
						break;
					case 3:
						m_ViewRegion.Pt[m_nViewRegionSelected][0].x += (SignX*Extend.x + Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][0].y += (SignY*Extend.y - Extend.y)/2;

						m_ViewRegion.Pt[m_nViewRegionSelected][1].x += (SignX*Extend.x - Extend.x)/2;
						m_ViewRegion.Pt[m_nViewRegionSelected][1].y += (SignY*Extend.y + Extend.y)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][0].x += (Extend.x)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][1].y += (Extend.y)/2;

//						m_ViewRegion.Pt[m_nViewRegionSelected][0].y -= (Extend.y)/2;
//						m_ViewRegion.Pt[m_nViewRegionSelected][1].x -= (Extend.x)/2;
						break;
				}				
			}
			else
			{
				CPoint pt[MAX_REGION_POINT];

				for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
				{
					pt[k].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
							+ Center.x - CurrentScrollPoint.x);
					pt[k].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
							+ Center.y - CurrentScrollPoint.y);

					if(k == m_nMinDistIndexOfViewRegion)
						pt[k] += (point-m_GrabPoint);
				}

				for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
					m_ViewRegion.Pt[m_nViewRegionSelected][k] = pt[k];

				m_ViewRegion.Rotate[m_nViewRegionSelected] = 0;
			}
		}

		if(m_ViewRegion.nType[m_nViewRegionSelected] == RT_RECT)
		{
			if(m_ViewRegion.Pt[m_nViewRegionSelected][0].x > m_ViewRegion.Pt[m_nViewRegionSelected][1].x) {
				int temp = m_ViewRegion.Pt[m_nViewRegionSelected][0].x;
				m_ViewRegion.Pt[m_nViewRegionSelected][0].x = m_ViewRegion.Pt[m_nViewRegionSelected][1].x;
				m_ViewRegion.Pt[m_nViewRegionSelected][1].x = temp;
			}

			if(m_ViewRegion.Pt[m_nViewRegionSelected][0].y > m_ViewRegion.Pt[m_nViewRegionSelected][1].y) {
				int temp = m_ViewRegion.Pt[m_nViewRegionSelected][0].y;
				m_ViewRegion.Pt[m_nViewRegionSelected][0].y = m_ViewRegion.Pt[m_nViewRegionSelected][1].y;
				m_ViewRegion.Pt[m_nViewRegionSelected][1].y = temp;
			}
		}

		m_nViewRegionSelected = -1;
		ReleaseCapture();
		Invalidate();
	}
	else if(m_nSelectRegion > 0)
	{
		m_rtSelectRegion.right = point.x;
		m_rtSelectRegion.bottom = point.y;
		
		if(m_rtSelectRegion.right < m_rtSelectRegion.left)
		{
			int temp = m_rtSelectRegion.right;
			m_rtSelectRegion.right = m_rtSelectRegion.left;
			m_rtSelectRegion.left = temp;
		}

		if(m_rtSelectRegion.bottom < m_rtSelectRegion.top)
		{
			int temp = m_rtSelectRegion.bottom;
			m_rtSelectRegion.bottom = m_rtSelectRegion.top;
			m_rtSelectRegion.top = temp;
		}

		SelOperation();

		m_nSelectRegion = 0;
		ReleaseCapture();	
	}
	else if(m_nCurrentImage > -1)
	{
		if(!m_bEditRegionState)
		{
			pDoc->m_Image[m_nCurrentImage].nPosX += point.x - m_SaveStartPoint.x;
			pDoc->m_Image[m_nCurrentImage].nPosY += point.y - m_SaveStartPoint.y;
		}

		m_nCurrentImage = -1;

		ReleaseCapture();	
		Invalidate();		
	}
	
	CScrollView::OnLButtonUp(nFlags, point);
}

void CImageProcessingView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPoint CurrentScrollPoint= GetScrollPosition();

	if(m_bEditRegionState && m_nViewRegionSelected >= 0 && (nFlags & MK_LBUTTON ))
	{
		CClientDC dc(this);
		CBrush *pOldBrush;

		dc.SetROP2(R2_NOT);
		pOldBrush = (CBrush *)dc.SelectStockObject(NULL_BRUSH);

		CPoint pt[MAX_REGION_POINT], Center(0, 0), MovePoint;
		double theta;
		double Scale;

		int k;
		for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
		{
			Center.x += m_ViewRegion.Pt[m_nViewRegionSelected][k].x;
			Center.y += m_ViewRegion.Pt[m_nViewRegionSelected][k].y;
		}

		Center.x /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];
		Center.y /= m_ViewRegion.nPtCnt[m_nViewRegionSelected];

		if(m_nEditRegionMode == ERM_PT_MOVE)
		{
	
			if(m_ViewRegion.nType[m_nViewRegionSelected] == RT_RECT)
			{
				CPoint p0 = m_ViewRegion.Pt[m_nViewRegionSelected][0];
				CPoint p1 = m_ViewRegion.Pt[m_nViewRegionSelected][1];

				pt[0].x = (LONG)((p0.x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (p0.y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.x - CurrentScrollPoint.x);
				pt[0].y = (LONG)((p0.x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (p0.y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.y - CurrentScrollPoint.y);

				pt[1].x = (LONG)((p1.x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (p0.y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.x - CurrentScrollPoint.x);
				pt[1].y = (LONG)((p1.x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (p0.y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.y - CurrentScrollPoint.y);

				pt[2].x = (LONG)((p1.x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (p1.y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.x - CurrentScrollPoint.x);
				pt[2].y = (LONG)((p1.x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (p1.y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.y - CurrentScrollPoint.y);

				pt[3].x = (LONG)((p0.x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (p1.y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.x - CurrentScrollPoint.x);
				pt[3].y = (LONG)((p0.x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (p1.y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.y - CurrentScrollPoint.y);

				k = m_nMinDistIndexOfViewRegion;

				dc.MoveTo(pt[(k+3)%4]);
				dc.LineTo(pt[k]+m_PrevPtMove);
				dc.LineTo(pt[(k+1)%4]);

				pt[k] += (point-m_GrabPoint);

				dc.MoveTo(pt[(k+3)%4]);
				dc.LineTo(pt[k]);
				dc.LineTo(pt[(k+1)%4]);
				
			}
			else
			{
				k = m_nMinDistIndexOfViewRegion;
				pt[k].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
						+ Center.x - CurrentScrollPoint.x);
				pt[k].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
						+ Center.y - CurrentScrollPoint.y);
								
				int kk1 = (k-1+m_ViewRegion.nPtCnt[m_nViewRegionSelected])%m_ViewRegion.nPtCnt[m_nViewRegionSelected];
				pt[kk1].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][kk1].x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (m_ViewRegion.Pt[m_nViewRegionSelected][kk1].y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.x - CurrentScrollPoint.x);
				pt[kk1].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][kk1].x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (m_ViewRegion.Pt[m_nViewRegionSelected][kk1].y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.y - CurrentScrollPoint.y);

				int kk2 = (k+1+m_ViewRegion.nPtCnt[m_nViewRegionSelected])%m_ViewRegion.nPtCnt[m_nViewRegionSelected];
				pt[kk2].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][kk2].x - Center.x)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected]) - (m_ViewRegion.Pt[m_nViewRegionSelected][kk2].y - Center.y)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.x - CurrentScrollPoint.x);
				pt[kk2].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][kk2].x - Center.x)*sin(m_ViewRegion.Rotate[m_nViewRegionSelected]) + (m_ViewRegion.Pt[m_nViewRegionSelected][kk2].y - Center.y)*cos(m_ViewRegion.Rotate[m_nViewRegionSelected])
					+ Center.y - CurrentScrollPoint.y);

				dc.MoveTo(pt[kk1]);
				dc.LineTo(pt[k]+m_PrevPtMove);
				dc.LineTo(pt[kk2]);

				pt[k] += (point-m_GrabPoint);

				dc.MoveTo(pt[kk1]);
				dc.LineTo(pt[k]);
				dc.LineTo(pt[kk2]);
			}

			m_PrevPtMove = (point-m_GrabPoint);

		}
		else
		{
			if(m_nEditRegionMode == ERM_ROTATE)
			{
				theta = atan2((double)point.y-(Center.y-CurrentScrollPoint.y), (double)point.x-(Center.x-CurrentScrollPoint.x));
				MovePoint = 0;
				Scale = 1;
			}
			else if(m_nEditRegionMode == ERM_MOVE)
			{
				theta = m_ViewRegion.Rotate[m_nViewRegionSelected];
				MovePoint = point-m_GrabPoint;
				Scale = 1;
			}
			else if(m_nEditRegionMode == ERM_SCALE)
			{
				theta = m_ViewRegion.Rotate[m_nViewRegionSelected];
				MovePoint = 0;
				
				if(m_GrabPoint == Center)
					Scale = Dist(point.x, point.y, Center.x-CurrentScrollPoint.x, Center.y-CurrentScrollPoint.y);
				else
					Scale = Dist(point.x, point.y, Center.x-CurrentScrollPoint.x, Center.y-CurrentScrollPoint.y)
							/ Dist(m_GrabPoint.x, m_GrabPoint.y, Center.x-CurrentScrollPoint.x, Center.y-CurrentScrollPoint.y);
			}
			else if(m_nEditRegionMode == ERM_PT_MOVE)
			{
				theta = m_ViewRegion.Rotate[m_nViewRegionSelected];
				MovePoint = 0;
				Scale = 1;
			}

			if(m_ViewRegion.nType[m_nViewRegionSelected] == RT_RECT)
			{

				pt[0].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*m_PrevScale*cos(m_PrevViewRegionRotate) - (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*m_PrevScale*sin(m_PrevViewRegionRotate)
					+ Center.x - CurrentScrollPoint.x);
				pt[0].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*m_PrevScale*sin(m_PrevViewRegionRotate) + (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*m_PrevScale*cos(m_PrevViewRegionRotate) 
					+ Center.y - CurrentScrollPoint.y);

				pt[1].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*m_PrevScale*cos(m_PrevViewRegionRotate) - (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*m_PrevScale*sin(m_PrevViewRegionRotate)
					+ Center.x - CurrentScrollPoint.x);
				pt[1].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*m_PrevScale*sin(m_PrevViewRegionRotate) + (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*m_PrevScale*cos(m_PrevViewRegionRotate) 
					+ Center.y - CurrentScrollPoint.y);

				pt[2].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*m_PrevScale*cos(m_PrevViewRegionRotate) - (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*m_PrevScale*sin(m_PrevViewRegionRotate)
					+ Center.x - CurrentScrollPoint.x);
				pt[2].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*m_PrevScale*sin(m_PrevViewRegionRotate) + (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*m_PrevScale*cos(m_PrevViewRegionRotate)
					+ Center.y - CurrentScrollPoint.y);

				pt[3].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*m_PrevScale*cos(m_PrevViewRegionRotate) - (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*m_PrevScale*sin(m_PrevViewRegionRotate)
					+ Center.x - CurrentScrollPoint.x);
				pt[3].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*m_PrevScale*sin(m_PrevViewRegionRotate) + (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*m_PrevScale*cos(m_PrevViewRegionRotate)
					+ Center.y - CurrentScrollPoint.y);

				dc.MoveTo(pt[3]+m_PrevMove);
				for(k = 0 ; k < 4 ; k++)
					dc.LineTo(pt[k]+m_PrevMove);

				pt[0].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*Scale*cos(theta) - (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*Scale*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[0].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*Scale*sin(theta) + (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*Scale*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				pt[1].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*Scale*cos(theta) - (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*Scale*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[1].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*Scale*sin(theta) + (m_ViewRegion.Pt[m_nViewRegionSelected][0].y - Center.y)*Scale*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				pt[2].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*Scale*cos(theta) - (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*Scale*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[2].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][1].x - Center.x)*Scale*sin(theta) + (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*Scale*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				pt[3].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*Scale*cos(theta) - (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*Scale*sin(theta)
					+ Center.x - CurrentScrollPoint.x);
				pt[3].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][0].x - Center.x)*Scale*sin(theta) + (m_ViewRegion.Pt[m_nViewRegionSelected][1].y - Center.y)*Scale*cos(theta)
					+ Center.y - CurrentScrollPoint.y);

				dc.MoveTo(pt[3]+MovePoint);
				for(k = 0 ; k < 4 ; k++)
					dc.LineTo(pt[k]+MovePoint);
			}
			else
			{
				for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
				{
					pt[k].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*m_PrevScale*cos(m_PrevViewRegionRotate) - (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*m_PrevScale*sin(m_PrevViewRegionRotate)
						+ Center.x - CurrentScrollPoint.x);
					pt[k].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*m_PrevScale*sin(m_PrevViewRegionRotate) + (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*m_PrevScale*cos(m_PrevViewRegionRotate)
						+ Center.y - CurrentScrollPoint.y);
				}

				dc.MoveTo(pt[m_ViewRegion.nPtCnt[m_nViewRegionSelected]-1]+m_PrevMove);
				for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
					dc.LineTo(pt[k]+m_PrevMove);


				for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
				{
					pt[k].x = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*Scale*cos(theta) - (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*Scale*sin(theta)
						+ Center.x - CurrentScrollPoint.x);
					pt[k].y = (LONG)((m_ViewRegion.Pt[m_nViewRegionSelected][k].x - Center.x)*Scale*sin(theta) + (m_ViewRegion.Pt[m_nViewRegionSelected][k].y - Center.y)*Scale*cos(theta)
						+ Center.y - CurrentScrollPoint.y);
				}

				dc.MoveTo(pt[m_ViewRegion.nPtCnt[m_nViewRegionSelected]-1]+MovePoint);
				for(k = 0 ; k < m_ViewRegion.nPtCnt[m_nViewRegionSelected] ; k++)
					dc.LineTo(pt[k]+MovePoint);
			}
		}

		dc.SelectObject(pOldBrush);

		m_PrevViewRegionRotate = theta;		
		m_PrevMove = MovePoint;
		m_PrevScale = Scale;
	}
	else if(m_nDrawRegion == RECT_REGION_DRAW && m_ViewRegion.nCnt < MAX_REGION  && (nFlags & MK_LBUTTON))
	{
		CClientDC dc(this);
		CBrush *pOldBrush;

		dc.SetROP2(R2_NOT);
		pOldBrush = (CBrush *)dc.SelectStockObject(NULL_BRUSH);
		
		CRect rt;

		rt = CRect(m_PrevPoint[0].x, m_PrevPoint[0].y, m_PrevPoint[1].x, m_PrevPoint[1].y);
		dc.Rectangle(rt);

		rt = CRect(m_PrevPoint[0].x, m_PrevPoint[0].y, point.x, point.y);
		dc.Rectangle(rt);

		m_PrevPoint[1] = point;

		dc.SelectObject(pOldBrush);
	}
	else if(m_nDrawRegion == REGION_DRAW && m_ViewRegion.nCnt < MAX_REGION)
	{
		CClientDC dc(this);
		CPen Pen(PS_SOLID, 1, RGB(255, 0, 0));
				
		int k;

		dc.SelectObject(&Pen);

		dc.MoveTo(m_PrevPoint[0]);
		for(k = 0 ; k < m_nDrawnPointCnt ; k++)
			dc.LineTo(m_PrevPoint[k]);

		dc.SetROP2(R2_NOT);

		if(m_nDrawnPointCnt >= 1)
		{
			dc.MoveTo(m_PrevPoint[m_nDrawnPointCnt-1]);
			dc.LineTo(m_PrevPoint[m_nDrawnPointCnt]);

			dc.MoveTo(m_PrevPoint[m_nDrawnPointCnt-1]);
			dc.LineTo(point);

			m_PrevPoint[m_nDrawnPointCnt] = point;
		}
	}
	else if(m_nSelectRegion > 0 && (nFlags & MK_LBUTTON ))
	{
		CClientDC dc(this);

		CRect rt(m_rtSelectRegion.left, m_rtSelectRegion.top, point.x, point.y);

		CBrush *pOldBrush;

		dc.SetROP2(R2_NOT);
		pOldBrush = (CBrush *)dc.SelectStockObject(NULL_BRUSH);
		
		dc.Rectangle(m_PrevRect);
		dc.Rectangle(rt);

		dc.SelectObject(pOldBrush);

		m_PrevRect = rt;
	}
	else if(m_nCurrentImage > -1 && (nFlags & MK_LBUTTON ) && !m_bEditRegionState)	// 현재 마우스 왼쪽 버튼이 눌려진 상태
	{
		CClientDC dc(this);

		CRect rt(pDoc->m_Image[m_nCurrentImage].nPosX, pDoc->m_Image[m_nCurrentImage].nPosY, 
			pDoc->m_Image[m_nCurrentImage].nPosX + pDoc->m_Image[m_nCurrentImage].nW, pDoc->m_Image[m_nCurrentImage].nPosY+pDoc->m_Image[m_nCurrentImage].nH);

		rt -= CurrentScrollPoint;
		rt += point -m_SaveStartPoint;

		CBrush *pOldBrush;

		dc.SetROP2(R2_NOT);
		pOldBrush = (CBrush *)dc.SelectStockObject(NULL_BRUSH);
		
		dc.Rectangle(m_PrevRect);
		dc.Rectangle(rt);

		dc.SelectObject(pOldBrush);

		m_PrevRect = rt;
	}
	
	CScrollView::OnMouseMove(nFlags, point);
}

void CImageProcessingView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_nCurrentImage = GetCurrentImageRect(point);

	if(m_bEditRegionState)
	{
		GetViewRegionSelected(nFlags, point);

		if(m_nViewRegionSelected >= 0)
		{
			int i, k;
			for(i = m_nViewRegionSelected ; i < MAX_REGION-1 ; i++)
			{
				for(k = 0 ; k < MAX_REGION_POINT ; k++)
					m_ViewRegion.Pt[i][k] = m_ViewRegion.Pt[i+1][k];

				m_ViewRegion.nPtCnt[i] = m_ViewRegion.nPtCnt[i+1];
				m_ViewRegion.nType[i] = m_ViewRegion.nType[i+1];
				m_ViewRegion.Rotate[i] = m_ViewRegion.Rotate[i+1];
			}

			m_ViewRegion.nCnt--;
			m_nViewRegionSelected = -1;

			Invalidate();	
		}	
	}
	else if(m_nCurrentImage > -1 && !m_bEditRegionState) 
	{
		ImageInfo SaveImage = pDoc->m_Image[m_nCurrentImage];
		for(int i = 0 ; i < pDoc->m_nImageCnt ; i++)
			if(i != m_nCurrentImage) delete [] pDoc->m_Image[i].Image1D;

		pDoc->m_Image[0] = SaveImage;
		pDoc->m_nImageCnt = 1;

		m_nCurrentImage = -1;

		Invalidate();
	}
	
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CImageProcessingView::OnGrayDuplicate() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray);

	DisplayCimage2D(ImageGray, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);
}

void CImageProcessingView::OnColorDuplicate() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageRed, **ImageGreen, **ImageBlue;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageRed = cmatrix(nH, nW);
	ImageGreen = cmatrix(nH, nW);
	ImageBlue = cmatrix(nH, nW);

	GetCurrentImageColor(ImageRed, ImageGreen, ImageBlue);

	DisplayCimage2DColor(ImageRed, ImageGreen, ImageBlue, nW, nH, nPosX+nW, nPosY);

	free_cmatrix(ImageRed, nH, nW);	
	free_cmatrix(ImageGreen, nH, nW);	
	free_cmatrix(ImageBlue, nH, nW);	
}

void CImageProcessingView::OnChap2() 
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(!pMain->m_pChap2->m_bCreated)
	{
		pMain->m_pChap2->Create(CChap2::IDD, pMain);
		pMain->m_pChap2->ShowWindow(SW_SHOW);

		pMain->m_pChap2->m_bCreated = TRUE;
	}	
}

void CImageProcessingView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);
	CMenu* pPopup = menu.GetSubMenu(0);
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y, AfxGetMainWnd());	
}

void CImageProcessingView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetCurrentImageRect(point);

	Invalidate();
	
	m_PastePoint = point;
	
	CScrollView::OnRButtonDown(nFlags, point);
}

void CImageProcessingView::OnScaleX2() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray, **ImageGrayG, **ImageGrayB;
	BYTE **OutputGray, **OutputGrayG, **OutputGrayB;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	ImageGrayG = cmatrix(nH, nW);
	ImageGrayB = cmatrix(nH, nW);

	OutputGray = cmatrix(nH*2, nW*2);
	OutputGrayG = cmatrix(nH*2, nW*2);
	OutputGrayB = cmatrix(nH*2, nW*2);

	GetCurrentImageColor(ImageGray, ImageGrayG, ImageGrayB);

	int x, y;
	for(y = 0 ; y < nH*2 ; y++)
		for(x = 0 ; x < nW*2 ; x++)
		{
			OutputGray[y][x] = ImageGray[y/2][x/2];
			OutputGrayG[y][x] = ImageGrayG[y/2][x/2];
			OutputGrayB[y][x] = ImageGrayB[y/2][x/2];
		}

	DisplayCimage2DColor(OutputGray, OutputGrayG, OutputGrayB, nW*2, nH*2, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);
	free_cmatrix(ImageGrayG, nH, nW);
	free_cmatrix(ImageGrayB, nH, nW);
	free_cmatrix(OutputGray, nH*2, nW*2);
	free_cmatrix(OutputGrayG, nH*2, nW*2);
	free_cmatrix(OutputGrayB, nH*2, nW*2);
}

void CImageProcessingView::OnScaleHalf() 
{
	int nW, nH, nPosX, nPosY;
	BYTE **ImageGray, **ImageGrayG, **ImageGrayB;
	BYTE **OutputGray, **OutputGrayG, **OutputGrayB;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	ImageGray = cmatrix(nH, nW);
	ImageGrayG = cmatrix(nH, nW);
	ImageGrayB = cmatrix(nH, nW);
	OutputGray = cmatrix(nH/2, nW/2);
	OutputGrayG = cmatrix(nH/2, nW/2);
	OutputGrayB = cmatrix(nH/2, nW/2);

	GetCurrentImageColor(ImageGray, ImageGrayG, ImageGrayB);

	int x, y;
	for(y = 0 ; y < nH/2 ; y++)
		for(x = 0 ; x < nW/2 ; x++)
		{
			OutputGray[y][x] = ImageGray[y*2][x*2];
			OutputGrayG[y][x] = ImageGrayG[y*2][x*2];
			OutputGrayB[y][x] = ImageGrayB[y*2][x*2];
		}


	DisplayCimage2DColor(OutputGray, OutputGrayG, OutputGrayB, nW/2, nH/2, nPosX+nW, nPosY);

	free_cmatrix(ImageGray, nH, nW);
	free_cmatrix(ImageGrayG, nH, nW);
	free_cmatrix(ImageGrayB, nH, nW);
	free_cmatrix(OutputGray, nH/2, nW/2);	
	free_cmatrix(OutputGrayG, nH/2, nW/2);	
	free_cmatrix(OutputGrayB, nH/2, nW/2);	
}

void CImageProcessingView::OnEditCopy() 
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(pDoc->m_nImageCnt <= 0) return;

	HGLOBAL hGlobal;
	DWORD dwBitsSize;
	LPSTR pGlobal;

	int nW = pDoc->m_Image[pDoc->m_nImageCnt-1].nW;
	int nH = pDoc->m_Image[pDoc->m_nImageCnt-1].nH;
	UINT nSize = GetBmp24Size(nW, nH);

	dwBitsSize = sizeof(BITMAPINFOHEADER) + nSize;

	m_BmiHeader.biWidth = nW;
	m_BmiHeader.biHeight = nH;
	m_BmiHeader.biSizeImage = nSize;

	hGlobal = (HGLOBAL)GlobalAlloc(GHND, dwBitsSize);
	
	if(hGlobal == NULL) return;

	pGlobal = (LPSTR)GlobalLock((HGLOBAL)hGlobal);

	memcpy(pGlobal, &m_BmiHeader, sizeof(BITMAPINFOHEADER));

	memcpy(pGlobal + sizeof(BITMAPINFOHEADER), 
		pDoc->m_Image[pDoc->m_nImageCnt-1].Image1D, nSize); 

	if(OpenClipboard())
	{
		EmptyClipboard();

		SetClipboardData(CF_DIB, hGlobal);

		CloseClipboard();
	}

	GlobalUnlock(hGlobal);
}

void CImageProcessingView::OnEditCut() 
{
	SendMessage(WM_COMMAND, ID_EDIT_COPY);

	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if(pDoc->m_nImageCnt <= 0) return;

	delete [] pDoc->m_Image[pDoc->m_nImageCnt-1].Image1D;
	pDoc->m_nImageCnt--;

	Invalidate();
}

#define IS_WIN30_DIB(lpbi) ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  // DIB bit count

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}

WORD WINAPI PaletteSize(LPSTR lpbi)
{

   if (((LPBITMAPINFOHEADER)(lpbi))->biCompression == BI_BITFIELDS)
	   return (sizeof(DWORD) * 3) + (::DIBNumColors (lpbi) * sizeof (RGBQUAD));
   else
   {		
	   if (IS_WIN30_DIB (lpbi))
		  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
	   else
		  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
   }
}

LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}

void CImageProcessingView::OnEditPaste() 
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	HGLOBAL hGlobal;
	CPoint CurrentScrollPoint = GetScrollPosition();

	CPoint Pos;
	if(m_PastePoint != CPoint(-1, -1))
		Pos = m_PastePoint + CurrentScrollPoint;
	else
		Pos = CPoint(0, 0) + CurrentScrollPoint;

	m_PastePoint= CPoint(-1, -1);

	if(!IsClipboardFormatAvailable(CF_DIB)) return; 
	if(!OpenClipboard()) return;  
        
	hGlobal = (HGLOBAL)GetClipboardData(CF_DIB);
	
	if(hGlobal)
	{ 
		LPSTR pGlobal  = (LPSTR)GlobalLock((HGLOBAL)hGlobal);

		if(pDoc->m_nImageCnt >= MAX_IMAGE ||
			*(LPDWORD)(pGlobal) != sizeof(BITMAPINFOHEADER)) {
			GlobalUnlock(hGlobal); 

			CloseClipboard(); 
			return;
		}

		int nH = ((LPBITMAPINFOHEADER)pGlobal)->biHeight;	
		int nW = ((LPBITMAPINFOHEADER)pGlobal)->biWidth;

		int biCompression = ((LPBITMAPINFOHEADER)pGlobal)->biCompression;

		int BitsPixel = ((LPBITMAPINFOHEADER)pGlobal)->biBitCount;

//		if(BitsPixel != 24 && BitsPixel != 32 && BitsPixel != 16) {
//			GlobalUnlock(hGlobal); 

//			CloseClipboard(); 
//			return;
//		}

		pDoc->m_Image[pDoc->m_nImageCnt].Image1D = new unsigned char[GetBmp24Size(nW, nH)];
		pDoc->m_Image[pDoc->m_nImageCnt].nW = nW;
		pDoc->m_Image[pDoc->m_nImageCnt].nH = nH;
		pDoc->m_Image[pDoc->m_nImageCnt].nPosX = Pos.x;
		pDoc->m_Image[pDoc->m_nImageCnt].nPosY = Pos.y;

		if(BitsPixel == 24)
		{
			memcpy(pDoc->m_Image[pDoc->m_nImageCnt].Image1D, pGlobal+sizeof(BITMAPINFOHEADER), GetBmp24Size(nW, nH));
		}
		else if(BitsPixel == 32)
		{
			int x, y;
			int offset, offset2;
			DWORD dwPixelValue;

			LPSTR lpDIBBits = (LPSTR)::FindDIBBits(pGlobal);
			
			for(y = 0 ; y < nH ; y++)
			{
				offset2 = (nW*1) * (nH-y-1);
				for(x = 0 ; x < nW ; x++)
				{
					offset = GetBmp24Pos(nW, nH, x, y);

					//dwPixelValue = ((DWORD *)(pGlobal+sizeof(BITMAPINFOHEADER)))[offset2+x];
					dwPixelValue = ((DWORD *)lpDIBBits)[offset2+x];

					pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = (BYTE)dwPixelValue;
					pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = (BYTE)(dwPixelValue>>8);
					pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset] = (BYTE)(dwPixelValue>>16);
				}
			}
		}
		else if(BitsPixel == 16)
		{
			int x, y;
			int offset, offset2;
			WORD dwPixelValue;

			LPSTR lpDIBBits = (LPSTR)::FindDIBBits(pGlobal);
			
			for(y = 0 ; y < nH ; y++)
			{
				offset2 = (nW+1)/2*2 * (nH-y-1);
				
				for(x = 0 ; x < nW ; x++)
				{
					offset = GetBmp24Pos(nW, nH, x, y);

					//dwPixelValue = ((WORD *)(pGlobal+sizeof(BITMAPINFOHEADER)))[offset2+x];
					dwPixelValue = ((WORD *)lpDIBBits)[offset2+x];

					if(biCompression == BI_BITFIELDS)
					{
						pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = (BYTE)(((dwPixelValue>>0) & 0x001F) * 8);
						pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = (BYTE)(((dwPixelValue>>5) & 0x003F) * 4);
						pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset] = (BYTE)(((dwPixelValue>>11) & 0x001F) * 8);
					}
					else
					{
						pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = (BYTE)(((dwPixelValue>>0) & 0x001F) * 8);
						pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = (BYTE)(((dwPixelValue>>5) & 0x001F) * 8);
						pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset] = (BYTE)(((dwPixelValue>>10) & 0x001F) * 8);
					}
				}
			}
		}
		else
		{
			LPSTR lpDIBPal = pGlobal + sizeof(BITMAPINFOHEADER);
			LPSTR lpDIBBits = (LPSTR)::FindDIBBits(pGlobal);

			int x, y;
			int offset, offset2, i, pix;
			BYTE inbyte;
			int bytesread;

			UINT mask = (1 << BitsPixel) - 1;

			i = 0;
			bytesread = 0;
			for(y = nH-1 ; y >= 0 ; y--)
			{
				offset = (nW*3+3)/4*4 * (nH-1-y);

				int bit_count = 0;
				for(x = 0 ; x < nW ; x++)
				{
					if (bit_count <= 0) {
						bit_count = 8;
						//inbyte = ((BYTE *)(pGlobal+sizeof(BITMAPINFOHEADER)))[i++];
						inbyte = ((BYTE *)lpDIBBits)[i++];
						bytesread++;
					}
					bit_count -= BitsPixel;
					pix = ( inbyte >> bit_count) & mask;

					offset2 = offset + 3*x;
					
					pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset2++] = lpDIBPal[pix*4];
					pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset2++] = lpDIBPal[pix*4+1];
					pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset2] = lpDIBPal[pix*4+2];
				}

				while (bytesread&3) {
					i++;
					bytesread++;
				}
			}
		}

		pDoc->m_nImageCnt++;
		Invalidate();

		GlobalUnlock(hGlobal);  
	} 
    
    CloseClipboard(); 
}

void CImageProcessingView::OnChap3() 
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(!pMain->m_pChap3->m_bCreated)
	{
		pMain->m_pChap3->Create(CChap3::IDD, pMain);
		pMain->m_pChap3->ShowWindow(SW_SHOW);

		pMain->m_pChap3->m_bCreated = TRUE;
	}
}

void CImageProcessingView::OnChap4() 
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(!pMain->m_pChap4->m_bCreated)
	{
		pMain->m_pChap4->Create(CChap4::IDD, pMain);
		pMain->m_pChap4->ShowWindow(SW_SHOW);

		pMain->m_pChap4->m_bCreated = TRUE;
	}	
}

void CImageProcessingView::OnChap5() 
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(!pMain->m_pChap5->m_bCreated)
	{
		pMain->m_pChap5->Create(CChap5::IDD, pMain);
		pMain->m_pChap5->ShowWindow(SW_SHOW);

		pMain->m_pChap5->m_bCreated = TRUE;
	}	
}

void CImageProcessingView::OnApplication() 
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	if(!pMain->m_pApplication->m_bCreated)
	{
		pMain->m_pApplication->Create(CApplication::IDD, pMain);
		pMain->m_pApplication->ShowWindow(SW_SHOW);

		pMain->m_pApplication->m_bCreated = TRUE;
	}	
}

BYTE CImageProcessingView::GetCurrPosColor(int x, int y, int Color)// Color 0 : Red, 1 : Green, 2 : Blue
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	int i, offset;

	for (i = pDoc->m_nImageCnt-1; i >= 0 ; i--) 
	{
		if(	pDoc->m_Image[i].nPosX <= x && 
			pDoc->m_Image[i].nPosX + pDoc->m_Image[i].nW > x &&
			pDoc->m_Image[i].nPosY <= y && 
			pDoc->m_Image[i].nPosY + pDoc->m_Image[i].nH > y)
		{
			offset = GetBmp24Pos(pDoc->m_Image[i].nW, pDoc->m_Image[i].nH, x-pDoc->m_Image[i].nPosX, y-pDoc->m_Image[i].nPosY);

			if(Color == 0) return pDoc->m_Image[i].Image1D[offset+2];
			else if(Color == 1) return pDoc->m_Image[i].Image1D[offset+1];
			else return pDoc->m_Image[i].Image1D[offset];
		}
	}

	if(Color == 0) return (BYTE)(0x00FFFFFF & 0x000000FF);
	else if(Color == 1) return (BYTE)((0x00FFFFFF & 0x0000FF00)/0x100);
	else return (BYTE)((0x00FFFFFF & 0x00FF0000)/0x10000);
}

void CImageProcessingView::GetCurrPosColor1D(int x, int y, BYTE *Image1D)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	int i, offset;

	for (i = pDoc->m_nImageCnt-1; i >= 0 ; i--) 
	{
		if(	pDoc->m_Image[i].nPosX <= x && 
			pDoc->m_Image[i].nPosX + pDoc->m_Image[i].nW > x &&
			pDoc->m_Image[i].nPosY <= y && 
			pDoc->m_Image[i].nPosY + pDoc->m_Image[i].nH > y)
		{
			offset = GetBmp24Pos(pDoc->m_Image[i].nW, pDoc->m_Image[i].nH, x-pDoc->m_Image[i].nPosX, y-pDoc->m_Image[i].nPosY);

			*Image1D = pDoc->m_Image[i].Image1D[offset];
			*(Image1D+1) = pDoc->m_Image[i].Image1D[offset+1];
			*(Image1D+2) = pDoc->m_Image[i].Image1D[offset+2];

			return;
		}
	}

	*Image1D = 255;
	*(Image1D+1) = 255;
	*(Image1D+2) = 255;
}

void CImageProcessingView::SelOperation()
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	int nW, nH;
	int x, y, offset;
	CPoint Pos;

	if(m_nSelectRegion == 1)
	{
		nW = m_rtSelectRegion.Width()+1;
		nH = m_rtSelectRegion.Height()+1;
	}
	else if(m_nSelectRegion == 2)
	{
		nW = (m_rtSelectRegion.Width()+1)*2;
		nH = (m_rtSelectRegion.Height()+1)*2;
	}

	CPoint CurrentScrollPoint = GetScrollPosition();

	pDoc->m_Image[pDoc->m_nImageCnt].Image1D = new unsigned char[GetBmp24Size(nW, nH)];
	pDoc->m_Image[pDoc->m_nImageCnt].nW = nW;
	pDoc->m_Image[pDoc->m_nImageCnt].nH = nH;
	pDoc->m_Image[pDoc->m_nImageCnt].nPosX = m_rtSelectRegion.left+CurrentScrollPoint.x;
	pDoc->m_Image[pDoc->m_nImageCnt].nPosY = m_rtSelectRegion.top+CurrentScrollPoint.y;

	Pos = CPoint(m_rtSelectRegion.left+CurrentScrollPoint.x, m_rtSelectRegion.top+CurrentScrollPoint.y);
	
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			offset = GetBmp24Pos(nW, nH, x, y);
			if(m_nSelectRegion == 1)
			{
				pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = GetCurrPosColor(x+Pos.x, y+Pos.y, 2);
				pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = GetCurrPosColor(x+Pos.x, y+Pos.y, 1);
				pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset] = GetCurrPosColor(x+Pos.x, y+Pos.y, 0);	
			}
			else if(m_nSelectRegion == 2)
			{
				pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = GetCurrPosColor(x/2+Pos.x, y/2+Pos.y, 2);
				pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset++] = GetCurrPosColor(x/2+Pos.x, y/2+Pos.y, 1);
				pDoc->m_Image[pDoc->m_nImageCnt].Image1D[offset] = GetCurrPosColor(x/2+Pos.x, y/2+Pos.y, 0);	
			}
		}
	
	pDoc->m_nImageCnt++;
	Invalidate();
}

void CImageProcessingView::OnSelRegion() 
{
	m_nSelectRegion = 1;
}

void CImageProcessingView::OnSelRegionZoom() 
{
	m_nSelectRegion = 2;	
}

void CImageProcessingView::OnSum() 
{
	ImageOperation(0);
}

void CImageProcessingView::OnSubtract() 
{
	ImageOperation(1);
}

void CImageProcessingView::OnDifference() 
{
	ImageOperation(4);	
}

void CImageProcessingView::OnEditRegion() 
{
	m_bEditRegionState = !m_bEditRegionState;
	Invalidate();
}

void CImageProcessingView::OnUpdateEditRegion(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bEditRegionState);	
}

void CImageProcessingView::OnDelRegion() 
{
	//if(m_nViewRegionSelected >= 0)
	{
		int i, k;
		for(i = 0 ; i < MAX_REGION-1 ; i++)
		{
			for(k = 0 ; k < MAX_REGION_POINT ; k++)
				m_ViewRegion.Pt[i][k] = m_ViewRegion.Pt[i+1][k];

			m_ViewRegion.nPtCnt[i] = m_ViewRegion.nPtCnt[i+1];
			m_ViewRegion.nType[i] = m_ViewRegion.nType[i+1];
			m_ViewRegion.Rotate[i] = m_ViewRegion.Rotate[i+1];
		}

		m_ViewRegion.nCnt--;

		Invalidate();	
	}
}

void CImageProcessingView::OnImageInfo()
{
	int nW, nH, nPosX, nPosY;

	if(!GetCurrentImageInfo(&nW, &nH, &nPosX, &nPosY)) return;

	CString Buf;

	Buf.Format(_T("%d * %d"), nW, nH);
	AfxMessageBox(Buf);
}
