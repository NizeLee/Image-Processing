//  Image.h  Subroutines Prototype, Structures and Constants
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
/////////////////////////////////////////////////////////////////////////////

#ifndef IMAGE_H_SUBROUTINE_STRUCTURE_CONSTANT
#define IMAGE_H_SUBROUTINE_STRUCTURE_CONSTANT

#define GRAY_CNT	256

// 영상 정보 저장 구조체
struct ImageInfo 
{
	int nW, nH;
	int nPosX, nPosY;

	unsigned char *Image1D;
};

#define IF_COLOR			0x00000000
#define IF_GRAY				0x00000001
#define IF_RGB_ORDER		0x00000002
#define IF_NOT_4B_ALIGN		0x00000004
#define IF_TOP_BOTTOM		0x00000008
#define IF_BMP_MASK			0x0000000F

#define IF_TYPE_RGB			0x00000000
#define IF_TYPE_YUV420		0x00000010
#define IF_TYPE_YUV422		0x00000020
#define IF_TYPE_YUV444		0x00000030
#define IF_TYPE_MASK		0x000000F0

#define IF_COLOR2GRAY_AVERAGE		0
#define IF_COLOR2GRAY_NTSC			1

class CIpPoint {
public:
	int x, y;

	CIpPoint() : x(0), y(0) {};
	CIpPoint(int xx, int yy) : x(xx), y(yy) {};
	bool operator == (const CIpPoint p) { 
		if(x == p.x && y == p.y) return true;
		else return false;
	}
	bool operator != (const CIpPoint p) { 
		if(x == p.x && y == p.y) return false;
		else return true;
	}
};

typedef unsigned long Color24;
#define COLOR_24_RGB(R, G, B) ((unsigned long)(((unsigned char)(R)|((unsigned short)((unsigned char)(G))<<8))|(((unsigned long)(unsigned char)(B))<<16)))

#define GetRed(RGB)			((RGB)& 0xff)
#define GetGreen(RGB)		((((unsigned short)(RGB)) >> 8)& 0xff)
#define GetBlue(RGB)		(((RGB)>>16)& 0xff)

class CIpRect {
public:
	int left, top, right, bottom;

	CIpRect() : left(0), right(0), top(0), bottom(0) {};
	CIpRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {};

	CIpPoint CenterPoint() {
		return CIpPoint((left+right)/2, (top+bottom)/2);
	}

	bool IsIncludeIn(CIpRect rt)
	{
		if(left >= rt.left && right <= rt.right &&
			top >= rt.top && bottom <= rt.bottom)
			return true;
		else
			return false;
	}

	int Height() {
		return bottom-top;
	}

	int Width() {
		return right-left;
	}

	void Union(CIpRect rt)
	{
		left = left < rt.left ? left : rt.left;
		right = right > rt.right ? right : rt.right;
		top = top < rt.top ? top : rt.top;
		bottom = bottom > rt.bottom ? bottom : rt.bottom;
	}

	void Intersect(CIpRect rt)
	{
		left = left > rt.left ? left : rt.left;
		right = right < rt.right ? right : rt.right;
		top = top > rt.top ? top : rt.top;
		bottom = bottom < rt.bottom ? bottom : rt.bottom;
	}

	bool IsRect()
	{
		if(left > right) return false;
		if(top > bottom) return false;

		return true;
	}
};


int IsPtInsidePolygon(CIpPoint *polygon, int N, CIpPoint p);

// 2차원 메모리 할당 및 해제
unsigned char  **cmatrix(int nH, int nW);
void free_cmatrix(unsigned char  **Image, int nH, int nW);

int **imatrix(int nH, int nW);
void free_imatrix(int **Image, int nH, int nW);

double **dmatrix(int nH, int nW);
void free_dmatrix(double **Image, int nH, int nW);

