//	Image4Win.cpp: Image processing for Windows (Implementation)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr

#include "stdafx.h"
#include "ImageProcessing.h"

#include "MainFrm.h"		
#include "ChildFrm.h"		
#include "ImageProcessingDoc.h"		
#include "ImageProcessingView.h"	

#include "Image.h"
#include "Image4Win.h"

#include <cmath>			

bool SaveImage2DColorJpeg(char *FileName, unsigned char **ImageRed, unsigned char **ImageGreen, unsigned char **ImageBlue, 
	int nW, int nH, bool bColor, int nQuality)
{
	int x, y, Pos;
	unsigned char *Image1D;
	
	Image1D = new unsigned char[GetBmp24Size(nW, nH)];

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			Pos = GetBmp24Pos(nW, nH, x, y);

			Image1D[Pos++] = ImageBlue[y][x];
			Image1D[Pos++] = ImageGreen[y][x];
			Image1D[Pos] = ImageRed[y][x];
		}
		
	bool bRtn = SaveJpeg((char *)FileName, Image1D, nW, nH, bColor, nQuality);

	delete [] Image1D;

	return bRtn;
}

void DrawTextOnImage2DGray(unsigned char **Image2DGray, int nW, int nH, int x, int y, CString Msg, UINT nFormat, unsigned char Color, bool bBig)
{
	unsigned char *img1D;

	img1D = new unsigned char[(nW*3+3)/4*4*nH];

	MakeImage2DGrayToImage1D(Image2DGray, nW, nH, img1D);

	HDC hDC, hCompDC;

	hDC = GetDC(NULL);

	hCompDC = CreateCompatibleDC(hDC);

	HBITMAP hBitmap;

	hBitmap = CreateCompatibleBitmap(hDC, nW, nH);
	
	SelectObject(hCompDC, hBitmap);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = nW;
	bmiHeader.biHeight = nH;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = (nW*3+3)/4*4  * nH;
	bmiHeader.biXPelsPerMeter = 2000;
	bmiHeader.biYPelsPerMeter = 2000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	SetStretchBltMode(hCompDC, HALFTONE);

	StretchDIBits(hCompDC,
		0, 0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		0,0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		img1D,
		(LPBITMAPINFO)&bmiHeader,
		DIB_RGB_COLORS,
		SRCCOPY);

	CFont font;
	font.CreatePointFont(150, _T("Arial"), NULL);
	
	if(bBig) SelectObject(hCompDC, font.GetSafeHandle());

	SetTextColor(hCompDC, RGB(Color, Color, Color));
	SetBkMode(hCompDC, TRANSPARENT);

	RECT Rt;
	Rt.left = x;
	Rt.top = y;
	DrawText(hCompDC, Msg, Msg.GetLength(), &Rt, DT_CALCRECT | nFormat);
	DrawText(hCompDC, Msg, Msg.GetLength(), &Rt, nFormat);

	BITMAPINFO bi;

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = nW;
	bi.bmiHeader.biHeight = nH;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = (nW*3+3)/4*4  * nH;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	GetDIBits(hCompDC, hBitmap, 0, nH, img1D, &bi,
                   DIB_RGB_COLORS);

	DeleteObject(hBitmap);
	
	ReleaseDC(NULL, hCompDC);
	ReleaseDC(NULL, hDC);
	DeleteDC(hCompDC);

	MakeImage1DToImage2DGray(img1D, true, nW, nH, Image2DGray);

	delete [] img1D;
}

