#if !defined(AFX_MAINPROCESSDLG_H__9D0BB200_9920_4FE1_84D5_8B823B5BAA47__INCLUDED_)
#define AFX_MAINPROCESSDLG_H__9D0BB200_9920_4FE1_84D5_8B823B5BAA47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainProcessDlg.h : header file
//
#include "ScreenImage.h"
#include <vfw.h>

#include <Afxsock.h>
#include "Image.h"

#define SAO_VIEW_RECT			0
#define SAO_DESKTOP_SCREEN		1
#define SAO_DESKTOP_WINDOW		2
#define SAO_DESKTOP_RECT		3

#define TID_SEQUENCE_RUN		0
#define TID_AVI_RUN				1
#define TID_YUV_RUN				2
#define TID_CAM_RUN				3
#define TID_IP_CAM_SOCK			4
#define TID_IP_CAM_WEB			5
#define TID_EMPTY_RUN			6

#define TID_SCREEN_CAPTURE		100


/////////////////////////////////////////////////////////////////////////////
// CMainProcessDlg dialog

class CMainProcessDlg : public CDialog
{
// Construction
public:
	CMainProcessDlg(CWnd* pParent = NULL);   // standard constructor

	CImageList m_ImageList;
	int m_nCurrentProcessingNum;

	bool m_bSequenceTimerRun, m_bCamTimerRun, m_bAVITimerRun, m_bYUVTimerRun, m_bIpCamWebRun, m_bEmptyTimerRun;
	bool m_bTimerPause;
	bool m_bFirstFrame;


	void SelFolder();
	void SelAvi();
	void SelYuv();

	void SequenceRun();
	void AviRun();
	void YuvRun();
	void EmptyRun();

	void ReadFile(CString SelFolder);
	void ReadFile(CString SelFolder, bool bSubFolder);

	HWND m_hWndCap;
	BITMAPINFO	m_BmInfo;
	BYTE **m_CurrentImageRed, **m_CurrentImageGreen, **m_CurrentImageBlue;
	int m_nMW, m_nMH;

	int m_nYuvW, m_nYuvH, m_nYuvType;
	bool m_bY4m;
	CString m_YuvFileName;
	CFile m_YuvFile;
	int m_nYuvLength;

	bool m_bGrabOn;
	int m_nElapsedTime;

	int m_nSequenceLength;


	CString m_AviFileName;
	PAVIFILE m_pavi;
	AVIFILEINFO m_fi;
	PAVISTREAM m_pstm;
	AVISTREAMINFO m_si;
	PGETFRAME m_pfrm; 
	int m_nAviLength;

	_int64 m_TimeFreq, m_TimeStart, m_TimeEnd;

	bool m_bInitSaveAvi;
	PAVISTREAM m_psCompressedForSave, m_psForSave;
	PAVIFILE m_pfileForSave;
	BITMAPINFOHEADER m_BmInfoForSaveAvi;
	int m_nSaveAviW, m_nSaveAviH;
	CRect m_nSaveAviRect;
	int m_nSaveAviOption;
	int m_nSaveAviFps;
	CScreenImage m_ScreenImage;
	int m_nTimerAviSaveNum;

	bool m_bConnected;
	
	bool AVIFileInitFunction();

// Dialog Data
	//{{AFX_DATA(CMainProcessDlg)
	enum { IDD = IDD_MAIN_PROCESS };
	CComboBox	m_SeqTypeCtrl;
	CButton	m_SourceType0Ctrl;
	CButton	m_SourceType1Ctrl;
	CButton	m_SourceType2Ctrl;
	CEdit	m_GwCtrl;
	CEdit	m_GhCtrl;
	CListBox	m_ListCtrl;
	CSliderCtrl	m_SleepCtrl;
	CListCtrl	m_FileListCtrl;
	int		m_nStart;
	int		m_nEnd;
	int		m_nGW;
	int		m_nGH;
	int		m_nFileType;
	int		m_nGap;
	int		m_nSleep;
	int		m_nSourceType;
	BOOL	m_bSubFolder;
	BOOL	m_bStep;
	int		m_nSeqType;
	BOOL	m_bSaveScreen;
	int		m_nPtCntOfCurrRegoin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainProcessDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMainProcessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPause();
	afx_msg void OnSelFolder2();
	afx_msg void OnGrab();
	afx_msg void OnDestroy();
	afx_msg void OnVs();
	afx_msg void OnCamOn();
	afx_msg void OnResetGNum();
	afx_msg void OnCustomdrawSleep(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelSource();
	afx_msg void OnRun();
	afx_msg void OnSaveScreen();
	afx_msg void OnDrawRectRegion();
	afx_msg void OnDrawRegion();
	afx_msg void OnSaveRegion();
	afx_msg void OnReadRegion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINPROCESSDLG_H__9D0BB200_9920_4FE1_84D5_8B823B5BAA47__INCLUDED_)