// 24비트 비트맵 영상의 크기 및 데이터 위치를 계산하는 함수
int GetBmp24Size(int nW, int nH);
int GetBmp24Pos(int nW, int nH, int x, int y);

// 비트맵 파일 읽기, raw 파일 읽기, 비트맵 파일 저장(24비트 컬러)
unsigned char  *ReadPpm(char *FileName, int *pW, int *pH);
unsigned char  *ReadBmp(char *FileName, int *pW, int *pH);
unsigned char  *ReadRaw(char *FileName, int nW, int nH);
bool SaveBmp(char *FileName, unsigned char  *Image1D, int nW, int nH);

unsigned char *ReadPcx(const char *FileName, int *pW, int *pH);
//unsigned char *ReadPcx(const wchar_t *FileName, int *pW, int *pH);

void Move(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH, int x0, int y0);
void Rotate(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH, int nOutW, int nOutH, double dDeg);
void ScaleX2(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH);
void HistogramEqualization(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH);
void MeanFilter(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH, int nWinSize);
void MedianFilter(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH, int nWinSize);
void Sobel(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH);
void SobelXY(unsigned char  **ImageGray, double **OutputGrayX, double **OutputGrayY, int nW, int nH);
void Laplacian(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH);
void LaplacianSharp(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH);
void Thinning(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH);
int Labeling(unsigned char  **ImageGray, int **Label, int nW, int nH, int nConnNumThre);
int Labeling(unsigned char **ImageGray, unsigned char ForegroundValue, int **Label, int nW, int nH, int nConnNumThre);
int Labeling(unsigned char **ImageGray, unsigned char ForegroundValue1, unsigned char ForegroundValue2, int **Label, int nW, int nH, int nConnNumThre);
int Labeling(unsigned char  **ImageGray, int **Label, int nW, int nH, int nConnNumThre, int *Area);
int Labeling(unsigned char  **ImageGray, int **Label, int nW, int nH, int nConnNumThre, int nConnNumThre2);
double DFT2D(unsigned char  **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH);
double RowColDFT2D(unsigned char  **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH);
void FFT2Radix(double *Xr, double *Xi, double *Yr, double *Yi, int nN, bool bInverse);
double FFT2D(unsigned char  **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH);
void FFT2DInverse(double **InputReal, double **InputImag, double **OutputDouble, int nW, int nH);
void DCT2D(unsigned char  **ImageGray, double **OutputDouble, int nW, int nH, int nBlockSize);
void pTileThresholding(unsigned char  **ImageGray, unsigned char **OutputGray, int nW, int nH, double dRate, int nType);
void ValleyThresholding(unsigned char  **ImageGray, unsigned char **OutputGray, int nW, int nH);
void IterateThresholding(unsigned char  **ImageGray, unsigned char **OutputGray, int nW, int nH);
void OtsuBinarization(unsigned char  **Image, int nW, int nH, unsigned char **Out) ;
void HoughTransform(unsigned char **ImageGray, double **HoughSpace, int nW, int nH);
void Turtle(unsigned char **ImageGray, int nW, int nH, CIpPoint *Boundary, int *pCnt);
void Turtle(int **Label, int nW, int nH, int SelLabel, CIpPoint *Boundary, int *pCnt);
void ComputekCosine(CIpPoint *Boundary, int nCnt, int k, double *kCosine);
void ComputekCosine2(CIpPoint *Boundary, int nCnt, int k, double *kCosine, bool *bConves, int xC, int yC);
void DrawLine(unsigned char **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char Color);
void DrawLineColor(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int x0, int y0, int x1, int y1, Color24 Color);
void DrawLine1D(unsigned char *Image1D, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char R, unsigned char G, unsigned char B);
void Dot(unsigned char **ImageGray, int nW, int nH, int x, int y);
void Dot(unsigned char **ImageGray, int nW, int nH, int x, int y, unsigned char Color);
void DilationBinary(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize);
void DilationSquareBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void DilationHorizonBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void DilationVertialBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ErosionBinary(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize);
void ErosionSquareBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ErosionHorizonBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ErosionVertialBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void DilationGray(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize);
void ErosionGray(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize);
bool InverseMatrix(double  **x, double **Inverse, int nN, double *Determinant);