void DrawTextOnImage2DColor(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int x, int y, CString Msg, UINT nFormat, Color24 Color, bool bBig)
{
	unsigned char *img1D;

	img1D = new unsigned char[(nW*3+3)/4*4*nH];

	MakeImage2DColorToImage1D(R, G, B, nW, nH, img1D);

	HDC hDC, hCompDC;

	hDC = GetDC(NULL);

	hCompDC = CreateCompatibleDC(hDC);

	HBITMAP hBitmap;

	hBitmap = CreateCompatibleBitmap(hDC, nW, nH);
	
	SelectObject(hCompDC, hBitmap);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = nW;
	bmiHeader.biHeight = nH;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = (nW*3+3)/4*4  * nH;
	bmiHeader.biXPelsPerMeter = 2000;
	bmiHeader.biYPelsPerMeter = 2000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	SetStretchBltMode(hCompDC, HALFTONE);

	StretchDIBits(hCompDC,
		0, 0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		0,0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		img1D,
		(LPBITMAPINFO)&bmiHeader,
		DIB_RGB_COLORS,
		SRCCOPY);

	CFont font;
	font.CreatePointFont(150, _T("Arial"), NULL);
	
	if(bBig) SelectObject(hCompDC, font.GetSafeHandle());

	SetTextColor(hCompDC, Color);
	SetBkMode(hCompDC, TRANSPARENT);

	RECT Rt;
	Rt.left = x;
	Rt.top = y;
	DrawText(hCompDC, Msg, Msg.GetLength(), &Rt, DT_CALCRECT | nFormat);
	DrawText(hCompDC, Msg, Msg.GetLength(), &Rt, nFormat);

	BITMAPINFO bi;

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = nW;
	bi.bmiHeader.biHeight = nH;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = (nW*3+3)/4*4  * nH;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	GetDIBits(hCompDC, hBitmap, 0, nH, img1D, &bi,
                   DIB_RGB_COLORS);

	DeleteObject(hBitmap);
	
	ReleaseDC(NULL, hCompDC);
	ReleaseDC(NULL, hDC);
	DeleteDC(hCompDC);

	MakeImage1DToImage2DColor(img1D, true, nW, nH, R, G, B);

	delete [] img1D;
}

void DrawTextOnImage1D(unsigned char *Image1D, int nW, int nH, int x, int y, CString Msg, UINT nFormat, Color24 Color, bool bBig)
{

	HDC hDC, hCompDC;

	hDC = GetDC(NULL);

	hCompDC = CreateCompatibleDC(hDC);

	HBITMAP hBitmap;

	hBitmap = CreateCompatibleBitmap(hDC, nW, nH);
	
	SelectObject(hCompDC, hBitmap);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = nW;
	bmiHeader.biHeight = nH;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = (nW*3+3)/4*4  * nH;
	bmiHeader.biXPelsPerMeter = 2000;
	bmiHeader.biYPelsPerMeter = 2000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	SetStretchBltMode(hCompDC, HALFTONE);

	StretchDIBits(hCompDC,
		0, 0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		0,0,
		bmiHeader.biWidth,
		bmiHeader.biHeight,
		Image1D,
		(LPBITMAPINFO)&bmiHeader,
		DIB_RGB_COLORS,
		SRCCOPY);

	CFont font;
	font.CreatePointFont(150, _T("Arial"), NULL);
	
	if(bBig) SelectObject(hCompDC, font.GetSafeHandle());

	SetTextColor(hCompDC, Color);
	SetBkMode(hCompDC, TRANSPARENT);

	RECT Rt;
	Rt.left = x;
	Rt.top = y;
	
	DrawText(hCompDC, Msg, Msg.GetLength(), &Rt, DT_CALCRECT | nFormat);
	DrawText(hCompDC, Msg, Msg.GetLength(), &Rt, nFormat);

	BITMAPINFO bi;

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = nW;
	bi.bmiHeader.biHeight = nH;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = (nW*3+3)/4*4  * nH;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	GetDIBits(hCompDC, hBitmap, 0, nH, Image1D, &bi,
                   DIB_RGB_COLORS);

	DeleteObject(hBitmap);
	
	ReleaseDC(NULL, hCompDC);
	ReleaseDC(NULL, hDC);
	DeleteDC(hCompDC);
}

void SetViewMsg(CString ViewMsg, bool bErase)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return;

	pView->m_ViewMsg = ViewMsg;

	pView->Invalidate(bErase);
}

bool GetCurrentImageInfo(int *pW, int *pH, int *pPosX, int *pPosY, int nIndex)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->GetCurrentImageInfo(pW, pH, pPosX, pPosY, nIndex);
}

bool GetCurrentImageGray(unsigned char **ImageGray, int nIndex)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->GetCurrentImageGray(ImageGray, nIndex);
}

bool DisplayCimage2D(unsigned char **ImageGray, int nW, int nH, int nPosX, int nPosY, bool bErase, bool bDelete, int Rate)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->DisplayCimage2D(ImageGray, nW, nH, nPosX, nPosY, bErase, bDelete, Rate);
}

