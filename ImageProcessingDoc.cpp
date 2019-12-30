// ImageProcessingDoc.cpp : implementation of the CImageProcessingDoc class
//

#include "stdafx.h"
#include "ImageProcessing.h"

#include "ImageProcessingDoc.h"

#include "LodePng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingDoc

IMPLEMENT_DYNCREATE(CImageProcessingDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessingDoc, CDocument)
	//{{AFX_MSG_MAP(CImageProcessingDoc)
	ON_COMMAND(ID_FILE_OPEN_CUR, OnFileOpenCur)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingDoc construction/destruction



CImageProcessingDoc::CImageProcessingDoc()
{
	m_nImageCnt = 0;
}

CImageProcessingDoc::~CImageProcessingDoc()
{
	for(int i = 0 ; i < m_nImageCnt ; i++)
		delete [] m_Image[i].Image1D;
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CImageProcessingDoc serialization

void CImageProcessingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingDoc diagnostics

#ifdef _DEBUG
void CImageProcessingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageProcessingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageProcessingDoc commands

BOOL CImageProcessingDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if(m_nImageCnt >= MAX_IMAGE) return FALSE;

	CString FileName = lpszPathName;
	FileName.MakeUpper();

	char mbcsFileName[MAX_PATH];
	int len = WideCharToMultiByte(CP_ACP, 0, lpszPathName, -1, NULL, NULL, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, lpszPathName, -1, mbcsFileName, len, NULL, NULL);

	if(FileName.Right(4) == _T(".BMP"))
	{
		m_Image[m_nImageCnt].Image1D = ReadBmp(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return FALSE;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".PNG"))
	{
		m_Image[m_nImageCnt].Image1D = ReadPng(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return FALSE;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".PPM"))
	{
		m_Image[m_nImageCnt].Image1D = ReadPpm(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return FALSE;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".256"))
	{
		m_Image[m_nImageCnt].nW = 256;
		m_Image[m_nImageCnt].nH = 256;

		m_Image[m_nImageCnt].Image1D = ReadRaw(mbcsFileName, m_Image[m_nImageCnt].nW, m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return FALSE;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".320"))
	{
		m_Image[m_nImageCnt].nW = 320;
		m_Image[m_nImageCnt].nH = 240;

		m_Image[m_nImageCnt].Image1D = ReadRaw(mbcsFileName, m_Image[m_nImageCnt].nW, m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return FALSE;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".PCX"))
	{
		m_Image[m_nImageCnt].Image1D = ReadPcx(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return FALSE;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".JPG"))
	{
		m_Image[m_nImageCnt].Image1D = ReadJpeg(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return FALSE;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else
		return FALSE;
	
	return TRUE;
}

BOOL CImageProcessingDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if(m_nImageCnt <= 0) return FALSE;

	CString FileName = lpszPathName;
	FileName.MakeUpper();

	if(FileName.Right(4) == _T(".BMP"))
	{
		char mbcsFileName[MAX_PATH];
		int len = WideCharToMultiByte(CP_ACP, 0, lpszPathName, -1, NULL, NULL, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, lpszPathName, -1, mbcsFileName, len, NULL, NULL);

		return SaveBmp(mbcsFileName, m_Image[m_nImageCnt-1].Image1D, m_Image[m_nImageCnt-1].nW, m_Image[m_nImageCnt-1].nH);
	}
	else if(FileName.Right(4) == _T(".JPG"))
	{
		char mbcsFileName[MAX_PATH];
		int len = WideCharToMultiByte(CP_ACP, 0, lpszPathName, -1, NULL, NULL, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, lpszPathName, -1, mbcsFileName, len, NULL, NULL);

		return SaveJpeg(mbcsFileName, m_Image[m_nImageCnt-1].Image1D, m_Image[m_nImageCnt-1].nW, m_Image[m_nImageCnt-1].nH,
			true, 75);
	}
	
	return FALSE;
//	return CDocument::OnSaveDocument(lpszPathName);
}

void CImageProcessingDoc::OnFileOpenCur() 
{
	CFileDialog dlg(TRUE, NULL, NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("Image Files(*.bmp, *.ppm, *.pcx, *.jpg, *.png, *.320, *256)|\
*.bmp;*.ppm;*.pcx;*.jpg;*.png;*.320;*.256|\
BMP Files(*.bmp)|*.bmp|PPM Files(*.ppm)|*.ppm|PCX Files(*.pcx)|*.pcx|\
JPEG Files(*.jpg)|*.jpg|PNG Files(*.png)|*.png|RAW Files(*.320, *.256)|\
*.320;*256|All Files(*.*)|*.*||"));
	
	if(dlg.DoModal() != IDOK) return;

	FileOpenCur(dlg.GetPathName());
}

void CImageProcessingDoc::FileOpenCur(CString FilePathName) 
{
	if(m_nImageCnt >= MAX_IMAGE) return;

	CString FileName = FilePathName;
	FileName.MakeUpper();

	char mbcsFileName[MAX_PATH];
	int len = WideCharToMultiByte(CP_ACP, 0, FileName, -1, NULL, NULL, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, FileName, -1, mbcsFileName, len, NULL, NULL);

	if(FileName.Right(4) == _T(".BMP"))
	{
		m_Image[m_nImageCnt].Image1D = ReadBmp(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".PNG"))
	{
		m_Image[m_nImageCnt].Image1D = ReadPng(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".PPM"))
	{
		m_Image[m_nImageCnt].Image1D = ReadPpm(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".256"))
	{
		m_Image[m_nImageCnt].nW = 256;
		m_Image[m_nImageCnt].nH = 256;

		m_Image[m_nImageCnt].Image1D = ReadRaw(mbcsFileName, m_Image[m_nImageCnt].nW, m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".320"))
	{
		m_Image[m_nImageCnt].nW = 320;
		m_Image[m_nImageCnt].nH = 240;

		m_Image[m_nImageCnt].Image1D = ReadRaw(mbcsFileName, m_Image[m_nImageCnt].nW, m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".PCX"))
	{
		m_Image[m_nImageCnt].Image1D = ReadPcx(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else if(FileName.Right(4) == _T(".JPG"))
	{
		m_Image[m_nImageCnt].Image1D = ReadJpeg(mbcsFileName, &m_Image[m_nImageCnt].nW, &m_Image[m_nImageCnt].nH);

		if(m_Image[m_nImageCnt].Image1D == NULL) return;

		m_Image[m_nImageCnt].nPosX = 0;
		m_Image[m_nImageCnt].nPosY = 0;

		m_nImageCnt++;
	}
	else 
		return;

	UpdateAllViews (NULL);	
}
