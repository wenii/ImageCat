﻿
// ImageCatDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ImageCat.h"
#include "ImageCatDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HOTKEY_ID_CUT 999
#define HOTKEY_ID_ESC 888

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CImageCatDlg 对话框
CImageCatDlg::CImageCatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGECAT_DIALOG, pParent)
	, m_curentImageIndex(0)
	, m_toolbarWidth(0)
	, m_toolbarHeight(0)
	, m_screenWidth(0)
	, m_screenHeight(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageCatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageCatDlg, CDialogEx)
	ON_COMMAND(ID_TOOL_BAR_BTN_OPEN, onToolbarBtnOpen)
	ON_COMMAND(ID_TOOL_BAR_BTN_SAVE, onToolbarBtnSave)
	ON_COMMAND(ID_TOOL_BAR_BTN_DELETE, onToolbarBtnDelete)
	ON_COMMAND(ID_TOOL_BAR_BTN_MIRROR, onToolbarBtnMirror)
	ON_COMMAND(ID_TOOL_BAR_BTN_ROTATE_CW, onToolbarBtnRotateCW)
	ON_COMMAND(ID_TOOL_BAR_BTN_CUT, onToolbarBtnCut)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_USER_MESSAGE_CUT_QUIT, OnCutQuit)
	ON_MESSAGE(WM_USER_MESSAGE_NEXT_IMAGE, OnNextImage)
	ON_MESSAGE(WM_USER_MESSAGE_PREV_IMAGE, OnPrevImage)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOVE()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnDisplay)
END_MESSAGE_MAP()


// CImageCatDlg 消息处理程序

BOOL CImageCatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	::RegisterHotKey(m_hWnd, HOTKEY_ID_CUT, MOD_WIN | MOD_ALT | MOD_CONTROL, NULL);//注册全局快捷键 CTRL + WIN + ALT
	::RegisterHotKey(m_hWnd, HOTKEY_ID_ESC, MOD_ALT, VK_F1);//注册全局快捷键 CTRL + WIN + ALT
	// 获取屏幕尺寸
	m_screenWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率

	// 初始化工具栏
	initToolbar();

	// 初始化画布
	m_canvas.Create(IDD_DIALOG_CANVAS, this);
	m_canvas.ShowWindow(SW_SHOW);

	// 初始化蒙板
	m_mask.Create(IDD_DIALOG_MASK, this);
	m_mask.ShowWindow(SW_HIDE);


	CString arg = getCommandLineArg();
	if (arg != _T(""))
	{
		m_imagePath = arg;
		SetWindowText(m_imagePath);
		CString path = m_imagePath.Left(m_imagePath.ReverseFind('\\'));
		storageAllImageNameFromPath(path);
		setCurrentImageIndex();

		if (isSupportFileFormatImage(m_imagePath))
		{
			// 加载图片
			m_canvas.loadImage(m_imagePath);
		}
		else
		{
			MessageBox(_T("不支持的图片格式！"), _T("提示"), MB_OK);
		}
	}

	// 最大化窗口
	ShowWindow(SW_MAXIMIZE);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CImageCatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CImageCatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//drawImage();
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImageCatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImageCatDlg::onToolbarBtnOpen()
{
	std::cout << "CImageCatDlg::onToolbarBtnOpen" << std::endl;
	CString filter = _T(".jpg||.jpeg||.bmp||.png||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString name = dlg.GetPathName();
		m_imagePath = name;
		SetWindowText(m_imagePath);
		CString path = m_imagePath.Left(m_imagePath.ReverseFind('\\'));
		m_ImageNameArray.clear();
		storageAllImageNameFromPath(path);
		setCurrentImageIndex();

		if (isSupportFileFormatImage(m_imagePath))
		{
			// 加载图片
			m_canvas.loadImage(m_imagePath);
		}
		else
		{
			MessageBox(_T("不支持的图片格式！"), _T("提示"), MB_OK);
		}
	}
}


