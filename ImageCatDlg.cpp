
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
END_MESSAGE_MAP()


// CImageCatDlg 对话框



CImageCatDlg::CImageCatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGECAT_DIALOG, pParent)
	, m_expandRatio(1.0f)
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
	CPaintDC dc(this); // device context for painting

	CRect		rect;
	GetClientRect(&rect);
	CImage		image;
	image.Load(_T("_0YR8175.jpg"));
	
	int orgImageWidth = image.GetWidth();
	int orgImageHeight = image.GetHeight();
	
	int imageWidth = image.GetWidth();
	int imageHeight = image.GetHeight();
	int rectWidth = rect.Width();
	int rectHeight = rect.Height();



	if (imageWidth <= rectWidth && imageHeight <= rectHeight)
	{
		// 绘制在中间
		int screenOrgX =(rectWidth - imageWidth) / 2;
		int screenOrgY =(rectHeight - imageHeight) / 2;
		image.BitBlt(dc, screenOrgX, screenOrgY);
		//image.StretchBlt(dc, screenOrgX, screenOrgY, rect.Width(), rect.Height(), 0, 0, image.GetWidth(), image.GetHeight());
	}
	else if (imageWidth > rectWidth || imageHeight > rectHeight)
	{
		float ratio = 0.0f;
		if (imageWidth > imageHeight) 
		{
			if (imageWidth > imageHeight)
			ratio = rectWidth / (float)imageWidth;
		}
		else 
		{
			ratio = rectHeight / (float)imageHeight;
		}
		

		imageHeight = imageHeight * ratio * m_expandRatio;
		imageWidth = imageWidth * ratio * m_expandRatio;
		int screenOrgX = (rectWidth - imageWidth) / 2;
		int screenOrgY = (rectHeight - imageHeight) / 2; 

		CDC memDC;
		CBitmap memBitmap;
		memDC.CreateCompatibleDC(NULL);
		memBitmap.CreateCompatibleBitmap(&dc, rectWidth, rectHeight);
		CBitmap* pOldBit = memDC.SelectObject(&memBitmap);
		memDC.FillSolidRect(0, 0, rectWidth, rectHeight, RGB(255, 255, 255));
		
		SetStretchBltMode(memDC, STRETCH_HALFTONE);
		image.StretchBlt(memDC, screenOrgX, screenOrgY, imageWidth, imageHeight, 0, 0, image.GetWidth(), image.GetHeight());
		dc.BitBlt(0, 0, rectWidth, rectHeight, &memDC, 0, 0, SRCCOPY);
		memBitmap.DeleteObject();
		memDC.DeleteDC();
	}
	
	image.Destroy();    //没有Destroy()会有内存泄漏。Detach();不行的
}



void CImageCatDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	Invalidate();
	// TODO: 在此处添加消息处理程序代码
	
}


BOOL CImageCatDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "[OnMouseWheel]  x:" << pt.x << " y:" << pt.y << " zDelta:" << zDelta << " nFlags:" << nFlags << std::endl;

	const int maxDelta = 12000;
	static int delta = 0;
	delta += zDelta;
	if (delta > maxDelta)
	{
		delta = maxDelta;
	}
	else if (delta < 0)
	{
		delta = 0;
	}

	if ((delta >= 0 && delta <= maxDelta)) 
	{
		m_expandRatio = delta / float(maxDelta) * 8.0f + 1.0;
	} 
	else
	{
		m_expandRatio = 1.0f;
	}

	std::cout << "delta:" << delta << " expandRatio:" << m_expandRatio << std::endl;
	//Invalidate(FALSE);
	CRect		rect;
	GetClientRect(&rect);
	InvalidateRect(rect);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CImageCatDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "[OnMouseMove]  x:" << point.x << " y:" << point.y << " nFlags:" << nFlags << std::endl;
	CDialogEx::OnMouseMove(nFlags, point);
}




BOOL CImageCatDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}
