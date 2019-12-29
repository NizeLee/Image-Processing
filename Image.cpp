//  Image.cpp: implementation of the Image Processing Subroutines
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

#include "stdafx.h"
#include "ImageProcessing.h"

#include "MainFrm.h"		// 추가
#include "ChildFrm.h"		// 추가
#include "ImageProcessingDoc.h"		// 추가
#include "ImageProcessingView.h"	// 추가

#include <cmath>			// 추가


int IsPtInsidePolygon(CIpPoint *polygon, int N, CIpPoint p)
{
	int counter = 0;
	int i;
	double xinters;
	CIpPoint p1, p2;

	p1 = polygon[0];
	for (i = 1; i <= N; i++) {
		p2 = polygon[i % N];
		if (p.y > min(p1.y, p2.y)) {
			if (p.y <= max(p1.y, p2.y)) {
				if (p.x <= max(p1.x, p2.x)) {
					if (p1.y != p2.y) {
						xinters = (p.y - p1.y)*(p2.x - p1.x) / (p2.y - p1.y) + p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}

	if (counter % 2 == 0)
		return false;
	else
		return true;
}

// 2차원 메모리 할당
unsigned char **cmatrix(int nH, int nW) {
	unsigned char **Temp;
	
	Temp = new unsigned char *[nH];
	for(int y = 0 ; y < nH ; y++)
		Temp[y] = new unsigned char[nW];

	return Temp;
}

// 2차원 메모리 해제
void free_cmatrix(unsigned char **Image, int nH, int nW) {
	for(int y = 0 ; y < nH ; y++)
		delete [] Image[y];

	delete [] Image;
}

// 2차원 메모리 할당
int **imatrix(int nH, int nW) {
	int **Temp;
	
	Temp = new int *[nH];
	for(int y = 0 ; y < nH ; y++)
		Temp[y] = new int[nW];

	return Temp;
}

// 2차원 메모리 해제
void free_imatrix(int **Image, int nH, int nW) {
	for(int y = 0 ; y < nH ; y++)
		delete [] Image[y];

	delete [] Image;
}

// 2차원 메모리 할당
double **dmatrix(int nH, int nW) {
	double **Temp;
	
	Temp = new double *[nH];
	for(int y = 0 ; y < nH ; y++)
		Temp[y] = new double[nW];

	return Temp;
}

// 2차원 메모리 해제
void free_dmatrix(double **Image, int nH, int nW) {
	for(int y = 0 ; y < nH ; y++)
		delete [] Image[y];

	delete [] Image;
}

// 24비트 비트맵 영상의 크기 계산
int GetBmp24Size(int nW, int nH) {
	return (nW*3+3)/4*4 * nH;
}

// 24비트 비트맵 영상의 데이터 위치를 계산
int GetBmp24Pos(int nW, int nH, int x, int y) {
	return (nW*3+3)/4*4 * (nH-1-y) + x*3;
}

#define LEN_MAX	256
bool GetAsciiPpm(unsigned char **R, unsigned char **G, unsigned char **B, FILE* file_in,int h,int w)
{
	int	y,x; int color_r,color_g,color_b;
	for	(y=0;y<h;y++) 
	{
		for (x=0;x<w;x++)
		{
			//if(fscanf(file_in,"%d%d%d",&color_r,&color_g,&color_b)!=3)
			if(fscanf_s(file_in,"%d%d%d",&color_r,&color_g,&color_b)!=3)
			{
				return false;
			}
			R[y][x]=(unsigned char)color_r;
			G[y][x]=(unsigned char)color_g;
			B[y][x]=(unsigned char)color_b;
		}
	}

	return true;
}

bool GetAsciiPgm(unsigned char **image, FILE* file_in, int h, int w)
{
	int	y,x,lum;
	for	(y=0;y<h;y++) 
	{
		for(x=0;x<w;x++)
		{
			//if(fscanf(file_in,"%d",&lum)!=1)
			if(fscanf_s(file_in,"%d",&lum)!=1)
			{
				return false;
			}
			image[y][x]=(unsigned char)lum;
		}
	}

	return true;
}

unsigned char *ReadPpm(char *FileName, int *pW, int *pH)
{	
	FILE * file_in;
	char line[LEN_MAX];
	int	i,imax,h,w;
	unsigned char *Image1D;
	
	fopen_s(&file_in, FileName, "rb");
	if(!file_in)
	{
		return NULL;
	}
	if(fgetc(file_in)=='P') 
		fscanf_s(file_in,"%d\n",&i);
	else
	{ 
		return NULL;
	}
	while(fgets(line,LEN_MAX,file_in)!=NULL)
	{
		if(line[0]=='#') continue;
		else
		{	
			sscanf_s(line, "%d %d\n",	&w,	&h);
			break;
		}
	}

	*pW = w;
	*pH = h;

	if(i != 6) return NULL;
	Image1D = new unsigned char[GetBmp24Size(*pW, *pH)];
	unsigned char **Image2DR = cmatrix(*pH, *pW);
	unsigned char **Image2DG = cmatrix(*pH, *pW);
	unsigned char **Image2DB = cmatrix(*pH, *pW);
	int x, y;


	fscanf_s(file_in, "%d\n", &imax);
	switch (i)
	{
		case 2:
			GetAsciiPgm(Image2DR,file_in,w,h);
			MakeImage2DGrayToImage1D(Image2DR, *pW, *pH, Image1D);
			break;
		case 3:
			GetAsciiPpm(Image2DR, Image2DG, Image2DB, file_in,w,h);
			MakeImage2DColorToImage1D(Image2DR, Image2DG, Image2DB, *pW, *pH, Image1D);
			break;
		case 5:
			for(y = 0 ; y < *pH ; y++)
			{
				fread(Image2DR[y],sizeof(unsigned char),*pW,file_in);
			}
			MakeImage2DGrayToImage1D(Image2DR, *pW, *pH, Image1D);
			break;
		case 6:
			for(y = 0 ; y < *pH ; y++)
			{
				for(x = 0 ; x < *pW ; x++)
				{
					fread(&Image2DR[y][x],sizeof(unsigned char),1,file_in);
					fread(&Image2DG[y][x],sizeof(unsigned char),1,file_in);
					fread(&Image2DB[y][x],sizeof(unsigned char),1,file_in);
				}
			}
			MakeImage2DColorToImage1D(Image2DR, Image2DG, Image2DB, *pW, *pH, Image1D);
			break;
		default:
			break;					
	}

	free_cmatrix(Image2DR, *pH, *pW);
	free_cmatrix(Image2DG, *pH, *pW);
	free_cmatrix(Image2DB, *pH, *pW);

	fclose(file_in);
	
	return Image1D;
}

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER_ {
        unsigned short		bfType;
        unsigned long		bfSize;
        unsigned short		bfReserved1;
        unsigned short		bfReserved2;
        unsigned long		bfOffBits;
} BITMAPFILEHEADER_;

typedef struct tagBITMAPINFOHEADER_ {
        unsigned long       biSize;
        long				biWidth;
        long				biHeight;
        unsigned short      biPlanes;
        unsigned short      biBitCount;
        unsigned long       biCompression;
        unsigned long       biSizeImage;
        long				biXPelsPerMeter;
        long				biYPelsPerMeter;
        unsigned long       biClrUsed;
        unsigned long       biClrImportant;
} BITMAPINFOHEADER_;

typedef struct tagRGBQUAD_ {
        unsigned char    rgbBlue;
        unsigned char    rgbGreen;
        unsigned char    rgbRed;
        unsigned char    rgbReserved;
} RGBQUAD_;
#pragma pack(pop)

#define BI_RGB_        0L

// 비트맵 파일 읽기
unsigned char *ReadBmp(char *FileName, int *pW, int *pH) {
	BITMAPFILEHEADER_ bmiFileHeader;
	BITMAPINFOHEADER_ bmiHeader;
	
	FILE *fp;
	int nColors;
	
	// 파일 열기
	fopen_s(&fp, FileName, "rb");
	// 파일 열기 실패
	if(!fp) return NULL;

	// 파일 헤더 정보 읽기
	fread(&bmiFileHeader, sizeof(BITMAPFILEHEADER_), 1, fp);

	// 식별자 인식
	if(bmiFileHeader.bfType != 'M'*0x0100 + 'B') {
		fclose(fp);
		return NULL;
	}

	// 비트맵 정보 헤더 읽기
	fread(&bmiHeader, sizeof(BITMAPINFOHEADER_), 1, fp);
	

	// 압축 여부 확인 - 압축되어 있다면 종료
	if(bmiHeader.biCompression != BI_RGB_) {
	    fclose(fp);
	    return NULL;
	}

	// 만일 Colors이 0이면 화소당 비트수에 대한 최대 컬러 개수
	if(bmiHeader.biClrUsed == 0) 
		nColors = 1 << bmiHeader.biBitCount;
	else
		nColors = bmiHeader.biClrUsed;

	// 팔레트 
	RGBQUAD_ *Palette = NULL;

	// 팔레트 정보 읽기
	switch (bmiHeader.biBitCount) {
		case 24:
			break;

		case 1:
		case 4:
		case 8:
			Palette = new RGBQUAD_[nColors];

			int i;
			for(i = 0 ; i < nColors ; i++) {
				unsigned char r, g, b, temp;
	
				fread(&b, sizeof(unsigned char), 1, fp);
				fread(&g, sizeof(unsigned char), 1, fp);
				fread(&r, sizeof(unsigned char), 1, fp);
				fread(&temp, sizeof(unsigned char), 1, fp);

				Palette[i].rgbRed = r;
				Palette[i].rgbGreen = g;
				Palette[i].rgbBlue = b;
			}
			break;
	}

	// 비트맵 데이터 위치로 이동
	fseek(fp, bmiFileHeader.bfOffBits, SEEK_SET);

	*pW = bmiHeader.biWidth;
	*pH = bmiHeader.biHeight;
	
	unsigned char *Image1D = NULL;

	// 1차원 메모리 할당
	Image1D = new unsigned char[GetBmp24Size(*pW, *pH)];

	// 24비트 컬러
	if(bmiHeader.biBitCount == 24)
		fread(Image1D, sizeof(unsigned char), *pH*((*pW*3+3)/4*4), fp);

	long Row, Col;

	// 1, 4, 8 비트 컬러
	for(Row = 0 ; Row < bmiHeader.biHeight ; Row++)
	{
		if(bmiHeader.biBitCount != 24) 
		{
			int BitCount = 0;
			unsigned int mask = (1 << bmiHeader.biBitCount) - 1;

			unsigned char ReadByte = 0;
			int ReadByteCnt = 0;

			for(Col = 0; Col < *pW ; Col++) 
			{				
				int PaletteIndex = 0;

				if (BitCount <= 0) {
					BitCount = 8;
					fread(&ReadByte, sizeof(unsigned char), 1, fp);
					ReadByteCnt++;
				}

				BitCount -= bmiHeader.biBitCount;

				// 팔레트 인덱스 번호 계산
				PaletteIndex = (ReadByte >> BitCount) & mask;

				int Pos;

				// 24비트 컬러로 저장
				Pos = (((*pW*3+3)/4*4) * Row) + Col*3;	
				Image1D[Pos++] = Palette[PaletteIndex].rgbBlue;
				Image1D[Pos++] = Palette[PaletteIndex].rgbGreen;
				Image1D[Pos] = Palette[PaletteIndex].rgbRed;
			}

			// 윗줄의 위치 결정
			while (ReadByteCnt&3) 
			{
				char temp;
				fread(&temp, sizeof(char), 1, fp);
				ReadByteCnt++;
			}
		}
	}

	if(Palette) delete [] Palette;

	fclose(fp);

	return Image1D;
}

// raw 파일 읽기
unsigned char *ReadRaw(char *FileName, int nW, int nH) {
	unsigned char *ReadBytes;
	int x, y, Pos;

	FILE *fp;
	
	fopen_s(&fp, FileName, "rb");
	// 파일 열기 실패
	if(!fp) return NULL;
	
	unsigned char *Image1D;

	// 메모리 할당
	Image1D = new unsigned char[GetBmp24Size(nW, nH)];
	ReadBytes = new unsigned char[nW];

	// 1차원 비트맵 구조로 회색조 raw 영상 읽기
	for(y = 0 ; y < nH ; y++)
	{
		fread(ReadBytes, sizeof(unsigned char), nW, fp);

		for(x = 0 ; x < nW ; x++)
		{
			Pos = GetBmp24Pos(nW, nH, x, y);
			Image1D[Pos++] = ReadBytes[x];
			Image1D[Pos++] = ReadBytes[x];
			Image1D[Pos] = ReadBytes[x];
		}
	}

	delete [] ReadBytes;

	fclose(fp);

	return Image1D;
}

// 비트맵 파일 저장(24비트 컬러)
bool SaveBmp(char *FileName, unsigned char *Image1D, int nW, int nH) {
	unsigned long dwBitsSize;

	unsigned long size;
		
	size = GetBmp24Size(nW, nH);
	dwBitsSize = sizeof(BITMAPFILEHEADER_) + sizeof(BITMAPINFOHEADER_) + size;

	// 헤더 정보 저장
	BITMAPINFOHEADER_ bmiHeader;
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER_);
	bmiHeader.biWidth = nW;
	bmiHeader.biHeight = nH;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 24;
	bmiHeader.biCompression = BI_RGB_;
	bmiHeader.biSizeImage = size;
	bmiHeader.biXPelsPerMeter = 2000;
	bmiHeader.biYPelsPerMeter = 2000;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER_ bmiFileHeader;
	bmiFileHeader.bfType = 'M'*0x0100 + 'B';
	bmiFileHeader.bfSize = dwBitsSize;
	bmiFileHeader.bfReserved1 = 0;
	bmiFileHeader.bfReserved2 = 0;

	bmiFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER_) + sizeof(BITMAPINFOHEADER_);

	FILE *fp;

	// 파일 열기
	fopen_s(&fp, FileName, "wb");

	// 파일 열기 실패
	if(!fp) return false;

	// 헤더 쓰기
	fwrite(&bmiFileHeader, sizeof(BITMAPFILEHEADER_), 1, fp);
	fwrite(&bmiHeader, sizeof(BITMAPINFOHEADER_), 1, fp);

	// 비트맵 정보 쓰기
	fwrite(Image1D, sizeof(unsigned char), size, fp);

	fclose(fp);

	return true;
}

void Move(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int x0, int y0)
{
	int x, y;
	int xx, yy;
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			xx =  x - x0;
			yy = y - y0;

			if(xx < 0 || xx >= nW || yy < 0 || yy >= nH)
				OutputGray[y][x] = 0;
			else
				OutputGray[y][x] = ImageGray[yy][xx];
		}
}

void ScaleX2(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y;
	int nOutW = nW*2, nOutH = nH*2;	// nW, nH: 원영상의 폭과 높이
									// nOutW, nOutH: 변환된 영상의 폭과 높이

	for(y = 0 ; y < nOutH ; y++)
		for(x = 0 ; x < nOutW ; x++)
		{
				OutputGray[y][x] = ImageGray[y/2][x/2];
		}
}

void Rotate(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nOutW, int nOutH, double dDeg)
{
	int x,y;	
	int xx, yy;
	double dAng = dDeg * acos(-1.) / 180.;	// 각도를 라디안으로 수정

	for(y = 0 ; y < nOutH ; y++)
		for(x = 0 ; x < nOutW ; x++)
		{
			xx = (int)((x-nOutW/2)*cos(dAng)+(y-nOutH/2)*sin(dAng));
			yy = (int)(-(x-nOutW/2)*sin(dAng)+(y-nOutH/2)*cos(dAng));

			xx += nW/2;
			yy += nH/2;

			if(xx < 0 || xx >= nW || yy < 0 || yy >= nH)
				OutputGray[y][x] = 0;
			else
				OutputGray[y][x] = ImageGray[yy][xx];
		}
}

void HistogramEqualization(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
  	unsigned char Table[GRAY_CNT];
 	int CumHisto[GRAY_CNT], Histogram[GRAY_CNT] ={0};
	int x, y, i;
	unsigned char gray;
		
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			Histogram[ImageGray[y][x]]++;

	int nSum = 0;
	for (i=0; i< GRAY_CNT ; i++) {
		nSum += Histogram[i];
		CumHisto[i] = nSum;
	}

	for (i=0; i< GRAY_CNT ; i++) 
		Table[i] = (unsigned char)((CumHisto[i] * (GRAY_CNT-1)) / (nW*nH));
 
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			gray = Table[ImageGray[y][x]];
			if (gray < 0) gray = 0;
			else if (gray >= GRAY_CNT) gray = GRAY_CNT-1;
			OutputGray[y][x] = gray;
		}
}

void MeanFilter(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nWinSize)
{
	int x, y;
	int dx, dy;
	int nSum;
	int xx, yy;

	if(nWinSize <= 0) return;

	double nRange = nWinSize/2.;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			nSum = 0;
			for(dy = -(int)nRange ; dy < nRange ; dy++)
				for(dx = -(int)nRange ; dx < nRange ; dx++)
				{
					xx = x+dx;
					yy = y+dy;

					if(xx < 0) xx = 0;
					if(xx >= nW) xx = nW-1;
					if(yy < 0) yy = 0;
					if(yy >= nH) yy = nH-1;

					nSum += ImageGray[yy][xx];
				}
			
			OutputGray[y][x] = nSum/(nWinSize*nWinSize);
		}
}

void MedianFilter(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nWinSize)
{
	int x, y;
	int dx, dy;
	int xx, yy;

	if(nWinSize <= 0) return;

	double nRange = nWinSize/2.;

	unsigned char *Sort, nInsert;
	int nIndex, i;

	Sort = new unsigned char[nWinSize*nWinSize];

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			nIndex = 0;
			for(dy = -(int)nRange ; dy < nRange ; dy++)
				for(dx = -(int)nRange ; dx < nRange ; dx++)
				{
					xx = x+dx;
					yy = y+dy;

					if(xx < 0) xx = 0;
					if(xx >= nW) xx = nW-1;
					if(yy < 0) yy = 0;
					if(yy >= nH) yy = nH-1;

					nInsert = ImageGray[yy][xx];

					// 삽입 정렬
					if(nIndex == 0) Sort[nIndex] = nInsert;
					else
					{
						for(i = nIndex - 1 ; i >= 0 && Sort[i] > nInsert ; i--)
							Sort[i+1] = Sort[i];

						Sort[i+1] = nInsert;
					}
					nIndex++;
				}	
			OutputGray[y][x] = Sort[nWinSize*nWinSize/2];
		}

	delete [] Sort;
}

void Sobel(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y;
	int nGx, nGy;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x <= 0 || x >= nW-1 || y <= 0 || y >= nH-1)
				OutputGray[y][x] = 0;
			else
			{
				nGx = ImageGray[y-1][x+1] + 2*ImageGray[y][x+1] + ImageGray[y+1][x+1]
					- ImageGray[y-1][x-1] - 2*ImageGray[y][x-1] - ImageGray[y+1][x-1];

				nGy = ImageGray[y+1][x-1] + 2*ImageGray[y+1][x] + ImageGray[y+1][x+1]
					- ImageGray[y-1][x-1] - 2*ImageGray[y-1][x] - ImageGray[y-1][x+1];
				
				OutputGray[y][x] = (abs(nGx) + abs(nGy))/8;
			}
		}
}

void SobelXY(unsigned char **ImageGray, double **OutputGrayX, double **OutputGrayY, int nW, int nH)
{
	int x, y;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x <= 0 || x >= nW-1 || y <= 0 || y >= nH-1)
			{
				OutputGrayX[y][x] = 0;
				OutputGrayY[y][x] = 0;
			}
			else
			{
				OutputGrayX[y][x] = ImageGray[y-1][x+1] + 2*ImageGray[y][x+1] + ImageGray[y+1][x+1]
					- ImageGray[y-1][x-1] - 2*ImageGray[y][x-1] - ImageGray[y+1][x-1];

				OutputGrayY[y][x] = ImageGray[y+1][x-1] + 2*ImageGray[y+1][x] + ImageGray[y+1][x+1]
					- ImageGray[y-1][x-1] - 2*ImageGray[y-1][x] - ImageGray[y-1][x+1];
			}
		}
}

void Laplacian(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y;
	int nSum;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x <= 0 || x >= nW-1 || y <= 0 || y >= nH-1)
				OutputGray[y][x] = 0;
			else
			{
				nSum =  4*ImageGray[y][x] - ImageGray[y-1][x] - ImageGray[y][x-1] 
					- ImageGray[y][x+1] - ImageGray[y+1][x];

				nSum = (nSum+GRAY_CNT)/2;

				if(nSum < 0) nSum = 0;
				else if(nSum >= GRAY_CNT) nSum = GRAY_CNT-1;
				
				OutputGray[y][x] = nSum;
			}
		}
}

void LaplacianSharp(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y;
	int nSum;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x <= 0 || x >= nW-1 || y <= 0 || y >= nH-1)
				OutputGray[y][x] = 0;
			else
			{
				nSum =  4*ImageGray[y][x] - ImageGray[y-1][x] - ImageGray[y][x-1] 
					- ImageGray[y][x+1] - ImageGray[y+1][x];

				nSum = ImageGray[y][x] + nSum;

				if(nSum < 0) nSum = 0;
				else if(nSum >= GRAY_CNT) nSum = GRAY_CNT-1;
				
				OutputGray[y][x] = nSum;
			}
		}
}

int NeighborMatch(unsigned char  **ImageGray, int x, int y, int pat)
{
	switch (pat) {

	   case 0:
	      if ((ImageGray[y-1][x-1] != 0 || ImageGray[y-1][x] != 0 || ImageGray[y-1][x+1] != 0) &&
		  (ImageGray[y+1][x-1] != 0 || ImageGray[y+1][x] != 0 || ImageGray[y+1][x+1] != 0) &&
		  (ImageGray[y][x-1] == 0 && ImageGray[y][x+1] == 0) )
		  return (TRUE);
	      break;

	   case 1:
	      if ((ImageGray[y-1][x-1] != 0 || ImageGray[y][x-1] != 0 || ImageGray[y+1][x-1] != 0) &&
		  (ImageGray[y-1][x+1] != 0 || ImageGray[y][x+1] != 0 || ImageGray[y+1][x+1] != 0) &&
		  (ImageGray[y+1][x] == 0 && ImageGray[y-1][x] == 0) )
		  return (TRUE);
	      break;

	   case 2:
	      if ((ImageGray[y-1][x+1] != 0 || ImageGray[y-1][x] != 0 || ImageGray[y-1][x-1] != 0 ||
		   ImageGray[y][x-1] != 0 || ImageGray[y+1][x-1] != 0) &&
		   (ImageGray[y][x+1] == 0 && ImageGray[y+1][x] == 0 && ImageGray[y+1][x+1] == 2) )
                  return (TRUE);
	      break;

	   case 3:
	      if ((ImageGray[y-1][x-1] != 0 || ImageGray[y][x-1] != 0 || ImageGray[y+1][x-1] != 0 ||
		   ImageGray[y+1][x] != 0 || ImageGray[y+1][x+1] != 0) &&
		   (ImageGray[y][x+1] == 0 && ImageGray[y-1][x] == 0 && ImageGray[y-1][x+1] == 2) )
                  return (TRUE);
	      break;

	   case 4:
	      if ((ImageGray[y+1][x-1] != 0 || ImageGray[y+1][x] != 0 || ImageGray[y+1][x+1] != 0 ||
		   ImageGray[y][x+1] != 0 || ImageGray[y-1][x+1] != 0) &&
		   (ImageGray[y-1][x] == 0 && ImageGray[y][x-1] == 0 && ImageGray[y-1][x-1] == 2) )
                  return (TRUE);
	      break;

	   case 5:
	      if ((ImageGray[y+1][x+1] != 0 || ImageGray[y][x+1] != 0 || ImageGray[y-1][x+1] != 0 ||
		   ImageGray[y-1][x] != 0 || ImageGray[y-1][x-1] != 0) &&
		   (ImageGray[y][x-1] == 0 && ImageGray[y+1][x] == 0 && ImageGray[y+1][x-1] == 2) )
                  return (TRUE);
	      break;

	   default:
	      break;
	}

        return (FALSE);

}