void CImageCatDlg::onToolbarBtnSave()
{
	std::cout << "CImageCatDlg::onToolbarBtnSave" << std::endl;
	if (m_imagePath == _T(""))
	{
		return;
	}

	//保存功能
	CString file = m_imagePath.Right(m_imagePath.GetLength() - m_imagePath.ReverseFind(_T('\\')) - 1);
	CString fileName = file.Left(file.ReverseFind(_T('.')));
	CString fileSuffix = file.Right(file.GetLength() - file.ReverseFind(_T('.')) - 1);
	CString formatArray[5] = { _T(".jpg"), _T(".jpg"), _T(".jpeg"), _T(".bmp"), _T(".png") };
	CString filter = _T(".jpg||.jpeg||.bmp||.png||");
	if (fileSuffix == _T(".jpg"))
	{
		filter = _T(".jpg||.jpeg||.bmp||.png||");
	}
	else if (fileSuffix == _T("jpeg"))
	{
		filter = _T(".jpeg||.jpg||.bmp||.png||");
		formatArray[1] = _T(".jpeg");
		formatArray[2] = _T(".jpg");
		formatArray[3] = _T(".bmp");
		formatArray[4] = _T(".png");
	}
	else if (fileSuffix == _T("bmp"))
	{
		filter = _T(".bmp||.jpg||.jpeg||.png||");
		formatArray[1] = _T(".bmp");
		formatArray[2] = _T(".jpg");
		formatArray[3] = _T(".jpeg");
		formatArray[4] = _T(".png");
	}
	else if (fileSuffix == _T("png"))
	{
		filter = _T(".png||.jpg||.jpeg||.bmp||");
		formatArray[1] = _T(".png");
		formatArray[2] = _T(".jpg");
		formatArray[3] = _T(".jpeg");
		formatArray[4] = _T(".bmp");
	}

	CFileDialog dlg(FALSE, NULL, LPCTSTR(fileName), OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString name = dlg.GetPathName();
		CString suffix = formatArray[dlg.m_ofn.nFilterIndex];

		m_canvas.saveFile(name + suffix);
	}
}

void CImageCatDlg::onToolbarBtnDelete()
{
	if (m_imagePath != _T(""))
	{
		std::cout << "CImageCatDlg::onToolbarBtnDelete" << std::endl;
		if (MessageBox(_T("是否要删除文件？"), _T("删除"), MB_ICONEXCLAMATION | MB_OKCANCEL) == IDOK)
		{
			std::cout << "删除了文件:" << m_imagePath << std::endl;
			DeleteFile(m_imagePath);
			CString needDeletFile = m_imagePath;
			nextImage();
			auto itr = std::find(m_ImageNameArray.begin(), m_ImageNameArray.end(), needDeletFile);
			if (itr != m_ImageNameArray.end())
			{
				m_ImageNameArray.erase(itr);
			}
		}
	}
}

void CImageCatDlg::onToolbarBtnMirror()
{
	std::cout << "CImageCatDlg::onToolbarBtnMirror" << std::endl;
	m_canvas.mirror();
}

void CImageCatDlg::onToolbarBtnRotateCW()
{
	std::cout << "CImageCatDlg::onToolbarBtnRotateCW" << std::endl;
	m_canvas.rotation(CCanvasDlg::ROTATE_DIR_CW);
}
void CImageCatDlg::onToolbarBtnCut()
{
	std::cout << "CImageCatDlg::onToolbarBtnRotateCW" << std::endl;
	cutImage();
}


void CImageCatDlg::cutImage()
{
	m_mask.reset();
	m_mask.snapshot();
	m_mask.SetWindowPos(NULL, 0, 0, m_screenWidth, m_screenHeight, 0);
	m_mask.ShowWindow(SW_SHOW);
}

void CImageCatDlg::quitCutImage()
{
	m_mask.reset();
	m_mask.ShowWindow(SW_HIDE);
}

HRESULT CImageCatDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == HOTKEY_ID_CUT)
	{
		std::cout << "key----------HOTKEY_ID_CUT" << std::endl;
		cutImage();
	}
	else if (wParam == HOTKEY_ID_ESC)
	{
		std::cout << "key..........HOTKEY_ID_ESC...." << std::endl;
		//m_screen.ShowWindow(SW_HIDE);
		m_mask.ShowWindow(SW_HIDE);
	}
	return TRUE;
}



