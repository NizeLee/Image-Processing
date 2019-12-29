// MainProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "MainProcessDlg.h"
#include "Image.h"
#include  "Image4Win.h"

#include "LodePng.h"

#include "MainFrm.h"		// 추가
#include "ChildFrm.h"		// 추가
#include "ImageProcessingDoc.h"		// 추가
#include "ImageProcessingView.h"	// 추가

#include "YuvOptionDlg.h"
#include "SaveScreenOpt.h"

#include <afxtempl.h>

#include <io.h>
#include <windowsx.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CNewScreenImage : public CImage
{
public:
    unsigned char *CaptureRect(const CRect& rect) throw();
};

unsigned char * CNewScreenImage::CaptureRect(const CRect& rect)
{
   // detach and destroy the old bitmap if any attached
   CImage::Destroy();
   
   // create a screen and a memory device context
   HDC hDCScreen = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
   HDC hDCMem = ::CreateCompatibleDC(hDCScreen);
   // create a compatible bitmap and select it in the memory DC
   HBITMAP hBitmap = 
      ::CreateCompatibleBitmap(hDCScreen, rect.Width(), rect.Height());
   HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCMem, hBitmap);
 
   // bit-blit from screen to memory device context
   // note: CAPTUREBLT flag is required to capture layered windows
   DWORD dwRop = SRCCOPY | CAPTUREBLT;
   BOOL bRet = ::BitBlt(hDCMem, 0, 0, rect.Width(), rect.Height(), 
                        hDCScreen, rect.left, rect.top, dwRop);
   // attach bitmap handle to this object

   if(!bRet) return NULL;

   Attach(hBitmap);

   	BITMAPINFO bi;
	unsigned char *img1D = new unsigned char[GetBmp24Size(rect.Width()+1, rect.Height()+1)];


	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = rect.Width()+1;
	bi.bmiHeader.biHeight = rect.Height()+1;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = ((rect.Width()+1)*3+3)/4*4  * (rect.Height()+1);
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	GetDIBits(hDCMem, hBitmap, 0, rect.Height()+1, img1D, &bi,
                   DIB_RGB_COLORS);

//	char FileName[MAX_PATH];
//	sprintf_s(FileName, "%s\\Team.bmp", ExeFolderPathMBCS);

//	SaveBmp(FileName, img1D, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight);

//	sprintf_s(FileName, "%s\\Team.bmp", ExeFolderPathMBCS);

 
   // restore the memory DC and perform cleanup
   ::SelectObject(hDCMem, hBmpOld);
   ::DeleteDC(hDCMem);
   ::DeleteDC(hDCScreen);

   return img1D;
}


/////////////////////////////////////////////////////////////////////////////
// CMainProcessDlg dialog


CMainProcessDlg::CMainProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainProcessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainProcessDlg)
	m_nStart = 0;
	m_nEnd = 0;
	m_nGW = 320;
	m_nGH = 240;
	m_nFileType = 0;
	m_nGap = 30;
	m_nSleep = 0;
	m_nSourceType = 0;
	m_bSubFolder = FALSE;
	m_bStep = FALSE;
	m_nSeqType = 10;
	m_bSaveScreen = FALSE;
	m_nPtCntOfCurrRegoin = 4;
	//}}AFX_DATA_INIT

	m_bSequenceTimerRun = false;
	m_bCamTimerRun = false;
	m_bAVITimerRun = false;
	m_bYUVTimerRun = false;
	m_bIpCamWebRun = false;
	m_bEmptyTimerRun = false;

	m_bTimerPause = false;

	m_nMW = 2000;
	m_nMH = 2000;

	AVIFileInit();

	m_psForSave = NULL;
	m_psCompressedForSave = NULL;
	m_pfileForSave = NULL;

	m_bConnected = false;
}

void CMainProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainProcessDlg)
	DDX_Control(pDX, IDC_SEQ_TYPE, m_SeqTypeCtrl);
	DDX_Control(pDX, IDC_SOURCE_TYPE0, m_SourceType0Ctrl);
	DDX_Control(pDX, IDC_SOURCE_TYPE1, m_SourceType1Ctrl);
	DDX_Control(pDX, IDC_SOURCE_TYPE2, m_SourceType2Ctrl);
	DDX_Control(pDX, IDC_GW, m_GwCtrl);
	DDX_Control(pDX, IDC_GH, m_GhCtrl);
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	DDX_Control(pDX, IDC_SLEEP, m_SleepCtrl);
	DDX_Control(pDX, IDC_FILE_LIST, m_FileListCtrl);
	DDX_Text(pDX, IDC_START, m_nStart);
	DDX_Text(pDX, IDC_END, m_nEnd);
	DDX_Text(pDX, IDC_GW, m_nGW);
	DDX_Text(pDX, IDC_GH, m_nGH);
	DDX_Radio(pDX, IDC_TYPE, m_nFileType);
	DDX_Text(pDX, IDC_GAP, m_nGap);
	DDX_Slider(pDX, IDC_SLEEP, m_nSleep);
	DDX_Radio(pDX, IDC_SOURCE_TYPE0, m_nSourceType);
	DDX_Check(pDX, IDC_SUB_FOLDER, m_bSubFolder);
	DDX_Check(pDX, IDC_STEP, m_bStep);
	DDX_CBIndex(pDX, IDC_SEQ_TYPE, m_nSeqType);
	DDX_Check(pDX, IDC_SAVE_SCREEN, m_bSaveScreen);
	DDX_Text(pDX, IDC_PT_CNT, m_nPtCntOfCurrRegoin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CMainProcessDlg)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, OnDblclkFileList)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_SEL_FOLDER2, OnSelFolder2)
	ON_BN_CLICKED(IDC_GRAB, OnGrab)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_VS, OnVs)
	ON_BN_CLICKED(IDC_CAM_ON, OnCamOn)
	ON_BN_CLICKED(IDC_RESET_G_NUM, OnResetGNum)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLEEP, OnCustomdrawSleep)
	ON_BN_CLICKED(IDC_SEL_SOURCE, OnSelSource)
	ON_BN_CLICKED(IDC_RUN, OnRun)
	ON_BN_CLICKED(IDC_SAVE_SCREEN, OnSaveScreen)
	ON_BN_CLICKED(IDC_DRAW_RECT_REGION, OnDrawRectRegion)
	ON_BN_CLICKED(IDC_DRAW_REGION, OnDrawRegion)
	ON_BN_CLICKED(IDC_SAVE_REGION, OnSaveRegion)
	ON_BN_CLICKED(IDC_READ_REGION, OnReadRegion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainProcessDlg message handlers

BOOL CMainProcessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LV_COLUMN lvColumn;
	SHFILEINFO shInfo;
	CRect FileListRect;
	HIMAGELIST hImageList=(HIMAGELIST)SHGetFileInfo(_T("C:\\"),0,&shInfo,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
	m_ImageList.Attach(hImageList);

	m_FileListCtrl.GetClientRect(&FileListRect);

	TCHAR *list = _T("File name");
	int nWidth;
	nWidth = FileListRect.Width()-55;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.pszText = list;
	lvColumn.iSubItem = 0;
	lvColumn.cx = nWidth;
	m_FileListCtrl.InsertColumn(0, &lvColumn);
	m_FileListCtrl.InsertColumn(1, _T("Path"), LVCFMT_LEFT, 200, 0);

	long lSetStyle;
	lSetStyle = GetWindowLong(m_FileListCtrl.m_hWnd, GWL_STYLE);
	lSetStyle &= ~LVS_TYPEMASK;
	lSetStyle |= LVS_REPORT;
	SetWindowLong(m_FileListCtrl.m_hWnd, GWL_STYLE, lSetStyle);

	m_FileListCtrl.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_FileListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_FileListCtrl.SetBkColor(RGB(255,255,255));
	m_FileListCtrl.SetTextBkColor(RGB(255,255,200));
	m_FileListCtrl.SetTextColor(RGB(100,20,20));


	m_CurrentImageRed = cmatrix(m_nMH, m_nMW);
	m_CurrentImageGreen = cmatrix(m_nMH, m_nMW);
	m_CurrentImageBlue = cmatrix(m_nMH, m_nMW);

	m_SleepCtrl.SetRange(0, 1000);

	m_SeqTypeCtrl.InsertString(0, _T("*.*"));
	m_SeqTypeCtrl.InsertString(1, _T("BMP"));
	m_SeqTypeCtrl.InsertString(2, _T("JPG"));
	m_SeqTypeCtrl.InsertString(3, _T("PCX"));
	m_SeqTypeCtrl.InsertString(4, _T("256"));
	m_SeqTypeCtrl.InsertString(5, _T("320"));
	m_SeqTypeCtrl.InsertString(6, _T("640"));
	m_SeqTypeCtrl.InsertString(7, _T("BMP+JPG"));
	m_SeqTypeCtrl.InsertString(8, _T("PPM"));
	m_SeqTypeCtrl.InsertString(9, _T("PNG"));
	m_SeqTypeCtrl.InsertString(10, _T("Images"));

	m_SeqTypeCtrl.SetCurSel(10);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMainProcessDlg::OnTimer(UINT_PTR nIDEvent) 
{
	UpdateData(TRUE);

	if(nIDEvent == TID_SCREEN_CAPTURE && m_nTimerAviSaveNum >= 1)
	{
//		m_ScreenImage.CaptureRect(m_nSaveAviRect);

		m_nSaveAviW = m_nSaveAviRect.Width();
		m_nSaveAviH = m_nSaveAviRect.Height();

		BYTE *SaveAviFrameImage = new BYTE [GetBmp24Size(m_nSaveAviW, m_nSaveAviH)];

		HDC hDCScreen = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		HDC hDCMem = ::CreateCompatibleDC(hDCScreen);
		// create a compatible bitmap and select it in the memory DC
		HBITMAP hBitmap = 
			::CreateCompatibleBitmap(hDCScreen, m_nSaveAviRect.Width(), m_nSaveAviRect.Height());
		HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCMem, hBitmap);

		// bit-blit from screen to memory device context
		// note: CAPTUREBLT flag is required to capture layered windows
		DWORD dwRop = SRCCOPY | CAPTUREBLT;
		BOOL bRet = ::BitBlt(hDCMem, 0, 0, m_nSaveAviRect.Width(), m_nSaveAviRect.Height(), 
							hDCScreen, 
							m_nSaveAviRect.left, m_nSaveAviRect.top, dwRop);
		// attach bitmap handle to this object
		//Attach(hBitmap);

		BITMAPINFO bi;

		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
		bi.bmiHeader.biWidth = m_nSaveAviRect.Width();
		bi.bmiHeader.biHeight = m_nSaveAviRect.Height();
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 24;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biSizeImage = (m_nSaveAviRect.Width()*3+3)/4*4  * m_nSaveAviRect.Height();
		bi.bmiHeader.biClrUsed = 0;
		bi.bmiHeader.biClrImportant = 0;

		GetDIBits(hDCMem, hBitmap, 0, m_nSaveAviRect.Height(), SaveAviFrameImage, &bi,
                   DIB_RGB_COLORS);

		// restore the memory DC and perform cleanup
		::SelectObject(hDCMem, hBmpOld);
		::DeleteDC(hDCMem);
		::DeleteDC(hDCScreen);

		//m_ScreenImage.CaptureRect(m_nSaveAviRect);

		AVIStreamWrite(m_psCompressedForSave, m_nTimerAviSaveNum-1, 1, 
			SaveAviFrameImage, GetBmp24Size(m_nSaveAviW, m_nSaveAviH),
			AVIIF_KEYFRAME, NULL, NULL);

		m_nTimerAviSaveNum++;

		delete [] SaveAviFrameImage;
	}
	else
	{
		while(true)
		{
			if(m_bTimerPause) break;

			if(nIDEvent == TID_IP_CAM_WEB)
				;
			else if(nIDEvent == TID_YUV_RUN)
			{
				Sleep(m_nSleep);

				if(m_nCurrentProcessingNum > m_nEnd) 
				{
					KillTimer(TID_YUV_RUN);

					SetDlgItemText(IDC_RUN, _T("Run"));

					m_SourceType0Ctrl.EnableWindow(TRUE);
					m_SourceType1Ctrl.EnableWindow(TRUE);
					m_SourceType2Ctrl.EnableWindow(TRUE);

					m_bYUVTimerRun = false;

					m_YuvFile.Close();

					break;
				}

				int nW = m_nYuvW, nH = m_nYuvH;
			
				BYTE *ReadY = new BYTE [nW*nH];
				BYTE *ReadU = new BYTE [nW*nH];///4];
				BYTE *ReadV = new BYTE [nW*nH];///4];

				BYTE T = 0;

				if(m_bY4m)
				{
					while(T !=  0x0A)
					{
						m_YuvFile.Read(&T, 1);
					}
				}

				m_YuvFile.Read(ReadY, nW*nH);

				if(m_nYuvType == 0)
				{
					m_YuvFile.Read(ReadU, nW*nH/4);
					m_YuvFile.Read(ReadV, nW*nH/4);
				}
				else if(m_nYuvType == 1)
				{
					m_YuvFile.Read(ReadU, nW*nH/2);
					m_YuvFile.Read(ReadV, nW*nH/2);
				}
				else if(m_nYuvType == 2)
				{
					m_YuvFile.Read(ReadU, nW*nH);
					m_YuvFile.Read(ReadV, nW*nH);
				}

				BYTE **R, **G, **B;

				R = cmatrix(nH, nW);
				G = cmatrix(nH, nW);
				B = cmatrix(nH, nW);

				YUV2RGB(ReadY, ReadU, ReadV, R, G, B, nW, nH, m_nYuvType);

				DisplayCimage2DColor(R, G, B, nW, nH, 0, 0, false, true);
	
				CString DispFrameNum;
				DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nYuvLength-1);
				SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

				m_nCurrentProcessingNum++;

				free_cmatrix(R, nH, nW);
				free_cmatrix(G, nH, nW);
				free_cmatrix(B, nH, nW);

				delete [] ReadY;
				delete [] ReadU;
				delete [] ReadV;
	
				m_bFirstFrame = false;

				if(m_YuvFile.GetPosition() == m_YuvFile.GetLength())
				{
					m_bYUVTimerRun = false;
					SetDlgItemText(IDC_RUN, _T("Run"));

					m_SourceType0Ctrl.EnableWindow(TRUE);
					m_SourceType1Ctrl.EnableWindow(TRUE);
					m_SourceType2Ctrl.EnableWindow(TRUE);

					KillTimer(TID_YUV_RUN);

					m_YuvFile.Close();
				}
			}
			else if(nIDEvent == TID_AVI_RUN)
			{
				Sleep(m_nSleep);
				
				LPBITMAPINFOHEADER pbmih;
				pbmih = (LPBITMAPINFOHEADER) AVIStreamGetFrame(m_pfrm, m_nCurrentProcessingNum);


				BYTE *image = (unsigned char *) ((LPSTR)pbmih + (WORD)pbmih->biSize); 

				bool bExit = false;
				BYTE *Image1D = NULL;

				if(pbmih->biBitCount == 24 || pbmih->biBitCount == 32 || pbmih->biBitCount == 16) 
				{
					Image1D = new BYTE[GetBmp24Size(pbmih->biWidth, pbmih->biHeight)];

					if(pbmih->biBitCount == 24)
						memcpy(Image1D, image, GetBmp24Size(pbmih->biWidth, pbmih->biHeight));
					else if(pbmih->biBitCount == 32)
					{
						int x, y, offset1, offset2;
						for (y = 0; y < pbmih->biHeight ; y++)
							for (x = 0; x < pbmih->biWidth ; x++)
							{
								offset1 = (y*pbmih->biWidth+x)*pbmih->biBitCount/8;
								offset2 = (y*pbmih->biWidth+x)*24/8;
								memcpy(Image1D+offset2, image+offset1, 3);
							}
					}
					else if(pbmih->biBitCount == 16)
					{
						int x, y, offset1, offset2;
						for (y = 0; y < pbmih->biHeight ; y++)
							for (x = 0; x < pbmih->biWidth ; x++)
							{
								offset1 = (y*pbmih->biWidth*2+3)/4*4 + x*2;

								offset2 = (y*pbmih->biWidth+x)*24/8;
							
								WORD img = *(WORD *)(image+offset1);

								if(pbmih->biCompression == BI_BITFIELDS)
								{
									Image1D[offset2++] = (BYTE)(((img>>0) & 0x001F) * 8);
									Image1D[offset2++] = (BYTE)(((img>>5) & 0x003F) * 4);
									Image1D[offset2] = (BYTE)(((img>>11) & 0x001F) * 8);
								}
								else
								{
									Image1D[offset2++] = (BYTE)(((img>>0) & 0x001F) * 8);
									Image1D[offset2++] = (BYTE)(((img>>5) & 0x001F) * 8);
									Image1D[offset2] = (BYTE)(((img>>10) & 0x001F) * 8);
								}
							}
					}

					CString SelFolder;
					CString FileName;

					GetDlgItemText(IDC_DISP_FOLDER2, SelFolder);
					if(m_bGrabOn && !SelFolder.IsEmpty())
					{
						if(m_nFileType == 0)
						{
							FileName.Format(_T("%s\\aa%07d.jpg"), SelFolder, m_nCurrentProcessingNum);

							char mbcsFileName[MAX_PATH];
							int len = WideCharToMultiByte(CP_ACP, 0, FileName, -1, NULL, NULL, NULL, NULL);
							WideCharToMultiByte(CP_ACP, 0, FileName, -1, mbcsFileName, len, NULL, NULL);

							SaveJpeg(mbcsFileName, Image1D, pbmih->biWidth, pbmih->biHeight, true, 100);
						}
						else if(m_nFileType == 1)
						{
							FileName.Format(_T("%s\\aa%07d.bmp"), SelFolder, m_nCurrentProcessingNum);

							char mbcsFileName[MAX_PATH];
							int len = WideCharToMultiByte(CP_ACP, 0, FileName, -1, NULL, NULL, NULL, NULL);
							WideCharToMultiByte(CP_ACP, 0, FileName, -1, mbcsFileName, len, NULL, NULL);

							SaveBmp(mbcsFileName, Image1D, pbmih->biWidth, pbmih->biHeight);
						}
						else if(m_nFileType == 2)
						{
							FileName.Format(_T("%s\\aa%07d.%d"), SelFolder, m_nCurrentProcessingNum, m_nGW);
						}

						CString Msg;
						Msg.Format(_T("Grabbing(%7d)"), m_nCurrentProcessingNum);
						SetDlgItemText(IDC_GRAB, Msg);
					}
					else
					{
						m_bGrabOn = false;
						SetDlgItemText(IDC_GRAB, _T("Grab"));
					}
					//

					// Processing
					DisplayCimage1D(Image1D, pbmih->biWidth, pbmih->biHeight, 0, 0, false, true);

					delete [] Image1D;

					m_bFirstFrame = false;

					CString DispFrameNum;
					DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nAviLength-1);
					SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);
				
					m_nCurrentProcessingNum++;
				}
				else
					bExit = true;

				if((DWORD)m_nCurrentProcessingNum >= m_si.dwLength || bExit || m_nCurrentProcessingNum > m_nEnd) 
				{
					AVIStreamGetFrameClose(m_pfrm);
					AVIStreamRelease(m_pstm);    
					AVIFileRelease(m_pavi);    
				
					KillTimer(TID_AVI_RUN);

					SetDlgItemText(IDC_RUN, _T("Run"));

					m_SourceType0Ctrl.EnableWindow(TRUE);
					m_SourceType1Ctrl.EnableWindow(TRUE);
					m_SourceType2Ctrl.EnableWindow(TRUE);

					m_bAVITimerRun = FALSE;

					break;
				}
	////////
			}
			else if(nIDEvent == TID_SEQUENCE_RUN)
			{
				Sleep(m_nSleep);

				if(m_nCurrentProcessingNum > m_nEnd) 
				{
					KillTimer(TID_SEQUENCE_RUN);

					SetDlgItemText(IDC_RUN, _T("Run"));

					m_SourceType0Ctrl.EnableWindow(TRUE);
					m_SourceType1Ctrl.EnableWindow(TRUE);
					m_SourceType2Ctrl.EnableWindow(TRUE);

					m_bSequenceTimerRun = false;

					break;
				}

				m_FileListCtrl.SetItemState(m_nCurrentProcessingNum, LVIS_SELECTED, LVIS_SELECTED);
				m_FileListCtrl.EnsureVisible(m_nCurrentProcessingNum, FALSE);

				CString FileName = m_FileListCtrl.GetItemText(m_nCurrentProcessingNum, 1);
				FileName.MakeUpper();

				BYTE *Image1D = NULL;
				int nW, nH;

				char mbcsFileName[MAX_PATH];
				int len = WideCharToMultiByte(CP_ACP, 0, FileName, -1, NULL, NULL, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, FileName, -1, mbcsFileName, len, NULL, NULL);

				if(FileName.Right(4) == _T(".BMP"))
				{
					Image1D = ReadBmp(mbcsFileName, &nW, &nH);
				}
				else if (FileName.Right(4) == _T(".PNG"))
				{
					Image1D = ReadPng(mbcsFileName, &nW, &nH);
				}
				else if (FileName.Right(4) == _T(".PPM"))
				{
					Image1D = ReadPpm(mbcsFileName, &nW, &nH);
				}
				else if(FileName.Right(4) == _T(".256"))
				{
					nW = 256;
					nH = 256;

					Image1D = ReadRaw(mbcsFileName, nW, nH);
				}
				else if(FileName.Right(4) == _T(".320"))
				{
					nW = 320;
					nH = 240;

					Image1D = ReadRaw(mbcsFileName, nW, nH);
				}
				else if(FileName.Right(4) == _T(".640"))
				{
					nW = 640;
					nH = 480;

					Image1D = ReadRaw(mbcsFileName, nW, nH);
				}
				else if(FileName.Right(4) == _T(".JPG"))
				{
					Image1D = ReadJpeg(mbcsFileName, &nW, &nH);
				}

				if(Image1D)
				{
					// Processing
					DisplayCimage1D(Image1D, nW, nH, 0, 0, false, true);

					//

					m_bFirstFrame = false;
					delete [] Image1D;
				}
			
				CString DispFrameNum;
				DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nSequenceLength-1);
				SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

				m_nCurrentProcessingNum++;

			}
			else if(nIDEvent == TID_CAM_RUN)
			{
				capGrabFrame(m_hWndCap);

				DisplayCimage2DColor(m_CurrentImageRed, m_CurrentImageGreen, m_CurrentImageBlue, 
					m_nGW, m_nGH, 0, 0, false, true);

				CString SelFolder;
				CString FileName;

				GetDlgItemText(IDC_DISP_FOLDER2, SelFolder);

				if(m_bGrabOn && !SelFolder.IsEmpty())
				{
					if(m_nElapsedTime > m_nGap)
					{
						if(m_nFileType == 0)
						{
							FileName.Format(_T("%s\\aa%07d.jpg"), SelFolder, m_nCurrentProcessingNum);
						
							char mbcsFileName[MAX_PATH];
							int len = WideCharToMultiByte(CP_ACP, 0, FileName, -1, NULL, NULL, NULL, NULL);
							WideCharToMultiByte(CP_ACP, 0, FileName, -1, mbcsFileName, len, NULL, NULL);
						
							SaveImage2DColorJpeg(mbcsFileName, 
								m_CurrentImageRed, m_CurrentImageGreen, m_CurrentImageBlue,
								m_nGW, m_nGH, true, 100);
						}
						else if(m_nFileType == 1)
						{
							FileName.Format(_T("%s\\aa%07d.bmp"), SelFolder, m_nCurrentProcessingNum);

							char mbcsFileName[MAX_PATH];
							int len = WideCharToMultiByte(CP_ACP, 0, FileName, -1, NULL, NULL, NULL, NULL);
							WideCharToMultiByte(CP_ACP, 0, FileName, -1, mbcsFileName, len, NULL, NULL);

							SaveImage2DColorBmp(mbcsFileName, 
								m_CurrentImageRed, m_CurrentImageGreen, m_CurrentImageBlue,
								m_nGW, m_nGH);
						}
						else if(m_nFileType == 2)
						{
							FileName.Format(_T("%s\\aa%07d.%d"), SelFolder, m_nCurrentProcessingNum, m_nGW);
						}

						CString Msg;
						Msg.Format(_T("Grabbing(%7d)"), m_nCurrentProcessingNum);
						SetDlgItemText(IDC_GRAB, Msg);

						m_nCurrentProcessingNum++;

						m_nElapsedTime = 0;
					}
				}
				else
				{
					m_bGrabOn = false;
					SetDlgItemText(IDC_GRAB, _T("Grab"));
				}

				QueryPerformanceCounter((LARGE_INTEGER*)&m_TimeEnd);
				double timeDiff = (double)(m_TimeEnd - m_TimeStart)/(double)m_TimeFreq; 

				m_nElapsedTime += (int)(timeDiff*1000);

				QueryPerformanceFrequency((LARGE_INTEGER*)&m_TimeFreq);
				QueryPerformanceCounter((LARGE_INTEGER*)&m_TimeStart);
			}
			else if(nIDEvent == TID_EMPTY_RUN)
			{
				Sleep(m_nSleep);

				if(m_nCurrentProcessingNum > m_nEnd) 
				{
					KillTimer(TID_SEQUENCE_RUN);

					SetDlgItemText(IDC_RUN, _T("Run"));

					m_SourceType0Ctrl.EnableWindow(TRUE);
					m_SourceType1Ctrl.EnableWindow(TRUE);
					m_SourceType2Ctrl.EnableWindow(TRUE);

					m_bEmptyTimerRun = false;

					break;
				}

				CNewScreenImage Cap;
				CRect rt;

				GetWindowRect(rt);
				rt.left = 0;
				rt.top = 0;
				rt.right = ::GetSystemMetrics(SM_CXSCREEN)-1;
				rt.bottom = ::GetSystemMetrics(SM_CYSCREEN)-1;

				CClientDC dc(this);
				unsigned char *Image1D;
				int nW, nH;
				
				Image1D = Cap.CaptureRect(rt);
				nW = rt.right - rt.left + 1;
				nH = rt.bottom - rt.top + 1;

				if(Image1D)
				{
					DisplayCimage1D(Image1D, nW, nH, 0, 0, false, true);

					delete [] Image1D;
				}

				CString DispFrameNum;
				DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nEnd);
				SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

				m_nCurrentProcessingNum++;
			}

			if(m_bInitSaveAvi && m_bSaveScreen && m_nSaveAviOption == SAO_VIEW_RECT)
			{
				BYTE *SaveAviFrameImage = new BYTE [GetBmp24Size(m_nSaveAviW, m_nSaveAviH)];
				int x, y;
				
				CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
				CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

				if(pChild) 
				{
					CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

					if(pView)
					{
						int Offset;
						for(y = 0 ; y < m_nSaveAviH ; y++)
						{
							Offset = (m_nSaveAviW*3+3)/4*4 * (m_nSaveAviH-y-1);
							for(x = 0 ; x < m_nSaveAviW ; x++)
							{
								pView->GetCurrPosColor1D(x, y, SaveAviFrameImage+Offset+x*3);
							}
						}
					}
				}

				AVIStreamWrite(m_psCompressedForSave, m_nCurrentProcessingNum-1, 1, 
					SaveAviFrameImage, GetBmp24Size(m_nSaveAviW, m_nSaveAviH),
					AVIIF_KEYFRAME, NULL, NULL);

				delete [] SaveAviFrameImage;
			}

			if(m_bStep) m_bTimerPause = true;

			break;
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CMainProcessDlg::OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION index;

	index = m_FileListCtrl.GetFirstSelectedItemPosition();

	if((int)index-1 >= 0 && (int)index-1 < m_FileListCtrl.GetItemCount())
	{
		m_nStart = (int)(index-1);
		UpdateData(FALSE);
	}	
	
	*pResult = 0;

	CString FileName = m_FileListCtrl.GetItemText(m_nStart, 1);

	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return;
	CImageProcessingDoc *pDoc = (CImageProcessingDoc *)pChild->GetActiveDocument();
	if(pDoc)
		pDoc->FileOpenCur(FileName);
}