void DrawLabelBoundBox(CIpRect *pBoundBox, int LabelCnt, unsigned char **Image2DGray, unsigned char Color);
void MakeImage2DGrayToImage1D(unsigned char **Image2DGray, int nW, int nH, unsigned char *Image1D);
void MakeImage2DColorToImage1D(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, unsigned char *Image1D);
void MakeImage1DToImage2DGray(unsigned char *Image1D, bool bBottomTop, int nW, int nH, unsigned char **Image2DGray, int nDesW = 0, int nDesH = 0);
void MakeImage1DToImage2DGrayNew20(unsigned char *Image1D, bool bBottomTop, int nMethod, int nW, int nH, unsigned char **Image2DGray, int nDesW = 0, int nDesH = 0);
void MakeImage1DToImage2DColor(unsigned char *Image1D, bool bBottomTop, int nW, int nH, unsigned char **Image2DRed, unsigned char **Image2DGreen, unsigned char **Image2DBlue, int nDesW = 0, int nDesH = 0);
void DrawCross(unsigned char **Image, int nW, int nH, CIpPoint pt, int Gray);
void DrawCrossColor(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, CIpPoint pt, Color24 Color);
void BigDot(unsigned char **ImageGray, int nW, int nH, int x, int y, unsigned char Color);
double Dist(int x1, int y1, int x2, int y2);
double FDist(double x1, double y1, double x2, double y2);
bool SaveImage2DColorBmp(char *FileName, unsigned char **ImageRed, unsigned char **ImageGreen, unsigned char **ImageBlue, 
	int nW, int nH);