void Thinning(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y;
	int FourNY[4] = {0, -1, 0, 1};
	int FourNX[4] = {1, 0, -1, 0};

	for(y = 0 ; y < nH ; y++) 
		for(x = 0 ; x < nW; x++) 
		{
			if(ImageGray[y][x] > GRAY_CNT/2) OutputGray[y][x] = 1;
			else OutputGray[y][x] = 0;
		}

	BOOL Remain = TRUE;
	int j, Pattern;
	BOOL bSkeleton;
	while(Remain) 
	{
		Remain = FALSE;
		for(j = 0 ; j < 4; j++) 
		{
			for(y = 1 ; y < nH-1 ; y++) 
				for(x = 1 ; x < nW-1 ; x++) 
				{
					if(OutputGray[y][x] == 1 && OutputGray[y+FourNY[j]][x+FourNX[j]] == 0) {
						bSkeleton = FALSE;
						for(Pattern = 0 ; Pattern < 6; Pattern++) {
							if (NeighborMatch(OutputGray, x, y, Pattern)) {
								bSkeleton = TRUE;
								break;
							}
						}

						if(bSkeleton) OutputGray[y][x] = 2;
						else {
							OutputGray[y][x] = 3;
							Remain = TRUE;
						}
					}
				}

			for(y = 1 ; y < nH-1 ; y++) 
				for(x = 1 ; x < nW-1 ; x++) 
				{
					if(OutputGray[y][x] == 3) OutputGray[y][x] = 0;
				}
		}
	}

	for(y = 0 ; y < nH ; y++) 
		for(x = 0 ; x < nW ; x++) 
		{
			if(OutputGray[y][x] == 2) OutputGray[y][x] = GRAY_CNT-1;
			else OutputGray[y][x] = 0;
        }
}
	
int Labeling(unsigned char **ImageGray, int **Label, int nW, int nH, int nConnNumThre)
{
	int x, y, num, left, top, k; 
	int *r, *area;

	r = new int[nW*nH];
	area = new int[nW*nH];

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ImageGray[y][x] > 128) Label[y][x] = 0;
			else Label[y][x] = -1;

	for (x = 0 ; x < nW ; x++) {
		Label[0][x] = -1;
		Label[nH-1][x] = -1;
	}
	for (y = 0 ; y < nH ; y++) {
		Label[y][0] = -1;
		Label[y][nW-1] = -1;
	}

	num = -1;
	for (y = 0; y < nH; y++) {
		for (x = 0; x < nW; x++) {
			if (y > 0 && x > 0) {
				if (Label[y][x] >= 0) {
					left = Label[y][x-1];
					top = Label[y-1][x];
					if (left == -1 && top != -1) {
						Label[y][x] = r[top];
					}
					else if (left != -1 && top == -1) {
						Label[y][x] = r[left];
					}
					else if (left == -1 && top == -1) {
						num++;
						if (num >= nW*nH) {
							delete [] r;
							delete [] area;
							return 0;
						}
		       
						r[num] = num;
						Label[y][x] = r[num];
					}
					else if (left != -1 && top != -1) {
						if (r[left] == r[top]) {
							Label[y][x] = r[left];
						}
						else if (r[left] > r[top]) {
							Label[y][x] = r[top];
							r[left] = r[top];
						}
						else {
							Label[y][x] = r[left];
							r[top] = r[left];
						}
					}
				}
			}
		}
	}
	
	for (k = 0; k <= num; k++) {
		if (k != r[k]) r[k] = r[r[k]];
			area[k] = 0;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] > -1) {
				Label[y][x] = r[Label[y][x]];
				area[Label[y][x]]++;
			}
		}

	int cnt=0;
	for (k = 0; k <= num; k++) {
		if (area[k] > nConnNumThre) r[k] = cnt++;
		else r[k] = -1;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] >= 0) 
				Label[y][x] = r[Label[y][x]];
		}

	delete [] r;
	delete [] area;

	return cnt;
}

int Labeling(unsigned char **ImageGray, unsigned char ForegroundValue, int **Label, int nW, int nH, int nConnNumThre)
{
	int x, y, num, left, top, k; 
	int *r, *area;

	r = new int[nW*nH];
	area = new int[nW*nH];

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ImageGray[y][x] == ForegroundValue) Label[y][x] = 0;
			else Label[y][x] = -1;

	for (x = 0 ; x < nW ; x++) {
		Label[0][x] = -1;
		Label[nH-1][x] = -1;
	}
	for (y = 0 ; y < nH ; y++) {
		Label[y][0] = -1;
		Label[y][nW-1] = -1;
	}

	num = -1;
	for (y = 0; y < nH; y++) {
		for (x = 0; x < nW; x++) {
			if (y > 0 && x > 0) {
				if (Label[y][x] >= 0) {
					left = Label[y][x-1];
					top = Label[y-1][x];
					if (left == -1 && top != -1) {
						Label[y][x] = r[top];
					}
					else if (left != -1 && top == -1) {
						Label[y][x] = r[left];
					}
					else if (left == -1 && top == -1) {
						num++;
						if (num >= nW*nH) {
							delete [] r;
							delete [] area;
							return 0;
						}
		       
						r[num] = num;
						Label[y][x] = r[num];
					}
					else if (left != -1 && top != -1) {
						if (r[left] == r[top]) {
							Label[y][x] = r[left];
						}
						else if (r[left] > r[top]) {
							Label[y][x] = r[top];
							r[left] = r[top];
						}
						else {
							Label[y][x] = r[left];
							r[top] = r[left];
						}
					}
				}
			}
		}
	}
	
	for (k = 0; k <= num; k++) {
		if (k != r[k]) r[k] = r[r[k]];
			area[k] = 0;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] > -1) {
				Label[y][x] = r[Label[y][x]];
				area[Label[y][x]]++;
			}
		}

	int cnt=0;
	for (k = 0; k <= num; k++) {
		if (area[k] > nConnNumThre) r[k] = cnt++;
		else r[k] = -1;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] >= 0) 
				Label[y][x] = r[Label[y][x]];
		}

	delete [] r;
	delete [] area;

	return cnt;
}

int Labeling(unsigned char **ImageGray, unsigned char ForegroundValue1, unsigned char ForegroundValue2, int **Label, int nW, int nH, int nConnNumThre)
{
	int x, y, num, left, top, k; 
	int *r, *area;

	r = new int[nW*nH];
	area = new int[nW*nH];

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ForegroundValue1 <= ImageGray[y][x] && ImageGray[y][x] <= ForegroundValue2) Label[y][x] = 0;
			else Label[y][x] = -1;

	for (x = 0 ; x < nW ; x++) {
		Label[0][x] = -1;
		Label[nH-1][x] = -1;
	}
	for (y = 0 ; y < nH ; y++) {
		Label[y][0] = -1;
		Label[y][nW-1] = -1;
	}

	num = -1;
	for (y = 1; y < nH; y++) {
		for (x = 1; x < nW; x++) {
			//if (y > 0 && x > 0)
			{
				if (Label[y][x] < 0) continue;
				//if (Label[y][x] >= 0) 
				{
					left = Label[y][x-1];
					top = Label[y-1][x];

					if (left == -1 && top != -1) 
					{
						Label[y][x] = r[top];
					}
					else if (left != -1 && top == -1) 
					{
						Label[y][x] = r[left];
					}
					else if (left == -1 && top == -1) 
					{
						num++;
						if (num >= nW*nH) {
							delete [] r;
							delete [] area;
							return 0;
						}
		       
						r[num] = num;
						Label[y][x] = r[num];
					}
					else if (left != -1 && top != -1) 
					{
						int leftR = r[left];
						int topR = r[top];
						if (r[left] == topR) {
							Label[y][x] = leftR;
						}
						else if (r[left] > topR) {
							Label[y][x] = topR;
							r[left] = topR;
						}
						else {
							Label[y][x] = leftR;
							r[top] = leftR;
						}
					}
				}
			}
		}
	}
	
	for (k = 0; k <= num; k++) {
		if (k != r[k]) r[k] = r[r[k]];
			
		area[k] = 0;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			int *pLabel = &Label[y][x];
			if(*pLabel > -1) {
				*pLabel = r[*pLabel];
				area[*pLabel]++;
			}
		}

	int cnt=0;
	for (k = 0; k <= num; k++) {
		if (area[k] > nConnNumThre) r[k] = cnt++;
		else r[k] = -1;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			int *pLabel = &Label[y][x];
			if(*pLabel >= 0) 
				*pLabel = r[*pLabel];
		}

	delete [] r;
	delete [] area;

	return cnt;
}

int Labeling(unsigned char **ImageGray, int **Label, int nW, int nH, int nConnNumThre, int *Area)
{
	int x, y, num, left, top, k; 
	int *r, *rev;

	r = new int[nW*nH];
	rev = new int[nW*nH];

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ImageGray[y][x] > 128) Label[y][x] = 0;
			else Label[y][x] = -1;

	for (x = 0 ; x < nW ; x++) {
		Label[0][x] = -1;
		Label[nH-1][x] = -1;
	}
	for (y = 0 ; y < nH ; y++) {
		Label[y][0] = -1;
		Label[y][nW-1] = -1;
	}

	num = -1;
	for (y = 0; y < nH; y++) {
		for (x = 0; x < nW; x++) {
			if (y > 0 && x > 0) {
				if (Label[y][x] >= 0) {
					left = Label[y][x-1];
					top = Label[y-1][x];
					if (left == -1 && top != -1) {
						Label[y][x] = r[top];
					}
					else if (left != -1 && top == -1) {
						Label[y][x] = r[left];
					}
					else if (left == -1 && top == -1) {
						num++;
						if (num >= nW*nH) {
							delete [] r;
							return 0;
						}
		       
						r[num] = num;
						Label[y][x] = r[num];
					}
					else if (left != -1 && top != -1) {
						if (r[left] == r[top]) {
							Label[y][x] = r[left];
						}
						else if (r[left] > r[top]) {
							Label[y][x] = r[top];
							r[left] = r[top];
						}
						else {
							Label[y][x] = r[left];
							r[top] = r[left];
						}
					}
				}
			}
		}
	}
	
	for (k = 0; k <= num; k++) {
		if (k != r[k]) r[k] = r[r[k]];
			Area[k] = 0;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] > -1) {
				Label[y][x] = r[Label[y][x]];
				Area[Label[y][x]]++;
			}
		}

	int cnt=0;
	for (k = 0; k <= num; k++) {
		if (Area[k] > nConnNumThre) {
			rev[cnt] = k;
			r[k] = cnt++;
		}
		else r[k] = -1;
	}

	for(k = 0 ; k < cnt ; k++)
	{
		Area[k] = Area[rev[k]];
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] >= 0) 
				Label[y][x] = r[Label[y][x]];
		}

	delete [] r;
	delete [] rev;

	return cnt;
}

int Labeling(unsigned char **ImageGray, int **Label, int nW, int nH, int nConnNumThre, int nConnNumThre2)
{
	int x, y, num, left, top, k; 
	int *r, *area;

	r = new int[nW*nH];
	area = new int[nW*nH];

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ImageGray[y][x] > 128) Label[y][x] = 0;
			else Label[y][x] = -1;

	for (x = 0 ; x < nW ; x++) {
		Label[0][x] = -1;
		Label[nH-1][x] = -1;
	}
	for (y = 0 ; y < nH ; y++) {
		Label[y][0] = -1;
		Label[y][nW-1] = -1;
	}

	num = -1;
	for (y = 0; y < nH; y++) {
		for (x = 0; x < nW; x++) {
			if (y > 0 && x > 0) {
				if (Label[y][x] >= 0) {
					left = Label[y][x-1];
					top = Label[y-1][x];
					if (left == -1 && top != -1) {
						Label[y][x] = r[top];
					}
					else if (left != -1 && top == -1) {
						Label[y][x] = r[left];
					}
					else if (left == -1 && top == -1) {
						num++;
						if (num >= nW*nH) {
							delete [] r;
							delete [] area;
							return 0;
						}
		       
						r[num] = num;
						Label[y][x] = r[num];
					}
					else if (left != -1 && top != -1) {
						if (r[left] == r[top]) {
							Label[y][x] = r[left];
						}
						else if (r[left] > r[top]) {
							Label[y][x] = r[top];
							r[left] = r[top];
						}
						else {
							Label[y][x] = r[left];
							r[top] = r[left];
						}
					}
				}
			}
		}
	}
	
	for (k = 0; k <= num; k++) {
		if (k != r[k]) r[k] = r[r[k]];
			area[k] = 0;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] > -1) {
				Label[y][x] = r[Label[y][x]];
				area[Label[y][x]]++;
			}
		}

	int cnt=0;
	for (k = 0; k <= num; k++) {
		if (area[k] > nConnNumThre && area[k] < nConnNumThre2) r[k] = cnt++;
		else r[k] = -1;
	}

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++) {
			if(Label[y][x] >= 0) 
				Label[y][x] = r[Label[y][x]];
		}

	delete [] r;
	delete [] area;

	return cnt;
}

double DFT2D(unsigned char **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH)
{
	int x, y;
	int u, v;
	
	const double Pi = acos(-1.);

	_int64 freq, start, end;

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	for(v = 0 ; v < nH ; v++)
		for(u = 0 ; u < nW ; u++)
		{
			OutputReal[v][u] = OutputImag[v][u] = 0;
			for(y = 0 ; y < nH ; y++)
				for(x = 0 ; x < nW ; x++)
				{
					OutputReal[v][u] += ImageGray[y][x] * cos(-2.*Pi*((double)u*x/nW+(double)v*y/nH));
					OutputImag[v][u] += ImageGray[y][x] * sin(-2.*Pi*((double)u*x/nW+(double)v*y/nH));
				}
		}

	QueryPerformanceCounter((LARGE_INTEGER*)&end);
	// 처리 시간(초)
	double timeDiff = (double)(end - start)/(double)freq; 

	return timeDiff;
}

double RowColDFT2D(unsigned char **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH)
{
	int x, y;
	int u, v;
	double **Real, **Imag;

	const double Pi = acos(-1.);

	_int64 freq, start, end;

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	Real = dmatrix(nH, nW);
	Imag = dmatrix(nH, nW);

	for(y = 0 ; y < nH ; y++)
		for(u = 0 ; u < nW ; u++)
		{
			Real[y][u] = Imag[y][u] = 0;
			for(x = 0 ; x < nW ; x++)
			{
				Real[y][u] += ImageGray[y][x] * cos(-2.*Pi*u*x/nW);
				Imag[y][u] += ImageGray[y][x] * sin(-2.*Pi*u*x/nW);
			}
		}

	for(u = 0 ; u < nW ; u++)
		for(v = 0 ; v < nH ; v++)		
		{
			OutputReal[v][u] = OutputImag[v][u] = 0;
			for(y = 0 ; y < nH ; y++)
				{
					OutputReal[v][u] += Real[y][u] * cos(-2.*Pi*v*y/nH) - Imag[y][u] * sin(-2.*Pi*v*y/nH);
					OutputImag[v][u] += Real[y][u] * sin(-2.*Pi*v*y/nH) + Imag[y][u] * cos(-2.*Pi*v*y/nH);
				}
		}

	free_dmatrix(Real, nH, nW);
	free_dmatrix(Imag, nH, nW);

	QueryPerformanceCounter((LARGE_INTEGER*)&end);
	// 처리 시간(초)
	double timeDiff = (double)(end - start)/(double)freq; 

	return timeDiff;
}

void FFT2Radix(double *Xr, double *Xi, double *Yr, double *Yi, int nN, bool bInverse)
{
	int i, j, k;

	double T, Wr, Wi;
	const double Pi = acos(-1.);

	if(nN <= 1) return;

	for(i = 0 ; i < nN ; i++)
	{
		Yr[i] = Xr[i];
		Yi[i] = Xi[i];
	}

    j = 0;
    for (i = 1 ; i < (nN-1) ; i++) {
        k = nN/2;
        while(k <= j) {
            j = j - k;
            k = k/2;
        }
        j = j + k;
        if (i < j) {
            T = Yr[j];
            Yr[j] = Yr[i];
			Yr[i] = T;

			T = Yi[j];
            Yi[j] = Yi[i];
			Yi[i] = T;
        }
    }

	double Tr, Ti;
	int iter, j2, pos;
	k = nN >> 1;
	iter = 1;
	while(k > 0)
	{
		j = 0;
		j2 = 0;
		for(i = 0 ; i < nN >> 1 ; i++)
		{
			Wr = cos(2.*Pi*(j2*k)/nN);
			if(bInverse == 0)
				Wi = -sin(2.*Pi*(j2*k)/nN);
			else
				Wi = sin(2.*Pi*(j2*k)/nN);

			pos = j+(1 << (iter-1));

			Tr =	Yr[pos] * Wr - Yi[pos] * Wi;
			Ti = Yr[pos] * Wi +	Yi[pos] * Wr;

			Yr[pos] = Yr[j] - Tr;
			Yi[pos] = Yi[j] - Ti;

			Yr[j] += Tr;			
			Yi[j] += Ti;

			j += 1 << iter;
			if(j >= nN) j = ++j2;
		}

		k >>= 1;
		iter++;
	}

	if(bInverse)
	{
		for(i = 0 ; i < nN ; i++)
		{
			Yr[i] /= nN;
			Yi[i] /= nN;
		}
	}
}

double FFT2D(unsigned char **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH)
{
	int x, y;
	double *dRealX, *dImagX;
	double *dRealY, *dImagY;

	const double Pi = acos(-1.);

	_int64 freq, start, end;

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	dRealX = new double[nW];
	dImagX = new double[nW];
	dRealY = new double[nW];
	dImagY = new double[nW];

	for(y = 0 ; y < nH ; y++)
	{
		for(x = 0 ; x < nW ; x++)
		{
			dRealX[x] = ImageGray[y][x];
			dImagX[x] = 0.;
		}
		
		FFT2Radix(dRealX, dImagX, dRealY, dImagY, nW, false);

		for(x = 0 ; x < nW ; x++)
		{
			OutputReal[y][x] = dRealY[x];
			OutputImag[y][x] = dImagY[x];
		}
	}

	delete [] dRealX;
	delete [] dImagX;
	delete [] dRealY;
	delete [] dImagY;

	dRealX = new double[nH];
	dImagX = new double[nH];
	dRealY = new double[nH];
	dImagY = new double[nH];

	for(x = 0 ; x < nW ; x++)
	{
		for(y = 0 ; y < nH ; y++)
		{
			dRealX[y] = OutputReal[y][x];
			dImagX[y] = OutputImag[y][x];
		}
		
		FFT2Radix(dRealX, dImagX, dRealY, dImagY, nH, false);

		for(y = 0 ; y < nH ; y++)
		{
			OutputReal[y][x] = dRealY[y];
			OutputImag[y][x] = dImagY[y];
		}
	}

	delete [] dRealX;
	delete [] dImagX;
	delete [] dRealY;
	delete [] dImagY;

	QueryPerformanceCounter((LARGE_INTEGER*)&end);
	// 처리 시간(초)
	double timeDiff = (double)(end - start)/(double)freq; 

	return timeDiff;
}

void FFT2DInverse(double **InputReal, double **InputImag, double **OutputDouble, int nW, int nH)
{
	int x, y;
	double *dRealX, *dImagX;
	double *dRealY, *dImagY;
	double **OutputReal, **OutputImag;

	const double Pi = acos(-1.);

	OutputReal = dmatrix(nH, nW);
	OutputImag = dmatrix(nH, nW);

	dRealX = new double[nW];
	dImagX = new double[nW];
	dRealY = new double[nW];
	dImagY = new double[nW];

	for(y = 0 ; y < nH ; y++)
	{
		for(x = 0 ; x < nW ; x++)
		{
			dRealX[x] = InputReal[y][x];
			dImagX[x] = InputImag[y][x];
		}
		
		FFT2Radix(dRealX, dImagX, dRealY, dImagY, nW, true);

		for(x = 0 ; x < nW ; x++)
		{
			OutputReal[y][x] = dRealY[x];
			OutputImag[y][x] = dImagY[x];
		}
	}

	delete [] dRealX;
	delete [] dImagX;
	delete [] dRealY;
	delete [] dImagY;

	dRealX = new double[nH];
	dImagX = new double[nH];
	dRealY = new double[nH];
	dImagY = new double[nH];

	for(x = 0 ; x < nW ; x++)
	{
		for(y = 0 ; y < nH ; y++)
		{
			dRealX[y] = OutputReal[y][x];
			dImagX[y] = OutputImag[y][x];
		}
		
		FFT2Radix(dRealX, dImagX, dRealY, dImagY, nH, true);

		for(y = 0 ; y < nH ; y++)
		{
			OutputReal[y][x] = dRealY[y];
			OutputImag[y][x] = dImagY[y];
		}
	}

	delete [] dRealX;
	delete [] dImagX;
	delete [] dRealY;
	delete [] dImagY;

	for(x = 0 ; x < nW ; x++)
	{
		for(y = 0 ; y < nH ; y++)
		{
			OutputDouble[y][x] = OutputReal[y][x];
		}
	}

	free_dmatrix(OutputReal, nH, nW);
	free_dmatrix(OutputImag, nH, nW);
}