void CMainProcessDlg::OnPause() 
{
	m_bTimerPause = !m_bTimerPause;	
}

void CMainProcessDlg::OnSelFolder2() 
{
	ITEMIDLIST	*pidlBrowse;
	TCHAR pszPathname[MAX_PATH];

	BROWSEINFO BrInfo;
	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;
	memset(&BrInfo, 0 , sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("Select Folder");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	pidlBrowse = ::SHBrowseForFolder(&BrInfo);

	CString SelFolder;
	if(pidlBrowse != NULL)
	{
		SHGetPathFromIDList(pidlBrowse, pszPathname);
		SelFolder.Format(_T("%s"), pszPathname);

		SetDlgItemText(IDC_DISP_FOLDER2, SelFolder);
	}
}
static LRESULT	CALLBACK capCallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	UINT i;
	int x, y;

	CMainProcessDlg *pDlg = (CMainProcessDlg *)CMainProcessDlg::FromHandle(::GetParent(hWnd));

	for(i = 0 ; i < lpVHdr->dwBufferLength ; i+=3)
	{
		y = (lpVHdr->dwBufferLength-i-1)/(pDlg->m_nGW*3);
		x = i%(pDlg->m_nGW*3)/3;

		pDlg->m_CurrentImageRed[y][x] = *(lpVHdr->lpData+i+2);
		pDlg->m_CurrentImageGreen[y][x] = *(lpVHdr->lpData+i+1);
		pDlg->m_CurrentImageBlue[y][x] = *(lpVHdr->lpData+i);
	}

	return (LRESULT) true;
}

void CMainProcessDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	free_cmatrix(m_CurrentImageRed, m_nMH, m_nMW);
	free_cmatrix(m_CurrentImageGreen, m_nMH, m_nMW);
	free_cmatrix(m_CurrentImageBlue, m_nMH, m_nMW);	

	if(m_psForSave)
	{
		AVIStreamRelease(m_psForSave);
		m_psForSave = NULL;
	}

	if(m_psCompressedForSave)
	{
		AVIStreamRelease(m_psCompressedForSave);
		m_psCompressedForSave = NULL;
	}

	if(m_pfileForSave)
	{
		AVIFileRelease(m_pfileForSave);
		m_pfileForSave = NULL;
	}
		
	AVIFileExit();
}

void CMainProcessDlg::OnVs() 
{
	capDlgVideoSource(m_hWndCap);
}

void CMainProcessDlg::OnCamOn() 
{
	UpdateData(TRUE);

	if(m_nGW > m_nMW || m_nGH > m_nMH) return;

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun && !m_bEmptyTimerRun)
	{
		m_bCamTimerRun = true;
		m_bTimerPause = false;
		m_bFirstFrame = true;
		SetDlgItemText(IDC_CAM_ON, _T("Cam. OFF"));

		m_nCurrentProcessingNum = 0;
		m_bGrabOn = false;
		m_nElapsedTime = m_nGap+1;

		m_hWndCap = capCreateCaptureWindow(_T("Grab Window"), WS_CHILD,
			0, 0, m_nGW, m_nGH, GetSafeHwnd(), NULL);

		capSetCallbackOnFrame(m_hWndCap, capCallbackOnFrame);

		int Result;

		if(Result = capDriverConnect(m_hWndCap, 0))
		{
			capGetVideoFormat(m_hWndCap, &m_BmInfo, sizeof(BITMAPINFO));

			m_BmInfo.bmiHeader.biBitCount = 24;
			m_BmInfo.bmiHeader.biCompression = 0;
			m_BmInfo.bmiHeader.biWidth = m_nGW;
			m_BmInfo.bmiHeader.biHeight = m_nGH;
			m_BmInfo.bmiHeader.biSizeImage = m_BmInfo.bmiHeader.biWidth * m_BmInfo.bmiHeader.biHeight * 3;
		
			int rtn = capSetVideoFormat(m_hWndCap, &m_BmInfo, sizeof(BITMAPINFO));

			capOverlay(m_hWndCap, false);
			capPreview(m_hWndCap, false);
		}

		m_GwCtrl.EnableWindow(FALSE);
		m_GhCtrl.EnableWindow(FALSE);

		SetTimer(TID_CAM_RUN, 0, NULL);
	}
	else if(m_bCamTimerRun)
	{
		m_bCamTimerRun = false;
		SetDlgItemText(IDC_CAM_ON, _T("Cam. ON"));

		KillTimer(TID_CAM_RUN);

		m_GwCtrl.EnableWindow(TRUE);
		m_GhCtrl.EnableWindow(TRUE);

		capDriverDisconnect(m_hWndCap);
	}
}

