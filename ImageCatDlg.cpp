
// ImageCatDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ImageCat.h"
#include "ImageCatDlg.h"
#include "afxdialogex.h"

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
END_MESSAGE_MAP()


// CImageCatDlg 对话框



CImageCatDlg::CImageCatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGECAT_DIALOG, pParent)
	, m_expandRatio(1.0f)
	, m_ctrlKeyPress(false)
	, m_curentImageIndex(0)
	, m_delta(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageCatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageCatDlg, CDialogEx)
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
	m_image.Load(m_imagePath);

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
		drawImage();
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImageCatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 绘制图片

void CImageCatDlg::drawImage()
{
	if (m_imagePath == _T("")) 
	{
		return;
	}
	CPaintDC dc(this); // device context for painting

	CRect		rect;
	GetClientRect(&rect);
	
	int orgImageWidth = m_image.GetWidth();
	int orgImageHeight = m_image.GetHeight();
	
	int imageWidth = m_image.GetWidth();
	int imageHeight = m_image.GetHeight();
	int rectWidth = rect.Width();
	int rectHeight = rect.Height();

	{
		float ratio = 1.0f;
		if (imageWidth > imageHeight) 
		{
			if (imageWidth > rectWidth)
			ratio = rectWidth / (float)imageWidth;
		}
		else 
		{
			if(imageHeight > rectHeight)
			ratio = rectHeight / (float)imageHeight;
		}
		

		imageHeight = imageHeight * ratio * m_expandRatio;
		imageWidth = imageWidth * ratio * m_expandRatio;
		int screenOrgX = (rectWidth - imageWidth) / 2;
		int screenOrgY = (rectHeight - imageHeight) / 2; 


		CPoint offset = m_curMoveOffset;

		if (screenOrgX < 0) {
			screenOrgX += offset.x;
		}
		if (screenOrgY < 0) {
			screenOrgY += offset.y;
		}

		CDC memDC;
		CBitmap memBitmap;
		memDC.CreateCompatibleDC(NULL);
		memBitmap.CreateCompatibleBitmap(&dc, rectWidth, rectHeight);
		CBitmap* pOldBit = memDC.SelectObject(&memBitmap);
		memDC.FillSolidRect(0, 0, rectWidth, rectHeight, RGB(255, 255, 255));  
		
		SetStretchBltMode(memDC, STRETCH_HALFTONE);
		m_image.StretchBlt(memDC, screenOrgX, screenOrgY, imageWidth, imageHeight, 0, 0, m_image.GetWidth(), m_image.GetHeight());
		dc.BitBlt(0, 0, rectWidth, rectHeight, &memDC, 0, 0, SRCCOPY);
		memBitmap.DeleteObject();
		memDC.DeleteDC();
	}
}

bool CImageCatDlg::isFileFormatImage(CString fileName)
{
	CString suffix = fileName.Right(fileName.GetLength() - fileName.ReverseFind('.'));
	if (suffix == ".png"
		|| suffix == ".jpg"
		|| suffix == ".png"
		|| suffix == ".bmp"
		|| suffix == ".gif")
	{
		return true;
	}
	return false;
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
			if (isFileFormatImage(fileName))
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
				if (isFileFormatImage(fileName))
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
	m_curentImageIndex++;
	const int index = m_curentImageIndex % imageCount;
	m_imagePath = m_ImageNameArray[index];
	SetWindowText(m_imagePath);
	m_expandRatio = 1.0f;
	m_curMoveOffset = CPoint(0, 0);
	m_delta = 0;
	m_image.Destroy();
	m_image.Load(m_imagePath);
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
	m_expandRatio = 1.0f;
	m_curMoveOffset = CPoint(0, 0);
	m_delta = 0;
	m_image.Destroy();
	m_image.Load(m_imagePath);
}



void CImageCatDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType == SIZE_RESTORED)
	{
		int iWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
		int iHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率
		CRect   rcTemp;
		rcTemp.BottomRight() = CPoint(iWidth / 8 * 7, iHeight / 8 * 7);
		rcTemp.TopLeft() = CPoint(iWidth / 8, iHeight / 8);
		MoveWindow(&rcTemp);
		std::cout << "CImageCatDlg::onSize size restored." << std::endl;
	}

	Invalidate();
}


BOOL CImageCatDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "[OnMouseWheel]  x:" << pt.x << " y:" << pt.y << " zDelta:" << zDelta << " nFlags:" << nFlags << std::endl;
	if (nFlags & MK_CONTROL)
	{
		const int maxDelta = 12000;
		m_delta += zDelta;
		if (m_delta > maxDelta)
		{
			m_delta = maxDelta;
		}
		else if (m_delta < 0)
		{
			m_delta = 0;
		}

		if ((m_delta >= 0 && m_delta <= maxDelta))
		{
			m_expandRatio = m_delta / float(maxDelta) * 8.0f + 1.0;
		}
		else
		{
			m_expandRatio = 1.0f;
		}

		std::cout << "delta:" << m_delta << " expandRatio:" << m_expandRatio << std::endl;
		Invalidate();
	}
	else
	{
		if (zDelta < 0)
		{
			nextImage();
		}
		else
		{
			prevImage();
		}
		
		Invalidate();
	}
	
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CImageCatDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nFlags & MK_LBUTTON) {
		m_curMoveOffset += point - m_curMousePoint;
		m_curMousePoint = point;
		Invalidate();
	}
	CDialogEx::OnMouseMove(nFlags, point);
}




BOOL CImageCatDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}


void CImageCatDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	std::cout << "onLButtonDown::x:" << point.x << " y:" << point.y << std::endl;
	m_curMousePoint = point;
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
	m_expandRatio = 1.0f;
	m_curMoveOffset = CPoint(0, 0);
	m_delta = 0;
	Invalidate();
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
			Invalidate();
		}
		else if (pMsg->wParam == VK_UP)
		{
			prevImage();
			Invalidate();
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
