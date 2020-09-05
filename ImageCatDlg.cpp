
// ImageCatDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ImageCat.h"
#include "ImageCatDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
END_MESSAGE_MAP()



// CImageCatDlg 对话框
CImageCatDlg::CImageCatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGECAT_DIALOG, pParent)
	, m_curentImageIndex(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageCatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageCatDlg, CDialogEx)
	ON_COMMAND(ID_TOOL_BAR_BTN_SAVE, onToolbarBtnSave)
	ON_COMMAND(ID_TOOL_BAR_BTN_DELETE, onToolbarBtnDelete)
	ON_COMMAND(ID_TOOL_BAR_BTN_ROTATE_CCW, onToolbarBtnRotateCCW)
	ON_COMMAND(ID_TOOL_BAR_BTN_ROTATE_CW, onToolbarBtnRotateCW)
	ON_COMMAND(ID_TOOL_BAR_BTN_CUT, onToolbarBtnRotateCut)
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
	ON_WM_NCHITTEST()
	ON_WM_MOVE()
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

	// 初始化工具栏
	initToolbar();

	// 初始化画布
	m_canvas.Create(IDD_DIALOG_CANVAS, NULL);
	m_canvas.ShowWindow(SW_SHOW);

	// 初始化蒙板
	m_mask.Create(IDD_DIALOG_MASK, this);
	m_mask.ShowWindow(SW_HIDE);
	m_mask.setCDC(m_canvas.getMemDC());

	CString arg = getCommandLineArg();
	if (arg == _T(""))
	{
		return FALSE;
	}

	m_imagePath = arg;
	SetWindowText(m_imagePath);
	CString path = m_imagePath.Left(m_imagePath.ReverseFind('\\'));
	storageAllImageNameFromPath(path);
	setCurrentImageIndex();

	if (isSupportFileFormatImage(m_imagePath))
	{
		// 加载图片
		m_canvas.loadImage(m_imagePath);

		// 最大化窗口
		ShowWindow(SW_MAXIMIZE);
	}
	else
	{
		MessageBox(_T("不支持的图片格式！"), _T("提示"), MB_OK);
		exit(0);
	}

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

void CImageCatDlg::onToolbarBtnSave()
{
	std::cout << "CImageCatDlg::onToolbarBtnSave" << std::endl;
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
	std::cout << "CImageCatDlg::onToolbarBtnDelete" << std::endl;
	if (MessageBox(_T("是否要删除文件？"), _T("删除"), MB_ICONEXCLAMATION | MB_OKCANCEL) == IDOK)
	{
		std::cout << "删除了文件:" << m_imagePath << std::endl;
		DeleteFile(m_imagePath);
		nextImage();
	}
}

void CImageCatDlg::onToolbarBtnRotateCCW()
{
	std::cout << "CImageCatDlg::onToolbarBtnRotateCCW" << std::endl;
	m_canvas.rotation(-90);
}

void CImageCatDlg::onToolbarBtnRotateCW()
{
	std::cout << "CImageCatDlg::onToolbarBtnRotateCW" << std::endl;
	m_canvas.rotation(90);
}
void CImageCatDlg::onToolbarBtnRotateCut()
{
	std::cout << "CImageCatDlg::onToolbarBtnRotateCW" << std::endl;
	CRect		rect;
	GetClientRect(&rect);
	CRect screenRect = rect;
	ClientToScreen(&screenRect);
	m_mask.SetWindowPos(&m_canvas, screenRect.TopLeft().x, screenRect.TopLeft().y, screenRect.Width(), screenRect.Height() - m_toolbarHeight, 0);
	m_mask.ShowWindow(SW_SHOW);

	// 保存当前客户去图像

}



bool CImageCatDlg::isSupportFileFormatImage(CString fileName)
{
	CString suffix = fileName.Right(fileName.GetLength() - fileName.ReverseFind('.'));
	if (suffix == ".png"
		|| suffix == ".jpg"
		|| suffix == ".bmp"
		|| suffix == ".gif"
		|| suffix == ".jpeg")
	{
		return true;
	}
	return false;
}

void CImageCatDlg::initToolbar()
{
	if (m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS))
	{

		static UINT BASED_CODE DockTool[] = { ID_TOOL_BAR_BTN_SAVE, ID_TOOL_BAR_BTN_DELETE, ID_TOOL_BAR_BTN_ROTATE_CCW , ID_TOOL_BAR_BTN_ROTATE_CW, ID_TOOL_BAR_BTN_CUT };
		CBitmap bitmap;
		bitmap.LoadBitmapW(IDB_BITMAP_SAVE);
		m_toolbarlist.Create(32, 32, ILC_COLOR24, 0, 0);
		m_toolbarlist.Add(&bitmap, (CBitmap*)NULL);
		/*CBitmap bitmapPin;
		bitmapPin.LoadBitmapW(IDB_BITMAP_PIN);
		m_toolbarlist.Add(&bitmapPin, (CBitmap*)NULL);*/
		CBitmap bitmapDelete;
		bitmapDelete.LoadBitmapW(IDB_BITMAP_DELETE);
		m_toolbarlist.Add(&bitmapDelete, (CBitmap*)NULL);
		CBitmap bitmapRotateCCW;
		bitmapRotateCCW.LoadBitmapW(IDB_BITMAP_ROTATE_CCW);
		m_toolbarlist.Add(&bitmapRotateCCW, (CBitmap*)NULL);
		CBitmap bitmapRotateCW;
		bitmapRotateCW.LoadBitmapW(IDB_BITMAP_ROTATE_CW);
		m_toolbarlist.Add(&bitmapRotateCW, (CBitmap*)NULL);
		CBitmap bitmapCut;
		bitmapCut.LoadBitmapW(IDB_BITMAP_CUT);
		m_toolbarlist.Add(&bitmapCut, (CBitmap*)NULL);

		//设置工具栏按钮图片
		m_toolBar.GetToolBarCtrl().SetImageList(&m_toolbarlist);
		//设置工具栏按钮大小， 和按钮中位图大小
		SIZE sbutton, sImage;
		sbutton.cx = 48;
		sbutton.cy = 48;
		sImage.cx = 32;
		sImage.cy = 32;
		m_toolBar.SetSizes(sbutton, sImage);
		m_toolBar.SetButtons(DockTool, (UINT)5);

		m_toolbarWidth = 5 * 48;
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
		bool fileAttribute = GetFileAttributes(fullPath) & FILE_ATTRIBUTE_DIRECTORY;
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
			bool fileAttribute = GetFileAttributes(fullPath) & FILE_ATTRIBUTE_DIRECTORY;
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
		CRect toolBarRect;
		m_toolBar.GetWindowRect(&toolBarRect);
		m_toolBar.SetWindowPos(NULL, toolbarX, toolbarY, m_toolbarWidth, m_toolbarHeight, 0);
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
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_DOWN)
		{
			std::cout << "onKeyDown --------------" << std::endl;
			nextImage();
		}
		else if (pMsg->wParam == VK_UP)
		{
			prevImage();
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


LRESULT CImageCatDlg::OnNcHitTest(CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CDialogEx::OnNcHitTest(point);
}


void CImageCatDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	if (m_mask.GetSafeHwnd() != NULL)
	{
		CRect		rect;
		GetClientRect(&rect);
		CRect screenRect = rect;
		ClientToScreen(&screenRect);
		m_mask.SetWindowPos(NULL, screenRect.TopLeft().x, screenRect.TopLeft().y, screenRect.Width(), screenRect.Height() - m_toolbarHeight, 0);
	}
}