void CMainProcessDlg::OnGrab() 
{
	m_bGrabOn = !m_bGrabOn;
	m_nElapsedTime = m_nGap+1;

	if(m_bGrabOn)
		SetDlgItemText(IDC_GRAB, _T("Grabbing"));
	else
		SetDlgItemText(IDC_GRAB, _T("Grab"));
}

void CMainProcessDlg::OnResetGNum() 
{
	m_nCurrentProcessingNum = 0;
}

void CMainProcessDlg::OnCustomdrawSleep(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nSleep = m_SleepCtrl.GetPos();


	CString DispDelayTime;
	DispDelayTime.Format(_T("%dms"), m_nSleep);
	SetDlgItemText(IDC_DISP_SLEEP, DispDelayTime);
	
	*pResult = 0;
}

BOOL CMainProcessDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		pMsg->wParam = NULL;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CMainProcessDlg::OnSelSource() 
{
	UpdateData(TRUE);
	
	if(m_nSourceType == 0)
		SelFolder();
	else if(m_nSourceType == 1)
		SelAvi();
	else if(m_nSourceType == 2)
		SelYuv();	
}

void CMainProcessDlg::SelFolder() 
{
	ITEMIDLIST	*pidlBrowse;
	TCHAR pszPathname[MAX_PATH];

	BROWSEINFO BrInfo;
	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;
	memset(&BrInfo, 0 , sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("Select Folder");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	pidlBrowse = ::SHBrowseForFolder(&BrInfo);

	CString SelFolder;
	if(pidlBrowse != NULL)
	{
		SHGetPathFromIDList(pidlBrowse, pszPathname);
		SelFolder.Format(_T("%s"), pszPathname);

		ReadFile(SelFolder);
		SetDlgItemText(IDC_DISP_SOURCE, SelFolder);
	}		
}

void CMainProcessDlg::ReadFile(CString SelFolder)
{
	m_FileListCtrl.DeleteAllItems();

	UpdateData(TRUE);
	ReadFile(SelFolder, m_bSubFolder?true:false);

	CString DispFrameNum;
	DispFrameNum.Format(_T("(0,%d)"), m_nSequenceLength-1);
	SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);
}

void CMainProcessDlg::ReadFile(CString SelFolder, bool bSubFolder)
{
	CFileFind Finder; 
	BOOL Continue;
	SHFILEINFO shInfo;

	CString Name;

	Continue = Finder.FindFile(SelFolder+_T("\\*.*"));

	CList<CString, CString &> listSubFolder;

	while(Continue)
	{
		Continue = Finder.FindNextFile();

		if(Finder.IsDirectory() || Finder.IsDots())
		{
			if(Finder.IsDirectory() && !Finder.IsDots())
			{
				listSubFolder.AddTail(Finder.GetFilePath());
			}

			continue;
		}

		Name = Finder.GetFileName();
		Name.MakeUpper();
		if(m_nSeqType == 1 && Name.Right(4) != _T(".BMP"))
			continue;
		else if(m_nSeqType == 2 && Name.Right(4) != _T(".JPG"))
			continue;
		else if(m_nSeqType == 3 && Name.Right(4) != _T(".PCX"))
			continue;
		else if(m_nSeqType == 4 && Name.Right(4) != _T(".256"))
			continue;
		else if(m_nSeqType == 5 && Name.Right(4) != _T(".320"))
			continue;
		else if(m_nSeqType == 6 && Name.Right(4) != _T(".640"))
			continue;
		else if(m_nSeqType == 7 && (Name.Right(4) != _T(".BMP") && Name.Right(4) != _T(".JPG")))
			continue;
		else if(m_nSeqType == 8 && Name.Right(4) != _T(".PPM"))
			continue;
		else if(m_nSeqType == 9 && Name.Right(4) != _T(".PNG"))
			continue;
		else if(m_nSeqType == 10 && (Name.Right(4) != _T(".BMP") && Name.Right(4) != _T(".JPG")
			&& Name.Right(4) != _T(".PCX") && Name.Right(4) != _T(".256") && Name.Right(4) != _T(".320") && Name.Right(4) != _T(".PPM") 
			&& Name.Right(4) != _T(".PNG") 
			&& Name.Right(4) != _T(".640")))
			continue;

		LV_ITEM lvItem;

		int nItemNum = m_FileListCtrl.GetItemCount();
			
		SHGetFileInfo(Finder.GetFilePath(),0,&shInfo,sizeof(SHFILEINFO),SHGFI_TYPENAME|SHGFI_SYSICONINDEX);

		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem = nItemNum;
		lvItem.iSubItem = 0;
		lvItem.iImage = shInfo.iIcon;
		lvItem.pszText = Name.GetBuffer(Name.GetLength());
		m_FileListCtrl.InsertItem(&lvItem);

		m_FileListCtrl.SetItemText(nItemNum, 1, Finder.GetFilePath());
	}

	m_nEnd = m_FileListCtrl.GetItemCount()-1;
	m_nSequenceLength = m_FileListCtrl.GetItemCount();
	UpdateData(FALSE);

	if(!bSubFolder) return;

	POSITION pos = listSubFolder.GetHeadPosition();

	if(!pos) return;

	while(pos)
	{
		CString SubFolder = listSubFolder.GetNext(pos);
		ReadFile(SubFolder, bSubFolder);
	}
}