void DCT2D(unsigned char **ImageGray, double **OutputDouble, int nW, int nH, int nBlockSize)
{
	int x, y;
	int u, v;
	int BlockX, BlockY;

	const double Pi = acos(-1.);

	for(v = 0 ; v < nH ; v++)
		for(u = 0 ; u < nW ; u++)
			OutputDouble[v][u] = 0;

	for(BlockY = 0 ; BlockY < nH-nBlockSize+1 ; BlockY += nBlockSize)
		for(BlockX = 0 ; BlockX < nW-nBlockSize+1 ; BlockX += nBlockSize)
		{
			for(v = 0 ; v < nBlockSize ; v++)
				for(u = 0 ; u < nBlockSize ; u++)
				{
					OutputDouble[BlockY+v][BlockX+u] = 0;

					for(y = 0 ; y < nBlockSize ; y++)
						for(x = 0 ; x < nBlockSize ; x++)
						{
							OutputDouble[BlockY+v][BlockX+u] += 
								ImageGray[BlockY+y][BlockX+x] 
								* cos((2*x+1)*u*Pi/(2.*nBlockSize)) * cos((2*y+1)*v*Pi/(2.*nBlockSize));
						}

					if(u == 0)
						OutputDouble[BlockY+v][BlockX+u] *= sqrt(1./nBlockSize);
					else
						OutputDouble[BlockY+v][BlockX+u] *= sqrt(2./nBlockSize);

					if(v == 0)
						OutputDouble[BlockY+v][BlockX+u] *= sqrt(1./nBlockSize);
					else
						OutputDouble[BlockY+v][BlockX+u] *= sqrt(2./nBlockSize);
				}
		}
}

void pTileThresholding(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, double dRate, int nType)
{
	int x, y, i, Thre;
	int Sum;
	int Histogram[GRAY_CNT] ={0};

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			Histogram[ImageGray[y][x]]++;

	Sum = 0;

	if(nType == 0) // 상위
	{
		for(i = GRAY_CNT-1 ; i >= 0 ; i--)
		{
			Sum += Histogram[i];
			if(Sum > nW*nH*dRate)
			{
				Thre = i;
				break;
			}
		}
	}
	else	// 하위
	{
		for(i = 0 ; i < GRAY_CNT ; i++)
		{
			Sum += Histogram[i];
			if(Sum > nW*nH*dRate)
			{
				Thre = i;
				break;
			}
		}
	}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(!nType && ImageGray[y][x] > Thre ||	// 상위
				nType && ImageGray[y][x] < Thre)	// 하위
				OutputGray[y][x] = GRAY_CNT-1;
			else
				OutputGray[y][x] = 0;
}

void ValleyThresholding(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y, i, Thre;
	int M1, M2;
	int Max;
	int Histogram[GRAY_CNT] ={0};

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			Histogram[ImageGray[y][x]]++;

	Max = 0;
	for(i = 0 ; i < GRAY_CNT ; i++)
	{
		if(Histogram[i] > Max) 
		{
			Max = Histogram[i];
			M1 = i;
		}
	}

	Max = 0;
	for(i = 0 ; i < GRAY_CNT ; i++)
	{
		if(abs(i-M1)*Histogram[i] > Max) 
		{
			Max = abs(i-M1)*Histogram[i];
			M2 = i;
		}
	}

	Thre = (M1 + M2)/2;
	
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ImageGray[y][x] > Thre)
				OutputGray[y][x] = GRAY_CNT-1;
			else
				OutputGray[y][x] = 0;
}

void IterateThresholding(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH)
{
	int x, y, i, Thre, PrevThre;
	int M1, M2;
	int nCnt1, nCnt2;
	int Histogram[GRAY_CNT] ={0};

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			Histogram[ImageGray[y][x]]++;

	for(i = 0 ; i < GRAY_CNT ; i++)
	{
		if(Histogram[i] > 0)
		{
			M1 = i;
			break;
		}
	}

	for(i = GRAY_CNT-1 ; i >= 0 ; i--)
	{
		if(Histogram[i] > 0)
		{
			M2 = i;
			break;
		}
	}

	Thre = (M1 + M2)/2;
	PrevThre = -1;

	while(PrevThre != Thre)
	{
		M1 = M2 = 0;
		nCnt1 = nCnt2 = 0;
		for(i = 0 ; i < GRAY_CNT ; i++)
			if(i > Thre)
			{
				M2 += i*Histogram[i];
				nCnt2 += Histogram[i];
			}
			else
			{
				M1 += i*Histogram[i];
				nCnt1 += Histogram[i];
			}

		M1 /= nCnt1;
		M2 /= nCnt2;

		PrevThre = Thre;
		Thre = (M1 + M2)/2;
	}
	
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ImageGray[y][x] > Thre)
				OutputGray[y][x] = GRAY_CNT-1;
			else
				OutputGray[y][x] = 0;
}

void OtsuBinarization(unsigned char **Image, int nW, int nH, unsigned char **Out) 
{
	int His[256] = {0};
	int x, y;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			His[Image[y][x]]++;

    int sum = 0, i;
    for (i = 1 ; i < 256 ; i++)
        sum += i * His[i];

    int sumB = 0;
    double wB = 0, wF = 0;
    double mB, mF;
    double max = 0;
    double between;
    int threshold = 0;
    
	for (i = 0 ; i < 256 ; i++) 
	{
        wB += His[i];
        if(wB == 0) continue;
        wF = nW*nH - wB;
        if(wF == 0) break;
        sumB += i * His[i];

        mB = sumB / wB;
        mF = (sum - sumB) / wF;
        between = wB * wF * pow(mB - mF, 2);
        if (between > max) {
            max = between;
            threshold = i;
        }
    }

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(Image[y][x] > threshold)
				Out[y][x] = 255;
			else Out[y][x] = 0;
}

void HoughTransform(unsigned char **ImageGray, double **HoughSpace, int nW, int nH) 
{
	int nSW = 180;
	int nSH = (int)(sqrt((double)(nW*nW+nH*nH))/2)*2;
	int x, y;
	int r, theta;

	const double Pi = acos(-1.);

	for(r = 0 ; r < nSH ; r++)
		for(theta = 0 ; theta < nSW ; theta++)
			HoughSpace[r][theta] = 0;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(ImageGray[y][x] > GRAY_CNT/2)
			{
				for(theta = 0 ; theta < nSW ; theta++)
				{
					r = (int)(y*sin(theta*Pi/180) + x*cos(theta*Pi/180));

					HoughSpace[nSH/2+r/2][theta]++;
				}
			}
		}
}

void Turtle(unsigned char **ImageGray, int nW, int nH, CIpPoint *Boundary, int *pCnt)
{
	int x, y;
	int DirX, DirY;
	int TempDirX;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(x == 0 || x == nW-1 || y == 0 || y == nH-1)
				ImageGray[y][x] = 0;

	DirX = DirY = 0;
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(ImageGray[y][x] > GRAY_CNT/2) 
			{
				Boundary[0].x = x;
				Boundary[0].y = y;
				DirX = 1;
				goto Exit;
			}
Exit:

	*pCnt = 0;
	if(DirX == 0 && DirY == 0) return;

	BOOL Loop;
	Loop = TRUE;

	*pCnt = 1;

	Boundary[*pCnt].x = Boundary[*pCnt-1].x + DirX;
	Boundary[*pCnt].y = Boundary[*pCnt-1].y + DirY;

	while(Boundary[*pCnt] != Boundary[0])
	{
		if(ImageGray[Boundary[*pCnt].y][Boundary[*pCnt].x] > GRAY_CNT/2)
		{
			if(Boundary[*pCnt] != Boundary[*pCnt-1])
				(*pCnt)++;

			TempDirX = DirY ? DirY : 0;
			DirY = DirX ? -DirX : 0;
			DirX = TempDirX;

			Boundary[*pCnt].x = Boundary[*pCnt-1].x + DirX;
			Boundary[*pCnt].y = Boundary[*pCnt-1].y + DirY;
		}
		else
		{
			TempDirX = DirY ? -DirY : 0;
			DirY = DirX ? DirX : 0;
			DirX = TempDirX;
				
			Boundary[*pCnt].x = Boundary[*pCnt].x + DirX;
			Boundary[*pCnt].y = Boundary[*pCnt].y + DirY;
		}
	}
}

void Turtle(int **Label, int nW, int nH, int SelLabel, CIpPoint *Boundary, int *pCnt)
{
	int x, y;
	int DirX, DirY;
	int TempDirX;

	DirX = DirY = 0;
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(Label[y][x] == SelLabel) 
			{
				Boundary[0].x = x;
				Boundary[0].y = y;
				DirX = 1;
				goto Exit;
			}
Exit:

	*pCnt = 0;
	if(DirX == 0 && DirY == 0) return;

	BOOL Loop;
	Loop = TRUE;

	*pCnt = 1;

	Boundary[*pCnt].x = Boundary[*pCnt-1].x + DirX;
	Boundary[*pCnt].y = Boundary[*pCnt-1].y + DirY;

	while(Boundary[*pCnt] != Boundary[0])
	{
		if(Label[Boundary[*pCnt].y][Boundary[*pCnt].x] == SelLabel)
		{
			if(Boundary[*pCnt] != Boundary[*pCnt-1])
				(*pCnt)++;

			TempDirX = DirY ? DirY : 0;
			DirY = DirX ? -DirX : 0;
			DirX = TempDirX;

			Boundary[*pCnt].x = Boundary[*pCnt-1].x + DirX;
			Boundary[*pCnt].y = Boundary[*pCnt-1].y + DirY;
		}
		else
		{
			TempDirX = DirY ? -DirY : 0;
			DirY = DirX ? DirX : 0;
			DirX = TempDirX;
				
			Boundary[*pCnt].x = Boundary[*pCnt].x + DirX;
			Boundary[*pCnt].y = Boundary[*pCnt].y + DirY;
		}
	}
}

static double GaussFn(double x, double sigma)
{
	return exp(-1. * x * x /(2. * sigma * sigma));
}

static double MeanGaussFn(double x, double sigma)
{
	double z;
	const double PI = acos(-1.);

	z = (GaussFn(x, sigma) +GaussFn(x+0.5, sigma)+GaussFn(x-0.5, sigma))/3.;
	z /= sqrt(PI*2.*sigma*sigma);	

	return z;
}

void ComputekCosine(CIpPoint *Boundary, int nCnt, int k, double *kCosine)
{
	int i;
	double abs1, abs2, cosValue;

	const double Pi = acos(-1.);

	for(i = 0 ; i < nCnt ; i++)
	{
		abs1= sqrt((double)(Boundary[(i-k+nCnt)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i-k+nCnt)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i-k+nCnt)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i-k+nCnt)%nCnt].y - Boundary[i].y));
		abs2= sqrt((double)(Boundary[(i+k)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+k)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y));
		cosValue = ((double)(Boundary[(i-k+nCnt)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+k)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i-k+nCnt)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y)) /
			(abs1*abs2);
		
		kCosine[i] = cosValue;
	}

	{
		const int MaxMaskSize = 20;
		double Gauss[MaxMaskSize];
		int MaskSize=MaxMaskSize-1;

		for(i = 0 ; i < MaxMaskSize ; i++)
		{
			Gauss[i] = MeanGaussFn((double)i, 4.0);

			if(Gauss[i] < 0.00005)
			{
				MaskSize = i;
				break;
			}
		}

		double *NewkCosine;

		NewkCosine = new double[nCnt];

		for(i = 0 ; i < nCnt ; i++)
		{
			int k;

			NewkCosine[i] = kCosine[i]*Gauss[0];
			for(k = 1 ; k < MaskSize ; k++)
				NewkCosine[i] += kCosine[(i-k+nCnt)%nCnt]*Gauss[k] + kCosine[(i+k)%nCnt]*Gauss[k];

			if(NewkCosine[i] > 1) NewkCosine[i] = 1;
			else if(NewkCosine[i] < -1) NewkCosine[i] = -1;
		}

		for(i = 0 ; i < nCnt ; i++)
			kCosine[i] = NewkCosine[i];


		delete [] NewkCosine;
	}
}

void ComputekCosine2(CIpPoint *Boundary, int nCnt, int k, double *kCosine, bool *bConves, int xC, int yC)
{
	int i;
	double abs1, abs2, cosValue;
	double dist1, dist2, dist0;

	const double Pi = acos(-1.);

	for(i = 0 ; i < nCnt ; i++)
	{
		abs1= sqrt((double)(Boundary[(i-k+nCnt*10)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i-k+nCnt*10)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i-k+nCnt*10)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i-k+nCnt*10)%nCnt].y - Boundary[i].y));
		abs2= sqrt((double)(Boundary[(i+k)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+k)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y));
		cosValue = ((double)(Boundary[(i-k+nCnt*10)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+k)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i-k+nCnt*10)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y)) /
			(abs1*abs2);

		dist0 = Dist(xC, yC, Boundary[i].x, Boundary[i].y);
		dist1 = Dist(xC, yC, Boundary[(i-k+nCnt*10)%nCnt].x, Boundary[(i-k+nCnt*10)%nCnt].y);
		dist2 = Dist(xC, yC, Boundary[(i+k)%nCnt].x, Boundary[(i+k)%nCnt].y);

		if(dist0 > (dist1+dist2)/2) bConves[i] = true;
		else bConves[i] = false;
		
		kCosine[i] = cosValue;
	}

	double *NewkCosine;

	NewkCosine = new double[nCnt];

	for(i = 0 ; i < nCnt ; i++)
		NewkCosine[i] = kCosine[(i-1+nCnt)%nCnt]*0.25 + kCosine[i]*0.5 + kCosine[(i+1)%nCnt]*0.25;

	for(i = 0 ; i < nCnt ; i++)
		kCosine[i] = NewkCosine[i];


	delete [] NewkCosine;
}

void GaussianSmooth(CIpPoint *Boundary, int nCnt, double sigma)
{
	int i;
	CIpPoint *NewBoundary;
	NewBoundary = new CIpPoint[nCnt];

	const int MaxMaskSize = 20;
	double Gauss[MaxMaskSize];
	int MaskSize=MaxMaskSize-1;

	for(i = 0 ; i < MaxMaskSize ; i++)
	{
		Gauss[i] = MeanGaussFn((double)i, sigma /*4.0*/);

		if(Gauss[i] < 0.00005)
		{
			MaskSize = i;
			break;
		}
	}

	for(i = 0 ; i < nCnt ; i++)
	{
		int k;
		double SumX, SumY;

		SumX = Boundary[i].x * Gauss[0];
		SumY = Boundary[i].y * Gauss[0];

		for(k = 1 ; k < MaskSize ; k++)
		{
			SumX += Boundary[(i-k+nCnt)%nCnt].x * Gauss[k] + Boundary[(i+k)%nCnt].x * Gauss[k];
			SumY += Boundary[(i-k+nCnt)%nCnt].y * Gauss[k] + Boundary[(i+k)%nCnt].y * Gauss[k];
		}

		NewBoundary[i].x = (int)SumX;
		NewBoundary[i].y = (int)SumY;
	}

	for(i = 0 ; i < nCnt ; i++)
		Boundary[i] = NewBoundary[i];


	delete [] NewBoundary;
}

void GaussianSmooth(double *Data, int nCnt, double sigma)
{
	int i;
	double *NewData;
	NewData = new double[nCnt];

	const int MaxMaskSize = 20;
	double Gauss[MaxMaskSize];
	int MaskSize=MaxMaskSize-1;

	for(i = 0 ; i < MaxMaskSize ; i++)
	{
		Gauss[i] = MeanGaussFn((double)i, sigma /*4.0*/);

		if(Gauss[i] < 0.00005)
		{
			MaskSize = i;
			break;
		}
	}

	for(i = 0 ; i < nCnt ; i++)
	{
		int k;
		double Sum;

		Sum = Data[i] * Gauss[0];

		for(k = 1 ; k < MaskSize ; k++)
			Sum += Data[(i-k+nCnt)%nCnt] * Gauss[k] + Data[(i+k)%nCnt] * Gauss[k];

		NewData[i] = Sum;
	}

	for(i = 0 ; i < nCnt ; i++)
		Data[i] = NewData[i];


	delete [] NewData;
}

void DrawLine(unsigned char **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char Color)
{
	int nDiffX = abs(x0-x1);
	int nDiffY = abs(y0-y1);

	int x, y;
	int nFrom, nTo;

	if(nDiffY == 0 && nDiffX == 0)
	{
		y = y0;
		x = x0;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
			ImageGray[y][x] = Color;
	}
	else if(nDiffX == 0)
	{
		x = x0;
		
		nFrom = (y0 < y1 ? y0 : y1);
		if(nFrom < 0) nFrom = 0;
		nTo = (y0 < y1 ? y1 : y0);
		if(nTo >= nH) nTo = nH-1;

		for(y = nFrom ; y <= nTo ; y++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
				ImageGray[y][x] = Color;
		}
	}
	else if(nDiffY == 0)
	{
		y = y0;

		nFrom = (x0 < x1 ? x0 : x1);
		if(nFrom < 0) nFrom = 0;
		nTo = (x0 < x1 ? x1 : x0);
		if(nTo >= nW) nTo = nW-1;

		for(x = nFrom ; x <= nTo ; x++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
				ImageGray[y][x] = Color;
		}
	}
	else if(nDiffY > nDiffX)
	{
		nFrom = (y0 < y1 ? y0 : y1);
		if(nFrom < 0) nFrom = 0;
		nTo = (y0 < y1 ? y1 : y0);
		if(nTo >= nH) nTo = nH-1;

		for(y = nFrom ; y <= nTo ; y++)
		{
			x = (y-y0)*(x0-x1)/(y0-y1) + x0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			ImageGray[y][x] = Color;
		}
	}
	else
	{
		nFrom = (x0 < x1 ? x0 : x1);
		if(nFrom < 0) nFrom = 0;
		nTo = (x0 < x1 ? x1 : x0);
		if(nTo >= nW) nTo = nW-1;

		for(x = nFrom ; x <= nTo ; x++)
		{
			y = (x-x0)*(y0-y1)/(x0-x1) + y0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			ImageGray[y][x] = Color;
		}
	}
}

void DrawLineColor(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int x0, int y0, int x1, int y1, Color24 Color)
{
	int nDiffX = abs(x0-x1);
	int nDiffY = abs(y0-y1);

	int x, y;

	if(nDiffY == 0 && nDiffX == 0)
	{
		y = y0;
		x = x0;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
		{
			R[y][x] = GetRValue(Color);
			G[y][x] = GetGValue(Color);
			B[y][x] = GetBValue(Color);
		}
	}
	else if(nDiffX == 0)
	{
		x = x0;
		for(y = (y0 < y1 ? y0 : y1) ; y <= (y0 < y1 ? y1 : y0) ; y++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
				
			R[y][x] = GetRValue(Color);
			G[y][x] = GetGValue(Color);
			B[y][x] = GetBValue(Color);
		}
	}
	else if(nDiffY == 0)
	{
		y = y0;
		for(x = (x0 < x1 ? x0 : x1) ; x <= (x0 < x1 ? x1 : x0) ; x++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
				
			R[y][x] = GetRValue(Color);
			G[y][x] = GetGValue(Color);
			B[y][x] = GetBValue(Color);
		}
	}
	else if(nDiffY > nDiffX)
	{
		for(y = (y0 < y1 ? y0 : y1) ; y <= (y0 < y1 ? y1 : y0) ; y++)
		{
			x = (y-y0)*(x0-x1)/(y0-y1) + x0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			
			R[y][x] = GetRValue(Color);
			G[y][x] = GetGValue(Color);
			B[y][x] = GetBValue(Color);;
		}
	}
	else
	{
		for(x = (x0 < x1 ? x0 : x1) ; x <= (x0 < x1 ? x1 : x0) ; x++)
		{
			y = (x-x0)*(y0-y1)/(x0-x1) + y0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			
			R[y][x] = GetRValue(Color);
			G[y][x] = GetGValue(Color);
			B[y][x] = GetBValue(Color);
		}
	}
}

void DrawLine1D(unsigned char *Image1D, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char R, unsigned char G, unsigned char B)
{
	int nDiffX = abs(x0-x1);
	int nDiffY = abs(y0-y1);

	int x, y;
	int nFrom, nTo;
	int Pos;

	if(nDiffY == 0 && nDiffX == 0)
	{
		y = y0;
		x = x0;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
		{	
			Pos = GetBmp24Pos(nW, nH, x, y);
			Image1D[Pos++] = B;
			Image1D[Pos++] = G;
			Image1D[Pos] = R;
		}
	}
	else if(nDiffX == 0)
	{
		x = x0;

		nFrom = (y0 < y1 ? y0 : y1);
		if(nFrom < 0) nFrom = 0;
		nTo = (y0 < y1 ? y1 : y0);
		if(nTo >= nH) nTo = nH-1;

		for(y = nFrom ; y <= nTo ; y++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			{	
				Pos = GetBmp24Pos(nW, nH, x, y);
				Image1D[Pos++] = B;
				Image1D[Pos++] = G;
				Image1D[Pos] = R;
			}
		}
	}
	else if(nDiffY == 0)
	{
		y = y0;

		nFrom = (x0 < x1 ? x0 : x1);
		if(nFrom < 0) nFrom = 0;
		nTo = (x0 < x1 ? x1 : x0);
		if(nTo >= nW) nTo = nW-1;

		for(x = nFrom ; x <= nTo ; x++)
		{
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			{	
				Pos = GetBmp24Pos(nW, nH, x, y);
				Image1D[Pos++] = B;
				Image1D[Pos++] = G;
				Image1D[Pos] = R;
			}
		}
	}
	else if(nDiffY > nDiffX)
	{
		nFrom = (y0 < y1 ? y0 : y1);
		if(nFrom < 0) nFrom = 0;
		nTo = (y0 < y1 ? y1 : y0);
		if(nTo >= nH) nTo = nH-1;

		for(y = nFrom ; y <= nTo ; y++)
		{
			x = (y-y0)*(x0-x1)/(y0-y1) + x0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			{	
				Pos = GetBmp24Pos(nW, nH, x, y);
				Image1D[Pos++] = B;
				Image1D[Pos++] = G;
				Image1D[Pos] = R;
			}
		}
	}
	else
	{
		nFrom = (x0 < x1 ? x0 : x1);
		if(nFrom < 0) nFrom = 0;
		nTo = (x0 < x1 ? x1 : x0);
		if(nTo >= nW) nTo = nW-1;

		for(x = nFrom ; x <= nTo ; x++)
		{
			y = (x-x0)*(y0-y1)/(x0-x1) + y0;
			if(x < 0 || x >= nW || y < 0 || y >= nH) continue;
			{	
				Pos = GetBmp24Pos(nW, nH, x, y);
				Image1D[Pos++] = B;
				Image1D[Pos++] = G;
				Image1D[Pos] = R;
			}
		}
	}
}

void Dot(unsigned char **ImageGray, int nW, int nH, int x, int y)
{
	int i, j;

	for(j = -2 ; j <= 2 ; j++)
		for(i = -2 ; i <= 2 ; i++)
		{
			if(x+i >=0 && y+j >= 0 && x+i < nW && y+j < nH)
			{
				ImageGray[y+j][x+i] = GRAY_CNT-1;
			}
		}	
}

void Dot(unsigned char **ImageGray, int nW, int nH, int x, int y, unsigned char Color)
{
	int i, j;

	for(j = -2 ; j <= 2 ; j++)
		for(i = -2 ; i <= 2 ; i++)
		{
			if(x+i >=0 && y+j >= 0 && x+i < nW && y+j < nH)
			{
				ImageGray[y+j][x+i] = Color;
			}
		}	
}

