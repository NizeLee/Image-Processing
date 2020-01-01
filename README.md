# Image-Processing
* Image Processing (Windows)
* C/C++ development tool for image processing, computer vision and machine learning

## Introduction
* Open program sources (without OpenCV)
* Visual Studio 2019, 2017, 2015, 2013 and lower 
* GUI: MFC
* Win32 and x64

## Source codes
* Bitmap load and save (cpp), jpeg load and save (lib)
* Format conversion (RGB, YUV420, YUV422, YUV444)
* Low level image processing (spatial filtering (including fast versions), frequency transform (DFT, FFT, DCT), ...)
* Geometric transformation (affine, bilinear, perspective, ...)
* Simple operations (Hough, boundary following, labeling, thinning, morphological operations, ...)
* Drawing functions for text (Windows), line, circle and ellipse
* ROI setting (AABB and polygons)
* Web camera processing
* Image sequence saving
* Easy debugging (image, text, listbox)
* **Sequential processing**
  + Image sequences (bmp, jpeg, pcx, ppm, png, raw)
  + Avi sequences
  + YUV sequences
<br><br> <img src="Image_processing_01.gif"></img>  
* **Multi-layer perceptron (MLP)**
* **Avi recording**
  + Examples: [pupil segmentqtion](https://sites.google.com/site/khuaris/home/pupil-segmentation), [video stitching](https://sites.google.com/site/khuaris/home/video-stitching)

## Example code for sequential processing
1. Add a new class
2. Add a new processing method (1D array image - BGR, 24bit, 4Byte aligned) 
3. Add an instance member to CMainProcessDlg class (MainProcessDlg.h)
4. Call the processing method as follows: 
``` C++
void CMainProcessDlg::OnTimer(UINT_PTR nIDEvent) {
    ...
			else if(nIDEvent == TID_SEQUENCE_RUN) {
				Sleep(m_nSleep);
				...
				else if(FileName.Right(4) == _T(".JPG")) {
					Image1D = ReadJpeg(mbcsFileName, &nW, &nH);
				}

				if(Image1D) {
					// Processing
					DisplayCimage1D(Image1D, nW, nH, 0, 0, false, true);
					// Instance.NewMethodExample(Image1D, nW, nH); // call 
					//
					m_bFirstFrame = false;
					delete [] Image1D;
				}
			
				CString DispFrameNum;
				DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nSequenceLength-1);
				SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

				m_nCurrentProcessingNum++;
			}
  ...
}
```
## Visual Studio
* Visual Studio 2019, 2017, 2015, 2013 and lower 
  + MFC libraries are required
   - If the MFC libraries were not installed,
    1. Run 'Visual Studio Installer' by selecting the menu: [Tools]-[Get Tools and Features...]
    2. Check [Desktop development with C++] in Workloads tab 
    3. Check [MFC and ATL support(x86 and x64)] in Summary
    4. Click [Modify] button
  + Project setting
<br> <img src="Image_processing_02.png"></img>  
* Visual Studio 2013 and lower 
  + Replace JpegLib.lib, JpegLibD.lib, JpegLib_64.lib and JpegLibD_64.lib with those in Libs(2013)