void CMainProcessDlg::SelAvi() 
{
	CFileDialog dlg(TRUE, _T("*.avi"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("AVI 파일(*.avi)|*.avi||"));
	
	if(dlg.DoModal() == IDOK) 
	{
		CString FileName = dlg.GetPathName();
		SetDlgItemText(IDC_DISP_SOURCE, FileName);

		GetDlgItemText(IDC_DISP_SOURCE, m_AviFileName);

		if(m_AviFileName.IsEmpty())
		{
			return;
		}
 
		if(AVIFileOpen(&m_pavi, m_AviFileName, OF_READ | OF_SHARE_DENY_NONE, NULL) != 0) 
		{
			return;
		}
		AVIFileInfo(m_pavi, &m_fi, sizeof(AVIFILEINFO));

		UINT stream;

		for(stream = 0 ; stream < m_fi.dwStreams ; stream++)
		{
			AVIFileGetStream(m_pavi, &m_pstm, 0, stream);
			AVIStreamInfo(m_pstm, &m_si, sizeof(AVISTREAMINFO)); 
 
			if (m_si.fccType == streamtypeVIDEO) 
			{
				m_pfrm = AVIStreamGetFrameOpen(m_pstm, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT);
				m_nAviLength = AVIStreamLength(m_pstm);
				break;
			}
		}

		if(stream == m_fi.dwStreams || m_pfrm == NULL)
		{
			AVIStreamRelease(m_pstm);    
			AVIFileRelease(m_pavi);    

			return;
		}

		m_nStart = 0;
		m_nEnd = m_nAviLength-1;

		UpdateData(FALSE);

		CString DispFrameNum;
		DispFrameNum.Format(_T("(0,%d)"), m_nAviLength-1);
		SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

		AVIStreamGetFrameClose(m_pfrm);
		AVIStreamRelease(m_pstm);    
		AVIFileRelease(m_pavi);    

		///////////////////
	}	
}

void CMainProcessDlg::SelYuv() 
{
	CFileDialog dlg(TRUE, _T("*.yuv"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("YUV 파일(*.yuv, *.y4m)|*.yuv;*.y4m||"));
	
	if(dlg.DoModal() == IDOK) 
	{
		CString FileName = dlg.GetPathName();
		SetDlgItemText(IDC_DISP_SOURCE, FileName);

		CYuvOptionDlg dlg;
		if(dlg.DoModal() == IDOK)
		{
			m_nYuvW = dlg.m_nW;
			m_nYuvH = dlg.m_nH;
			m_nYuvType = dlg.m_nYuvType;
			m_bY4m = dlg.m_bY4m;


			GetDlgItemText(IDC_DISP_SOURCE, m_YuvFileName);

			if(m_YuvFileName.IsEmpty() || m_nYuvType < 0)
			{
				m_nYuvType = -1;
				return;
			}
				

			BOOL bOpen = m_YuvFile.Open(m_YuvFileName, CFile::modeRead | CFile::typeBinary);
			if(!bOpen)
			{
				m_nYuvType = -1;
				return;
			}

			BYTE T = 0;
			int nHeaderEnd = 0;
			int nType = 0;

			if(m_bY4m)
			{
				BYTE Param = NULL;
				m_nYuvW = 0;
				m_nYuvH = 0;
				nType = 0;
				while(T !=  0x0A)
				{
					m_YuvFile.Read(&T, 1);
					nHeaderEnd++;

					if(Param == NULL)
						Param = T;
					else if(Param == 'W' && T >= '0' && T <= '9')
						m_nYuvW = m_nYuvW*10 + T-'0';
					else if(Param == 'H' && T >= '0' && T <= '9')
						m_nYuvH = m_nYuvH*10 + T-'0';
					else if(Param == 'C' && T >= '0' && T <= '9')
						nType = nType*10 + T-'0';

					if(T == ' ')
						Param = NULL;
				}
			}

			if(nType == 420)
				m_nYuvType = 0;
			else if(nType == 422)
				m_nYuvType = 1;
			else if(nType == 444)
				m_nYuvType = 2;


			CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
			pMain->DlgPrintf(_T("W: %d"), m_nYuvW);
			pMain->DlgPrintf(_T("H: %d"), m_nYuvH);

			if(nType > 0) pMain->DlgPrintf(_T("Type: YUV%d"), nType);

			int nFrameSize;

			if(m_nYuvType == 0)
				nFrameSize = m_nYuvW*m_nYuvH*3/2;
			else if(m_nYuvType == 1)
				nFrameSize = m_nYuvW*m_nYuvH*2;
			else if(m_nYuvType == 2)
				nFrameSize = m_nYuvW*m_nYuvH*3;

			if(m_bY4m)
				nFrameSize += 6;

			if(m_bY4m)
				m_nYuvLength = (int)((m_YuvFile.GetLength()-nHeaderEnd)/nFrameSize);
			else
				m_nYuvLength = (int)(m_YuvFile.GetLength()/nFrameSize);

			m_YuvFile.Close();

			m_nStart = 0;
			m_nEnd = m_nYuvLength-1;
			UpdateData(FALSE);

			CString DispFrameNum;
			DispFrameNum.Format(_T("(0,%d)"), m_nYuvLength-1);
			SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);
		}
		else
			m_nYuvType = -1;
	}
}

void CMainProcessDlg::OnRun() 
{
	UpdateData(TRUE);
	
	if(m_nSourceType == 0)
		SequenceRun();
	else if(m_nSourceType == 1)
		AviRun();
	else if(m_nSourceType == 2)
		YuvRun();
	else if(m_nSourceType == 3)
		EmptyRun();
}

void CMainProcessDlg::EmptyRun()
{
	UpdateData(TRUE);

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun && !m_bEmptyTimerRun)
	{
		m_bEmptyTimerRun = true;
		m_bTimerPause = false;
		m_bFirstFrame = true;
		SetDlgItemText(IDC_RUN, _T("Stop"));

		m_SourceType0Ctrl.EnableWindow(FALSE);
		m_SourceType1Ctrl.EnableWindow(FALSE);
		m_SourceType2Ctrl.EnableWindow(FALSE);

		m_nStart = 0;
		m_nEnd = 9999;
		UpdateData(FALSE);

		CString DispFrameNum;
		DispFrameNum.Format(_T("(0,%d)"), m_nEnd);
		SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

		m_nCurrentProcessingNum = m_nStart;
		
		SetTimer(TID_EMPTY_RUN, 0, NULL);
	}
	else if(m_bEmptyTimerRun)
	{
		m_bEmptyTimerRun = false;
		SetDlgItemText(IDC_RUN, _T("Run"));

		m_SourceType0Ctrl.EnableWindow(TRUE);
		m_SourceType1Ctrl.EnableWindow(TRUE);
		m_SourceType2Ctrl.EnableWindow(TRUE);

		KillTimer(TID_EMPTY_RUN);
	}
}

void CMainProcessDlg::SequenceRun() 
{
	UpdateData(TRUE);

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun && !m_bEmptyTimerRun)
	{
		m_bSequenceTimerRun = true;
		m_bTimerPause = false;
		m_bFirstFrame = true;
		SetDlgItemText(IDC_RUN, _T("Stop"));

		m_SourceType0Ctrl.EnableWindow(FALSE);
		m_SourceType1Ctrl.EnableWindow(FALSE);
		m_SourceType2Ctrl.EnableWindow(FALSE);

		m_nCurrentProcessingNum = m_nStart;
		
		SetTimer(TID_SEQUENCE_RUN, 0, NULL);
	}
	else if(m_bSequenceTimerRun)
	{
		m_bSequenceTimerRun = false;
		SetDlgItemText(IDC_RUN, _T("Run"));

		m_SourceType0Ctrl.EnableWindow(TRUE);
		m_SourceType1Ctrl.EnableWindow(TRUE);
		m_SourceType2Ctrl.EnableWindow(TRUE);

		KillTimer(TID_SEQUENCE_RUN);
	}
}