bool GetCurrentImageColor(unsigned char **ImageRed, unsigned char **ImageGreen, unsigned char **ImageBlue, int nIndex)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->GetCurrentImageColor(ImageRed, ImageGreen, ImageBlue, nIndex);
}

bool GetCurrentImage1DColor(unsigned char *Image1D, int nIndex)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->GetCurrentImage1DColor(Image1D, nIndex);
}

bool DisplayCimage1D(unsigned char *Image1D, int nW, int nH, int nPosX, int nPosY, bool bErase, bool bDelete, int Rate)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->DisplayCimage1D(Image1D, nW, nH, 
		nPosX, nPosY, bErase, bDelete, Rate);
}

bool DisplayCimage2DColor(unsigned char **ImageRed, unsigned char **ImageGreen, unsigned char **ImageBlue, int nW, int nH, 
	int nPosX, int nPosY, bool bErase, bool bDelete, int Rate)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->DisplayCimage2DColor(ImageRed, ImageGreen, ImageBlue, nW, nH, 
		nPosX, nPosY, bErase, bDelete, Rate);
}

bool DisplayIimage2D(int **ImageInt, int nW, int nH, int nPosX, int nPosY, bool bErase, bool bDelete)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->DisplayIimage2D(ImageInt, nW, nH, nPosX, nPosY, bErase, bDelete);
}

bool DisplayDimage2D(double **ImageDouble, int nW, int nH, int nPosX, int nPosY, bool bErase, bool bDelete)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return false;
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(!pView) return false;

	return pView->DisplayDimage2D(ImageDouble, nW, nH, nPosX, nPosY, bErase, bDelete);
}

void Compression(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y;
	int u, v;
	int BlockX, BlockY;
	double **DCT, **InverseDCT;
	int nBlockSize = 8;
	int nZeroCount = 0;

	int Q[8][8] = {
		16, 11, 10, 16, 24, 40, 51, 61,
		12, 12, 14, 19, 26, 58, 60, 66,
		14, 13, 16, 24, 40, 57, 69, 57,
		14, 17, 22, 29, 51, 87, 80, 62,
		18, 22, 37, 56, 68, 109, 103, 77,
		24, 36, 55, 64, 81, 104, 113, 92,
		49, 64, 78, 87, 103, 121, 120, 101,
		72, 92, 95, 98, 112, 100, 103, 99
	};

	DCT = dmatrix(nH, nW);
	InverseDCT = dmatrix(nH, nW);

	const double Pi = acos(-1.);

	for(v = 0 ; v < nH ; v++)
		for(u = 0 ; u < nW ; u++)
			DCT[v][u] = 0;

	for(BlockY = 0 ; BlockY < nH-nBlockSize+1 ; BlockY += nBlockSize)
		for(BlockX = 0 ; BlockX < nW-nBlockSize+1 ; BlockX += nBlockSize)
		{
			for(v = 0 ; v < nBlockSize ; v++)
				for(u = 0 ; u < nBlockSize ; u++)
				{
					DCT[BlockY+v][BlockX+u] = 0;

					for(y = 0 ; y < nBlockSize ; y++)
						for(x = 0 ; x < nBlockSize ; x++)
						{
							DCT[BlockY+v][BlockX+u] += 
								ImageGray[BlockY+y][BlockX+x] 
								* cos((2*x+1)*u*Pi/(2.*nBlockSize)) * cos((2*y+1)*v*Pi/(2.*nBlockSize));
						}

					if(u == 0)
						DCT[BlockY+v][BlockX+u] *= sqrt(1./nBlockSize);
					else
						DCT[BlockY+v][BlockX+u] *= sqrt(2./nBlockSize);

					if(v == 0)
						DCT[BlockY+v][BlockX+u] *= sqrt(1./nBlockSize);
					else
						DCT[BlockY+v][BlockX+u] *= sqrt(2./nBlockSize);

					DCT[BlockY+v][BlockX+u] = (int)(DCT[BlockY+v][BlockX+u])/Q[v][u];

					if(DCT[BlockY+v][BlockX+u] == 0) nZeroCount++;
				}
		}

	double C1, C2;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			InverseDCT[y][x] = 0;

	for(BlockY = 0 ; BlockY < nH-nBlockSize+1 ; BlockY += nBlockSize)
		for(BlockX = 0 ; BlockX < nW-nBlockSize+1 ; BlockX += nBlockSize)
		{
			for(y = 0 ; y < nBlockSize ; y++)
				for(x = 0 ; x < nBlockSize ; x++)
				{
					InverseDCT[BlockY+y][BlockX+x] = 0;

					for(v = 0 ; v < nBlockSize ; v++)
						for(u = 0 ; u < nBlockSize ; u++)
						{
							if(u == 0)
								C1 = sqrt(1./nBlockSize);
							else
								C1 = sqrt(2./nBlockSize);

							if(v == 0)
								C2 = sqrt(1./nBlockSize);
							else
								C2 = sqrt(2./nBlockSize);

							InverseDCT[BlockY+y][BlockX+x] += 
								C1 * C2  * DCT[BlockY+v][BlockX+u] * Q[v][u]
								* cos((2*x+1)*u*Pi/(2.*nBlockSize)) * cos((2*y+1)*v*Pi/(2.*nBlockSize));
						}

					if(InverseDCT[BlockY+y][BlockX+x] < 0)
						OutputGray[BlockY+y][BlockX+x] = 0;
					else if(InverseDCT[BlockY+y][BlockX+x] > GRAY_CNT-1)
						OutputGray[BlockY+y][BlockX+x] = GRAY_CNT-1;
					else
						OutputGray[BlockY+y][BlockX+x] = (unsigned char)InverseDCT[BlockY+y][BlockX+x];
				}
		}

	// RMSE 
	double Rmse = 0;
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			Rmse += pow((double)(OutputGray[y][x] - ImageGray[y][x]), 2);
	Rmse /= nW*nH;
	Rmse = sqrt(Rmse);

	CString ViewMsg;
	ViewMsg.Format(_T("0: %d, RMSE: %7.3lf"), nZeroCount, Rmse);
	SetViewMsg(ViewMsg);

	free_dmatrix(DCT, nH, nW);
	free_dmatrix(InverseDCT, nH, nW);
}

