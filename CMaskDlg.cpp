// CMaskDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImageCat.h"
#include "CMaskDlg.h"
#include "afxdialogex.h"
#include <math.h>

#define HOTKEY_ID_COPY_COLOR 998

// CMaskDlg 对话框

IMPLEMENT_DYNAMIC(CMaskDlg, CDialogEx)

CMaskDlg::CMaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MASK, pParent)
	, m_finishCut(false)
	, m_state(STATE_BEGIN)
	, m_startResize(false)
	, m_resizeDirect(ADJUST_NONE)
	, m_inBoxPoint(-1, -1)
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
	ON_MESSAGE(WM_USER_MESSAGE_SAVE_TO_MEM, OnSaveToMem)
	ON_MESSAGE(WM_USER_MESSAGE_SAVE_TO_FILE, OnSaveToFile)
	ON_MESSAGE(WM_USER_MESSAGE_PIN, OnPin)
	ON_MESSAGE(WM_USER_MESSAGE_DRAW, OnUserDraw)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CMaskDlg::snapshot()
{
	CDC screenDC;
	screenDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	if (m_memCDC.m_hDC == NULL)
	{
		m_memCDC.CreateCompatibleDC(&screenDC);
	}
	m_screenDCbitmap.DeleteObject();
	m_screenDCbitmap.CreateCompatibleBitmap(&screenDC, m_screenWidth, m_screenHeight);
	m_memCDC.SelectObject(m_screenDCbitmap);

	m_memCDC.BitBlt(0, 0, m_screenWidth, m_screenHeight, &screenDC, 0, 0, SRCCOPY);
}

// CMaskDlg 消息处理程序