void CMainProcessDlg::AviRun() 
{
	UpdateData(TRUE);

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun && !m_bEmptyTimerRun)
	{
		m_nAviLength = 0;
		m_bAVITimerRun = true;
		m_bTimerPause = false;

		m_bFirstFrame = true;
		
		GetDlgItemText(IDC_DISP_SOURCE, m_AviFileName);

		if(m_AviFileName.IsEmpty())
		{
			m_bAVITimerRun = false;
			return;
		}
  
		if(AVIFileOpen(&m_pavi, m_AviFileName, OF_READ | OF_SHARE_DENY_NONE, NULL) != 0) 
		{
			m_bAVITimerRun = false;
			return;
		}
		AVIFileInfo(m_pavi, &m_fi, sizeof(AVIFILEINFO));

		UINT stream;

		for(stream = 0 ; stream < m_fi.dwStreams ; stream++)
		{
			AVIFileGetStream(m_pavi, &m_pstm, 0, stream);
			AVIStreamInfo(m_pstm, &m_si, sizeof(AVISTREAMINFO)); 
 
			if (m_si.fccType == streamtypeVIDEO) 
			{
				m_pfrm = AVIStreamGetFrameOpen(m_pstm, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT);
				m_nAviLength = AVIStreamLength(m_pstm);
				break;
			}
		}

		if(stream == m_fi.dwStreams || m_pfrm == NULL)
		{
			AVIStreamRelease(m_pstm);    
			AVIFileRelease(m_pavi);    

			m_bAVITimerRun = false;

			return;
		}

		SetDlgItemText(IDC_RUN, _T("Stop"));

		m_SourceType0Ctrl.EnableWindow(FALSE);
		m_SourceType1Ctrl.EnableWindow(FALSE);
		m_SourceType2Ctrl.EnableWindow(FALSE);

		m_nCurrentProcessingNum = m_nStart;

		SetTimer(TID_AVI_RUN, 0, NULL);
	}
	else if(m_bAVITimerRun)
	{
		AVIStreamGetFrameClose(m_pfrm);
		AVIStreamRelease(m_pstm);    
		AVIFileRelease(m_pavi);    

		KillTimer(TID_AVI_RUN);

		m_bAVITimerRun = false;
		SetDlgItemText(IDC_RUN, _T("Run"));

		m_SourceType0Ctrl.EnableWindow(TRUE);
		m_SourceType1Ctrl.EnableWindow(TRUE);
		m_SourceType2Ctrl.EnableWindow(TRUE);
	}
	
}

void CMainProcessDlg::YuvRun() 
{
	UpdateData(TRUE);

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun && !m_bEmptyTimerRun)
	{
		m_bYUVTimerRun = true;
		m_bTimerPause = false;
		m_bFirstFrame = true;

		GetDlgItemText(IDC_DISP_SOURCE, m_YuvFileName);

		if(m_YuvFileName.IsEmpty() || m_nYuvType < 0)
		{
			m_bYUVTimerRun = false;
			return;
		}

		BOOL bOpen = m_YuvFile.Open(m_YuvFileName, CFile::modeRead | CFile::typeBinary);
		if(!bOpen)
		{
			m_bYUVTimerRun = false;
			return;
		}

		SetDlgItemText(IDC_RUN, _T("Stop"));

		m_SourceType0Ctrl.EnableWindow(FALSE);
		m_SourceType1Ctrl.EnableWindow(FALSE);
		m_SourceType2Ctrl.EnableWindow(FALSE);

		m_nCurrentProcessingNum = m_nStart;

		BYTE T = 0;
		int nHeaderEnd = 0;

		if(m_bY4m)
		{
			BYTE Param = NULL;
//			m_nYuvW = 0;
//			m_nYuvH = 0;
			while(T !=  0x0A)
			{
				m_YuvFile.Read(&T, 1);
				nHeaderEnd++;

/*				if(Param == NULL)
					Param = T;
				else if(Param == 'W' && T >= '0' && T <= '9')
					m_nYuvW = m_nYuvW*10 + T-'0';
				else if(Param == 'H' && T >= '0' && T <= '9')
					m_nYuvH = m_nYuvH*10 + T-'0';

				if(T == ' ')
					Param = NULL;*/
			}
		}

		int nFrameSize;

		if(m_nYuvType == 0)
			nFrameSize = m_nYuvW*m_nYuvH*3/2;
		else if(m_nYuvType == 1)
			nFrameSize = m_nYuvW*m_nYuvH*2;
		else if(m_nYuvType == 2)
			nFrameSize = m_nYuvW*m_nYuvH*3;

		if(m_bY4m)
			nFrameSize += 6;

		if(m_bY4m)
			m_YuvFile.Seek(m_nStart*nFrameSize+nHeaderEnd, CFile::begin);
		else
			m_YuvFile.Seek(m_nStart*nFrameSize, CFile::begin);
		
		SetTimer(TID_YUV_RUN, 0, NULL);
	}
	else if(m_bYUVTimerRun)
	{
		m_bYUVTimerRun = false;
		SetDlgItemText(IDC_RUN, _T("Run"));

		m_SourceType0Ctrl.EnableWindow(TRUE);
		m_SourceType1Ctrl.EnableWindow(TRUE);
		m_SourceType2Ctrl.EnableWindow(TRUE);

		KillTimer(TID_YUV_RUN);

		m_YuvFile.Close();
	}
}

void CMainProcessDlg::OnSaveScreen() 
{
	UpdateData(TRUE);

	if(m_bSaveScreen)
	{
		WORD wVer;
		bool bInit;
	
		wVer = HIWORD(VideoForWindowsVersion());
		if (wVer < 0x010a)
			bInit = false;
		else
			bInit = true;

		if(m_psForSave != NULL)	{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)	{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)	{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}

		if(bInit)
		{
			bInit = AVIFileInitFunction();
		}

		m_bInitSaveAvi = bInit;
	}
	else
	{
		if(m_nSaveAviOption != SAO_VIEW_RECT)
			KillTimer(TID_SCREEN_CAPTURE);

		m_bInitSaveAvi = false;

		if(m_psForSave)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}

		if(m_pfileForSave)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
	}
}

bool CMainProcessDlg::AVIFileInitFunction()
{
	AVISTREAMINFO strhdr;
	HRESULT hr;
	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};

//	char tmpName[MAX_PATH];

	CFileDialog FileSaveDlg(FALSE, _T("*.avi"), _T("Data.avi"), 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
			_T("AVI Files(*.avi)|*.avi||"));

	if(FileSaveDlg.DoModal() != IDOK)
		return false;

	CSaveScreenOpt dlg;

	m_nSaveAviW = 640;
	m_nSaveAviH = 480;

	m_nSaveAviFps = 30;
/*
#define SAO_VIEW_RECT			0
#define SAO_DESKTOP_SCREEN		1
#define SAO_DESKTOP_WINDOW		2
#define SAO_DESKTOP_RECT		3
*/
	if(dlg.DoModal() == IDOK)
	{
		m_nSaveAviFps = dlg.m_nFPS;

		if(dlg.m_bSaveView){
			m_nSaveAviW = dlg.m_nW;
			m_nSaveAviH = dlg.m_nH;

			m_nSaveAviOption = SAO_VIEW_RECT;
		}
		else
		{
			if(dlg.m_nNotViewOpt == 0)
				m_nSaveAviOption = SAO_DESKTOP_SCREEN;
			else if(dlg.m_nNotViewOpt == 1)
				m_nSaveAviOption = SAO_DESKTOP_WINDOW;
			else
				m_nSaveAviOption = SAO_DESKTOP_RECT;

			m_nSaveAviRect = dlg.m_rtClientRect;
		}
	}
	else
		return false;

	m_BmInfoForSaveAvi.biSize = sizeof(BITMAPINFOHEADER);

	if(m_nSaveAviOption == SAO_VIEW_RECT)
	{
		m_BmInfoForSaveAvi.biWidth = m_nSaveAviW;
		m_BmInfoForSaveAvi.biHeight = m_nSaveAviH;
	}
	else
	{
		m_BmInfoForSaveAvi.biWidth = m_nSaveAviRect.Width();
		m_BmInfoForSaveAvi.biHeight = m_nSaveAviRect.Height();
	}

	m_BmInfoForSaveAvi.biPlanes = 1;
	m_BmInfoForSaveAvi.biBitCount = 24;
	m_BmInfoForSaveAvi.biCompression = BI_RGB;
	m_BmInfoForSaveAvi.biSizeImage = (m_BmInfoForSaveAvi.biWidth*3+3)/4*4  * m_BmInfoForSaveAvi.biHeight;
	m_BmInfoForSaveAvi.biXPelsPerMeter = 0;
	m_BmInfoForSaveAvi.biYPelsPerMeter = 0;
	m_BmInfoForSaveAvi.biClrUsed = 0;
	m_BmInfoForSaveAvi.biClrImportant = 0;
			