bool CImageCatDlg::isSupportFileFormatImage(CString fileName)
{
	CString suffix = fileName.Right(fileName.GetLength() - fileName.ReverseFind('.'));
	if (suffix == ".png"
		|| suffix == ".jpg"
		|| suffix == ".bmp"
		|| suffix == ".gif"
		|| suffix == ".jpeg"
		|| suffix == ".PNG"
		|| suffix == ".JPG"
		|| suffix == ".BMP"
		|| suffix == ".JPEG"
		|| suffix == ".GIF")
	{
		return true;
	}
	return false;
}

void CImageCatDlg::initToolbar()
{
	if (m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS))
	{
		m_toolBar.EnableToolTips();
		static UINT BASED_CODE DockTool[] = { ID_TOOL_BAR_BTN_OPEN, ID_TOOL_BAR_BTN_SAVE, ID_TOOL_BAR_BTN_DELETE, ID_TOOL_BAR_BTN_MIRROR , ID_TOOL_BAR_BTN_ROTATE_CW, ID_TOOL_BAR_BTN_CUT };

		m_toolbarlist.Create(32, 32, ILC_COLOR32, 0, 0);
		m_toolbarlist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_OPEN)));
		m_toolbarlist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_SAVE)));
		m_toolbarlist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_DELETE)));
		m_toolbarlist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_MIRROR)));
		m_toolbarlist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_ROTATE_RIGHT)));
		m_toolbarlist.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_CUT)));

		//设置工具栏按钮图片
		m_toolBar.GetToolBarCtrl().SetImageList(&m_toolbarlist);

		//设置工具栏按钮大小， 和按钮中位图大小
		SIZE sbutton, sImage;
		sbutton.cx = 48;
		sbutton.cy = 48;
		sImage.cx = 32;
		sImage.cy = 32;
		m_toolBar.SetSizes(sbutton, sImage);
		m_toolBar.SetButtons(DockTool, (UINT)6);

		m_toolbarWidth = 6 * 48;
		m_toolbarHeight = 48;
	}
}



CString CImageCatDlg::getCommandLineArg()
{
	CString arg = _T("");
	LPWSTR* szArglist = NULL;
	int nArgs = 0;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL != szArglist)
	{
		//szArglist就是保存参数的数组
		//nArgs是数组中参数的个数
		//数组的第一个元素表示进程的path，也就是szArglist[0]，其他的元素依次是输入参数。
		std::cout << szArglist << std::endl;
		if (nArgs == 2)
		{
			arg = szArglist[1];
		}
		LocalFree(szArglist);
	}
	
	return arg;
}

void CImageCatDlg::storageAllImageNameFromPath(CString path)
{
	CString filtPath = path +  "\\*.*";
	HANDLE file;
	WIN32_FIND_DATA fileData;
	file = FindFirstFile(filtPath.GetBuffer(), &fileData);
	if (file != INVALID_HANDLE_VALUE)
	{
		CString fileName = fileData.cFileName;
		CString fullPath = path + "\\" + fileName;
		DWORD attribute = GetFileAttributes(fullPath);
		bool fileAttribute = (attribute & FILE_ATTRIBUTE_DIRECTORY) || (attribute & FILE_ATTRIBUTE_HIDDEN);
		if (!fileAttribute)
		{
			if (isSupportFileFormatImage(fileName))
			{
				m_ImageNameArray.push_back(fullPath);
			}
		}
		while (FindNextFile(file, &fileData))
		{
			CString fileName = fileData.cFileName;
			fullPath = path + "\\" + fileName;
			DWORD attribute = GetFileAttributes(fullPath);

			bool fileAttribute = (attribute & FILE_ATTRIBUTE_DIRECTORY) || (attribute & FILE_ATTRIBUTE_HIDDEN);
			if (!fileAttribute)
			{
				if (isSupportFileFormatImage(fileName))
				{
					m_ImageNameArray.push_back(fullPath);
				}
			}
		}
	}
}

void CImageCatDlg::setCurrentImageIndex()
{
	int imageCount = m_ImageNameArray.size();
	for (int i = 0; i < imageCount; ++i)
	{
		if (m_ImageNameArray[i] == m_imagePath)
		{
			m_curentImageIndex = i;
			break;
		}
	}
}