BOOL CMaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_toolbarDlg.Create(IDD_DIALOG_CUT_TOOL_BAR, this);
	m_toolbarDlg.ShowWindow(SW_HIDE);

	::RegisterHotKey(m_hWnd, HOTKEY_ID_COPY_COLOR, 0, 'C');//注册全局快捷键 c

	m_screenWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMaskDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	if (m_memCDC.m_hDC != NULL)
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

		// 画背景
		memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &m_memCDC, 0, 0, SRCCOPY);

		// 画遮罩
		Gdiplus::Graphics graphics(memDC.m_hDC);
		Gdiplus::SolidBrush brush(Gdiplus::Color(200, 0, 0, 0));
		Gdiplus::Rect screenRect(rect.TopLeft().x, rect.TopLeft().y, rect.Width(), rect.Height());
		graphics.FillRectangle(&brush, screenRect);
		

		// 画框选区域
		CRect selectedRect(m_firstPoint.x, m_firstPoint.y, m_curPoint.x, m_curPoint.y);
		int width = selectedRect.Width();
		int height = selectedRect.Height();
		memDC.BitBlt(m_firstPoint.x, m_firstPoint.y, width, height, &m_memCDC, m_firstPoint.x, m_firstPoint.y, SRCCOPY);

		// 画线
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 3, RGB(32, 128, 240));
			CPen* pOldPen = memDC.SelectObject(&pen);
			CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
			CBrush* pOlBrush = memDC.SelectObject(pBrush);
			CPoint clientCurPoint(m_curPoint);
			memDC.Rectangle(m_firstPoint.x, m_firstPoint.y, m_curPoint.x, m_curPoint.y);
			memDC.SelectObject(pOldPen);
			memDC.SelectObject(pOlBrush);
		}

		// 画圈
		{
			Gdiplus::Pen white(Gdiplus::Color(255, 255, 255, 255), 1.6f);
			Gdiplus::SolidBrush blueBrush(Gdiplus::Color(255, 32, 128, 240));
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

			CPoint circlePoseArray[8] = {
				m_firstPoint,
				CPoint(m_firstPoint.x + width / 2, m_firstPoint.y),
				CPoint(m_firstPoint.x + width, m_firstPoint.y),
				CPoint(m_firstPoint.x, m_firstPoint.y + height / 2),
				CPoint(m_firstPoint.x + width, m_firstPoint.y + height / 2),
				CPoint(m_firstPoint.x, m_firstPoint.y + height),
				CPoint(m_firstPoint.x + width / 2, m_firstPoint.y + height),
				CPoint(m_firstPoint.x + width, m_firstPoint.y + height)
			};
			for (int i = 0; i < 8; i++)
			{
				CPoint p = circlePoseArray[i];
				graphics.FillEllipse(&blueBrush, Gdiplus::Rect(p.x - 4, p.y - 4, 8, 8));
				graphics.DrawEllipse(&white, p.x - 5, p.y - 5, 10, 10);
			}
		}

		// 画放大框
		
		{
			CPoint cursorPoint(-1, -1);
			if (m_state == STATE_BOX_SELECT)
				cursorPoint = m_curPoint;
			else if (m_state == STATE_BOX_ADJUST)
				cursorPoint = m_inBoxPoint;
			if (cursorPoint.x > 0)
			{
				const int poseOffset = 8;
				const int colorPointBoxWidth = 50;
				const int magnifierBoxWidth = 150;
				const int centerRectWidth = 10;
				memDC.StretchBlt(cursorPoint.x + poseOffset, cursorPoint.y + poseOffset, magnifierBoxWidth, magnifierBoxWidth, &m_memCDC, cursorPoint.x - colorPointBoxWidth / 2, cursorPoint.y - colorPointBoxWidth / 2, colorPointBoxWidth, colorPointBoxWidth, SRCCOPY);
				Gdiplus::Pen white(Gdiplus::Color(255, 255, 255, 255), 1);
				Gdiplus::Pen blank(Gdiplus::Color(255, 0, 0, 0), 1);
				Gdiplus::Rect rect(cursorPoint.x + poseOffset, cursorPoint.y + poseOffset, magnifierBoxWidth, magnifierBoxWidth);
				// 画描边
				graphics.DrawRectangle(&blank, rect);
				Gdiplus::Rect rectblank(cursorPoint.x + poseOffset + 1, cursorPoint.y + poseOffset + 1, magnifierBoxWidth - 2, magnifierBoxWidth - 2);
				graphics.DrawRectangle(&white, rectblank);

				Gdiplus::Pen white2(Gdiplus::Color(255, 255, 255, 255), 1);

				// 画中心区域框
				graphics.DrawRectangle(&blank, Gdiplus::Rect(cursorPoint.x + (magnifierBoxWidth - centerRectWidth) / 2 + poseOffset, cursorPoint.y + (magnifierBoxWidth - centerRectWidth) / 2 + poseOffset, centerRectWidth, centerRectWidth));
				graphics.DrawRectangle(&white, Gdiplus::Rect(cursorPoint.x + (magnifierBoxWidth - centerRectWidth) / 2 + poseOffset + 1, cursorPoint.y + (magnifierBoxWidth - centerRectWidth) / 2 + poseOffset + 1, centerRectWidth - 2, centerRectWidth - 2));

				// 画十字线
				
				Gdiplus::Pen littleBlue(Gdiplus::Color(255, 178, 211, 250), 9);
				const int crossLeftBeginX = cursorPoint.x + poseOffset + 1;
				const int crossLeftEndX = cursorPoint.x + (magnifierBoxWidth - centerRectWidth) / 2 + poseOffset - 1;
				const int crossLeftY = cursorPoint.y + magnifierBoxWidth / 2 + poseOffset;
				const int crossLeftlength = crossLeftEndX - crossLeftBeginX;
				const int shorLineLength = crossLeftlength / 10;
				graphics.DrawLine(&littleBlue, Gdiplus::Point(crossLeftBeginX, crossLeftY), Gdiplus::Point(crossLeftEndX, crossLeftY));

				const int crossRightBeginX = cursorPoint.x + (magnifierBoxWidth + centerRectWidth) / 2 + poseOffset + 1;
				const int crossRightEndX = cursorPoint.x + magnifierBoxWidth + poseOffset - 1;
				const int crossRightY = cursorPoint.y + magnifierBoxWidth / 2 + poseOffset;
				const int crossRightLength = crossRightEndX - crossRightBeginX;
				graphics.DrawLine(&littleBlue, Gdiplus::Point(crossRightBeginX, crossRightY), Gdiplus::Point(crossRightEndX, crossRightY));
				
				const int crossTopX = cursorPoint.x + magnifierBoxWidth / 2 + poseOffset;
				const int crossTopBeginY = cursorPoint.y + poseOffset + 1;
				const int crossTopEndY = cursorPoint.y + (magnifierBoxWidth - centerRectWidth) / 2 + poseOffset - 1;
				const int crossTopLength = crossTopEndY - crossTopBeginY;
				graphics.DrawLine(&littleBlue, Gdiplus::Point(crossTopX, crossTopBeginY), Gdiplus::Point(crossTopX, crossTopEndY));

				const int crossBottomX = cursorPoint.x + magnifierBoxWidth / 2 + poseOffset;
				const int crossBottomBeginY = cursorPoint.y + (magnifierBoxWidth + centerRectWidth) / 2 + poseOffset + 1;
				const int crossBottomEndY = cursorPoint.y + magnifierBoxWidth + poseOffset - 1;
				const int crossBottomLength = crossBottomEndY - crossBottomBeginY;
				graphics.DrawLine(&littleBlue, Gdiplus::Point(crossBottomX, crossBottomBeginY), Gdiplus::Point(crossBottomX, crossBottomEndY));

				// 画下面文字背景
				const int textBgX = cursorPoint.x + poseOffset;
				const int textBgY = cursorPoint.y + poseOffset + magnifierBoxWidth;
				const int textBgWidth = magnifierBoxWidth;
				const int textBgHeight = magnifierBoxWidth / 3;

				Gdiplus::SolidBrush brush(Gdiplus::Color(220, 0, 0, 0));
				Gdiplus::Rect textBbRect(textBgX, textBgY, textBgWidth, textBgHeight);
				graphics.FillRectangle(&brush, textBbRect);


				const int colorShowX = textBgX + 20;
				const int colorShowY = textBgY + textBgHeight / 4;

				// 画颜色方框
				const int selectColorRegionWidth = centerRectWidth / 3;
				memDC.StretchBlt(colorShowX, colorShowY, centerRectWidth, centerRectWidth, &m_memCDC, cursorPoint.x - selectColorRegionWidth / 2, cursorPoint.y - selectColorRegionWidth / 2, selectColorRegionWidth, selectColorRegionWidth, SRCCOPY);
				graphics.DrawRectangle(&white2, Gdiplus::Rect(colorShowX, colorShowY, centerRectWidth, centerRectWidth));

				int colorValueX = colorShowX + centerRectWidth + 10;
				int colorValueY = colorShowY - 5;

				// 获取RGB
				COLORREF rgb = ::GetPixel(m_memCDC.GetSafeHdc(), cursorPoint.x, cursorPoint.y);
				int r = GetRValue(rgb);
				int g = GetGValue(rgb);
				int b = GetBValue(rgb);
				std::cout << "R:" << r << " G:" << g << " B:" << b << std::endl;
				CString strR;
				strR.Format(_T("%d,"), r);
				CString strG;
				strG.Format(_T("%d,"), g);
				CString strB;
				strB.Format(_T("%d"), b);

				m_colorRGB.Format(_T("rgb(%d,%d,%d)"), r, g, b);

				// 画rgb值文字
				Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
				Gdiplus::FontFamily fontf(_T("微软雅黑"));
				Gdiplus::Font font(&fontf, 10);
				graphics.DrawString(strR, strR.GetLength(), &font, Gdiplus::PointF(colorValueX, colorValueY), &textBrush);
				graphics.DrawString(strG, strG.GetLength(), &font, Gdiplus::PointF(colorValueX + 30, colorValueY), &textBrush);
				graphics.DrawString(strB, strB.GetLength(), &font, Gdiplus::PointF(colorValueX + 60, colorValueY), &textBrush);

				// 画提示文字
				CString tips = _T("按c复制颜色值");
				graphics.DrawString(tips, tips.GetLength(), &font, Gdiplus::PointF(colorValueX - 5, colorValueY + 20), &textBrush);
			}
		}

		// 画尺寸
		{
			
			CString sizeText;
			sizeText.Format(_T("%d x %d"), abs(selectedRect.Width()), abs(selectedRect.Height()));
			Gdiplus::FontFamily fontf(_T("微软雅黑"));
			Gdiplus::Font font(&fontf, 10);
			Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
			Gdiplus::RectF rcf;
			graphics.MeasureString(sizeText, sizeText.GetLength(), &font, Gdiplus::PointF(0, 0), &rcf);

			const int width = rcf.Width;
			const int height = rcf.Height;

			const int sizeWidth = width + 8;
			const int sizeHeight = height + 8;
			const int sizeX = m_firstPoint.x;
			const int sizeY = m_firstPoint.y - sizeHeight - 5;
			Gdiplus::Rect textBbRect(sizeX, sizeY, sizeWidth, sizeHeight);
			Gdiplus::SolidBrush brush(Gdiplus::Color(220, 0, 0, 0));
			graphics.FillRectangle(&brush, textBbRect);

			graphics.DrawString(sizeText, sizeText.GetLength(), &font, Gdiplus::PointF(sizeX + 4, sizeY + 4), &textBrush);
		}

		dc.BitBlt(0, 0, rectWidth, rectHeight, &memDC, 0, 0, SRCCOPY);
	}
}