void DilationBinary(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize)
{
	//	nShape 
	// 0: 사각, 1: 수평, 2: 수직

	int x, y;
	int dx, dy;
	int xx, yy;
	BOOL bDone;

	if(nSize <= 0) return;

	double nRange = nSize/2.;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			bDone = false;
			OutputGray[y][x] = 0;
			for(dy = -(int)nRange ; dy < nRange ; dy++)
			{
				if(nShape == 1 && dy != 0) continue;

				for(dx = -(int)nRange ; dx < nRange ; dx++)
				{
					if(nShape == 2 && dx != 0) continue;

					xx = x+dx;
					yy = y+dy;

					if(xx < 0) xx = 0;
					if(xx >= nW) xx = nW-1;
					if(yy < 0) yy = 0;
					if(yy >= nH) yy = nH-1;

					if(ImageGray[yy][xx] > GRAY_CNT/2) 
					{
						OutputGray[y][x] = GRAY_CNT-1;
						bDone = TRUE;
						break;
					}
				}
				if(bDone) break;
			}
		}
}

void DilationSquareBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut)
{
	unsigned char **ImageBinary2D;

	ImageBinary2D = cmatrix(nH, nW);

	int x, y, dx, dy;
	int Sum, LeftSum;
	int SideA, SideB;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			ImageBinary2D[y][x] = Image2DGray[y][x] > 128 ? 1 : 0;

	int *SumStrip = new int[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
			SumStrip[x] += ImageBinary2D[(0+dy+nH)%nH][x];
	}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Sum = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
					Sum += SumStrip[(x+dx+nW)%nW];
				LeftSum = SumStrip[(x+SideA+nW)%nW];
			}
			else
			{
				Sum -= LeftSum;
				Sum += SumStrip[(x+SideB+nW)%nW];
				LeftSum = SumStrip[(x+SideA+nW)%nW];
			}

			if(x == nW-1)
			{
				for(dx = 0 ; dx < nW ; dx++)
					SumStrip[dx] += -ImageBinary2D[(y+SideA+nH)%nH][dx]+ImageBinary2D[(y+SideB+1+nH)%nH][dx];
			}

			if(Sum > 0)
				Image2DGrayOut[y][x] = 255;
			else
				Image2DGrayOut[y][x] = 0;
		}

	delete [] SumStrip;

	free_cmatrix(ImageBinary2D, nH, nW);
}

void DilationHorizonBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut)
{
	unsigned char **ImageBinary2D;

	ImageBinary2D = cmatrix(nH, nW);

	int x, y, dx;
	int Sum, Left;
	int SideA, SideB;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			ImageBinary2D[y][x] = Image2DGray[y][x] > 128 ? 1 : 0;

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Sum = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
					Sum += ImageBinary2D[y][(x+dx+nW)%nW];
				Left = ImageBinary2D[y][(x+SideA+nW)%nW];
			}
			else
			{
				Sum -= Left;
				Sum += ImageBinary2D[y][(x+SideB+nW)%nW];
				Left = ImageBinary2D[y][(x+SideA+nW)%nW];
			}

			
			if(Sum > 0)
				Image2DGrayOut[y][x] = 255;
			else
				Image2DGrayOut[y][x] = 0;
		}

	free_cmatrix(ImageBinary2D, nH, nW);
}

void DilationVertialBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut)
{
	unsigned char **ImageBinary2D;

	ImageBinary2D = cmatrix(nH, nW);

	int x, y, dy;
	int Sum, Top;
	int SideA, SideB;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			ImageBinary2D[y][x] = Image2DGray[y][x] > 128 ? 1 : 0;

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
		for(y = 0 ; y < nH ; y++)
		{
			if(y == 0)
			{
				Sum = 0;
				for(dy = SideA ; dy <= SideB ; dy++)
					Sum += ImageBinary2D[(y+dy+nH)%nH][x];
				Top = ImageBinary2D[(y+SideA+nH)%nH][x];
			}
			else
			{
				Sum -= Top;
				Sum += ImageBinary2D[(y+SideB+nH)%nH][x];
				Top = ImageBinary2D[(y+SideA+nH)%nH][x];
			}

			
			if(Sum > 0)
				Image2DGrayOut[y][x] = 255;
			else
				Image2DGrayOut[y][x] = 0;
		}

	free_cmatrix(ImageBinary2D, nH, nW);
}

void ErosionBinary(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize)
{
	//	nShape 
	// 0: 사각, 1: 수평, 2: 수직

	int x, y;
	int dx, dy;
	int xx, yy;
	BOOL bDone;

	if(nSize <= 0) return;

	double nRange = nSize/2.;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			bDone = false;
			OutputGray[y][x] = GRAY_CNT-1;
			for(dy = -(int)nRange ; dy < nRange ; dy++)
			{
				if(nShape == 1 && dy != 0) continue;

				for(dx = -(int)nRange ; dx < nRange ; dx++)
				{
					if(nShape == 2 && dx != 0) continue;

					xx = x+dx;
					yy = y+dy;

					if(xx < 0) xx = 0;
					if(xx >= nW) xx = nW-1;
					if(yy < 0) yy = 0;
					if(yy >= nH) yy = nH-1;

					if(ImageGray[yy][xx] < GRAY_CNT/2) 
					{
						OutputGray[y][x] = 0;
						bDone = TRUE;
						break;
					}
				}
				if(bDone) break;
			}
		}
}

void ErosionSquareBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut)
{
	unsigned char **ImageBinary2D;

	ImageBinary2D = cmatrix(nH, nW);

	int x, y, dx, dy;
	int Sum, LeftSum;
	int SideA, SideB;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			ImageBinary2D[y][x] = Image2DGray[y][x] > 128 ? 1 : 0;

	int *SumStrip = new int[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
			SumStrip[x] += ImageBinary2D[(0+dy+nH)%nH][x];
	}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Sum = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
					Sum += SumStrip[(x+dx+nW)%nW];
				LeftSum = SumStrip[(x+SideA+nW)%nW];
			}
			else
			{
				Sum -= LeftSum;
				Sum += SumStrip[(x+SideB+nW)%nW];
				LeftSum = SumStrip[(x+SideA+nW)%nW];
			}

			if(x == nW-1)
			{
				for(dx = 0 ; dx < nW ; dx++)
					SumStrip[dx] += -ImageBinary2D[(y+SideA+nH)%nH][dx]+ImageBinary2D[(y+SideB+1+nH)%nH][dx];
			}

			if(Sum == nSize*nSize)
				Image2DGrayOut[y][x] = 255;
			else
				Image2DGrayOut[y][x] = 0;
		}

	delete [] SumStrip;

	free_cmatrix(ImageBinary2D, nH, nW);
}

void ErosionHorizonBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut)
{
	unsigned char **ImageBinary2D;

	ImageBinary2D = cmatrix(nH, nW);

	int x, y, dx;
	int Sum, Left;
	int SideA, SideB;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			ImageBinary2D[y][x] = Image2DGray[y][x] > 128 ? 1 : 0;

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Sum = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
					Sum += ImageBinary2D[y][(x+dx+nW)%nW];
				Left = ImageBinary2D[y][(x+SideA+nW)%nW];
			}
			else
			{
				Sum -= Left;
				Sum += ImageBinary2D[y][(x+SideB+nW)%nW];
				Left = ImageBinary2D[y][(x+SideA+nW)%nW];
			}

			
			if(Sum == nSize)
				Image2DGrayOut[y][x] = 255;
			else
				Image2DGrayOut[y][x] = 0;
		}

	free_cmatrix(ImageBinary2D, nH, nW);
}

void ErosionVertialBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut)
{
	unsigned char **ImageBinary2D;

	ImageBinary2D = cmatrix(nH, nW);

	int x, y, dy;
	int Sum, Top;
	int SideA, SideB;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			ImageBinary2D[y][x] = Image2DGray[y][x] > 128 ? 1 : 0;

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
		for(y = 0 ; y < nH ; y++)
		{
			if(y == 0)
			{
				Sum = 0;
				for(dy = SideA ; dy <= SideB ; dy++)
					Sum += ImageBinary2D[(y+dy+nH)%nH][x];
				Top = ImageBinary2D[(y+SideA+nH)%nH][x];
			}
			else
			{
				Sum -= Top;
				Sum += ImageBinary2D[(y+SideB+nH)%nH][x];
				Top = ImageBinary2D[(y+SideA+nH)%nH][x];
			}

			
			if(Sum == nSize)
				Image2DGrayOut[y][x] = 255;
			else
				Image2DGrayOut[y][x] = 0;
		}

	free_cmatrix(ImageBinary2D, nH, nW);
}


void DilationGray(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize)
{
	//	nShape 
	// 0: 사각, 1: 수평, 2: 수직

	int x, y;
	int dx, dy;
	int xx, yy;
	unsigned char Max;

	if(nSize <= 0) return;

	double nRange = nSize/2.;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			Max = 0;
			for(dy = -(int)nRange ; dy < nRange ; dy++)
			{
				if(nShape == 1 && dy != 0) continue;

				for(dx = -(int)nRange ; dx < nRange ; dx++)
				{
					if(nShape == 2 && dx != 0) continue;

					xx = x+dx;
					yy = y+dy;

					if(xx < 0) xx = 0;
					if(xx >= nW) xx = nW-1;
					if(yy < 0) yy = 0;
					if(yy >= nH) yy = nH-1;

					if(ImageGray[yy][xx] > Max) 
						Max = ImageGray[yy][xx];
				}
			}
			OutputGray[y][x] = Max;
		}
}

void ErosionGray(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nShape, int nSize)
{
	//	nShape 
	// 0: 사각, 1: 수평, 2: 수직

	int x, y;
	int dx, dy;
	int xx, yy;
	unsigned char Min;

	if(nSize <= 0) return;

	double nRange = nSize/2.;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			Min = GRAY_CNT-1;
			for(dy = -(int)nRange ; dy < nRange ; dy++)
			{
				if(nShape == 1 && dy != 0) continue;

				for(dx = -(int)nRange ; dx < nRange ; dx++)
				{
					if(nShape == 2 && dx != 0) continue;

					xx = x+dx;
					yy = y+dy;

					if(xx < 0) xx = 0;
					if(xx >= nW) xx = nW-1;
					if(yy < 0) yy = 0;
					if(yy >= nH) yy = nH-1;

					if(ImageGray[yy][xx] < Min)
						Min = ImageGray[yy][xx];
				}
			}
			OutputGray[y][x] = Min;
		}
}

bool InverseMatrix(double  **x, double **Inverse, int nN, double *Determinant)
{
	int i, j, k, l, kk;
	double TempDeterminant, Pivot;
	bool bFinish;

	int nM = nN*2;

	double **TempMatrix;

	const double eps = 1.0e-10;

	TempMatrix = dmatrix(nN, nM);

	for(i = 0 ; i < nN ; i++)
	{
		for(j = 0 ; j < nN ; j++)
			TempMatrix[i][j] = x[i][j];

		for(j = nN ; j < nM ; j++)
			TempMatrix[i][j] = 0.;
	}

	for(i = 0 ; i < nN ; i++) 
		TempMatrix[i][nN+i] = 1.;

	TempDeterminant = 1.;
	for(i = 0 ; i < nN ; i++) 
	{
		if(fabs(TempMatrix[i][i]) < eps) 
		{
			if(i == (nN-1)) 
			{
                *Determinant = 0.;
				free_dmatrix(TempMatrix, nN, nM);
				return false;
			}
            
			bFinish = false;
			for(l = i+1; l < nN ; l++) 
			{
				if(fabs(x[l][i]) >= eps) 
				{
					for(kk = i ; kk < nM ; kk++) 
						TempMatrix[i][kk] += TempMatrix[l][kk];
					
					bFinish = true;
				}
			}
		
			if(!bFinish)
			{
				*Determinant = 0.;
				free_dmatrix(TempMatrix, nN, nM);
				return false;
			}
		}	
		Pivot = TempMatrix[i][i];
		TempDeterminant *= Pivot;
	   
		for(j = i+1 ; j < nM ; j++) 
			TempMatrix[i][j] /= Pivot;

		TempMatrix[i][i] = 1.;
	   
		for (j = 0 ; j < nN ; j++) 
		{
			if (i != j) 
			{
				for(k = i+1 ; k < nM ; k++) 
					TempMatrix[j][k] -= TempMatrix[j][i]*TempMatrix[i][k];
				
				TempMatrix[j][i] = 0.;
			}
		}
	}

	*Determinant = TempDeterminant;

	for(i = 0 ; i < nN ; i++)
	   for(j = 0 ; j < nN ; j++)
	      Inverse[i][j] = TempMatrix[i][j+nN];

	free_dmatrix(TempMatrix, nN, nM);

	return true;
}

void DrawLabelBoundBox(CIpRect *pBoundBox, int LabelCnt, unsigned char **Image2DGray, unsigned char Color)
{
	int i;
	int x, y;

	for(i = 0 ; i < LabelCnt ; i++)
	{
		for(y = pBoundBox[i].top ; y <= pBoundBox[i].bottom ; y++)
		{
			Image2DGray[y][pBoundBox[i].left] = Color;
			Image2DGray[y][pBoundBox[i].right] = Color;
//			Image2DGray[y][pBoundBox[i].left+1] = Color;
//			Image2DGray[y][pBoundBox[i].right-1] = Color;
		}
		for(x = pBoundBox[i].left ; x <= pBoundBox[i].right ; x++)
		{
			Image2DGray[pBoundBox[i].top][x] = Color;
			Image2DGray[pBoundBox[i].bottom][x] = Color;
//			Image2DGray[pBoundBox[i].top+1][x] = Color;
//			Image2DGray[pBoundBox[i].bottom-1][x] = Color;
		}
	}
}

void MakeImage2DGrayToImage1D(unsigned char **Image2DGray, int nW, int nH, unsigned char *Image1D)
{
	int x, y, Offset;
	
	for(y = 0 ; y < nH ; y++)
	{
		Offset = (nW*3+3)/4*4 * (nH-y-1);
		for(x = 0 ; x < nW ; x++)
		{
			Image1D[Offset+x*3+2] = Image2DGray[y][x];
			Image1D[Offset+x*3+1] = Image2DGray[y][x];
			Image1D[Offset+x*3] = Image2DGray[y][x];
		}
	}
}

void MakeImage2DColorToImage1D(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, unsigned char *Image1D)
{
	int x, y, Offset;
	
	for(y = 0 ; y < nH ; y++)
	{
		Offset = (nW*3+3)/4*4 * (nH-y-1);
		for(x = 0 ; x < nW ; x++)
		{
			Image1D[Offset+x*3+2] = R[y][x];
			Image1D[Offset+x*3+1] = G[y][x];
			Image1D[Offset+x*3] = B[y][x];
		}
	}
}

void MakeImage1DToImage2DGray(unsigned char *Image1D, bool bBottomTop, int nW, int nH, unsigned char **Image2DGray, int nDesW, int nDesH)
{
	int x, y, Offset;

	if(nDesW <= 0) nDesW = nW;
	if(nDesH <= 0) nDesH = nH;
	
	if(nDesW == nW && nDesH == nH)
	{
		for(y = 0 ; y < nH ; y++)
		{
			if(bBottomTop)
				Offset = (nW*3+3)/4*4 * (nH-y-1);
			else
				Offset = (nW*3+3)/4*4 * y;
			for(x = 0 ; x < nW ; x++)
			{
				Image2DGray[y][x] = (Image1D[Offset+x*3+2] +
								Image1D[Offset+x*3+1] +
								Image1D[Offset+x*3])/3;
			}
		}
	}
	else
	{		
		for(y = 0 ; y < nDesH ; y++)
		{
			if(bBottomTop)
				Offset = (nW*3+3)/4*4 * (nH-y*nH/nDesH-1);
			else
				Offset = (nW*3+3)/4*4 * y*nH/nDesH;

			for(x = 0 ; x < nDesW ; x++)
			{
				Image2DGray[y][x] = (Image1D[Offset+x*nW/nDesW*3+2] +
								Image1D[Offset+x*nW/nDesW*3+1] +
								Image1D[Offset+x*nW/nDesW*3])/3;
			}
		}
	}
}

void MakeImage1DToImage2DGrayNew20(unsigned char *Image1D, bool bBottomTop, int nMethod, int nW, int nH, unsigned char **Image2DGray, int nDesW, int nDesH)
{
	int x, y, Offset;

	if(nDesW <= 0) nDesW = nW;
	if(nDesH <= 0) nDesH = nH;
	
	if(nDesW == nW && nDesH == nH)
	{
		for(y = 0 ; y < nH ; y++)
		{
			if(bBottomTop)
				Offset = (nW*3+3)/4*4 * (nH-y-1);
			else
				Offset = (nW*3+3)/4*4 * y;
			for(x = 0 ; x < nW ; x++)
			{
				if(nMethod == IF_COLOR2GRAY_AVERAGE)
					Image2DGray[y][x] = (Image1D[Offset+x*3+2] +
									Image1D[Offset+x*3+1] +
									Image1D[Offset+x*3])/3;
				else if(nMethod == IF_COLOR2GRAY_NTSC)
				{
					double Gray = Image1D[Offset+x*3+2]*0.299 +
								Image1D[Offset+x*3+1]*0.7152 +
								Image1D[Offset+x*3]*0.114;
					Image2DGray[y][x] = Gray > 255 ? 255 : (Gray < 0 ? 0 : (unsigned char)Gray);
				}
			}
		}
	}
	else
	{		
		for(y = 0 ; y < nDesH ; y++)
		{
			if(bBottomTop)
				Offset = (nW*3+3)/4*4 * (nH-y*nH/nDesH-1);
			else
				Offset = (nW*3+3)/4*4 * y*nH/nDesH;

			for(x = 0 ; x < nDesW ; x++)
			{
				if(nMethod == IF_COLOR2GRAY_AVERAGE)
					Image2DGray[y][x] = (Image1D[Offset+x*nW/nDesW*3+2] +
								Image1D[Offset+x*nW/nDesW*3+1] +
								Image1D[Offset+x*nW/nDesW*3])/3;
				else if(nMethod == IF_COLOR2GRAY_NTSC)
				{
					double Gray = Image1D[Offset+x*nW/nDesW*3+2]*0.299 +
								Image1D[Offset+x*nW/nDesW*3+1]*0.7152 +
								Image1D[Offset+x*nW/nDesW*3]*0.114;
					Image2DGray[y][x] = Gray > 255 ? 255 : (Gray < 0 ? 0 : (unsigned char)Gray);
				}
			}
		}
	}
}

void MakeImage1DToImage2DColor(unsigned char *Image1D, bool bBottomTop, int nW, int nH, unsigned char **Image2DRed, unsigned char **Image2DGreen, unsigned char **Image2DBlue, int nDesW, int nDesH)
{
	int x, y, Offset;

	if(nDesW <= 0) nDesW = nW;
	if(nDesH <= 0) nDesH = nH;
	
	if(nDesW == nW && nDesH == nH)
	{
		for(y = 0 ; y < nH ; y++)
		{
			if(bBottomTop)
				Offset = (nW*3+3)/4*4 * (nH-y-1);
			else
				Offset = (nW*3+3)/4*4 * y;

			for(x = 0 ; x < nW ; x++)
			{
				Image2DRed[y][x] = Image1D[Offset+x*3+2];
				Image2DGreen[y][x] = Image1D[Offset+x*3+1];
				Image2DBlue[y][x] = Image1D[Offset+x*3];
			}
		}
	}
	else
	{		
		for(y = 0 ; y < nDesH ; y++)
		{
			if(bBottomTop)
				Offset = (nW*3+3)/4*4 * (nH-y*nH/nDesH-1);
			else
				Offset = (nW*3+3)/4*4 * y*nH/nDesH;

			for(x = 0 ; x < nDesW ; x++)
			{
				Image2DRed[y][x] = Image1D[Offset+x*nW/nDesW*3+2];
				Image2DGreen[y][x] = Image1D[Offset+x*nW/nDesW*3+1];
				Image2DBlue[y][x] = Image1D[Offset+x*nW/nDesW*3];
			}
		}
	}
}

void DrawCross(unsigned char **Image, int nW, int nH, CIpPoint pt, int Gray)
{
	DrawLine(Image, nW, nH, pt.x-5, pt.y, pt.x+5, pt.y, Gray);
	DrawLine(Image, nW, nH, pt.x-5, pt.y+1, pt.x+5, pt.y+1, Gray);
	DrawLine(Image, nW, nH, pt.x-5, pt.y-1, pt.x+5, pt.y-1, Gray);
	DrawLine(Image, nW, nH, pt.x, pt.y-5, pt.x, pt.y+5, Gray);
	DrawLine(Image, nW, nH, pt.x+1, pt.y-5, pt.x+1, pt.y+5, Gray);
	DrawLine(Image, nW, nH, pt.x-1, pt.y-5, pt.x-1, pt.y+5, Gray);
}

void DrawCrossColor(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, CIpPoint pt, Color24 Color)
{
	DrawLineColor(R, G, B, nW, nH, pt.x-5, pt.y, pt.x+5, pt.y, Color);
	DrawLineColor(R, G, B, nW, nH, pt.x-5, pt.y+1, pt.x+5, pt.y+1, Color);
	DrawLineColor(R, G, B, nW, nH, pt.x-5, pt.y-1, pt.x+5, pt.y-1, Color);
	DrawLineColor(R, G, B, nW, nH, pt.x, pt.y-5, pt.x, pt.y+5, Color);
	DrawLineColor(R, G, B, nW, nH, pt.x+1, pt.y-5, pt.x+1, pt.y+5, Color);
	DrawLineColor(R, G, B, nW, nH, pt.x-1, pt.y-5, pt.x-1, pt.y+5, Color);
}

void BigDot(unsigned char **ImageGray, int nW, int nH, int x, int y, unsigned char Color)
{
	int i, j;

	for(j = -3 ; j <= 3 ; j++)
		for(i = -3 ; i <= 3 ; i++)
		{
			if(x+i >=0 && y+j >= 0 && x+i < nW && y+j < nH)
			{
				ImageGray[y+j][x+i] = Color;
			}
		}	
}

