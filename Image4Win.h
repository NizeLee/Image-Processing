//	Image4Win.h: Image processing for Windows (Header)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr

#ifndef IMAGE_FOR_WINDOWS_H_SUBROUTINE_STRUCTURE_CONSTANT
#define IMAGE_FOR_WINDOWS_H_SUBROUTINE_STRUCTURE_CONSTANT

unsigned char *ReadJpeg(const char *FileName, int *pW, int *pH);
//unsigned char *ReadJpeg(const wchar_t *FileName, int *pW, int *pH);
unsigned char *ReadJpegFromMemory(unsigned char *jpg_buffer, int jpg_size, int *pW, int *pH);
bool SaveJpeg(const char *FileName, unsigned char *Image1D, int nW, int nH, bool bColor, int nQuality);
//bool SaveJpeg(const wchar_t *FileName, unsigned char *Image1D, int nW, int nH, bool bColor, int nQuality);
bool SaveImage2DColorJpeg(char *FileName, unsigned char **ImageRed, unsigned char **ImageGreen, unsigned char **ImageBlue, 
	int nW, int nH, bool bColor, int nQuality);

void DrawTextOnImage2DGray(unsigned char **Image2DGray, int nW, int nH, int x, int y, CString Msg, UINT nFormat, unsigned char Color, bool bBig);
void DrawTextOnImage2DColor(unsigned char **R, unsigned char **G, unsigned char **B, int nW, int nH, int x, int y, CString Msg, UINT nFormat, Color24 Color, bool bBig);
void DrawTextOnImage1D(unsigned char *Image1D, int nW, int nH, int x, int y, CString Msg, UINT nFormat, Color24 Color, bool bBig);

void SetViewMsg(CString ViewMsg, bool bErase = true);

// 마지막 영상의 폭, 높이, 위치 읽기
bool GetCurrentImageInfo(int *pW, int *pH, int *pPosX, int *pPosY, int nIndex = 0);
// 마지막 영상을 2차원 회색조 정보로 읽기
bool GetCurrentImageGray(unsigned char  **ImageGray, int nIndex = 0);
// 2차원 회색조 영상을 출력
bool DisplayCimage2D(unsigned char  **ImageGray, int nW, int nH, int nPosX, int nPosY, bool bErase = true, bool bDelete = false, int Rate = 100);
// 마지막 영상을 2차원 컬러 정보로 읽기
bool GetCurrentImageColor(unsigned char  **ImageRed, unsigned char  **ImageGreen, unsigned char  **ImageBlue, int nIndex = 0);
bool GetCurrentImage1DColor(unsigned char  *Image1D, int nIndex = 0);
bool DisplayCimage1D(unsigned char  *Image1D, int nW, int nH, int nPosX, int nPosY, bool bErase = true, bool bDelete = false, int Rate = 100);
// 2차원 컬러 영상을 출력
bool DisplayCimage2DColor(unsigned char  **ImageRed, unsigned char  **ImageGreen, unsigned char  **ImageBlue, int nW, int nH, 
	int nPosX, int nPosY, bool bErase = true, bool bDelete = false, int Rate = 100);

bool DisplayIimage2D(int **ImageInt, int nW, int nH, int nPosX, int nPosY, bool bErase = true, bool bDelete = false);
bool DisplayDimage2D(double **ImageDouble, int nW, int nH, int nPosX, int nPosY, bool bErase = true, bool bDelete = false);

void Compression(unsigned char  **ImageGray, unsigned char  **OutputGray, int nW, int nH);
void Filtering(unsigned char  **ImageGray, double **OutputDouble, int nW, int nH, int nPosX, int nPosY, 
	int nFilterType1, int nFilterType2, int nDC, int nN);
void ImageOperation(int nOperationType);

#endif