void CImageCatDlg::nextImage()
{
	const int imageCount = m_ImageNameArray.size();
	if (imageCount == 0) 
	{
		return;
	}
	m_curentImageIndex++;
	const int index = m_curentImageIndex % imageCount;
	m_imagePath = m_ImageNameArray[index];
	SetWindowText(m_imagePath);
	m_canvas.loadImage(m_imagePath);
}

void CImageCatDlg::prevImage()
{
	const int imageCount = m_ImageNameArray.size();
	if (imageCount == 0)
	{
		return;
	}

	if (m_curentImageIndex == 0)
	{
		m_curentImageIndex = imageCount - 1;
	}
	else
	{
		m_curentImageIndex--;
	}
	
	const int index = m_curentImageIndex % imageCount;
	m_imagePath = m_ImageNameArray[index];
	SetWindowText(m_imagePath);
	m_canvas.loadImage(m_imagePath);
} 



void CImageCatDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect		rect;
	GetClientRect(&rect);
	if (m_toolBar.GetSafeHwnd() != NULL)
	{
		int	toolbarX = (rect.Width() - m_toolbarWidth) / 2;
		int toolbarY = rect.Height() - m_toolbarHeight;
		m_toolBar.SetWindowPos(NULL, toolbarX, toolbarY, m_toolbarWidth, m_toolbarHeight, SWP_SHOWWINDOW);
		m_toolBar.Invalidate();
	}
	if (m_canvas.GetSafeHwnd() != NULL)
	{
		m_canvas.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height() - m_toolbarHeight, 0);
	}
	if (m_mask.GetSafeHwnd() != NULL)
	{
		CRect screenRect = rect;
		ClientToScreen(&screenRect);
		m_mask.SetWindowPos(NULL, screenRect.TopLeft().x, screenRect.TopLeft().y, screenRect.Width(), screenRect.Height() - m_toolbarHeight, 0);
	}
}


BOOL CImageCatDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "[OnMouseWheel]  x:" << pt.x << " y:" << pt.y << " zDelta:" << zDelta << " nFlags:" << nFlags << std::endl;
	
	if (!(nFlags & MK_CONTROL))
	{
		if (zDelta < 0)
		{
			nextImage();
		}
		else
		{
			prevImage();
		}
		
	}
	
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CImageCatDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnMouseMove(nFlags, point);
}




BOOL CImageCatDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CDialogEx::OnEraseBkgnd(pDC);
}


void CImageCatDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	std::cout << "onLButtonDown::x:" << point.x << " y:" << point.y << std::endl;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CImageCatDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "OnLButtonUp::x:" << point.x << " y:" << point.y << std::endl;
	CDialogEx::OnLButtonUp(nFlags, point);
}




void CImageCatDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "OnLButtonDblClk------------" << std::endl;
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


BOOL CImageCatDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	std::cout << "PreTranslateMessage" << std::endl;
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			std::cout << "onKeyDown ----- vk_escape" << std::endl;
			return true;
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}



void CImageCatDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
}

LRESULT CImageCatDlg::OnCutQuit(WPARAM wParam, LPARAM lParam)
{
	std::cout << "OnCancel" << std::endl;
	quitCutImage();
	return 0;
}

LRESULT CImageCatDlg::OnNextImage(WPARAM wParam, LPARAM lParam)
{
	nextImage();
	return 0;
}

LRESULT CImageCatDlg::OnPrevImage(WPARAM wParam, LPARAM lParam)
{
	prevImage();
	return 0;
}

BOOL CImageCatDlg::OnDisplay(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXT* pTTT = (NMTTDISPINFO*)pNMHDR;
	UINT nID = pNMHDR->idFrom;//获取工具栏按钮ID
	if (nID)
	{
		nID = m_toolBar.CommandToIndex(nID); //根据ID获取按钮索引
		if (nID != -1)
		{
			m_toolbarTips.LoadStringW(IDS_STRING_OPEN + nID);
			pTTT->lpszText = (LPWSTR)(LPCTSTR)m_toolbarTips; //设置提示信息文本
			pTTT->hinst = AfxGetResourceHandle();
		}
	}
	return TRUE;
}


void CAboutDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}
