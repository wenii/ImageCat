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
	//SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 50, LWA_ALPHA);
	//SetWindowPos(&wndTop, m_rc.left, m_rc.top, m_rc.right, m_rc.bottom, SWP_SHOWWINDOW);
	//修改鼠标贯标形状
	//SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_CROSS));

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
		::ScreenToClient(this->GetParent()->GetSafeHwnd(), &clientPoint);
		memDC.BitBlt(clientPoint.x, clientPoint.y, width, height, m_memCDC, clientPoint.x, clientPoint.y, SRCCOPY);

		// 画线
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 3, RGB(32, 128, 240));
			CPen* pOldPen = memDC.SelectObject(&pen);
			CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
			CBrush* pOlBrush = memDC.SelectObject(pBrush);
			CPoint clientCurPoint(m_curPoint);
			::ScreenToClient(this->GetParent()->GetSafeHwnd(), &m_curPoint);
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
			::ScreenToClient(this->GetParent()->GetSafeHwnd(), &m_curPoint);
			//memDC.
			int r1 = 10;
			//memDC.Ellipse(clientPoint.x - r1, clientPoint.y - r1, clientPoint.x   + r1, clientPoint.y + r1);//使用Ellipse画第1个半径r的圆
			//memDC.Rectangle(clientPoint.x, clientPoint.y, m_curPoint.x, m_curPoint.y);
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
	m_firstPoint = point;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMaskDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nFlags & MK_LBUTTON)
	{
		m_curPoint = point;
		Invalidate();
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
	
}


BOOL CMaskDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}