//	if(!access(FileSaveDlg.GetPathName(), 00))
//	{
	/*	CFile DelFile;
		BOOL bOpen;
		
		bOpen = DelFile.Open(FileSaveDlg.GetPathName(), CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
	
		if(!bOpen) return false;

		DelFile.Close();*/
//	}
	

	hr = AVIFileOpen(&m_pfileForSave,		    // returned file pointer
			   //tmpName,		            // file name
			   FileSaveDlg.GetPathName(),
			   OF_CREATE | OF_WRITE,	    // mode to open file with
			   NULL);			    // use handler determined
						// from file extension....
	if(hr) 
	{
		if(m_pfileForSave != NULL) 
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

	// The video stream will run in 30ths of a second....

	_fmemset(&strhdr, 0, sizeof(strhdr));
//	memset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType                = streamtypeVIDEO;// stream type
	strhdr.fccHandler             = 0;
	strhdr.dwScale                = 1;
	
	strhdr.dwRate                 = m_nSaveAviFps;		    // fps

    strhdr.dwSuggestedBufferSize  = m_BmInfoForSaveAvi.biSizeImage;

	// And create the stream;
	hr = AVIFileCreateStream(m_pfileForSave,		    // file pointer
					 &m_psForSave,		    // returned stream pointer
					 &strhdr);	    // stream header

	if (hr != AVIERR_OK) {
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

//	_fmemset(&opts, 0, sizeof(opts));

		memset(&opts, 0, sizeof(opts));
		opts.fccType = streamtypeVIDEO;
		//opts.fccHandler             = getFOURCC(_compressor);
		//opts.fccHandler  = 0;
		opts.fccHandler            = mmioFOURCC('D','I','B',' '); // Uncompressed
		//opts.fccHandler             = mmioFOURCC('C','V','I','D'); // Cinpak
		//opts.fccHandler             = mmioFOURCC('I','V','3','2'); // Intel video 3.2
	//	opts.fccHandler             = mmioFOURCC('M','S','V','C'); // Microsoft video 1
		//opts.fccHandler             = mmioFOURCC('I','V','5','0'); // Intel video 5.0
		opts.dwKeyFrameEvery = 0;//5
		opts.dwQuality		= 7500;
		//opts.dwBytesPerSecond
		opts.dwFlags                = 8;//AVICOMPRESSF_KEYFRAMES;
		//opts.lpFormat 
		//opts.cbFormat
		//opts.lpParms
		//opts.cbParms = 4;
		//opts.dwInterleaveEvery

	///////////////////
	if(!AVISaveOptions(NULL, 0, 1, &m_psForSave, (LPAVICOMPRESSOPTIONS FAR *) &aopts))  // option dlg
	{
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

	hr = AVIMakeCompressedStream(&m_psCompressedForSave, m_psForSave, &opts, NULL);
	if (hr != AVIERR_OK) {
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

    hr = AVIStreamSetFormat(m_psCompressedForSave, 0,
			   &m_BmInfoForSaveAvi,	    // stream format
			   m_BmInfoForSaveAvi.biSize +   // format size
			   m_BmInfoForSaveAvi.biClrUsed * sizeof(RGBQUAD));

	if (hr != AVIERR_OK) {
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}

		
		return false;
	}

	if(m_nSaveAviOption != SAO_VIEW_RECT)
	{
		m_nTimerAviSaveNum = 1;

		SetTimer(TID_SCREEN_CAPTURE, 1000/m_nSaveAviFps, NULL);
	}
	
	return true;
}

void CMainProcessDlg::OnDrawRectRegion() 
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(pChild) 
	{
		CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

		pView->m_nDrawRegion = RECT_REGION_DRAW;
		pView->m_nDrawnPointCnt = 0;
	}	
}

void CMainProcessDlg::OnDrawRegion() 
{
	UpdateData(TRUE);

	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(pChild) 
	{
		CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

		pView->m_nDrawRegion = REGION_DRAW;
		pView->m_nPtCntOfCurrRegoin = m_nPtCntOfCurrRegoin;
		pView->m_nDrawnPointCnt = 0;
	}	
}

void CMainProcessDlg::OnSaveRegion() 
{
	CFileDialog dlg(FALSE, _T("*.rgn"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("RGN 파일(*.rgn)|*.rgn||"));
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return;
		
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();
	
	if(dlg.DoModal() == IDOK)
	{
		CFile cfile;
		BOOL bOpen;
		int i, k;

		bOpen = cfile.Open(dlg.GetPathName (), CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);

		if(!bOpen) return;

		cfile.Write(&(pView->m_ViewRegion.nCnt), sizeof(int));
		for(i = 0 ; i < pView->m_ViewRegion.nCnt ; i++)
		{
			cfile.Write(&(pView->m_ViewRegion.nPtCnt[i]), sizeof(int));
			cfile.Write(&(pView->m_ViewRegion.nType[i]), sizeof(int));
			cfile.Write(&(pView->m_ViewRegion.Rotate[i]), sizeof(double));
			
			for(k = 0 ; k < pView->m_ViewRegion.nPtCnt[i] ; k++)
				cfile.Write(&(pView->m_ViewRegion.Pt[i][k]), sizeof(CPoint));
		}

		cfile.Close();
	}	
}

void CMainProcessDlg::OnReadRegion() 
{
	CFileDialog dlg(TRUE, _T("*.rgn"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("RGN 파일(*.rgn)|*.rgn||"));
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return;
		
	CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

	if(dlg.DoModal() == IDOK)
	{
		CFile cfile;
		BOOL bOpen;
		int i, k;

		bOpen = cfile.Open(dlg.GetPathName (), CFile::modeRead | CFile::typeBinary);

		if(!bOpen) return;

		cfile.Read(&(pView->m_ViewRegion.nCnt), sizeof(int));
		for(i = 0 ; i < pView->m_ViewRegion.nCnt ; i++)
		{
			cfile.Read(&(pView->m_ViewRegion.nPtCnt[i]), sizeof(int));
			cfile.Read(&(pView->m_ViewRegion.nType[i]), sizeof(int));
			cfile.Read(&(pView->m_ViewRegion.Rotate[i]), sizeof(double));
			
			for(k = 0 ; k < pView->m_ViewRegion.nPtCnt[i] ; k++)
				cfile.Read(&(pView->m_ViewRegion.Pt[i][k]), sizeof(CPoint));
		}

		cfile.Close();

		pView->Invalidate();
	}
}


static void SortPointFromLeft(CPoint data[], int size){
	int i, k;
	CPoint t;

	for (i = 0; i<size - 1; i++) {
		for (k = size - 1; k>i; k--){
			if (data[k - 1].x > data[k].x){
				t = data[k];
				data[k] = data[k - 1];
				data[k - 1] = t;
			}
		}
	}

}