void CMaskDlg::boxChanged()
{
	if (m_state == STATE_BOX_ADJUST)
	{
		// 修改toolbar 位置
		int toolbarWidth = m_toolbarDlg.getToolbarWidth();
		int toolbarHeight = m_toolbarDlg.getToolbarHeight();
		int x = m_curPoint.x - toolbarWidth;
		int y = m_curPoint.y + 10;
		m_toolbarDlg.SetWindowPos(NULL, x, y, toolbarWidth, toolbarHeight, 0);
		m_toolbarDlg.ShowWindow(SW_SHOW);
	}
	Invalidate();
}

void CMaskDlg::reset()
{
	m_firstPoint = CPoint(0, 0);
	m_curPoint = CPoint(0, 0);
	m_state = STATE_BEGIN;
	m_resizeDirect = ADJUST_NONE;
	Invalidate();
}

CRect CMaskDlg::getBoxRect()
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
	else if (m_firstPoint.x > m_curPoint.x&& m_firstPoint.y < m_curPoint.y)
	{
		leftPoint = CPoint(m_curPoint.x, m_firstPoint.y);
		rightPoint = CPoint(m_firstPoint.x, m_curPoint.y);
	}
	else if (m_firstPoint.x > m_curPoint.x&& m_firstPoint.y > m_curPoint.y)
	{
		leftPoint = m_curPoint;
		rightPoint = m_firstPoint;
	}
	return CRect(leftPoint, rightPoint);
}