double Dist(int x1, int y1, int x2, int y2)
{
	return sqrt((double)(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

double FDist(double x1, double y1, double x2, double y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

bool SaveImage2DColorBmp(char *FileName, unsigned char **ImageRed, unsigned char **ImageGreen, unsigned char **ImageBlue, 
	int nW, int nH)
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

	bool bRtn = SaveBmp(FileName, Image1D, nW, nH);

	delete [] Image1D;

	return bRtn;
}

void MedianBinaryFast2DGray(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut)
{
	unsigned char **ImageBinary2D;

	ImageBinary2D = cmatrix(nH, nW);

	int x, y, dx, dy;
	int Mean, LeftSum;
	int SideA, SideB;
	double Out;
	bool First = true;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			ImageBinary2D[y][x] = Image2DGray[y][x] > 128 ? 1 : 0;

	int *SumStrip = new int[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
			SumStrip[x] += ImageBinary2D[(0+dy+nH)%nH][x];
	}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Mean = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
					Mean += SumStrip[(x+dx+nW)%nW];
				LeftSum = SumStrip[(x+SideA+nW)%nW];
			}
			else
			{
				Mean -= LeftSum;
				Mean += SumStrip[(x+SideB+nW)%nW];
				LeftSum = SumStrip[(x+SideA+nW)%nW];
			}

			if(x == nW-1)
			{
				for(dx = 0 ; dx < nW ; dx++)
					SumStrip[dx] += -ImageBinary2D[(y+SideA+nH)%nH][dx]+ImageBinary2D[(y+SideB+1+nH)%nH][dx];
			}


			Out = Mean/(double)(nSize*nSize);

			if(Out < 0.5)
				Image2DGrayOut[y][x] = 0;
			else
				Image2DGrayOut[y][x] = 255;
		}

	delete [] SumStrip;

	free_cmatrix(ImageBinary2D, nH, nW);
}

void ComputekCosine(CIpPoint *Boundary, int nCnt, int *LeftK, int *RightK, double *kCosine, double sigma, bool *bConvex, int xC, int yC)
{
	int i;
	double abs1, abs2, cosValue;

	const double Pi = acos(-1.);

	for(i = 0 ; i < nCnt ; i++)
	{
		abs1= sqrt((double)(Boundary[(i-LeftK[i]+nCnt)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i-LeftK[i]+nCnt)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i-LeftK[i]+nCnt)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i-LeftK[i]+nCnt)%nCnt].y - Boundary[i].y));

		abs2= sqrt((double)(Boundary[(i+RightK[i])%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+RightK[i])%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i+RightK[i])%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+RightK[i])%nCnt].y - Boundary[i].y));

		cosValue = ((double)(Boundary[(i-LeftK[i]+nCnt)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+RightK[i])%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i-LeftK[i]+nCnt)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+RightK[i])%nCnt].y - Boundary[i].y)) /
			(abs1*abs2);

		double dist0 = Dist(xC, yC, Boundary[i].x, Boundary[i].y);
		double dist1 = Dist(xC, yC, Boundary[(i-LeftK[i]+nCnt)%nCnt].x, Boundary[(i-LeftK[i]+nCnt)%nCnt].y);
		double dist2 = Dist(xC, yC, Boundary[(i+RightK[i])%nCnt].x, Boundary[(i+RightK[i])%nCnt].y);

		if(dist0 > (dist1+dist2)/2) bConvex[i] = true;
		else bConvex[i] = false;
		
		kCosine[i] = cosValue;

/*		double Average = 0;
		int k;
		int nAverageCnt = 0;


		for(k = MinValue(LeftK[i]/2, RightK[i]/2) ; k <= MinValue(LeftK[i], RightK[i]) ; k++)
		{
			abs1= sqrt((double)(Boundary[(i-k+nCnt)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i-k+nCnt)%nCnt].x - Boundary[i].x) + 
			(double)(Boundary[(i-k+nCnt)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i-k+nCnt)%nCnt].y - Boundary[i].y));
			abs2= sqrt((double)(Boundary[(i+k)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+k)%nCnt].x - Boundary[i].x) + 
				(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y));
			cosValue = ((double)(Boundary[(i-k+nCnt)%nCnt].x - Boundary[i].x)*(double)(Boundary[(i+k)%nCnt].x - Boundary[i].x) + 
				(double)(Boundary[(i-k+nCnt)%nCnt].y - Boundary[i].y)*(double)(Boundary[(i+k)%nCnt].y - Boundary[i].y)) /
				(abs1*abs2);

			Average += cosValue;
			nAverageCnt++;
		}

		kCosine[i] = Average/nAverageCnt;*/
		
	}

	
	{
		const int MaxMaskSize = 20;
		double Gauss[MaxMaskSize];
		int MaskSize=MaxMaskSize-1;

		for(i = 0 ; i < MaxMaskSize ; i++)
		{
			Gauss[i] = MeanGaussFn((double)i, sigma);

			if(Gauss[i] < 0.00005)
			{
				MaskSize = i;
				break;
			}
		}

		double *NewkCosine;

		NewkCosine = new double[nCnt];

		for(i = 0 ; i < nCnt ; i++)
		{
			int k;

			NewkCosine[i] = kCosine[i]*Gauss[0];
			for(k = 1 ; k < MaskSize ; k++)
				NewkCosine[i] += kCosine[(i-k+nCnt)%nCnt]*Gauss[k] + kCosine[(i+k)%nCnt]*Gauss[k];

			if(NewkCosine[i] > 1) NewkCosine[i] = 1;
			else if(NewkCosine[i] < -1) NewkCosine[i] = -1;
		}

		for(i = 0 ; i < nCnt ; i++)
			kCosine[i] = NewkCosine[i];


		delete [] NewkCosine;
	}
}

void DrawArrow(CDC *pDC, int x1, int y1, int x2, int y2, Color24 Color, int nWidth)
{
	CPen pen(PS_SOLID, nWidth, Color);
	CPen *pOldPen;

	pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);

	double Theta = atan2((double)(y1-y2), double(x1-x2));

	double Theta1 = Theta + acos(-1.)/6;
	double Theta2 = Theta - acos(-1.)/6;

	int xx0, yy0;

	xx0 = (int)(x2 + 5*cos(Theta));
	yy0 = (int)(y2 + 5*sin(Theta));

	int x, y;
	x = (int)(x2 + 7*cos(Theta1));
	y = (int)(y2 + 7*sin(Theta1));

	pDC->MoveTo(x2, y2);
	pDC->LineTo(x, y);

	x = (int)(x2 + 7*cos(Theta2));
	y = (int)(y2 + 7*sin(Theta2));

	pDC->MoveTo(x2, y2);
	pDC->LineTo(x, y);

	pDC->SelectObject(pOldPen);
}

void DrawArrow(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, unsigned char Color)
{
	DrawLine(ImageGray, nW, nH, x1, y1, x2, y2, Color);

	double Theta = atan2((double)(y1-y2), double(x1-x2));

	double Theta1 = Theta + acos(-1.)/6;
	double Theta2 = Theta - acos(-1.)/6;

	int xx0, yy0;

	xx0 = (int)(x2 + 5*cos(Theta));
	yy0 = (int)(y2 + 5*sin(Theta));

	int x, y;
	x = (int)(x2 + 7*cos(Theta1));
	y = (int)(y2 + 7*sin(Theta1));

	DrawLine(ImageGray, nW, nH, x2, y2, x, y, Color);

	x = (int)(x2 + 7*cos(Theta2));
	y = (int)(y2 + 7*sin(Theta2));

	DrawLine(ImageGray, nW, nH, x2, y2, x, y, Color);
}

void DrawBigArrow(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, unsigned char Color)
{
	DrawBigLine(ImageGray, nW, nH, x1, y1, x2, y2, Color);
	
	double Theta = atan2((double)(y1-y2), (double)(x1-x2));

	double Theta1 = Theta + acos(-1.)/6;
	double Theta2 = Theta - acos(-1.)/6;

	int xx0, yy0;

	xx0 = (int)(x2 + 5*cos(Theta));
	yy0 = (int)(y2 + 5*sin(Theta));

	int x, y;
	x = (int)(x2 + 7*cos(Theta1));
	y = (int)(y2 + 7*sin(Theta1));

	DrawBigLine(ImageGray, nW, nH, x2, y2, x, y, Color);

	x = (int)(x2 + 7*cos(Theta2));
	y = (int)(y2 + 7*sin(Theta2));

	DrawBigLine(ImageGray, nW, nH, x2, y2, x, y, Color);
}

void DrawBigLine(unsigned char **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, unsigned char Color)
{
	int dx, dy;

	for(dy = -1 ; dy <= 1; dy++)
		for(dx = -1 ; dx <= 1 ; dx++)
			DrawLine(ImageGray, nW, nH, x0+dx, y0+dy, x1+dx, y1+dy, Color);
}

void SetLabelBoundBox(int **Label, int nW, int nH, CIpRect *pBoundBox, int LabelCnt)
{
	int i;
	int x, y;

	for(i = 0 ; i < LabelCnt ; i++)
		pBoundBox[i] = CIpRect(nW, nH, 0, 0);

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			if(Label[y][x] < 0) continue;
			if(pBoundBox[Label[y][x]].left > x) pBoundBox[Label[y][x]].left = x;
			if(pBoundBox[Label[y][x]].top > y) pBoundBox[Label[y][x]].top = y;
			if(pBoundBox[Label[y][x]].right < x) pBoundBox[Label[y][x]].right = x;
			if(pBoundBox[Label[y][x]].bottom < y) pBoundBox[Label[y][x]].bottom = y;
		}
}

double ComputeSD(int *data, int cnt)
{
	int i;
	double SD;
	double Mean;

	Mean = 0.;
	for(i = 0 ; i < cnt ; i++)
		Mean += (double)data[i];

	Mean /= (double)cnt;

	SD = 0.;
	for(i = 0 ; i < cnt ; i++)
		SD += ((double)data[i] - Mean)*((double)data[i] - Mean);

	SD /= (double)cnt;

	return(sqrt(SD));
} 

void DrawRect(CIpRect rt, unsigned char **Image2DGray, int nW, int nH, unsigned char Color)
{
	int x, y;

	for(x = rt.left ; x <= rt.right ; x++)
	{
		y = rt.top;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
			Image2DGray[y][x] = Color;

		y = rt.bottom;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
			Image2DGray[y][x] = Color;
	}

	for(y = rt.top ; y <= rt.bottom ; y++)
	{
		x = rt.left;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
			Image2DGray[y][x] = Color;

		x = rt.right;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
			Image2DGray[y][x] = Color;
	}
}

void DrawRect1D(CIpRect rt, unsigned char *Image1D, int nW, int nH, unsigned char R, unsigned char G, unsigned char B)
{
	int x, y;
	int Pos;

	for(x = rt.left ; x <= rt.right ; x++)
	{
		y = rt.top;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
		{
			Pos = GetBmp24Pos(nW, nH, x, y);
			Image1D[Pos++] = B;
			Image1D[Pos++] = G;
			Image1D[Pos] = R;
		}

		y = rt.bottom;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
		{
			Pos = GetBmp24Pos(nW, nH, x, y);
			Image1D[Pos++] = B;
			Image1D[Pos++] = G;
			Image1D[Pos] = R;
		}
	}

	for(y = rt.top ; y <= rt.bottom ; y++)
	{
		x = rt.left;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
		{
			Pos = GetBmp24Pos(nW, nH, x, y);
			Image1D[Pos++] = B;
			Image1D[Pos++] = G;
			Image1D[Pos] = R;
		}

		x = rt.right;
		if(!(x < 0 || x >= nW || y < 0 || y >= nH))
		{
			Pos = GetBmp24Pos(nW, nH, x, y);
			Image1D[Pos++] = B;
			Image1D[Pos++] = G;
			Image1D[Pos] = R;
		}
	}
}

void DrawBigRect(CIpRect rt, unsigned char **Image2DGray, int nW, int nH, unsigned char Color)
{
	int x, y;

	for(x = rt.left ; x <= rt.right ; x++)
	{
		for(y = rt.top-1 ; y <= rt.top+1 ; y++)
			if(!(x < 0 || x >= nW || y < 0 || y >= nH))
				Image2DGray[y][x] = Color;

		for(y = rt.bottom-1 ; y <= rt.bottom+1 ; y++)
			if(!(x < 0 || x >= nW || y < 0 || y >= nH))
				Image2DGray[y][x] = Color;
	}

	for(y = rt.top ; y <= rt.bottom ; y++)
	{
		for(x = rt.left-1 ; x <= rt.left+1 ; x++)
			if(!(x < 0 || x >= nW || y < 0 || y >= nH))
				Image2DGray[y][x] = Color;

		for(x = rt.right-1 ; x <= rt.right+1 ; x++)
			if(!(x < 0 || x >= nW || y < 0 || y >= nH))
				Image2DGray[y][x] = Color;
	}
}

void KMeanBinary(unsigned char **Image, unsigned char ***OutputGray, int nW, int nH, int nK) 
{
	int *Center;
	int *NewCenter;
	int *ClsCnt;
	int i, k;
	int mindist;
	int dist;
	int His[256] = {0}, Cls[256];
	BOOL End;
	int *Thre;
	int Iteration;
	int x, y;
	
	if(nK < 2) return;

	for(y = 0 ; y < nH; y++)
		for(x = 0 ; x < nW ; x++)
			His[Image[y][x]]++;

	Center = new int[nK];
	NewCenter = new int[nK];
	ClsCnt = new int[nK];
	Thre = new int[nK-1];

	Center[0] = 0; while(His[Center[0]++] <= 0); // Histogram이 존재하는 가장 작은 graylevel + 1
	Center[nK-1] = 255; while(His[Center[nK-1]--] <= 0); // Histogram이 존재하는 가장 큰 graylevel - 1

	for(i = 1 ; i < nK-1 ; i++)
		Center[i] = Center[0] + (Center[nK-1]-Center[0])/(nK-1)*i;

	End = FALSE;
	Iteration = 0;
	while(!End)
	{
		Iteration++;
		for(i = 0 ; i < nK ; i++)
			ClsCnt[i] = 0;

		for(k = 0 ; k < 256 ; k++)
		{
			if(His[k] > 0)
			{
				mindist = 256;
				for(i = 0 ; i < nK ; i++)
				{
					dist = abs(Center[i] - k);

					if(dist < mindist)
					{
						mindist = dist;
						Cls[k] = i;
					}
				}
				
				ClsCnt[Cls[k]] += His[k];
			}
		}

		for(i = 0 ; i < nK ; i++)
			if(ClsCnt[i] != 0)
				NewCenter[i] = 0;
			else 
				NewCenter[i] = Center[i];

		for(k = 0 ; k < 256 ; k++)
		{
			if(His[k] > 0) NewCenter[Cls[k]] += k*His[k];
		}

		for(i = 0 ; i < nK ; i++)
			if(ClsCnt[i] != 0)
				NewCenter[i] /= ClsCnt[i];

		End = 1;
		for(i = 0 ; i < nK ; i++)
		{
			if(NewCenter[i] == Center[i]) End *= 1;
			else End *= 0;

			Center[i] = NewCenter[i];
		}

	}

	for(i = 0 ; i < nK-1 ; i++)
	{
		Thre[i] = (Center[i] + Center[i+1])/2;
	}

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			for(i = 0 ; i < nK ; i++)
			{
				if(i == 0) {
					if(Image[y][x] < Thre[i])
						OutputGray[i][y][x] = 255;
					else
						OutputGray[i][y][x] = 0;
				}
				else if(i == nK-1) {
					if(Thre[i-1] <= Image[y][x])
						OutputGray[i][y][x] = 255;
					else
						OutputGray[i][y][x] = 0;
				}
				else if(Thre[i-1] <= Image[y][x] && Image[y][x] < Thre[i])
				{
					OutputGray[i][y][x] = 255;
				}
				else
				{
					OutputGray[i][y][x] = 0;
				}
			}
		}

	delete [] Center;
	delete [] NewCenter;
	delete [] ClsCnt;
	delete [] Thre;
}

void Fill(unsigned char **Image, int nW, int nH)
{
	int x, y;
	int **Label;
	unsigned char **Temp;

	Label = imatrix(nH, nW);
	Temp = cmatrix(nH, nW);

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			Temp[y][x] = ~Image[y][x];	
	
	int Cnt = Labeling(Temp, Label, nW, nH, 0);

	for(y = 1; y < nH-1 ; y++)
		for(x = 1 ; x < nW-1 ; x++)
			if(Label[y][x] == 0) Image[y][x] = 0;
			else Image[y][x] = 255;


	free_imatrix(Label, nH, nW);
	free_cmatrix(Temp, nH, nW);
}

void RemoveSmallRegion(unsigned char **Image, int nW, int nH, int nConnNumThre)
{
	int x, y;
	int **Label;

	Label = imatrix(nH, nW);
	
	
	int Cnt = Labeling(Image, Label, nW, nH, nConnNumThre);

	for(y = 0; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(Label[y][x] >= 0) Image[y][x] = 255;
			else Image[y][x] = 0;


	free_imatrix(Label, nH, nW);
}


///// http://people.cs.uchicago.edu/~pff/dt/
#define INF 1E20

template <class T>
inline T square(const T &x) { return x*x; };

/* dt of 1d function using squared distance */
double *DistanceTransform(double *f, int n) {
  double *d = new double[n];
  int *v = new int[n];
  double *z = new double[n+1];
  int k = 0, q;

  v[0] = 0;
  z[0] = -INF;
  z[1] = +INF;
  for (q = 1; q <= n-1; q++) {
    double s  = ((f[q]+square(q))-(f[v[k]]+square(v[k])))/(2*q-2*v[k]);
    while (s <= z[k]) {
      k--;
      s  = ((f[q]+square(q))-(f[v[k]]+square(v[k])))/(2*q-2*v[k]);
    }
    k++;
    v[k] = q;
    z[k] = s;
    z[k+1] = +INF;
  }

  k = 0;
  for (q = 0; q <= n-1; q++) {
    while (z[k+1] < q)
      k++;
    d[q] = square(q-v[k]) + f[v[k]];
  }

  delete [] v;
  delete [] z;

  return d;
}

/* dt of 2d function using squared distance */
void DistanceTransform(double **Output, int nW, int nH)
{
	double *f = new double [nW > nH ? nW : nH];
	int x, y;

	for(x = 0 ; x < nW ; x++)
	{
		for(y = 0 ; y < nH ; y++)
			f[y] = Output[y][x];

		double *d = DistanceTransform(f, nH);

		for(y = 0 ; y < nH ; y++)
			Output[y][x] = d[y];

		delete [] d;
	}

	for(y = 0 ; y < nH ; y++)
	{
		for(x = 0 ; x < nW ; x++)
			f[x] = Output[y][x];

		double *d = DistanceTransform(f, nW);

		for(x = 0 ; x < nW ; x++)
			Output[y][x] = d[x];

		delete [] d;
	}

	delete [] f; 
}


/* dt of binary image using squared distance */
//static image<float> *dt(image<uchar> *im, uchar on = 1) 
void DistanceTransform(unsigned char **Image, double **Output, int nW, int nH)
{
	int x, y;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			if(Image[y][x] > 128)
				Output[y][x] = 0;
			else
				Output[y][x] = INF;

	DistanceTransform(Output, nW, nH);
}

double VectorAbs(double x, double y)
{
	return sqrt(x*x + y*y);
}

void DrawEllipse(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, unsigned char Color)
{
	int t;

	if(x1 > x2) {
		t = x1;
		x1 = x2;
		x2 = t;
	}

	if(y1 > y2) {
		t = y1; 
		y1 = y2;
		y2 = t;
	}

	int RX = (x2 - x1) / 2;
    int RY = (y2 - y1) / 2;
    int CX = (x2 + x1) / 2;
    int CY = (y2 + y1) / 2;

	int Angle;
	int LastX, LastY;

    for (Angle = 0 ; Angle < 360 ; Angle++)
	{
		int x = (int)(CX + cos(Angle*3.14159/180) * RX);
        int y = (int)(CY + sin(Angle*3.14159/180) * RY);

        if (Angle > 0) DrawLine(ImageGray, nW, nH, LastX, LastY, x, y, Color);

        LastX = x;
        LastY = y;
	}
}

void CirclePoints(unsigned char **ImageGray, int nW, int nH, int cx, int cy, int x, int y, unsigned char Color)
{    
    if (x == 0) {
		if(cy+y >= 0 && cy+y < nH && cx >= 0 && cx < nW)
			ImageGray[cy+y][cx]= Color;

		if(cy-y >= 0 && cy-y < nH && cx >= 0 && cx < nW)
			ImageGray[cy-y][cx]= Color;
		if(cy >= 0 && cy < nH && cx+y >= 0 && cx+y < nW)
			ImageGray[cy][cx+y]= Color;
		if(cy >= 0 && cy < nH && cx-y >= 0 && cx-y < nW)
			ImageGray[cy][cx-y]= Color;
    } else if (x == y) {
		if(cy+y >= 0 && cy+y < nH && cx+x >= 0 && cx+x < nW)
			ImageGray[cy+y][cx+x]= Color;

		if(cy+y >= 0 && cy+y < nH && cx-x >= 0 && cx-x < nW)
			ImageGray[cy+y][cx-x]= Color;
		if(cy-y >= 0 && cy-y < nH && cx+x >= 0 && cx+x < nW)
			ImageGray[cy-y][cx+x]= Color;
		if(cy-y >= 0 && cy-y < nH && cx-x >= 0 && cx-x < nW)
			ImageGray[cy-y][cx-x]= Color;
    } else if (x < y) {
		if(cy+y >= 0 && cy+y < nH && cx+x >= 0 && cx+x < nW)
			ImageGray[cy+y][cx+x]= Color;

		if(cy+y >= 0 && cy+y < nH && cx-x >= 0 && cx-x < nW)
			ImageGray[cy+y][cx-x]= Color;
		if(cy-y >= 0 && cy-y < nH && cx+x >= 0 && cx+x < nW)
			ImageGray[cy-y][cx+x]= Color;
		if(cy-y >= 0 && cy-y < nH && cx-x >= 0 && cx-x < nW)
			ImageGray[cy-y][cx-x]= Color;
		if(cy+x >= 0 && cy+x < nH && cx+y >= 0 && cx+y < nW)
			ImageGray[cy+x][cx+y]= Color;
		if(cy+x >= 0 && cy+x < nH && cx-y >= 0 && cx-y < nW)
			ImageGray[cy+x][cx-y]= Color;
		if(cy-x >= 0 && cy-x < nH && cx+y >= 0 && cx+y < nW)
			ImageGray[cy-x][cx+y]= Color;
		if(cy-x >= 0 && cy-x < nH && cx-y >= 0 && cx-y < nW)
			ImageGray[cy-x][cx-y]= Color;
    }
}

void DrawCircle(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int radius, unsigned char Color)
{
	int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;
	
    CirclePoints(ImageGray, nW, nH, x1, y1, x, y, Color);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2*x+1;
        } else {
            y--;
            p += 2*(x-y)+1;
        }
        CirclePoints(ImageGray, nW, nH, x1, y1, x, y, Color);
    }
}

void DrawCircle2(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int radius, unsigned char Color)
{
	int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;
	
    CirclePoints(ImageGray, nW, nH, x1, y1, x, y, Color);

    while (x < y) {
		x+=2;
        if (p < 0) {
            p += 2*x+1;
        } else {
			y-=2;
            p += 2*(x-y)+1;
        }
        CirclePoints(ImageGray, nW, nH, x1, y1, x, y, Color);
    }
}

