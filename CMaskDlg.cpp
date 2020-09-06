// CMaskDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImageCat.h"
#include "CMaskDlg.h"
#include "afxdialogex.h"


// CMaskDlg 对话框

IMPLEMENT_DYNAMIC(CMaskDlg, CDialogEx)

CMaskDlg::CMaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MASK, pParent)
	, m_memCDC(NULL)
	, m_finishCut(false)
	, m_state(STATE_BEGIN)
	, m_startResize(false)
	, m_resizeDirect(ADJUST_NONE)
{

}

CMaskDlg::~CMaskDlg()
{
}

void CMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMaskDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CMaskDlg::setCDC(CDC* cdc)
{
	m_memCDC = cdc;
}

// CMaskDlg 消息处理程序


BOOL CMaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	::SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(m_hWnd, 0, 200, LWA_ALPHA); // 120是透明度，范围是0～255

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMaskDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	if (m_memCDC->m_hDC != NULL)
	{
		CRect rect;
		GetClientRect(&rect);

		int rectWidth = rect.Width();
		int rectHeight = rect.Height();

		CDC memDC;
		CBitmap memBitmap;
		memDC.CreateCompatibleDC(NULL);
		memBitmap.CreateCompatibleBitmap(&dc, rectWidth, rectHeight);
		CBitmap* pOldBit = memDC.SelectObject(&memBitmap);
		memDC.FillSolidRect(0, 0, rectWidth, rectHeight, RGB(0, 0, 0));

		CRect selectedRect(m_firstPoint.x, m_firstPoint.y, m_curPoint.x, m_curPoint.y);
		int width = selectedRect.Width();
		int height = selectedRect.Height();
		CPoint clientPoint(m_firstPoint);
		memDC.BitBlt(clientPoint.x, clientPoint.y, width, height, m_memCDC, clientPoint.x, clientPoint.y, SRCCOPY);

		// 画线
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 3, RGB(32, 128, 240));
			CPen* pOldPen = memDC.SelectObject(&pen);
			CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
			CBrush* pOlBrush = memDC.SelectObject(pBrush);
			CPoint clientCurPoint(m_curPoint);
			memDC.Rectangle(clientPoint.x, clientPoint.y, m_curPoint.x, m_curPoint.y);
			memDC.SelectObject(pOldPen);
			memDC.SelectObject(pOlBrush);
		}

		// 画圈
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
			CPen* pOldPen = memDC.SelectObject(&pen);
			CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
			CBrush* pOlBrush = memDC.SelectObject(pBrush);
			CPoint clientCurPoint(m_curPoint);
			{
				Gdiplus::Graphics graphics(memDC.m_hDC);
				Gdiplus::Pen white(Gdiplus::Color(255, 255, 255, 255), 2);
				Gdiplus::Pen blue(Gdiplus::Color(255, 32, 128, 240), 4);
				graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

				CPoint circlePoseArray[8] = {
					clientPoint,
					CPoint(clientPoint.x + width / 2, clientPoint.y),
					CPoint(clientPoint.x + width, clientPoint.y),
					CPoint(clientPoint.x, clientPoint.y + height / 2),
					CPoint(clientPoint.x + width, clientPoint.y + height / 2),
					CPoint(clientPoint.x, clientPoint.y + height),
					CPoint(clientPoint.x + width / 2, clientPoint.y + height),
					CPoint(clientPoint.x + width, clientPoint.y + height)
				};
				for (int i = 0; i < 8; i++)
				{
					CPoint p = circlePoseArray[i];
					graphics.DrawEllipse(&white, p.x - 5, p.y - 5, 10, 10);
					graphics.DrawEllipse(&blue, p.x - 2, p.y - 2, 4, 4);
					
				}

			}

			memDC.SelectObject(pOldPen);
			memDC.SelectObject(pOlBrush);
		}

		dc.BitBlt(0, 0, rectWidth, rectHeight, &memDC, 0, 0, SRCCOPY);
	}
}


void CMaskDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "onLButtonDown:point.x:" << point.x << "point.y:" << point.y << std::endl;
	if (m_state == STATE_BEGIN)
	{
		std::cout << "STATE_BEGIN -> STATE_BOX_SELECT" << std::endl;
		m_state = STATE_BOX_SELECT;
		m_firstPoint = point;
	}
	m_moveBeginPoint = point;
	
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMaskDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_state == STATE_BOX_SELECT)
	{
		if (nFlags & MK_LBUTTON)
		{
			m_curPoint = point;
			Invalidate();
		}
	}
	
	std::cout << "CMaskDlg::OnMouseMove state:" << m_state << std::endl;

	if (m_state == STATE_BOX_ADJUST)
	{
		CPoint leftPoint = m_firstPoint;
		CPoint rightPoint = m_curPoint;
		if (m_firstPoint.x < m_curPoint.x && m_firstPoint.y < m_curPoint.y)
		{
			leftPoint = m_firstPoint;
			rightPoint = m_curPoint;
		}
		else if (m_firstPoint.x < m_curPoint.x && m_firstPoint.y > m_curPoint.y)
		{
			leftPoint = CPoint(m_firstPoint.x, m_curPoint.y);
			rightPoint = CPoint(m_curPoint.x, m_firstPoint.y);
		}
		else if (m_firstPoint.x > m_curPoint.x && m_firstPoint.y < m_curPoint.y)
		{
			leftPoint = CPoint(m_curPoint.x, m_firstPoint.y);
			rightPoint = CPoint(m_firstPoint.x, m_curPoint.y);
		}
		else if (m_firstPoint.x > m_curPoint.x&& m_firstPoint.y > m_curPoint.y)
		{
			leftPoint = m_curPoint;
			rightPoint = m_firstPoint;
		}
		std::cout << "left.x:" << leftPoint.x << " left.y:" << leftPoint.y << "right.x:" << rightPoint.x << "right.y:" << rightPoint.y << std::endl;

		// 根据当前鼠标位置确定鼠标形状
		if ((m_resizeDirect == ADJUST_MOVE)
			|| (m_resizeDirect == ADJUST_NONE && point.x > leftPoint.x && point.x < rightPoint.x && point.y > leftPoint.y && point.y < rightPoint.y))
		{
			// 十字拖动图标
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZEALL));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_MOVE;
				}
				CPoint offset = point - m_moveBeginPoint;
				leftPoint += offset;
				rightPoint += offset;
				m_firstPoint = leftPoint;
				m_curPoint = rightPoint;
				m_moveBeginPoint = point;
				Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP_LEFT)
			|| (m_resizeDirect == ADJUST_NONE && (point.x < leftPoint.x && point.y < leftPoint.y))/*左上角*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENWSE));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_TOP_LEFT;
				}
				m_firstPoint = point;
				m_curPoint = rightPoint;
				Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM_RIGHT)
			|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x && point.y > rightPoint.y)/*右下角*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENWSE));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_BOTTOM_RIGHT;
				}
				m_firstPoint = leftPoint;
				m_curPoint = point;
				Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP_RIGHT)
			|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x && point.y < leftPoint.y)/*右上角*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENESW));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_TOP_RIGHT;
				}
				leftPoint.y = point.y;
				rightPoint.x = point.x;
				m_firstPoint = leftPoint;
				m_curPoint = rightPoint;
				Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM_LEFT)
			|| (m_resizeDirect == ADJUST_NONE && point.x < leftPoint.x && point.y > rightPoint.y)/*左下角*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENESW));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_BOTTOM_LEFT;
				}
				leftPoint.x = point.x;
				rightPoint.y = point.y;
				m_firstPoint = leftPoint;
				m_curPoint = rightPoint;
				Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_RIGHT)
			|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x) && (point.y > leftPoint.y&& point.y < rightPoint.y)/*右边*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZEWE));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_RIGHT;
				}
				rightPoint.x = point.x;
				m_firstPoint = leftPoint;
				m_curPoint = rightPoint;
				Invalidate();
			}
		}
		else if((m_resizeDirect == ADJUST_RESIZE_DIRECT_LEFT)
			|| (m_resizeDirect == ADJUST_NONE && point.x < leftPoint.x) && (point.y > leftPoint.y && point.y < rightPoint.y)/*左边*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZEWE));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_LEFT;
				}
				leftPoint.x = point.x;
				m_firstPoint = leftPoint;
				m_curPoint = rightPoint;
				Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP)
			|| (m_resizeDirect == ADJUST_NONE && point.y < leftPoint.y && point.x > leftPoint.x && point.x < rightPoint.x)/*上边*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENS));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_TOP;
				}
				leftPoint.y = point.y;
				m_firstPoint = leftPoint;
				m_curPoint = rightPoint;
				Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM)
			|| (m_resizeDirect == ADJUST_NONE && point.y > rightPoint.y && point.x > leftPoint.x && point.x < rightPoint.x)/*下边*/)
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENS));
			if (nFlags & MK_LBUTTON)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_BOTTOM;
				}
				rightPoint.y = point.y;
				m_firstPoint = leftPoint;
				m_curPoint = rightPoint;
				Invalidate();
			}
		}
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
	
}


BOOL CMaskDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}


void CMaskDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	Invalidate();
	// TODO: 在此处添加消息处理程序代码
}


BOOL CMaskDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMaskDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	m_firstPoint = CPoint(0, 0);
	m_curPoint = CPoint(0, 0);
	m_state = STATE_BEGIN;
	m_resizeDirect = ADJUST_NONE;
	SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_CROSS));
}


void CMaskDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_state == STATE_BOX_SELECT)
	{
		if (m_curPoint != m_firstPoint)
		{
			std::cout << "STATE_BOX_SELECT -> STATE_BOX_ADJUST" << std::endl;
			m_state = STATE_BOX_ADJUST;
		}
		else
		{
			std::cout << "STATE_BOX_SELECT -> STATE_BEGIN" << std::endl;
			m_state = STATE_BEGIN;
		}
	}
	else if (m_state == STATE_BOX_ADJUST)
	{
		if (m_resizeDirect != ADJUST_NONE)
		{
			m_resizeDirect = ADJUST_NONE;
		}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}