void CMaskDlg::fillBoxImage(CDC* cdc)
{
	CRect boxRect = getBoxRect();
	int width = boxRect.Width();
	int height = boxRect.Height();

	cdc->CreateCompatibleDC(&m_memCDC);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&m_memCDC, width, height);
	cdc->SelectObject(&bitmap);
	cdc->BitBlt(0, 0, width, height, &m_memCDC, boxRect.left , boxRect.top, SRCCOPY);
	bitmap.Detach();
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
	CRect boxRect = getBoxRect();
	CPoint leftPoint = boxRect.TopLeft();
	CPoint rightPoint = boxRect.BottomRight();

	if (m_state == STATE_BOX_ADJUST)
	{
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
				boxChanged();
			}
			m_inBoxPoint = point;
			Invalidate();
		}
		else
		{
			std::cout << "out of box region." << std::endl;
			m_inBoxPoint = CPoint(-1, -1);
			Invalidate();

			if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP_LEFT)
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
					boxChanged();
				}
			}
			else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM_RIGHT)
				|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x&& point.y > rightPoint.y)/*右下角*/)
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
					boxChanged();
				}
			}
			else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP_RIGHT)
				|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x&& point.y < leftPoint.y)/*右上角*/)
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
					boxChanged();
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
					boxChanged();
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
					boxChanged();
				}
			}
			else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_LEFT)
				|| (m_resizeDirect == ADJUST_NONE && point.x < leftPoint.x) && (point.y > leftPoint.y&& point.y < rightPoint.y)/*左边*/)
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
					boxChanged();
				}
			}
			else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP)
				|| (m_resizeDirect == ADJUST_NONE && point.y < leftPoint.y && point.x > leftPoint.x&& point.x < rightPoint.x)/*上边*/)
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
					boxChanged();
				}
			}
			else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM)
				|| (m_resizeDirect == ADJUST_NONE && point.y > rightPoint.y&& point.x > leftPoint.x&& point.x < rightPoint.x)/*下边*/)
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
					boxChanged();
				}
			}
		}
	}
	else if (m_state == STATE_BOX_DRAW)
	{
		if (point.x > leftPoint.x&& point.x < rightPoint.x && point.y > leftPoint.y&& point.y < rightPoint.y)
		{
			// 十字拖动图标
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZEALL));
		}
		else
		{
			SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_NO));
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
	
	SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_CROSS));
	if (!bShow)
	{
		m_toolbarDlg.ShowWindow(SW_HIDE);
		SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));
	}
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
			boxChanged();
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