void DrawFillCircle(unsigned char **ImageGray, int nW, int nH, int x1, int y1, int radius, unsigned char Color)
{
	int x, y;

	for(y = y1-radius ; y <= y1+radius ; y++)
		for(x = x1-radius ; x <= x1+radius ; x++)
		{
			if(Dist(x, y, x1, y1) <= radius)
			{
				if(x >= 0 && x < nW && y >= 0 && y < nH)
					ImageGray[y][x] = Color;
			}
		}
}

void FastMedianFilter(unsigned char **ImageGray, unsigned char **OutputGray, int nW, int nH, int nOneSideSize)
{
	// nOneSideSize = 1 => WinSize = 3
	// nOneSideSize = 2 -> WinSize = 5;

	int median, delta;
	int prev, next;
	int His[256] = {0};
	int direction = 1;
	int x, y;
	int x1, x2, y1, y2;
	int xx, yy;
	int i, sum;
	int inValue, outValue;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			OutputGray[y][x] = 0;

	int middle=((nOneSideSize*2+1)*(nOneSideSize*2+1)+1)/2;

	for(y = 0 ; y < nOneSideSize*2+1 ; y++)
		for(x = 0 ; x < nOneSideSize*2+1 ; x++)
			His[ImageGray[y][x]]++;

	sum = 0;
	for(i=0 ; i < 256 ; i++)
	{
		sum += His[i];
		if(sum >= middle)
			break;
	}
	median = i;
	delta = sum - His[i];

	OutputGray[nOneSideSize][nOneSideSize] = median;

	x = nOneSideSize+1;
	for(y = nOneSideSize ; y < nH-nOneSideSize ; y++)
	{
		y1 = y - nOneSideSize;
		y2 = y + nOneSideSize;

		for(; x >= nOneSideSize && x < nW-nOneSideSize ; x += direction)
		{
			prev = x-direction*(nOneSideSize+1);
			next = x+direction*nOneSideSize;

			for(yy = y1 ; yy <= y2 ; yy++)
			{

				outValue = ImageGray[yy][prev];
				inValue = ImageGray[yy][next];

				if(outValue == inValue)
					continue;
				His[outValue]--;
				if(outValue < median)
					delta--;
				His[inValue]++;
				if(inValue < median)
					delta++;
			}

			if(delta >= middle)
			{
				while(delta >= middle)
				{
					if(His[--median] > 0)
						delta -= His[median];
				}
			}
			else
			{
				while(delta + His[median] < middle)
				{
					if(His[median] > 0)
						delta += His[median];
					median++;
				}
			}

			OutputGray[y][x] = median;
		} 

		if(y < nH-nOneSideSize-1)
		{
			x -= direction;
			direction *= -1;

			prev = y1;
			next = y2+1;


			x1 = x - nOneSideSize;
			x2 = x + nOneSideSize;

			for(xx = x1; xx <= x2; xx++)
			{
				outValue = ImageGray[prev][xx];
				inValue = ImageGray[next][xx];

				if(outValue == inValue)
					continue;
				His[outValue]--;
				if(outValue < median)
					delta--;
				His[inValue]++;
				if(inValue < median)
					delta++;
			}

			if(delta >= middle)
			{
				while(delta >= middle)
				{
					if(His[--median] > 0)
						delta -= His[median];
				}
			}
			else
			{
				while(delta + His[median] < middle)
				{
					if(His[median] > 0)
						delta += His[median];
					median++;
				}
			}

			OutputGray[y+1][x] = median;
			x += direction;
		}
	}
}

void GetAffineParameters(CIpPoint D1, CIpPoint D2, CIpPoint D3, CIpPoint V1, CIpPoint V2, CIpPoint V3,
	double *pA1, double *pA2, double *pA0, double *pB1, double *pB2, double *pB0)
{
	double **T = dmatrix(3, 3);
	double **IT = dmatrix(3, 3);
	double **V = dmatrix(3, 3);

	T[0][0] = D1.x;
	T[0][1] = D1.y;
	T[0][2] = 1;

	T[1][0] = D2.x;
	T[1][1] = D2.y;
	T[1][2] = 1;

	T[2][0] = D3.x;
	T[2][1] = D3.y;
	T[2][2] = 1;

	V[0][0] = V1.x;  // visual
	V[0][1] = V1.y;
	V[0][2] = 1;

	V[1][0] = V2.x;
	V[1][1] = V2.y;
	V[1][2] = 1;

	V[2][0] = V3.x;
	V[2][1] = V3.y;
	V[2][2] = 1;

	double det;
	InverseMatrix(T, IT, 3, &det);


	*pA1 = IT[0][0] * V[0][0] + IT[0][1] * V[1][0] + IT[0][2] * V[2][0];
	*pA2 = IT[1][0] * V[0][0] + IT[1][1] * V[1][0] + IT[1][2] * V[2][0];
	*pA0 = IT[2][0] * V[0][0] + IT[2][1] * V[1][0] + IT[2][2] * V[2][0];

	*pB1 = IT[0][0] * V[0][1] + IT[0][1] * V[1][1] + IT[0][2] * V[2][1];
	*pB2 = IT[1][0] * V[0][1] + IT[1][1] * V[1][1] + IT[1][2] * V[2][1];
	*pB0 = IT[2][0] * V[0][1] + IT[2][1] * V[1][1] + IT[2][2] * V[2][1];


	free_dmatrix(T, 3, 3);
	free_dmatrix(IT, 3, 3);
	free_dmatrix(V, 3, 3);
}

void Affine(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut,
	double a1, double a2, double a0, double b1, double b2, double b0)
{
	int x, y;
	int xx, yy;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			xx = (int)(a0 + x*a1 + y*a2 + 0.5);
			yy = (int)(b0 + x*b1 + y*b2 + 0.5);

			if(xx >= 0 && xx < nW && yy >= 0 && yy < nH)
				Image2DGrayOut[y][x] = Image2DGray[yy][xx];
			else
				Image2DGrayOut[y][x] = 0;

		}
}

void Bilinear(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut,
	double u0, double v0, double u1, double v1, double u2, double v2, double u3, double v3) 
{
	double u01, u23;
	double v03, v12;

	double doubleU, doubleV;

	int x, y;
	int mapX, mapY;
	double xRate, yRate;

	for(y = 0 ; y < nH ; y++)
	{
		yRate = (double)y/nH;
		for(x = 0 ; x < nW ; x++)
		{
			xRate = (double)x/nW;

			u01 = u0 + (u1 - u0) * xRate;
			u23 = u3 + (u2 - u3) * xRate;
			doubleU = u01 + (u23 - u01) * yRate;

			v03 = v0 + (v3 -v0) * yRate;
			v12 = v1 + (v2 -v1) * yRate;
			doubleV = v03 + (v12 - v03) * xRate;

			mapX = (int)(doubleU + 0.5);
			mapY = (int)(doubleV + 0.5);
			
			if(mapX >= 0 && mapX < nW && mapY >= 0 && mapY < nH)
				Image2DGrayOut[y][x] = Image2DGray[mapY][mapX];
			else
				Image2DGrayOut[y][x] = 0;
		}
	}
	////
}

void Mean2DGrayFast(double **Image2D, int nW, int nH, int nSize, double **Image2DOut)
{
	int x, y, dx, dy;
	double Mean, Out, LeftSum;
	int SideA, SideB;
	bool First = true;
	double *RowImage, *RowOut;
	int xx, yy1, yy2;

	double *SumStrip = new double[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
		{
			if(dy >= 0 && dy < nH)
				SumStrip[x] += Image2D[dy][x];
		}
	}

	for(y = 0 ; y < nH ; y++)
	{
		RowOut = Image2DOut[y];
		RowImage = Image2D[y];
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Mean = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
				{
					xx = x+dx;
					if(xx >= 0 && xx < nW)
						Mean += SumStrip[xx];
				}
				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else 
					LeftSum = 0;
			}
			else
			{
				Mean -= LeftSum;
				xx = x+SideB;
				if(xx >= 0 && xx < nW)
					Mean += SumStrip[xx];

				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else
					LeftSum = 0;
			}

			if(x == nW-1)
			{
				yy1 = y+SideA;
				yy2 = y+SideB+1;
				for(dx = 0 ; dx < nW ; dx++)
				{
					if(yy1 >= 0 && yy1 < nH)
						SumStrip[dx] -= Image2D[yy1][dx];

					if(yy2 >= 0 && yy2 < nH)
						SumStrip[dx] += Image2D[yy2][dx];
				}
			}
			
			int startX = SideA + x < 0 ? 0 : SideA + x;
			int endX = SideB + x > nW-1 ? nW-1 : SideB + x;
			int startY = SideA + y < 0 ? 0 : SideA + y;
			int endY = SideB + y > nH-1 ? nH-1 : SideB + y;

			//Out = Mean/(nSize*nSize);
			Out = Mean/((endX-startX+1)*(endY-startY+1));

			RowOut[x] = Out;
		}
	}

	delete [] SumStrip;
}

void Mean2DGrayFast(unsigned char **Image2D, int nW, int nH, int nSize, unsigned char **Image2DOut)
{
	int x, y, dx, dy;
	double Mean, Out, LeftSum;
	int SideA, SideB;
	bool First = true;
	unsigned char *RowImage;
	unsigned char *RowOut;
	int xx, yy1, yy2;

	double *SumStrip = new double[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
		{
			if(dy >= 0 && dy < nH)
				SumStrip[x] += Image2D[dy][x];
		}
	}

	for(y = 0 ; y < nH ; y++)
	{
		RowOut = Image2DOut[y];
		RowImage = Image2D[y];
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Mean = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
				{
					xx = x+dx;
					if(xx >= 0 && xx < nW)
						Mean += SumStrip[xx];
				}
				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else 
					LeftSum = 0;
			}
			else
			{
				Mean -= LeftSum;
				xx = x+SideB;
				if(xx >= 0 && xx < nW)
					Mean += SumStrip[xx];

				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else
					LeftSum = 0;
			}

			if(x == nW-1)
			{
				yy1 = y+SideA;
				yy2 = y+SideB+1;
				for(dx = 0 ; dx < nW ; dx++)
				{
					if(yy1 >= 0 && yy1 < nH)
						SumStrip[dx] -= Image2D[yy1][dx];

					if(yy2 >= 0 && yy2 < nH)
						SumStrip[dx] += Image2D[yy2][dx];
				}
			}
			
			int startX = SideA + x < 0 ? 0 : SideA + x;
			int endX = SideB + x > nW-1 ? nW-1 : SideB + x;
			int startY = SideA + y < 0 ? 0 : SideA + y;
			int endY = SideB + y > nH-1 ? nH-1 : SideB + y;

			//Out = Mean/(nSize*nSize);
			Out = Mean/((endX-startX+1)*(endY-startY+1));

			RowOut[x] = (int)Out;
		}
	}

	delete [] SumStrip;
}

void Mean2DGrayFast(unsigned char **Image2D, int nW, int nH, int nSize, double **Image2DOut)
{
	int x, y, dx, dy;
	double Mean, Out, LeftSum;
	int SideA, SideB;
	bool First = true;
	unsigned char *RowImage;
	double *RowOut;
	int xx, yy1, yy2;

	double *SumStrip = new double[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
		{
			if(dy >= 0 && dy < nH)
				SumStrip[x] += Image2D[dy][x];
		}
	}

	for(y = 0 ; y < nH ; y++)
	{
		RowOut = Image2DOut[y];
		RowImage = Image2D[y];
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Mean = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
				{
					xx = x+dx;
					if(xx >= 0 && xx < nW)
						Mean += SumStrip[xx];
				}
				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else 
					LeftSum = 0;
			}
			else
			{
				Mean -= LeftSum;
				xx = x+SideB;
				if(xx >= 0 && xx < nW)
					Mean += SumStrip[xx];

				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else
					LeftSum = 0;
			}

			if(x == nW-1)
			{
				yy1 = y+SideA;
				yy2 = y+SideB+1;
				for(dx = 0 ; dx < nW ; dx++)
				{
					if(yy1 >= 0 && yy1 < nH)
						SumStrip[dx] -= Image2D[yy1][dx];

					if(yy2 >= 0 && yy2 < nH)
						SumStrip[dx] += Image2D[yy2][dx];
				}
			}
			
			int startX = SideA + x < 0 ? 0 : SideA + x;
			int endX = SideB + x > nW-1 ? nW-1 : SideB + x;
			int startY = SideA + y < 0 ? 0 : SideA + y;
			int endY = SideB + y > nH-1 ? nH-1 : SideB + y;

			//Out = Mean/(nSize*nSize);
			Out = Mean/((endX-startX+1)*(endY-startY+1));

			RowOut[x] = Out;
		}
	}

	delete [] SumStrip;
}

void MeanSd2DGrayFast(unsigned char **Image2D, int nW, int nH, int nSize, double **Mean2D, double **Sd2D)
{
	int x, y, dx, dy;
	int Mean, LeftSum;
	double SquareMean, Out, LeftSumSquare;
	int SideA, SideB;
	bool First = true;
	unsigned char *RowImage;
	double *RowMean, *RowSd;
	int xx, yy1, yy2;

	int *SumStripMean = new int[nW];
	double *SumStripSquareMean = new double[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStripMean[x] = 0;
		SumStripSquareMean[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
		{
			if(dy >= 0 && dy < nH)
			{
				int Value = Image2D[dy][x];
				SumStripMean[x] += Value;
				SumStripSquareMean[x] += Value*Value;
			}
		}
	}

	for(y = 0 ; y < nH ; y++)
	{
		RowMean = Mean2D[y];
		RowSd = Sd2D[y];

		RowImage = Image2D[y];
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Mean = 0;
				SquareMean = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
				{
					xx = x+dx;
					if(xx >= 0 && xx < nW)
					{
						Mean += SumStripMean[xx];
						SquareMean += SumStripSquareMean[xx];
					}
				}
				xx = x+SideA;
				if(xx >= 0 && xx < nW)
				{
					LeftSum = SumStripMean[xx];
					LeftSumSquare = SumStripSquareMean[xx];
				}
				else 
				{
					LeftSum = 0;
					LeftSumSquare = 0;
				}
			}
			else
			{
				Mean -= LeftSum;
				SquareMean -= LeftSumSquare;
				xx = x+SideB;
				if(xx >= 0 && xx < nW)
				{
					Mean += SumStripMean[xx];
					SquareMean += SumStripSquareMean[xx];
				}

				xx = x+SideA;
				if(xx >= 0 && xx < nW)
				{
					LeftSum = SumStripMean[xx];
					LeftSumSquare = SumStripSquareMean[xx];
				}
				else
				{
					LeftSum = 0;
					LeftSumSquare = 0;
				}
			}

			if(x == nW-1)
			{
				yy1 = y+SideA;
				yy2 = y+SideB+1;

				if(yy1 >= 0 && yy1 < nH)
					for(dx = 0 ; dx < nW ; dx++)
					{
						int OutValue = Image2D[yy1][dx];

						SumStripMean[dx] -= OutValue;
						SumStripSquareMean[dx] -= OutValue*OutValue;
					}

				if(yy2 >= 0 && yy2 < nH)
					for(dx = 0 ; dx < nW ; dx++)
					{
						int InValue = Image2D[yy2][dx];

						SumStripMean[dx] += InValue;
						SumStripSquareMean[dx] += InValue*InValue;
					}
			}
			
			int startX = SideA + x < 0 ? 0 : SideA + x;
			int endX = SideB + x > nW-1 ? nW-1 : SideB + x;
			int startY = SideA + y < 0 ? 0 : SideA + y;
			int endY = SideB + y > nH-1 ? nH-1 : SideB + y;

			//Out = Mean/(nSize*nSize);

			double RealSize = ((endX-startX+1)*(endY-startY+1));
			Out = Mean/RealSize;
			RowMean[x] = Out;

			Out = SquareMean/RealSize - Out*Out;
			RowSd[x] = sqrt(Out);
		}
	}

	delete [] SumStripSquareMean;
	delete [] SumStripMean;
}

void Sd2DGrayFast(unsigned char **Image2D, double **Mean2D, int nW, int nH, int nSize, double **Image2DOut)
{
	int x, y, dx, dy;
	double Mean, Out, LeftSum;
	int SideA, SideB;
	bool First = true;
	unsigned char *RowImage;
	double *RowOut;
	int xx, yy1, yy2;

	double *SumStrip = new double[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
		{
			if(dy >= 0 && dy < nH)
				SumStrip[x] += Image2D[dy][x]*Image2D[dy][x];
		}
	}

	for(y = 0 ; y < nH ; y++)
	{
		RowOut = Image2DOut[y];
		RowImage = Image2D[y];
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Mean = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
				{
					xx = x+dx;
					if(xx >= 0 && xx < nW)
						Mean += SumStrip[xx];
				}
				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else 
					LeftSum = 0;
			}
			else
			{
				Mean -= LeftSum;
				xx = x+SideB;
				if(xx >= 0 && xx < nW)
					Mean += SumStrip[xx];

				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else
					LeftSum = 0;
			}

			if(x == nW-1)
			{
				yy1 = y+SideA;
				yy2 = y+SideB+1;
				for(dx = 0 ; dx < nW ; dx++)
				{
					if(yy1 >= 0 && yy1 < nH)
						SumStrip[dx] -= Image2D[yy1][dx]*Image2D[yy1][dx];

					if(yy2 >= 0 && yy2 < nH)
						SumStrip[dx] += Image2D[yy2][dx]*Image2D[yy2][dx];
				}
			}
			
			int startX = SideA + x < 0 ? 0 : SideA + x;
			int endX = SideB + x > nW-1 ? nW-1 : SideB + x;
			int startY = SideA + y < 0 ? 0 : SideA + y;
			int endY = SideB + y > nH-1 ? nH-1 : SideB + y;

			//Out = Mean/(nSize*nSize);
			Out = Mean/((endX-startX+1)*(endY-startY+1));
			Out = Out - Mean2D[y][x]*Mean2D[y][x];

			RowOut[x] = sqrt(Out);
		}
	}

	delete [] SumStrip;
}

typedef unsigned long uint32;
typedef long int32;   
static inline int32 ilog2(float x)
{
    uint32 ix = (uint32&)x;
    uint32 exp = (ix >> 23) & 0xFF;
    int32 log2 = int32(exp) - 127;

    return log2;
}

inline float fast_log2 (float val)
 {
  //  assert (val > 0);

    int * const  exp_ptr = reinterpret_cast <int *> (&val);
    int          x = *exp_ptr;
    const int    log_2 = ((x >> 23) & 255) - 128;
    x &= ~(255 << 23);
    x += 127 << 23;
    *exp_ptr = x;

    return (val + log_2);
 }


double GetEntropy(int *His, int *list, int nWin)
{
	int i;
	double E = 0;
	double size = nWin*nWin;

	for(i = 0 ; i < 256; i++)
	{
		int ListValue = list[i];
		if(ListValue == -1) break;

		if(His[ListValue] > 0)
			E -= His[ListValue]/size*fast_log2((float)(His[ListValue]/size));
	}

	return E;
}

void FastEntropy(unsigned char **ImageGray, double **Output, int nW, int nH, int nOneSideSize)
{
	// nOneSideSize = 1 => WinSize = 3
	// nOneSideSize = 2 -> WinSize = 5;

	int list[256], k;
	int median, delta;
	int prev, next;
	int His[256] = {0};
	int direction = 1;
	int x, y;
	int x1, x2, y1, y2;
	int xx, yy;
	int i, sum;
	int inValue, outValue;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			Output[y][x] = 0;

	int middle=((nOneSideSize*2+1)*(nOneSideSize*2+1)+1)/2;

	for(k = 0 ; k < 256 ; k++)
		list[k] = -1;

	for(y = 0 ; y < nOneSideSize*2+1 ; y++)
		for(x = 0 ; x < nOneSideSize*2+1 ; x++)
		{
			His[ImageGray[y][x]]++;

			int data = ImageGray[y][x];
			for(k = 0 ; k < 256 ; k++)
			{
				if(list[k] == data) break;
				if(list[k] == -1) 
				{
					list[k] = data;
					break;
				}
			}
		}

	sum = 0;
	for(i=0 ; i < 256 ; i++)
	{
		sum += His[i];
		if(sum >= middle)
			break;
	}
	median = i;
	delta = sum - His[i];

	//OutputGray[nOneSideSize][nOneSideSize] = median;
	Output[nOneSideSize][nOneSideSize] = GetEntropy(His, list, nOneSideSize*2+1);

	x = nOneSideSize+1;
	for(y = nOneSideSize ; y < nH-nOneSideSize ; y++)
	{
		y1 = y - nOneSideSize;
		y2 = y + nOneSideSize;

		for(; x >= nOneSideSize && x < nW-nOneSideSize ; x += direction)
		{
			prev = x-direction*(nOneSideSize+1);
			next = x+direction*nOneSideSize;

			for(yy = y1 ; yy <= y2 ; yy++)
			{

				outValue = ImageGray[yy][prev];
				inValue = ImageGray[yy][next];

				if(outValue == inValue)
					continue;
				His[outValue]--;
				if(outValue < median)
					delta--;
				His[inValue]++;
				if(inValue < median)
					delta++;

				int data = inValue;
				
				if(His[inValue] == 1)
				{
					for(k = 0 ; k < 256 ; k++)
					{
						if(list[k] == data) break;
						if(list[k] == -1) 
						{
							list[k] = data;
							break;
						}
					}
				}

				if(His[outValue] == 0)
				{
					data = outValue;
					for(k = 0 ; k < 256 ; k++)
					{
						if(list[k] == data) 
						{
							if(k == 255)
							{
								list[k] = -1;
								break;
							}
							else
							{
								int kk;
								for(kk = k ; kk < 255 ; kk++)
								{
									list[kk] = list[kk+1];
									if(list[kk] == -1) break;
								}
								list[255] = -1;
							}
							break;
						}
					}
				}
			}

			if(delta >= middle)
			{
				while(delta >= middle)
				{
					if(His[--median] > 0)
						delta -= His[median];
				}
			}
			else
			{
				while(delta + His[median] < middle)
				{
					if(His[median] > 0)
						delta += His[median];
					median++;
				}
			}

			//OutputGray[y][x] = median;
			Output[y][x] = GetEntropy(His, list, nOneSideSize*2+1);
		} 

		if(y < nH-nOneSideSize-1)
		{
			x -= direction;
			direction *= -1;

			prev = y1;
			next = y2+1;


			x1 = x - nOneSideSize;
			x2 = x + nOneSideSize;

			for(xx = x1; xx <= x2; xx++)
			{
				outValue = ImageGray[prev][xx];
				inValue = ImageGray[next][xx];

				if(outValue == inValue)
					continue;
				His[outValue]--;
				if(outValue < median)
					delta--;
				His[inValue]++;
				if(inValue < median)
					delta++;

				int data = inValue;

				if(His[inValue] == 1)
				{
					for(k = 0 ; k < 256 ; k++)
					{
						if(list[k] == data) break;
						if(list[k] == -1) 
						{
							list[k] = data;
							break;
						}
					}
				}
				
				if(His[outValue] == 0)
				{
					data = outValue;
					for(k = 0 ; k < 256 ; k++)
					{
						if(list[k] == data) 
						{
							if(k == 255)
							{
								list[k] = -1;
								break;
							}
							else
							{
								int kk;
								for(kk = k ; kk < 255 ; kk++)
								{
									list[kk] = list[kk+1];
									if(list[kk] == -1) break;
								}
								list[255] = -1;
							}
							break;
						}
					}
				}
			}

			if(delta >= middle)
			{
				while(delta >= middle)
				{
					if(His[--median] > 0)
						delta -= His[median];
				}
			}
			else
			{
				while(delta + His[median] < middle)
				{
					if(His[median] > 0)
						delta += His[median];
					median++;
				}
			}

			//OutputGray[y+1][x] = median;
			Output[y+1][x] = GetEntropy(His, list, nOneSideSize*2+1);
			x += direction;
		}
	}
}

