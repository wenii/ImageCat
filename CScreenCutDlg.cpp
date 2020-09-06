// CScreenCutDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImageCat.h"
#include "CScreenCutDlg.h"
#include "afxdialogex.h"


// CScreenCutDlg 对话框

IMPLEMENT_DYNAMIC(CScreenCutDlg, CDialogEx)

CScreenCutDlg::CScreenCutDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SCREEN, pParent)
	, m_screenWidth(0)
	, m_screenHeight(0)
{

}

CScreenCutDlg::~CScreenCutDlg()
{
}

void CScreenCutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenCutDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CScreenCutDlg::snapshot()
{
	CDC screenDC;
	screenDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	if (m_screenMemDC.m_hDC == NULL)
	{
		m_screenMemDC.CreateCompatibleDC(&screenDC);
	}
	m_screenDCbitmap.DeleteObject();
	m_screenDCbitmap.CreateCompatibleBitmap(&screenDC, m_screenWidth, m_screenHeight);
	m_screenMemDC.SelectObject(m_screenDCbitmap);

	m_screenMemDC.BitBlt(0, 0, m_screenWidth, m_screenHeight, &screenDC, 0, 0, SRCCOPY);

}

CDC* CScreenCutDlg::getScreenMemDC()
{
	return &m_screenMemDC;
}

// CScreenCutDlg 消息处理程序


BOOL CScreenCutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
		// 获取屏幕尺寸
	m_screenWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CScreenCutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	dc.BitBlt(0, 0, m_screenWidth, m_screenHeight, &m_screenMemDC, 0, 0, SRCCOPY);
}




BOOL CScreenCutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