void Filtering(unsigned char **ImageGray, double **OutputDouble, int nW, int nH, int nPosX, int nPosY, 
	int nFilterType1, int nFilterType2, int nDC, int nN)
{
	double **FFTReal, **FFTImag;
	double **Filter;
	int u, v;

	FFTReal = dmatrix(nH, nW);
	FFTImag = dmatrix(nH, nW);

	Filter = dmatrix(nH, nW);

	FFT2D(ImageGray, FFTReal, FFTImag, nW, nH);

	for(v = 0 ; v < nH ; v++)
		for(u = 0 ; u < nW ; u++)
		{
			double Distance = sqrt(pow((double)((v+nH/2)%nH-nH/2), 2) + pow((double)((u+nW/2)%nW-nW/2), 2));
			if(nFilterType1 == 0) 
			{
				if(nFilterType2 == 0) // LPF
				{
					Filter[(v+nH/2)%nH][(u+nW/2)%nW] = 1;
					if(Distance > nDC)
						Filter[(v+nH/2)%nH][(u+nW/2)%nW] = 0;
				}
				else // HPF
				{
					Filter[(v+nH/2)%nH][(u+nW/2)%nW] = 1;
					if(Distance < nDC)
						Filter[(v+nH/2)%nH][(u+nW/2)%nW] = 0;
				}
			}
			else 
			{
				if(nFilterType2 == 0) // LPF
					Filter[(v+nH/2)%nH][(u+nW/2)%nW] = 1./(1+pow(Distance/nDC, nN));
				else // HPF
					Filter[(v+nH/2)%nH][(u+nW/2)%nW] = 1./(1+pow(nDC/Distance, nN));
			}
		}

	for(v = 0 ; v < nH ; v++)
		for(u = 0 ; u < nW ; u++)
		{				
			FFTReal[v][u] *= Filter[(v+nH/2)%nH][(u+nW/2)%nW];
			FFTImag[v][u] *= Filter[(v+nH/2)%nH][(u+nW/2)%nW];
		}

	FFT2DInverse(FFTReal, FFTImag, OutputDouble, nW, nH);

	DisplayDimage2D(Filter, nW, nH, nPosX, nPosY+nH);

	free_dmatrix(FFTReal, nH, nW);
	free_dmatrix(FFTImag, nH, nW);

	free_dmatrix(Filter, nH, nW);
}