void DcNotch2DGrayFast(unsigned char **Image2DGray, unsigned char **Image2DGrayOut, int nW, int nH, int nSize)
{
	int x, y, dx, dy;
	int Mean, Out, LeftSum;
	int SideA, SideB;
	bool First = true;
	unsigned char *RowImage, *RowOut;
	int xx, yy1, yy2;

	int *SumStrip = new int[nW];

	SideA = -1*nSize/2;
	SideB = (int)(nSize/2.+0.5) - 1;

	for(x = 0 ; x < nW ; x++)
	{
		SumStrip[x] = 0;

		for(dy = SideA ; dy <= SideB ; dy++)
		{
			if(dy >= 0 && dy < nH)
				SumStrip[x] += Image2DGray[dy][x];
		}
	}

	for(y = 0 ; y < nH ; y++)
	{
		RowOut = Image2DGrayOut[y];
		RowImage = Image2DGray[y];
		for(x = 0 ; x < nW ; x++)
		{
			if(x == 0)
			{
				Mean = 0;
				for(dx = SideA ; dx <= SideB ; dx++)
				{
					xx = x+dx;
					if(xx >= 0 && xx < nW)
						Mean += SumStrip[xx];
				}
				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else 
					LeftSum = 0;
			}
			else
			{
				Mean -= LeftSum;
				xx = x+SideB;
				if(xx >= 0 && xx < nW)
					Mean += SumStrip[xx];

				xx = x+SideA;
				if(xx >= 0 && xx < nW)
					LeftSum = SumStrip[xx];
				else
					LeftSum = 0;
			}

			if(x == nW-1)
			{
				yy1 = y+SideA;
				yy2 = y+SideB+1;
				for(dx = 0 ; dx < nW ; dx++)
				{
					if(yy1 >= 0 && yy1 < nH)
						SumStrip[dx] -= Image2DGray[yy1][dx];

					if(yy2 >= 0 && yy2 < nH)
						SumStrip[dx] += Image2DGray[yy2][dx];
				}
			}


			Out = RowImage[x] - Mean/(nSize*nSize) + 128;

			if(Out < 0)
				RowOut[x] = 0;
			else if(Out > 255)
				RowOut[x] = 255;
			else
				RowOut[x] = Out;
		}
	}

	delete [] SumStrip;
}

CIpPoint BMA(unsigned char **CurrImageGray, unsigned char **ReferImageGray, int left, int top, 
		 int nW, int nH, int nSearchLeft, int nSearchTop, int nSearchRight, int nSearchBottom, 
		 int BlockWidth, int BlockHeight)
{
	if(top < BlockHeight+nSearchBottom ||  top >= nH - BlockHeight-nSearchTop || 
		left < BlockWidth+nSearchRight || left >= nW - BlockWidth-nSearchLeft)
	{
		return CIpPoint(0,0);
	}

	int x, y, dx, dy;
	CIpPoint MinDx = CIpPoint(0, 0);
	double Diff, MinDiff = 255;
	int xx0, yy0, xx1, yy1;

	for(dy = -nSearchTop ; dy <= nSearchBottom ; dy++)
	{
		for(dx = -nSearchLeft ; dx <= nSearchRight ; dx++)
		{
			Diff = 0.;

			for(y = 0 ; y <= BlockHeight ; y++)
				for(x = 0 ; x <= BlockWidth ; x++)
				{
					yy0 = top+y;
					xx0 = left+x;

					xx1 = left+x+dx;
					yy1 = top+y+dy;

					Diff += abs(CurrImageGray[yy0][xx0] - ReferImageGray[yy1][xx1]);
				}

			Diff /= BlockWidth*BlockHeight;

			if(Diff < MinDiff)
			{
				MinDiff = Diff;
				MinDx = CIpPoint(dx, dy);
			}
		}
	}

	return MinDx;
}

void InversePerspectiveTable(int W, int H, int LW, int LH,
	CIpPoint p0, CIpPoint p1, CIpPoint p2, CIpPoint p3, CIpPoint **Table)
{
	double delx1, dely1, delx2, dely2, delx3, dely3;
	double a11, a12, a13;
	double a21, a22, a23;
	double a31, a32, a33;

	int u, v;
	double floatu, floatv;
	int i, j;
	double x, y;
//	unsigned long sourcebase;
//	double dA, dB, dC, dD, dE, dF, dG, dH, dI;

	delx1 = p1.x - p2.x;
	dely1 = p1.y - p2.y;
	delx2 = p3.x - p2.x;
	dely2 = p3.y - p2.y;
	delx3 = p0.x - p1.x + p2.x - p3.x;
	dely3 = p0.y - p1.y + p2.y - p3.y;

	if(delx3 == 0.0 && dely3 == 0.0)
	{
		a11 = p1.x - p0.x;
		a21 = p2.x - p1.x;
		a31 = p0.x;
		a12 = p1.y - p0.y;
		a22 = p2.y - p1.y;
		a32 = p0.y;
		a13 = 0.0;
		a23 = 0.0;
		a33 = 1.0;
	}
	else
	{
		a13 = (delx3*dely2-delx2*dely3)/(delx1*dely2-dely1*delx2);
		a23 = (delx1*dely3-dely1*delx3)/(delx1*dely2-dely1*delx2);
		a11 = p1.x - p0.x + a13*p1.x;
		a21 = p3.x - p0.x + a23*p3.x;
		a31 = p0.x;
		a12 = p1.y - p0.y + a13*p1.y;
		a22 = p3.y - p0.y + a23*p3.y;
		a32 = p0.y;
		a33 = 1.0;
	}

//	dA = a22 * a33 - a32 * a23;
//	dB = a31 * a23 - a21 * a33;
//	dC = a21 * a32 - a31 * a22;
//	dD = a32 * a13 - a12 * a33;
//	dE = a11 * a33 - a31 * a13;
//	dF = a31 * a12 - a11 * a32;
//	dG = a12 * a23 - a22 * a13;
//	dH = a21 * a13 - a11 * a23;
//	dI = a11 * a22 - a21 * a12;

	for(i = 0 ; i < LH ; i++)
	{
		y = (double)i/(double)LH;
		for(j = 0 ; j < LW ; j++)
		{
			x = (double)j/(double)LW;
			floatu = ((a11*x + a21*y + a31)/(a13*x + a23*y + a33));// * (double)W;
			floatv = ((a12*x + a22*y + a32)/(a13*x + a23*y + a33));// * (double)H;

			u = (int)(floatu + 0.5);
			v = (int)(floatv + 0.5);

			Table[i][j].x = u;
			Table[i][j].y = v;

			if(u >= W) 
				Table[i][j].x = W-1;
			else if(u < 0)
				Table[i][j].x = 0;
			
			if(v >= H) 
				Table[i][j].y = H-1;
			else if(v < 0)
				Table[i][j].y = 0;
		}
	}
}

void InversePerspective(int W, int H, int LW, int LH,
	unsigned char **img, unsigned char **out, CIpPoint **Table)
{
	int x, y, dx, dy;

	for(y = 0 ; y < LH ; y++)
		for(x = 0 ; x < LW ; x++)
		{
			dx = Table[y][x].x;
			dy = Table[y][x].y;

			if(dx < 0 || dy < 0) out[y][x] = 0;
			else out[y][x] = img[dy][dx];
		}
}


void PerspectiveTable(int W, int H, int TW, int TH, 
	CIpPoint p0, CIpPoint p1, CIpPoint p2, CIpPoint p3, CIpPoint **Table)
{
	double delx1, dely1, delx2, dely2, delx3, dely3;
	double a11, a12, a13;
	double a21, a22, a23;
	double a31, a32, a33;

	int u, v;
	double floatu, floatv;
	int i, j;
	double x, y;
//	unsigned long sourcebase;
	double dA, dB, dC, dD, dE, dF, dG, dH, dI;

	delx1 = p1.x - p2.x;
	dely1 = p1.y - p2.y;
	delx2 = p3.x - p2.x;
	dely2 = p3.y - p2.y;
	delx3 = p0.x - p1.x + p2.x - p3.x;
	dely3 = p0.y - p1.y + p2.y - p3.y;

	if(delx3 == 0.0 && dely3 == 0.0)
	{
		a11 = p1.x - p0.x;
		a21 = p2.x - p1.x;
		a31 = p0.x;
		a12 = p1.y - p0.y;
		a22 = p2.y - p1.y;
		a32 = p0.y;
		a13 = 0.0;
		a23 = 0.0;
		a33 = 1.0;
	}
	else
	{
		a13 = (delx3*dely2-delx2*dely3)/(delx1*dely2-dely1*delx2);
		a23 = (delx1*dely3-dely1*delx3)/(delx1*dely2-dely1*delx2);
		a11 = p1.x - p0.x + a13*p1.x;
		a21 = p3.x - p0.x + a23*p3.x;
		a31 = p0.x;
		a12 = p1.y - p0.y + a13*p1.y;
		a22 = p3.y - p0.y + a23*p3.y;
		a32 = p0.y;
		a33 = 1.0;
	}

	dA = a22 * a33 - a32 * a23;
	dB = a31 * a23 - a21 * a33;
	dC = a21 * a32 - a31 * a22;
	dD = a32 * a13 - a12 * a33;
	dE = a11 * a33 - a31 * a13;
	dF = a31 * a12 - a11 * a32;
	dG = a12 * a23 - a22 * a13;
	dH = a21 * a13 - a11 * a23;
	dI = a11 * a22 - a21 * a12;

	for(i = 0 ; i < TH ; i++)
	{
		y = (double)i;///(double)H;
		for(j = 0 ; j < TW ; j++)
		{
			x = (double)j;///(double)W;
			floatu = ((dA*x + dB*y + dC)/(dG*x + dH*y + dI)) * (double)W;
			floatv = ((dD*x + dE*y + dF)/(dG*x + dH*y + dI)) * (double)H;

			u = (int)(floatu + 0.5);
			v = (int)(floatv + 0.5);

			Table[i][j].x = u;
			Table[i][j].y = v;

			if(u >= W)
				Table[i][j].x = W-1;
			else if(u < 0)
				Table[i][j].x = 0;
			
			if(v >= H)
				Table[i][j].y = H-1;
			else if(v < 0)
				Table[i][j].y = 0;
		}
	}
}

void Perspective(int W, int H, int TW, int TH, 
	unsigned char **img, unsigned char **out, CIpPoint **Table)
{
	int x, y, dx, dy;

	for(y = 0 ; y < TH ; y++)
		for(x = 0 ; x < TW ; x++)
		{
			dx = Table[y][x].x;
			dy = Table[y][x].y;

			if(dx < 0 || dy < 0) out[y][x] = 0;
			else out[y][x] = img[dy][dx];
		}
}

void BilinearRadial(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut,
			  int topLeft) 
{
	int x, y;
	int mapX;
	//int mapY;

	topLeft = 300;
	for(y = 0 ; y < nH ; y++)
	{	
		for(x = 0 ; x < nW ; x++)
		{
			double fact = (x-nW/2.)*pow(fabs(nW/2.-x)/(nW/2.), 0.05);//*0.25*fabs( y)/pow(fabs( y), 0.8);//* (1./fabs(x-nW/2.))*1000;
			double dist = Dist(x, y, nW/2, nH-1);
			double MaxDist = Dist(0, 0, nW/2, nH-1);
		//	double fact = (x-nW/2.)*pow(dist/MaxDist, 1.2);
			mapX = (int)(nW/2.+ fact * (nW/2.) / ((topLeft)*y/(nH-1)+(nW/2.-topLeft)) + 0.5);
			
			if(mapX < 0)
				Image2DGrayOut[y][x] = Image2DGray[y][0];
			else if(mapX >= nW)
				Image2DGrayOut[y][x] = Image2DGray[y][nW-1];
			else
				Image2DGrayOut[y][x] = Image2DGray[y][mapX];

//			double fact2 = (nH-1-y)*pow((nH-1-y)/(nH-1.), 0.95);
//			mapY = (int)(nH-1-fact2+0.5);

//			if(mapX >= nW || mapY >= nH || mapX < 0 || mapY < 0)
//				Image2DGrayOut[y][x] = 0;
//			else
//				Image2DGrayOut[y][x] = Image2DGray[mapY][mapX];
				
		}
	}
	////
}

void InversePerspectivePt(int W, int H, int LW, int LH,
	CIpPoint p0, CIpPoint p1, CIpPoint p2, CIpPoint p3, double Ox, double Oy, double *Tx, double *Ty)
{
	double delx1, dely1, delx2, dely2, delx3, dely3;
	double a11, a12, a13;
	double a21, a22, a23;
	double a31, a32, a33;

	double x, y;
//	unsigned long sourcebase;
//	double dA, dB, dC, dD, dE, dF, dG, dH, dI;

	delx1 = p1.x - p2.x;
	dely1 = p1.y - p2.y;
	delx2 = p3.x - p2.x;
	dely2 = p3.y - p2.y;
	delx3 = p0.x - p1.x + p2.x - p3.x;
	dely3 = p0.y - p1.y + p2.y - p3.y;

	if(delx3 == 0.0 && dely3 == 0.0)
	{
		a11 = p1.x - p0.x;
		a21 = p2.x - p1.x;
		a31 = p0.x;
		a12 = p1.y - p0.y;
		a22 = p2.y - p1.y;
		a32 = p0.y;
		a13 = 0.0;
		a23 = 0.0;
		a33 = 1.0;
	}
	else
	{
		a13 = (delx3*dely2-delx2*dely3)/(delx1*dely2-dely1*delx2);
		a23 = (delx1*dely3-dely1*delx3)/(delx1*dely2-dely1*delx2);
		a11 = p1.x - p0.x + a13*p1.x;
		a21 = p3.x - p0.x + a23*p3.x;
		a31 = p0.x;
		a12 = p1.y - p0.y + a13*p1.y;
		a22 = p3.y - p0.y + a23*p3.y;
		a32 = p0.y;
		a33 = 1.0;
	}

//	dA = a22 * a33 - a32 * a23;
//	dB = a31 * a23 - a21 * a33;
//	dC = a21 * a32 - a31 * a22;
//	dD = a32 * a13 - a12 * a33;
//	dE = a11 * a33 - a31 * a13;
//	dF = a31 * a12 - a11 * a32;
//	dG = a12 * a23 - a22 * a13;
//	dH = a21 * a13 - a11 * a23;
//	dI = a11 * a22 - a21 * a12;

	y = (double)Oy/(double)LH;
	x = (double)Ox/(double)LW;
			
	*Tx = ((a11*x + a21*y + a31)/(a13*x + a23*y + a33));// * (double)W;
	*Ty = ((a12*x + a22*y + a32)/(a13*x + a23*y + a33));// * (double)H;
}

void DrawBoundary(unsigned char **ImageGray, int nW, int nH, CIpPoint *Boundary, int Cnt, unsigned char Color)
{
	int i;
	
	for(i = 0 ; i < Cnt ; i++)
	{
		if(Boundary[i].x < 0) continue;
		if(Boundary[i].y < 0) continue;
		if(Boundary[i].x >= nW) continue;
		if(Boundary[i].y >= nH) continue;

		ImageGray[Boundary[i].y][Boundary[i].x] = Color;
	}
}

double MeanAngleLabel(int **Label, int nLabelNum, CIpRect boundbox)
{
	double sumSine = 0., sumCosine = 0.;
	int cnt = 0;
	int x, y;
	double theta;
	double meanX = 0., meanY = 0.;

	for(y = boundbox.top ; y <= boundbox.bottom ; y++)
		for(x = boundbox.left ; x < boundbox.right ; x++)
			if(Label[y][x] == nLabelNum)
			{
				meanX += x;
				meanY += y;
				cnt++;
			}

	if(cnt > 0)
	{
		meanX /= cnt;
		meanY /= cnt;
	}

	for(y = boundbox.top ; y <= boundbox.bottom ; y++)
		for(x = boundbox.left ; x < boundbox.right ; x++)
			if(Label[y][x] == nLabelNum)
			{
				double angle = atan2(y-meanY, x-meanX);
				sumSine += sin(2.*atan2(y-meanY, x-meanX));
				sumCosine += cos(2.*atan2(y-meanY, x-meanX));
			}

	if(cnt == 0) theta = 0.;
	else theta = atan2(sumSine, sumCosine)/2.;

	return theta;
}

void Harris2DGray(unsigned char **Image2DGray, int nW, int nH, int nSize, double **Image2DGrayOut, double dThre, double dK)
{
	double GrdX, GrdY;
	double GrdX2, GrdY2, GrdXY;
	int x, y;
	int dx, dy;

	double **Temp;

	Temp = dmatrix(nH, nW);

	for (y = 0 ; y < nH; y++)
		for (x = 0 ; x < nW ; x++)
		{
			GrdX2=0.;
			GrdY2=0.;
			GrdXY=0.;

			for(dy = -1 * nSize/2 ; dy < (double)nSize/2. ; dy++)
				for(dx = -1 * nSize/2 ; dx < (double)nSize/2. ; dx++)
				{
					GrdX = (Image2DGray[(y+dy+nH)%nH][(x+dx+nW+1)%nW] - Image2DGray[(y+dy+nH)%nH][(x+dx+nW-1)%nW])/2.;
					GrdY = (Image2DGray[(y+dy+nH+1)%nH][(x+dx+nW)%nW] - Image2DGray[(y+dy+nH-1)%nH][(x+dx+nW)%nW])/2.;

					GrdX2 += GrdX*GrdX;
					GrdY2 += GrdY*GrdY;
					GrdXY += GrdX*GrdY;
				}
      
			GrdX2 /= nSize*2;
			GrdY2 /= nSize*2;
			GrdXY /= nSize*2;

			Temp[y][x] = (GrdX2*GrdY2 - GrdXY*GrdXY) -
				dK * (GrdX2+GrdY2)*(GrdX2+GrdY2);
		}

	for (y = 0 ; y < nH; y++)
		for (x = 0 ; x < nW ; x++)
		{
			if(dThre > 0)
			{
				if(Temp[y][x] > dThre &&
					Temp[y][x] >= Temp[(y+nH-1)%nH][(x+nW-1)%nW] &&
					Temp[y][x] >= Temp[(y+nH-1)%nH][(x+nW)%nW] &&
					Temp[y][x] >= Temp[(y+nH-1)%nH][(x+nW+1)%nW] &&
					Temp[y][x] >= Temp[(y+nH)%nH][(x+nW-1)%nW] &&
					Temp[y][x] >= Temp[(y+nH)%nH][(x+nW+1)%nW] &&
					Temp[y][x] >= Temp[(y+nH+1)%nH][(x+nW-1)%nW] &&
					Temp[y][x] >= Temp[(y+nH+1)%nH][(x+nW)%nW] &&
					Temp[y][x] >= Temp[(y+nH+1)%nH][(x+nW+1)%nW])
					Image2DGrayOut[y][x] = 255;
				else
					Image2DGrayOut[y][x] = 0;
			}
			else
				Image2DGrayOut[y][x] = Temp[y][x];

		}

	free_dmatrix(Temp, nH, nW);
}

void ShoulderBinary2DGray(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut)
{
	int x, y, i;
	int His[GRAY_CNT] = {0};
	int Max, Min;
	int MaxPos, MinPos;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
			His[Image2DGray[y][x]]++;

	Max = 0;
	MaxPos = 0;
	for(i = 0 ; i < GRAY_CNT ; i++)
	{
		if(His[i] > Max)
		{
			Max = His[i];
			MaxPos = i;
		}
	}

	for(i = GRAY_CNT ; i >= 0 ; i--)
	{
		if(His[i] > 0)
		{
			Min = His[i];
			MinPos = i;
			break;
		}
	}

	if(MinPos == MaxPos) 
	{
		if(Image2DGrayOut == NULL)
		{
			for(y = 0 ; y < nH ; y++)
				for(x = 0 ; x < nW ; x++)
					Image2DGray[y][x] = 0;
		}
		else
		{
			for(y = 0 ; y < nH ; y++)
				for(x = 0 ; x < nW ; x++)
					Image2DGrayOut[y][x] = 0;
		}
		
		return;
	}

	int PrevMax;
	int Curr;
	int CurrPos;

	PrevMax = Min;

	double a, b, c;

	a = (Min - Max);
	b = (MaxPos - MinPos);
	c = (MinPos - MaxPos)*MaxPos - (Min - Max)*MinPos;

	int Thre;
	double MaxDist, Dist;

	Thre = MinPos;
	MaxDist = 0;
	for(i = MinPos ; i >= MaxPos ; i--)
	{
		Curr = His[i];
		CurrPos = i;

		if(i != MinPos)
		{
			if(His[i] < PrevMax)
				Curr = PrevMax;
		}
		
		Dist = fabs(a*CurrPos + b*Curr + c) / sqrt(a*a + b*b);

		if(Dist > MaxDist)
		{
			MaxDist = Dist;
			Thre = i;
		}

		if(His[i] > PrevMax)
			PrevMax = His[i];
	}

	if(Image2DGrayOut == NULL)
	{
		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				if(Image2DGray[y][x] > Thre)
					Image2DGray[y][x] = 255;
				else
					Image2DGray[y][x] = 0;
			}
	}
	else
	{
		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				if(Image2DGray[y][x] > Thre)
					Image2DGrayOut[y][x] = 255;
				else
					Image2DGrayOut[y][x] = 0;
			}
	}
}

