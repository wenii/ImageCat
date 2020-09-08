// CPinDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImageCat.h"
#include "CPinDlg.h"
#include "afxdialogex.h"
#include "resource.h"

// CPinDlg 对话框

IMPLEMENT_DYNAMIC(CPinDlg, CDialogEx)

CPinDlg::CPinDlg(int width, int height, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PIN_BACK, pParent)
	, m_width(width)
	, m_height(height)
	, m_lastRect(0, 0, 0, 0)
{

}

CPinDlg::~CPinDlg()
{
}

void CPinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPinDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MENU_PIN_DESTROY, &CPinDlg::OnMenuItemPinDestroy)
END_MESSAGE_MAP()


// CPinDlg 消息处理程序


void CPinDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	

	if (m_cdc.GetSafeHdc() != NULL)
	{
		dc.BitBlt(0, 0, m_width, m_height, &m_cdc, 0, 0, SRCCOPY);
	}
}


void CPinDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
}

CDC* CPinDlg::getCDC()
{
	return &m_cdc;
}


void CPinDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nFlags & MK_LBUTTON)
	{
		int dx = point.x - m_lbtnDownPoint.x;
		int dy = point.y - m_lbtnDownPoint.y;
		m_lastRect.left += dx;
		m_lastRect.right += dx;
		m_lastRect.top += dy;
		m_lastRect.bottom += dy;             //获取新的位置  
		this->MoveWindow(&m_lastRect);     //将窗口移到新的位置  
		//this->SetWindowPos(NULL, m_lastRect.left, m_lastRect.top, m_width, m_height, 0);
		
		std::cout << "width:" << m_width << "height:" << m_height << std::endl;
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPinDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_lbtnDownPoint = point;
	GetWindowRect(&m_lastRect);
	CDialogEx::OnLButtonDown(nFlags, point);
}



void CPinDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ClientToScreen(&point);
	CMenu *pSubMenu = m_MenuTrack.GetSubMenu(0);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	CDialogEx::OnRButtonDown(nFlags, point);
}


BOOL CPinDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_MenuTrack.LoadMenu(IDR_MENU_PIN);

	//SetClassLong(this->m_hWnd, GCL_STYLE, GetClassLong(this->m_hWnd, GCL_STYLE) | CS_DROPSHADOW);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CPinDlg::OnMenuItemPinDestroy()
{
	// TODO: 在此添加命令处理程序代码
	std::cout << "CPinDlg::OnMenuItemPinDestroy" << std::endl;
	ShowWindow(SW_HIDE);
	delete this;
}


LRESULT CPinDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	LRESULT lrst = CDialogEx::WindowProc(message, wParam, lParam);
	if (!::IsWindow(m_hWnd))
		return lrst;

	if (message == WM_MOVE || message == WM_PAINT || message == WM_NCPAINT || message == WM_NCACTIVATE || message == WM_NOTIFY)
	{
		CDC* pWinDC = GetWindowDC();
		if (pWinDC)
			DrawBorder(pWinDC);
		ReleaseDC(pWinDC);
	}
	return lrst;
}

// 重画边框的具体代码
void CPinDlg::DrawBorder(CDC* pDC)
{
	if (m_hWnd)
	{
		CBrush Brush(RGB(32, 128, 240));
		CBrush* pOldBrush = pDC->SelectObject(&Brush);

		CRect rtWnd;
		GetWindowRect(&rtWnd);

		CPoint point;
		//填充顶部框架
		point.x = rtWnd.Width();
		point.y = GetSystemMetrics(SM_CYFRAME) - 3;
		pDC->PatBlt(0, 0, point.x, point.y, PATCOPY);
		//填充左侧框架
		point.x = GetSystemMetrics(SM_CXFRAME) - 3;
		point.y = rtWnd.Height();
		pDC->PatBlt(0, 0, point.x, point.y, PATCOPY);
		//填充底部框架
		point.x = rtWnd.Width();
		point.y = GetSystemMetrics(SM_CYFRAME) - 3;
		pDC->PatBlt(0, rtWnd.Height() - point.y, point.x, point.y, PATCOPY);
		//填充右侧框架
		point.x = GetSystemMetrics(SM_CXFRAME) - 3;
		point.y = rtWnd.Height();
		pDC->PatBlt(rtWnd.Width() - point.x, 0, point.x, point.y, PATCOPY);
	}
}