void MedianBinaryFast2DGray(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ComputekCosine(CIpPoint *Boundary, int nCnt, int *LeftK, int *RightK, double *kCosine, double sigma, bool *bConvex, int xC, int yC);
void GaussianSmooth(CIpPoint *Boundary, int nCnt, double sigma);
void GaussianSmooth(double *Data, int nCnt, double sigma);
void DrawArrow(CDC *pDC, int x1, int y1, int x2, int y2, Color24 Color, int nWidth = 1);
void DrawArrow(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, unsigned char Color);
void DrawBigArrow(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, unsigned char Color);
void DrawBigLine(unsigned char **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char Color);
void SetLabelBoundBox(int **Label, int nW, int nH, CIpRect *pBoundBox, int LabelCnt);
double ComputeSD(int *data, int cnt);
void DrawRect(CIpRect rt, unsigned char **Image2DGray, int nW, int nH, unsigned char Color);
void DrawRect1D(CIpRect rt, unsigned char *Image1D, int nW, int nH, unsigned char R, unsigned char G, unsigned char B);
void DrawBigRect(CIpRect rt, unsigned char **Image2DGray, int nW, int nH, unsigned char Color);
void KMeanBinary(unsigned char **Image, unsigned char ***OutputGray, int nW, int nH, int nK);
void Fill(unsigned char **Image, int nW, int nH);
void RemoveSmallRegion(unsigned char **Image, int nW, int nH, int nConnNumThre);
void DistanceTransform(unsigned char **Image, double **Output, int nW, int nH);
double VectorAbs(double x, double y);
void DrawEllipse(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, unsigned char Color);
void CirclePoints(unsigned char **ImageGray, int nW, int nH, int cx, int cy, int x, int y, unsigned char Color);
void DrawCircle(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int radius, unsigned char Color);
void DrawCircle2(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int radius, unsigned char Color);
void DrawFillCircle(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int radius, unsigned char Color);
void FastMedianFilter(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nOneSideSize);
void GetAffineParameters(CIpPoint D1, CIpPoint D2, CIpPoint D3, CIpPoint V1, CIpPoint V2, CIpPoint V3,
	double *pA1, double *pA2, double *pA0, double *pB1, double *pB2, double *pB0);
void Affine(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut,
	double a1, double a2, double a0, double b1, double b2, double b0);
void Bilinear(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut,
	double u0, double v0, double u1, double v1, double u2, double v2, double u3, double v3);
void Mean2DGrayFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void Mean2DGrayFast(unsigned char **Image2D, int nW, int nH, int nSize, double **Image2DOut);
void FastEntropy(unsigned char **ImageGray, double **Output, int nW, int nH, int nOneSideSize);
void Sd2DGrayFast(unsigned char **Image2D, double **Mean, int nW, int nH, int nSize, double **Image2DOut);
void MeanSd2DGrayFast(unsigned char **Image2D, int nW, int nH, int nSize, double **Mean2D, double **Sd2D);
void DcNotch2DGrayFast(unsigned char **Image2DGray, unsigned char **Image2DGrayOut, int nW, int nH, int nSize);
CIpPoint BMA(unsigned char **CurrImageGray, unsigned char **ReferImageGray, int left, int top, 
		 int nW, int nH, int nSearchLeft, int nSearchTop, int nSearchRight, int nSearchBottom, 
		 int BlockWidth, int BlockHeight);
void InversePerspectiveTable(int W, int H, int LW, int LH, CIpPoint p0, CIpPoint p1, CIpPoint p2, CIpPoint p3, CIpPoint **Table);
void InversePerspective(int W, int H, int LW, int LH,unsigned char **img, unsigned char **out, CIpPoint **Table);
void PerspectiveTable(int W, int H, int TW, int TH, CIpPoint p0, CIpPoint p1, CIpPoint p2, CIpPoint p3, CIpPoint **Table);
void Perspective(int W, int H, int TW, int TH, unsigned char **img, unsigned char **out, CIpPoint **Table);
void BilinearRadial(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut, int topLeft);
void InversePerspectivePt(int W, int H, int LW, int LH,
	CIpPoint p0, CIpPoint p1, CIpPoint p2, CIpPoint p3, double Ox, double Oy, double *Tx, double *Ty);
void DrawBoundary(unsigned char **ImageGray, int nW, int nH, CIpPoint *Boundary, int Cnt, unsigned char Color);
double MeanAngleLabel(int **Label, int nLabelNum, CIpRect boundbox);
void Harris2DGray(unsigned char **Image2DGray, int nW, int nH, int nSize, double **Image2DGrayOut, double dThre, double dK);
void ShoulderBinary2DGray(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut);
void VectorMedianFilter(int **OrgX, int **OrgY, int **OutX, int **OutY, int nW, int nH);
void YUV2RGB(unsigned char *Y, unsigned char *U, unsigned char *V, unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int nType);
double lerp(double v1, double v2, double ratio);
void BilinearInterpolaton(unsigned char **ImageGray, int nW, int nH, unsigned char **OutputGray, int nOW, int nOH);
void BilinearInterpolaton(unsigned char **ImageGray, int Left, int Top, int Right, int Bottom, unsigned char **OutputGray, int nOW, int nOH);
double Cubic(double offset, double v0, double v1, double v2, double v3);
double GetCubicRow(int x, int y, double offset, unsigned char **ImageGray, int nW, int nH);
double GetBicubic(double x, double y, unsigned char **ImageGray, int nW, int nH);
void BicublicInterpolation(unsigned char **ImageGray, int nW, int nH, unsigned char **OutputGray, int nOW, int nOH);
double GetPsnrRgb(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int nW, int nH);
double GetPsnrYuv(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int nW, int nH);
double GetPsnrY(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int nW, int nH);
void LabelMoment(int **Label, int LabelCnt, int nW, int nH, double *m2, double *m3);
bool InverseMatrix2(double  **a, double **y, int nN);

#endif