void ImageOperation(int nOperationType)
{
	int nW1, nH1, nPosX1, nPosY1,
		nW2, nH2, nPosX2, nPosY2,
		nW, nH;
	unsigned char **ImageGray1, **ImageGray2, **OutputGray;
	unsigned char **R1, **G1, **B1, **R2, **G2, **B2, **R, **G, **B;
	int x, y;

	if(!GetCurrentImageInfo(&nW1, &nH1, &nPosX1, &nPosY1)) return;
	if(!GetCurrentImageInfo(&nW2, &nH2, &nPosX2, &nPosY2, 1)) return;

	nW = (nW1 > nW2) ? nW2 : nW1;
	nH = (nH1 > nH2) ? nH2 : nH1;

	ImageGray1 = cmatrix(nH1, nW1);
	R1 = cmatrix(nH1, nW1);
	G1 = cmatrix(nH1, nW1);
	B1 = cmatrix(nH1, nW1);
	
	ImageGray2 = cmatrix(nH2, nW2);
	R2 = cmatrix(nH2, nW2);
	G2 = cmatrix(nH2, nW2);
	B2 = cmatrix(nH2, nW2);
	
	OutputGray = cmatrix(nH, nW);
	R = cmatrix(nH, nW);
	G = cmatrix(nH, nW);
	B = cmatrix(nH, nW);

	GetCurrentImageGray(ImageGray1);
	GetCurrentImageColor(R1, G1, B1);

	GetCurrentImageGray(ImageGray2, 1);
	GetCurrentImageColor(R2, G2, B2, 1);

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(nOperationType == 0)
			{
				R[y][x] = (R1[y][x]+R2[y][x])/2;
				G[y][x] = (G1[y][x]+G2[y][x])/2;
				B[y][x] = (B1[y][x]+B2[y][x])/2;
			}
			else if(nOperationType == 1)
			{
				R[y][x] = (R1[y][x]-R2[y][x]+GRAY_CNT-1)/2;
				G[y][x] = (G1[y][x]-G2[y][x]+GRAY_CNT-1)/2;
				B[y][x] = (B1[y][x]-B2[y][x]+GRAY_CNT-1)/2;
			}
			else if(nOperationType == 2)
				OutputGray[y][x] = 
					(ImageGray1[y][x] > GRAY_CNT/2 && ImageGray2[y][x] > GRAY_CNT/2) ? GRAY_CNT-1 : 0;
			else if(nOperationType == 3)
				OutputGray[y][x] = 
					(ImageGray1[y][x] > GRAY_CNT/2 || ImageGray2[y][x] > GRAY_CNT/2) ? GRAY_CNT-1 : 0;
			else if(nOperationType == 4)
			{
				R[y][x] = abs(R1[y][x]-R2[y][x]);
				G[y][x] = abs(G1[y][x]-G2[y][x]);
				B[y][x] = abs(B1[y][x]-B2[y][x]);
			}
		}

	if(nOperationType == 2 || nOperationType == 3)
		DisplayCimage2D(OutputGray, nW, nH, nPosX1+nW1, nPosY1);
	else
		DisplayCimage2DColor(R, G, B, nW, nH, nPosX1+nW1, nPosY1);
	
	free_cmatrix(ImageGray1, nH1, nW1);	
	free_cmatrix(R1, nH1, nW1);	
	free_cmatrix(G1, nH1, nW1);	
	free_cmatrix(B1, nH1, nW1);	

	free_cmatrix(ImageGray2, nH2, nW2);	
	free_cmatrix(R2, nH2, nW2);	
	free_cmatrix(G2, nH2, nW2);	
	free_cmatrix(B2, nH2, nW2);	

	free_cmatrix(OutputGray, nH, nW);	
	free_cmatrix(R, nH, nW);	
	free_cmatrix(G, nH, nW);	
	free_cmatrix(B, nH, nW);	
}