void VectorMedianFilter(int **OrgX, int **OrgY, int **OutX, int **OutY, int nW, int nH)
{
	int x, y;
	int dx, dy, tx, ty;
	double Min;
	double Sum;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{
			OutX[y][x] = 0;
			OutY[y][x] = 0;
		}

	for(y = 1 ; y < nH-1 ; y++)
		for(x = 1 ; x < nW-1 ; x++)
		{
			Min = (nW+nH)*9;

			OutX[y][x] = OrgX[y][x];
			OutY[y][x] = OrgY[y][x];
	
//			if(x1[y][x]*x1[y][x]+x2[y][x]*x1[y][x] < 2) continue;//

			for(dy = -1; dy < 2 ; dy++)
				for(dx = -1 ; dx < 2; dx++)
				{
					Sum = 0.;
					int Cnt;

					Cnt = 0;
					for(ty = -1 ; ty < 2; ty++)
						for(tx = -1 ; tx < 2; tx++)
						{
//							if(x1[y+ty][x+tx]*x1[y+ty][x+tx]+x2[y+ty][x+tx]*x1[y+ty][x+tx] == 0) continue;//

							Sum += sqrt((double)((OrgX[y+dy][x+dx] - OrgX[y+ty][x+tx])*(OrgX[y+dy][x+dx] - OrgX[y+ty][x+tx]) + 
								(OrgY[y+dy][x+dx] - OrgY[y+ty][x+tx])*(OrgY[y+dy][x+dx] - OrgY[y+ty][x+tx])));

							Cnt++;
						}

					if(Cnt == 0) continue;

					Sum /= (double)Cnt;
					if(Sum < Min)
					{
						Min = Sum;
						OutX[y][x] = OrgX[y+dy][x+dx];
						OutY[y][x] = OrgY[y+dy][x+dx];
					}
				}

		}
}

void YUV2RGB(unsigned char *Y, unsigned char *U, unsigned char *V, unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int nType)
{
	if(nType == 0) // YUV420
	{
		int x, y;
		int posY = 0, posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				posUV = (nW/2)*(y/2) + (x/2);

				int b = (int)(1.164*(Y[posY] - 16)							+ 2.018*(U[posUV] - 128));
				int g = (int)(1.164*(Y[posY] - 16) - 0.813*(V[posUV] - 128) - 0.391*(U[posUV] - 128));
				int r = (int)(1.164*(Y[posY] - 16) + 1.596*(V[posUV] - 128));

				R[y][x] = r>255? 255 : r<0 ? 0 : r;
				G[y][x] = g>255? 255 : g<0 ? 0 : g;
				B[y][x] = b>255? 255 : b<0 ? 0 : b;

				posY++;
			}
	}
	else if(nType == 1) // YUV422
	{
		int x, y;
		int posY = 0, posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				posUV = (nW/2)*y + (x/2);

				int b = (int)(1.164*(Y[posY] - 16)							+ 2.018*(U[posUV] - 128));
				int g = (int)(1.164*(Y[posY] - 16) - 0.813*(V[posUV] - 128) - 0.391*(U[posUV] - 128));
				int r = (int)(1.164*(Y[posY] - 16) + 1.596*(V[posUV] - 128));

				R[y][x] = r>255? 255 : r<0 ? 0 : r;
				G[y][x] = g>255? 255 : g<0 ? 0 : g;
				B[y][x] = b>255? 255 : b<0 ? 0 : b;

				posY++;
			}
	}
	else if(nType == 2) // YUV444
	{
		int x, y;
		int posYUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				int b = (int)(1.164*(Y[posYUV] - 16)							+ 2.018*(U[posYUV] - 128));
				int g = (int)(1.164*(Y[posYUV] - 16) - 0.813*(V[posYUV] - 128) - 0.391*(U[posYUV] - 128));
				int r = (int)(1.164*(Y[posYUV] - 16) + 1.596*(V[posYUV] - 128));

				R[y][x] = r>255? 255 : r<0 ? 0 : r;
				G[y][x] = g>255? 255 : g<0 ? 0 : g;
				B[y][x] = b>255? 255 : b<0 ? 0 : b;

				posYUV++;
			}
	}
}

void RGB2YUV(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char *Y, unsigned char *U, unsigned char *V, int nW, int nH, int nType)
{
	if(nType == 0) // YUV420
	{
		int x, y;
		int posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				int yy =  (int)((0.257 * R[y][x]) + (0.504 * G[y][x]) + (0.098 * B[y][x]) + 16);				
				int uu = (int)(-(0.148 * R[y][x]) - (0.291 * G[y][x]) + (0.439 * B[y][x]) + 128);
				int vv = (int)((0.439 * R[y][x]) - (0.368 * G[y][x]) - (0.071 * B[y][x]) + 128);
					
				Y[y*nW+x] = yy>255? 255 : yy<0 ? 0 : yy;

				posUV = (nW/2)*(y/2) + (x/2);

				U[posUV] = uu>255? 255 : uu<0 ? 0 : uu; // Set the last U and V
				V[posUV] = vv>255? 255 : vv<0 ? 0 : vv;
			}
	}
	else if(nType == 1) // YUV422
	{
		int x, y;
		int posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				int yy = (int)((0.257 * R[y][x]) + (0.504 * G[y][x]) + (0.098 * B[y][x]) + 16);
				int uu = (int)(-(0.148 * R[y][x]) - (0.291 * G[y][x]) + (0.439 * B[y][x]) + 128);
				int vv = (int)((0.439 * R[y][x]) - (0.368 * G[y][x]) - (0.071 * B[y][x]) + 128);
				
				Y[y*nW+x] = yy>255? 255 : yy<0 ? 0 : yy;

				posUV = (nW/2)*y + (x/2);

				U[posUV] = uu>255? 255 : uu<0 ? 0 : uu; // Set the last U and V
				V[posUV] = vv>255? 255 : vv<0 ? 0 : vv;
			}
	}
	else if(nType == 2) // YUV444
	{
		int x, y;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				int yy = (int)((0.257 * R[y][x]) + (0.504 * G[y][x]) + (0.098 * B[y][x]) + 16);
				int uu = (int)(-(0.148 * R[y][x]) - (0.291 * G[y][x]) + (0.439 * B[y][x]) + 128);
				int vv = (int)((0.439 * R[y][x]) - (0.368 * G[y][x]) - (0.071 * B[y][x]) + 128);
				
				Y[y*nW+x] = yy>255? 255 : yy<0 ? 0 : yy;
				U[y*nW+x] = uu>255? 255 : uu<0 ? 0 : uu; // Set the last U and V
				V[y*nW+x] = vv>255? 255 : vv<0 ? 0 : vv;
			}
	}
}

void YUVtoBGR1D(unsigned char *Y, unsigned char *U, unsigned char *V, unsigned char *Bgr, int nW, int nH, int nType)
{
	if(nType == 0) // YUV420
	{
		int x, y;
		int posY = 0, posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				posUV = (nW/2)*(y/2) + (x/2);

				int b = (int)(1.164*(Y[posY] - 16)							+ 2.018*(U[posUV] - 128));
				int g = (int)(1.164*(Y[posY] - 16) - 0.813*(V[posUV] - 128) - 0.391*(U[posUV] - 128));
				int r = (int)(1.164*(Y[posY] - 16) + 1.596*(V[posUV] - 128));

				int pos = GetBmp24Pos(nW, nH, x, y);
				Bgr[pos] = b>255? 255 : b<0 ? 0 : b;
				Bgr[pos+1] = g>255? 255 : g<0 ? 0 : g;
				Bgr[pos+2] = r>255? 255 : r<0 ? 0 : r;
				
				posY++;
			}
	}
	else if(nType == 0) // YUV422
	{
		int x, y;
		int posY = 0, posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				posUV = (nW/2)*y + (x/2);

				int b = (int)(1.164*(Y[posY] - 16)							+ 2.018*(U[posUV] - 128));
				int g = (int)(1.164*(Y[posY] - 16) - 0.813*(V[posUV] - 128) - 0.391*(U[posUV] - 128));
				int r = (int)(1.164*(Y[posY] - 16) + 1.596*(V[posUV] - 128));

				int pos = GetBmp24Pos(nW, nH, x, y);
				Bgr[pos] = b>255? 255 : b<0 ? 0 : b;
				Bgr[pos+1] = g>255? 255 : g<0 ? 0 : g;
				Bgr[pos+2] = r>255? 255 : r<0 ? 0 : r;
				
				posY++;
			}
	}
}

void BGR1DtoYUV(unsigned char *Bgr, unsigned char *Y, unsigned char *U, unsigned char *V, int nW, int nH, int nType)
{
	if(nType == 0) // YUV420
	{
		int x, y;
		int posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				int pos = GetBmp24Pos(nW, nH, x, y);

				int yy = (int)( (0.257 * Bgr[pos+2]) + (0.504 * Bgr[pos+1]) + (0.098 * Bgr[pos]) + 16);				
				int uu = (int)(-(0.148 * Bgr[pos+2]) - (0.291 * Bgr[pos+1]) + (0.439 * Bgr[pos]) + 128);
				int vv = (int)( (0.439 * Bgr[pos+2]) - (0.368 * Bgr[pos+1]) - (0.071 * Bgr[pos]) + 128);

				Y[y*nW+x] = yy>255? 255 : yy<0 ? 0 : yy;

				posUV = (nW/2)*(y/2) + (x/2);

				U[posUV] = uu>255? 255 : uu<0 ? 0 : uu; // Set the last U and V
				V[posUV] = vv>255? 255 : vv<0 ? 0 : vv;
			}
	}
	else if(nType == 0) // YUV422
	{
		int x, y;
		int posUV = 0;

		for(y = 0 ; y < nH ; y++)
			for(x = 0 ; x < nW ; x++)
			{
				int pos = GetBmp24Pos(nW, nH, x, y);

				int yy = (int)( (0.257 * Bgr[pos+2]) + (0.504 * Bgr[pos+1]) + (0.098 * Bgr[pos]) + 16);				
				int uu = (int)(-(0.148 * Bgr[pos+2]) - (0.291 * Bgr[pos+1]) + (0.439 * Bgr[pos]) + 128);
				int vv = (int)( (0.439 * Bgr[pos+2]) - (0.368 * Bgr[pos+1]) - (0.071 * Bgr[pos]) + 128);

				Y[y*nW+x] = yy>255? 255 : yy<0 ? 0 : yy;

				posUV = (nW/2)*y + (x/2);

				U[posUV] = uu>255? 255 : uu<0 ? 0 : uu; // Set the last U and V
				V[posUV] = vv>255? 255 : vv<0 ? 0 : vv;
			}
	}
}

void Convert1DtoYUV(unsigned char *Image1D, int nW, int nH, unsigned char *YUV, int nType)
{
	if(nType == 0) // YUV420
	{
		BGR1DtoYUV(Image1D, YUV, YUV+nW*nH, YUV+nW*nH+(nW*nH)/4, nW, nH, 0);
	}
	else if(nType == 1) // YUV422
	{
		BGR1DtoYUV(Image1D, YUV, YUV+nW*nH, YUV+nW*nH+(nW*nH)/2, nW, nH, 1);
	}
}

void ConvertYUVto1D(unsigned char *YUV, int nW, int nH, unsigned char *Image1D, int nType)
{
	if(nType == 0) // YUV420
		YUVtoBGR1D(YUV, YUV+nW*nH, YUV+nW*nH + (nW*nH)/4, Image1D, nW, nH, 0);
	else if(nType == 0) // YUV422
		YUVtoBGR1D(YUV, YUV+nW*nH, YUV+nW*nH + (nW*nH)/2, Image1D, nW, nH, 0);
}

double lerp(double v1, double v2, double ratio)
{
   return v1*(1-ratio)+v2*ratio;
}

void BilinearInterpolaton(unsigned char **ImageGray, int nW, int nH, unsigned char **OutputGray, int nOW, int nOH)
{
	int x, y;
	
	for(y = 0 ; y < nOH ; y++)
		for(x = 0 ; x < nOW ; x++)
		{
			double u = x/((double)nOW/nW);
			double v = y/((double)nOH/nH);

			int u0, v0, u1, v1;
			double du, dv;

			u0 = (int)floor(u);
			v0 = (int)floor(v);

			v1 = v0+1;
			u1 = u0+1;

			if(u1 >= nW) u1 = nW-1;
			if(v1 >= nH) v1 = nH-1;
  
			du=u-u0;
			dv=v-v0;

			double r0, r1, r2, r3;

			r0 = ImageGray[v0][u0];
			r1 = ImageGray[v0][u1];
			r2 = ImageGray[v1][u1];
			r3 = ImageGray[v1][u0];

			OutputGray[y][x] = (unsigned char)(lerp (lerp (r0, r1, du), lerp (r3, r2, du), dv));
		}
}

void BilinearInterpolaton(unsigned char **ImageGray, int Left, int Top, int Right, int Bottom, unsigned char **OutputGray, int nOW, int nOH)
{
	int x, y;
	int nW = Right-Left+1;
	int nH = Bottom-Top+1;
	
	for(y = 0 ; y < nOH ; y++)
		for(x = 0 ; x < nOW ; x++)
		{
			double u = x/((double)nOW/nW);
			double v = y/((double)nOH/nH);

			int u0, v0, u1, v1;
			double du, dv;

			u0 = (int)floor(u);
			v0 = (int)floor(v);

			v1 = v0+1;
			u1 = u0+1;

			if(u1 >= nW) u1 = nW-1;
			if(v1 >= nH) v1 = nH-1;
  
			du=u-u0;
			dv=v-v0;

			double r0, r1, r2, r3;

			r0 = ImageGray[Top+v0][Left+u0];
			r1 = ImageGray[Top+v0][Left+u1];
			r2 = ImageGray[Top+v1][Left+u1];
			r3 = ImageGray[Top+v1][Left+u0];

			OutputGray[y][x] = (unsigned char)(lerp (lerp (r0, r1, du), lerp (r3, r2, du), dv));
		}
}

double Cubic(double offset, double v0, double v1, double v2, double v3)
{
	
	return   (((( -7 * v0 + 21 * v1 - 21 * v2 + 7 * v3 ) * offset +
		( 15 * v0 - 36 * v1 + 27 * v2 - 6 * v3 ) ) * offset +
		( -9 * v0 + 9 * v2 ) ) * offset + (v0 + 16 * v1 + v2) ) / 18.0;
}

double GetCubicRow(int x, int y, double offset, unsigned char **ImageGray, int nW, int nH)
{
	double i0, i1, i2, i3;
	int x1, x2, x3;
	
	if(y < 0) y = 0;
	if(y >= nH) y = nH-1;
	
	if(x < 0) x = 0;
	x1 = x+1;
	if(x1 >= nW) x1 = x;
	x2 = x1+1;
	if(x2 >= nW) x2 = x1;
	x3 = x2+1;
	if(x3 >= nW) x3 = x2;
	
	i0 = ImageGray[y][x];
	i1 = ImageGray[y][x1];
	i2 = ImageGray[y][x2];
	i3 = ImageGray[y][x3];
	
	return Cubic(offset, i0, i1, i2, i3);
}

double GetBicubic(double x, double y, unsigned char **ImageGray, int nW, int nH)
{
	int xi,yi;
	double dx,dy;
	double i0, i1, i2, i3;
	
	xi = (int)floor(x);
	yi = (int)floor(y);
	dx=x-xi;
	dy=y-yi;
	
	i0 = GetCubicRow(xi-1, yi-1, dx, ImageGray, nW, nH);
	i1 = GetCubicRow(xi-1, yi,   dx, ImageGray, nW, nH);
	i2 = GetCubicRow(xi-1, yi+1, dx, ImageGray, nW, nH);
	i3 = GetCubicRow(xi-1, yi+2, dx, ImageGray, nW, nH);
	
	return Cubic(dy, i0, i1, i2, i3);
}
void BicublicInterpolation(unsigned char **ImageGray, int nW, int nH, unsigned char **OutputGray, int nOW, int nOH)
{ 
	int x, y;
	for(y = 0 ; y < nOH ; y++)
		for(x = 0 ; x < nOW ; x++)
		{
			double u = x*(double)nW/nOW;
			double v = y*(double)nH/nOH;
			
			double out = GetBicubic(u, v, ImageGray, nW, nH);
			if(out < 0)
				OutputGray[y][x] = 0;
			else if(out > 255)
				OutputGray[y][x] = 255;
			else
				OutputGray[y][x] = (unsigned char)out;
		}
}

double GetPsnrRgb(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int nW, int nH)
{
	int x, y;
	double Mse = 0.;
	double Mse1 = 0, Mse2 = 0, Mse3 = 0;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{	
//			Mse += (R[y][x] - R2[y][x])*(R[y][x] - R2[y][x]) +
//				(G[y][x] - G2[y][x])*(G[y][x] - G2[y][x]) +
//				(B[y][x] - B2[y][x])*(B[y][x] - B2[y][x]);

			Mse1 += (R[y][x] - R2[y][x])*(R[y][x] - R2[y][x]);
			Mse2 += (G[y][x] - G2[y][x])*(G[y][x] - G2[y][x]);
			Mse3 += (B[y][x] - B2[y][x])*(B[y][x] - B2[y][x]);

		}

	Mse1 /= nW*nH;
	Mse2 /= nW*nH;
	Mse3 /= nW*nH;

	Mse = (Mse1+Mse2+Mse3)/3;

	if(Mse == 0) return 100;

	return 10*log10(255*255/Mse);
}

double GetPsnrYuv(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int nW, int nH)
{
	int x, y;
	double Mse = 0.;
	double Mse1 = 0, Mse2 = 0, Mse3 = 0;

	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{	
			double y1 = 0.257*R[y][x]	+ 0.504*G[y][x] + 0.098*B[y][x] + 16;
			double v1 = 0.439*R[y][x]  - 0.368*G[y][x] - 0.071*B[y][x] + 128;
			double u1 = -0.148*R[y][x] - 0.291*G[y][x] + 0.439*B[y][x] + 128;

			y1 = y1>255? 255 : y1<0 ? 0 : y1;
			v1 = v1>255? 255 : v1<0 ? 0 : v1;
			u1 = u1>255? 255 : u1<0 ? 0 : u1;

			double y2 = 0.257*R2[y][x]	+ 0.504*G2[y][x] + 0.098*B2[y][x] + 16;
			double v2 = 0.439*R2[y][x] - 0.368*G2[y][x] - 0.071*B2[y][x] + 128;
			double u2 = -0.148*R2[y][x] - 0.291*G2[y][x] + 0.439*B2[y][x] + 128;

			y2 = y2>255? 255 : y2<0 ? 0 : y2;
			v2 = v2>255? 255 : v2<0 ? 0 : v2;
			u2 = u2>255? 255 : u2<0 ? 0 : u2;

			Mse1 += (y1-y2)*(y1-y2);
			Mse2 += (u1-u2)*(u1-u2);
			Mse3 += (v1-v2)*(v1-v2);
		}

	Mse1 /= nW*nH;
	Mse2 /= nW*nH;
	Mse3 /= nW*nH;

	Mse = (Mse1+Mse2+Mse3)/3;

	if(Mse == 0) return 100;

	return 10*log10(255*255/Mse);
}

double GetPsnrY(unsigned char **R, unsigned char **G, unsigned char **B, unsigned char **R2, unsigned char **G2, unsigned char **B2, int nW, int nH)
{
	int x, y;
	double Mse = 0.;
	
	for(y = 0 ; y < nH ; y++)
		for(x = 0 ; x < nW ; x++)
		{	
		//	double y1 = 0.257*R[y][x] + 0.504*G[y][x] + 0.098*B[y][x] +16;
		//	double y2 = 0.257*R2[y][x] + 0.504*G2[y][x] + 0.098*B2[y][x] +16;

			double y1 = 0.299*R[y][x] + 0.587*G[y][x] + 0.114*B[y][x];
			double y2 = 0.299*R2[y][x] + 0.587*G2[y][x] + 0.114*B2[y][x];

			Mse += (y1-y2)*(y1-y2);
		}


	Mse /= nW*nH;
	
	if(Mse == 0) return 100;

	return 10*log10(255*255/Mse);
}

void LabelMoment(int **Label, int LabelCnt, int nW, int nH, double *m2, double *m3)
{
	double *mu00, *mu20, *mu02, *mu11, *mu30, *mu03, *mu21, *mu12, r;
	CIpPoint *Center;
	int *Cnt;
	int x, y, i;

	Center = new CIpPoint[LabelCnt];
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
			i = Label[y][x];

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
			i = Label[y][x];

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
}

// LU decomposition
#define TINY 1.0e-20 
bool ludcmp(double **a, int nN, int *indx, double *d)
{
	int i, imax, j, k;
	double big, dum, sum, temp;
	double *vv = new double[nN];

	*d = 1.0;
	for (i = 0; i < nN; i++)
	{
		big = 0.0;
		for (j = 0; j < nN; j++)
		{
			if ((temp = fabs(a[i][j])) > big)
				big = temp;
		}

		if (big == 0.0)
		{
			delete[] vv;
			return false; // Singular
		}

		vv[i] = 1.0 / big;
	}

	for (j = 0; j < nN; j++)
	{
		for (i = 0; i < j; i++)
		{
			sum = a[i][j];
			for (k = 0; k < i; k++)
				sum -= a[i][k] * a[k][j];

			a[i][j] = sum;
		}

		big = 0.0;
		for (i = j; i < nN; i++)
		{
			sum = a[i][j];
			for (k = 0; k < j; k++)
				sum -= a[i][k] * a[k][j];

			a[i][j] = sum;
			if ((dum = vv[i] * fabs(sum)) >= big)
			{
				big = dum;
				imax = i;
			}
		}

		if (j != imax)
		{
			for (k = 0; k < nN; k++)
			{
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}

		indx[j] = imax;
		if (a[j][j] == 0.0) a[j][j] = TINY;

		if (j != nN - 1)
		{
			dum = 1.0 / (a[j][j]);
			for (i = j + 1; i < nN; i++)
				a[i][j] *= dum;
		}
	}
	delete[] vv;

	return true;
}

void lubksb(double **a, int nN, int *indx, double *b)
{
	int i, ii = -1, ip, j;
	double sum;

	for (i = 0; i < nN; i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];

		if (ii >= 0)
		{
			for (j = ii; j <= i - 1; j++)
			{
				sum -= a[i][j] * b[j];
			}
		}
		else if (sum)
			ii = i;

		b[i] = sum;
	}
	for (i = nN - 1; i >= 0; i--)
	{
		sum = b[i];
		for (j = i + 1; j < nN; j++)
			sum -= a[i][j] * b[j];
		b[i] = sum / a[i][i];
	}
}

bool InverseMatrix2(double  **a, double **y, int nN)
{
	double **CopyA = dmatrix(nN, nN);

	double *col = new double[nN];
	int *indx = new int[nN];
	double d;

	int r, c;
	for (r = 0; r < nN; r++)
		for (c = 0; c < nN; c++)
			CopyA[r][c] = a[r][c];

	if (!ludcmp(CopyA, nN, indx, &d))
	{
		free_dmatrix(CopyA, nN, nN);

		delete[] indx;
		delete[] col;

		return false;
	}

	int i, j;
	for (j = 0; j < nN; j++) {
		for (i = 0; i < nN; i++)
			col[i] = 0.0;
		col[j] = 1.0;

		lubksb(CopyA, nN, indx, col);

		for (i = 0; i < nN; i++)
			y[i][j] = col[i];
	}

	free_dmatrix(CopyA, nN, nN);

	delete[] indx;
	delete[] col;

	return true;
}
