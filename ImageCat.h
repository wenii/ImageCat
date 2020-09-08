
// ImageCat.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号

#include <Gdiplus.h> //引入头函数 
#pragma comment(lib, "Gdiplus.lib")  // 引入链接库
// CImageCatApp:
// 有关此类的实现，请参阅 ImageCat.cpp
//

class CImageCatApp : public CWinApp
{
public:
	CImageCatApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	ULONG_PTR m_gdiplusToken;

// 实现

	DECLARE_MESSAGE_MAP()
};
class CPinDlg;
class CPinApp : public CWinApp
{
public:
	CPinApp(CPinDlg* dlg);
public:
	CPinDlg* m_pinDlg;
public:
	virtual BOOL InitInstance();
};

extern CImageCatApp theApp;