LRESULT CMaskDlg::OnSaveToMem(WPARAM wParam, LPARAM lParam)
{
	std::cout << "OnSaveToMem" << std::endl;
	CDC cdc;
	fillBoxImage(&cdc);
	if (OpenClipboard())
	{
		//清空剪切板
		EmptyClipboard();
		//存入位图
		SetClipboardData(CF_BITMAP, cdc.GetCurrentBitmap()->m_hObject);
		CloseClipboard();
	}
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_CUT_QUIT, 0, 0);
	return 0;
}



LRESULT CMaskDlg::OnSaveToFile(WPARAM wParam, LPARAM lParam)
{
	std::cout << "OnSaveToFile" << std::endl;

	CTime tm;
	tm = CTime::GetCurrentTime();
	CString strTime = tm.Format("%Y%m%d%H%M%S");
	CString strDefName = _T("screenshot") + strTime;
	CString filter = _T(".bmp||.jpg||.jpeg||.png||");
	CString formatArray[5] = { _T(".jpg"), _T(".bmp"), _T(".jpg"), _T(".jpeg"), _T(".png") };
	CFileDialog dlg(FALSE, NULL, LPCTSTR(strDefName), OFN_HIDEREADONLY, filter);

	if (dlg.DoModal() == IDOK)
	{
		CString name = dlg.GetPathName();
		CString suffix = formatArray[dlg.m_ofn.nFilterIndex];

		CDC cdc;
		fillBoxImage(&cdc);
		CImage img;
		img.Attach((HBITMAP)cdc.GetCurrentBitmap()->GetSafeHandle());
		img.Save(name + suffix);
	}
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_CUT_QUIT, 0, 0);
	return 0;

}

LRESULT CMaskDlg::OnPin(WPARAM wParam, LPARAM lParam)
{
	std::cout << "OnPin" << std::endl;
	CRect boxRect = getBoxRect();
	int width = boxRect.Width();
	int height = boxRect.Height();
	std::cout << "----width:" << width << "height:" << height << std::endl;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN); //获取屏幕水平分辨率
	int screenHeight = GetSystemMetrics(SM_CYSCREEN); //获取屏幕垂直分辨率
	CPinDlg* pinDlg = new CPinDlg(width, height);
	pinDlg->Create(IDD_DIALOG_PIN_BACK, GetDesktopWindow());
	fillBoxImage(pinDlg->getCDC());
	::SetWindowPos(pinDlg->GetSafeHwnd(), HWND_TOPMOST, boxRect.left, boxRect.top, width, height, SWP_SHOWWINDOW);
	pinDlg->SetActiveWindow();
	pinDlg->Invalidate();
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_CUT_QUIT, 0, 0);

	return 0;

}

LRESULT CMaskDlg::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	std::cout << "OnUserDraw" << std::endl;
	if (m_state == STATE_BOX_ADJUST)
	{
		m_state = STATE_BOX_DRAW;
	}
	return 0;
}

HRESULT CMaskDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if (wParam == HOTKEY_ID_COPY_COLOR)
	{
		std::cout << "key----------HOTKEY_ID_COPY_COLOR" << std::endl;
		
		if (OpenClipboard())
		{
			//清空剪切板
			EmptyClipboard();
			//存入字符串
			HGLOBAL clipbuffer;
			char* buffer;
			EmptyClipboard();

			USES_CONVERSION;
			std::string strColor(W2A(m_colorRGB)); //CString-->std::string

			clipbuffer = GlobalAlloc(GMEM_DDESHARE, strColor.length() + 1);//strSrc为string类型 他就是要放在剪贴板上的内容
			buffer = (char*)GlobalLock(clipbuffer);
			strcpy_s(buffer, strColor.length() + 1, strColor.c_str());
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);

			CloseClipboard();
		}
		
	}
	
	return TRUE;
}


