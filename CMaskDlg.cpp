// CMaskDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImageCat.h"
#include "CMaskDlg.h"
#include "afxdialogex.h"
#include <math.h>

#define HOTKEY_ID_COPY_COLOR 998


State::State(CMaskDlg* target)
	: m_isLButtonDown(false)
	, m_target(target)
{
	m_target->setState(this);
}


void State::switchToNextState(int state)
{
	switch (state)
	{
	case STATE_BEGIN:
		onNextState(new BeginState(m_target));
		delete this;
		break;
	case STATE_BOX_SELECT:
		onNextState(new BoxSelectState(m_target));
		delete this;
		break;
	case STATE_BOX_SELECT_COMPLETE:
		onNextState(new BoxSelectCompleteState(m_target));
		delete this;
		break;
	case STATE_DRAW_LINE:
		onNextState(new DrawLineState(m_target));
		delete this;
		break;
	default:
		ASSERT(FALSE);
	}
}

void State::draw()
{
	CPaintDC dc(m_target);
	CDC* imageMemDC = m_target->getImageMemDC();

	CRect rect;
	::GetClientRect(m_target->GetSafeHwnd(), &rect);

	int rectWidth = rect.Width();
	int rectHeight = rect.Height();
	  
	CDC drawDC;
	CBitmap memBitmap;
	drawDC.CreateCompatibleDC(NULL);
	memBitmap.CreateCompatibleBitmap(&dc, rectWidth, rectHeight);
	CBitmap* pOldBit = drawDC.SelectObject(&memBitmap);

	// 画背景
	drawDC.BitBlt(0, 0, rect.Width(), rect.Height(), imageMemDC, 0, 0, SRCCOPY);

	// 画遮罩
	Gdiplus::Graphics graphics(drawDC.m_hDC);
	Gdiplus::SolidBrush brush(Gdiplus::Color(200, 0, 0, 0));
	Gdiplus::Rect screenRect(rect.TopLeft().x, rect.TopLeft().y, rect.Width(), rect.Height());
	graphics.FillRectangle(&brush, screenRect);

	onDraw(&drawDC, imageMemDC);

	dc.BitBlt(0, 0, rectWidth, rectHeight, &drawDC, 0, 0, SRCCOPY);
}

void State::drawBox(CDC* drawDC, CDC* imageMemDC)
{
	// 画框选区域
	int width = m_boxRect.Width();
	int height = m_boxRect.Height();
	if (width == 0 || height == 0)
	{
		return;
	}

	drawDC->BitBlt(m_boxRect.TopLeft().x, m_boxRect.TopLeft().y, width, height, imageMemDC, m_boxRect.TopLeft().x, m_boxRect.TopLeft().y, SRCCOPY);

	// 画线
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 3, RGB(32, 128, 240));
		CPen* pOldPen = drawDC->SelectObject(&pen);
		CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CBrush* pOlBrush = drawDC->SelectObject(pBrush);
		drawDC->Rectangle(m_boxRect.TopLeft().x, m_boxRect.TopLeft().y, m_boxRect.BottomRight().x, m_boxRect.BottomRight().y);
		drawDC->SelectObject(pOldPen);
		drawDC->SelectObject(pOlBrush);
	}
	Gdiplus::Graphics graphics(drawDC->m_hDC);

	// 画圈
	{
		Gdiplus::Pen white(Gdiplus::Color(255, 255, 255, 255), 1.6f);
		Gdiplus::SolidBrush blueBrush(Gdiplus::Color(255, 32, 128, 240));
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		int x = m_boxRect.TopLeft().x;
		int y = m_boxRect.TopLeft().y;
		CPoint circlePoseArray[8] = {
			m_boxRect.TopLeft(),
			CPoint(x + width / 2, y),
			CPoint(x + width, y),
			CPoint(x, y + height / 2),
			CPoint(x + width, y + height / 2),
			CPoint(x, y + height),
			CPoint(x + width / 2, y + height),
			CPoint(x + width, y + height)
		};
		for (int i = 0; i < 8; i++)
		{
			CPoint p = circlePoseArray[i];
			graphics.FillEllipse(&blueBrush, Gdiplus::Rect(p.x - 4, p.y - 4, 8, 8));
			graphics.DrawEllipse(&white, p.x - 5, p.y - 5, 10, 10);
		}
	}
}

void State::drawSizeText(CDC* drawDC, CDC* imageMemDC)
{
	CString sizeText;
	sizeText.Format(_T("%d x %d"), abs(m_boxRect.Width()), abs(m_boxRect.Height()));
	Gdiplus::FontFamily fontf(_T("微软雅黑"));
	Gdiplus::Font font(&fontf, 10);
	Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::RectF rcf;
	Gdiplus::Graphics graphics(drawDC->m_hDC);
	graphics.MeasureString(sizeText, sizeText.GetLength(), &font, Gdiplus::PointF(0, 0), &rcf);

	const int width = rcf.Width;
	const int height = rcf.Height;

	const int sizeWidth = width + 8;
	const int sizeHeight = height + 8;
	const int sizeX = m_boxRect.TopLeft().x;
	const int sizeY = m_boxRect.TopLeft().y - sizeHeight - 5;
	Gdiplus::Rect textBbRect(sizeX, sizeY, sizeWidth, sizeHeight);
	Gdiplus::SolidBrush brush(Gdiplus::Color(220, 0, 0, 0));
	graphics.FillRectangle(&brush, textBbRect);

	graphics.DrawString(sizeText, sizeText.GetLength(), &font, Gdiplus::PointF(sizeX + 4, sizeY + 4), &textBrush);
}

void State::drawMagnifierBox(CDC* drawDC, CDC* imageMemDC, CPoint cursorPoint)
{
	if (cursorPoint.x > 0)
	{
		const int poseOffset = 8;
		const int colorPointBoxWidth = 50;
		const int magnifierBoxWidth = 150;
		const int centerRectWidth = 10;
		drawDC->StretchBlt(cursorPoint.x + poseOffset, cursorPoint.y + poseOffset, magnifierBoxWidth, magnifierBoxWidth, imageMemDC, cursorPoint.x - colorPointBoxWidth / 2, cursorPoint.y - colorPointBoxWidth / 2, colorPointBoxWidth, colorPointBoxWidth, SRCCOPY);
		Gdiplus::Pen white(Gdiplus::Color(255, 255, 255, 255), 1);
		Gdiplus::Pen blank(Gdiplus::Color(255, 0, 0, 0), 1);
		Gdiplus::Rect rect(cursorPoint.x + poseOffset, cursorPoint.y + poseOffset, magnifierBoxWidth, magnifierBoxWidth);
		// 画描边
		Gdiplus::Graphics graphics(drawDC->m_hDC);
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
		drawDC->StretchBlt(colorShowX, colorShowY, centerRectWidth, centerRectWidth, imageMemDC, cursorPoint.x - selectColorRegionWidth / 2, cursorPoint.y - selectColorRegionWidth / 2, selectColorRegionWidth, selectColorRegionWidth, SRCCOPY);
		graphics.DrawRectangle(&white2, Gdiplus::Rect(colorShowX, colorShowY, centerRectWidth, centerRectWidth));

		int colorValueX = colorShowX + centerRectWidth + 10;
		int colorValueY = colorShowY - 5;

		// 获取RGB
		COLORREF rgb = ::GetPixel(imageMemDC->GetSafeHdc(), cursorPoint.x, cursorPoint.y);
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

		m_colorRGBValue.Format(_T("rgb(%d,%d,%d)"), r, g, b);

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


BeginState::BeginState(CMaskDlg* target)
	: State(target)
{

}


void BeginState::onLButtonDown(CPoint point)
{
	m_isLButtonDown = true;
	m_lbuttonDownPoint = point;

	// 切换到框选状态
	switchToNextState(STATE_BOX_SELECT);
}

void BeginState::onNextState(State* nextState)
{
	nextState->m_cursorPosition = m_cursorPosition;
	nextState->m_lbuttonDownPoint = m_lbuttonDownPoint;
	nextState->m_isLButtonDown = m_isLButtonDown;
}



BoxSelectState::BoxSelectState(CMaskDlg* target)
	: State(target)
{

}

void BoxSelectState::onMouseMove(CPoint point, bool isLButtonDown)
{
	m_cursorPosition = point;
	m_isLButtonDown = isLButtonDown;
	if (isLButtonDown)
	{
		m_boxRect = CRect(m_lbuttonDownPoint, m_cursorPosition);
		m_target->Invalidate();
	}
}

void BoxSelectState::onLButtonUp(CPoint point)
{
	m_isLButtonDown = false;

	// 切换到框选完成状态
	switchToNextState(STATE_BOX_SELECT_COMPLETE);
}

void BoxSelectState::onNextState(State* nextState)
{
	nextState->m_cursorPosition = m_cursorPosition;
	nextState->m_isLButtonDown = m_isLButtonDown;
	nextState->m_boxRect = m_boxRect;
	m_target->Invalidate();
}


void BoxSelectState::onDraw(CDC* drawDC, CDC* imageMemDC)
{
	drawBox(drawDC, imageMemDC);
	drawSizeText(drawDC, imageMemDC);
	drawMagnifierBox(drawDC, imageMemDC, m_cursorPosition);
}

BoxSelectCompleteState::BoxSelectCompleteState(CMaskDlg* target)
	: State(target)
	, m_resizeDirect(ADJUST_NONE)
	, m_isMouseInBox(false)
{

}

void BoxSelectCompleteState::adjustBoxRect()
{
	CPoint leftPoint = m_boxRect.TopLeft();
	CPoint rightPoint = m_boxRect.BottomRight();
	if (leftPoint.x < rightPoint.x && leftPoint.y < rightPoint.y)
	{
		leftPoint = leftPoint;
		rightPoint = rightPoint;
	}
	else if (leftPoint.x < rightPoint.x && leftPoint.y > rightPoint.y)
	{
		leftPoint = CPoint(leftPoint.x, rightPoint.y);
		rightPoint = CPoint(rightPoint.x, leftPoint.y);
	}
	else if (leftPoint.x > rightPoint.x && leftPoint.y < rightPoint.y)
	{
		leftPoint = CPoint(rightPoint.x, leftPoint.y);
		rightPoint = CPoint(leftPoint.x, rightPoint.y);
	}
	else if (leftPoint.x > rightPoint.x && leftPoint.y > rightPoint.y)
	{
		leftPoint = rightPoint;
		rightPoint = leftPoint;
	}
	m_boxRect = CRect(leftPoint, rightPoint);
}

void BoxSelectCompleteState::onLButtonDown(CPoint point)
{
	m_lbuttonDownPoint = point;
	m_moveBeginPoint = point;
}

void BoxSelectCompleteState::onLButtonUp(CPoint point)
{
	if (m_resizeDirect != ADJUST_NONE)
	{
		m_resizeDirect = ADJUST_NONE;
	}
}

void BoxSelectCompleteState::onMouseMove(CPoint point, bool isLButtonDown)
{
	adjustBoxRect();
	CPoint leftPoint = m_boxRect.TopLeft();
	CPoint rightPoint = m_boxRect.BottomRight();
	m_cursorPosition = point;

	// 根据当前鼠标位置确定鼠标形状
	if ((m_resizeDirect == ADJUST_MOVE)
		|| (m_resizeDirect == ADJUST_NONE && point.x > leftPoint.x && point.x < rightPoint.x && point.y > leftPoint.y && point.y < rightPoint.y))
	{
		// 十字拖动图标
		SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZEALL));
		if (isLButtonDown)
		{
			if (m_resizeDirect == ADJUST_NONE)
			{
				m_resizeDirect = ADJUST_MOVE;
			}
			CPoint offset = point - m_moveBeginPoint;
			leftPoint += offset;
			rightPoint += offset;
			m_boxRect = CRect(leftPoint, rightPoint);
			m_moveBeginPoint = point;
		}
		m_isMouseInBox = true;
		m_target->Invalidate();
	}
	else
	{
		m_isMouseInBox = false;
		m_target->Invalidate();

		if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP_LEFT)
			|| (m_resizeDirect == ADJUST_NONE && (point.x < leftPoint.x && point.y < leftPoint.y)))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENWSE));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_TOP_LEFT;
				}
				m_boxRect = CRect(point, rightPoint);
				m_target->Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM_RIGHT)
			|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x&& point.y > rightPoint.y))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENWSE));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_BOTTOM_RIGHT;
				}
				m_boxRect = CRect(leftPoint, point);
				m_target->Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP_RIGHT)
			|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x&& point.y < leftPoint.y))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENESW));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_TOP_RIGHT;
				}
				leftPoint.y = point.y;
				rightPoint.x = point.x;
				m_boxRect = CRect(leftPoint, rightPoint);
				m_target->Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM_LEFT)
			|| (m_resizeDirect == ADJUST_NONE && point.x < leftPoint.x && point.y > rightPoint.y))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENESW));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_BOTTOM_LEFT;
				}
				leftPoint.x = point.x;
				rightPoint.y = point.y;
				m_boxRect = CRect(leftPoint, rightPoint);
				m_target->Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_RIGHT)
			|| (m_resizeDirect == ADJUST_NONE && point.x > rightPoint.x) && (point.y > leftPoint.y&& point.y < rightPoint.y))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZEWE));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_RIGHT;
				}
				rightPoint.x = point.x;
				m_boxRect = CRect(leftPoint, rightPoint);
				m_target->Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_LEFT)
			|| (m_resizeDirect == ADJUST_NONE && point.x < leftPoint.x) && (point.y > leftPoint.y&& point.y < rightPoint.y))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZEWE));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_LEFT;
				}
				leftPoint.x = point.x;
				m_boxRect = CRect(leftPoint, rightPoint);
				m_target->Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_TOP)
			|| (m_resizeDirect == ADJUST_NONE && point.y < leftPoint.y && point.x > leftPoint.x&& point.x < rightPoint.x))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENS));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_TOP;
				}
				leftPoint.y = point.y;
				m_boxRect = CRect(leftPoint, rightPoint);
				m_target->Invalidate();
			}
		}
		else if ((m_resizeDirect == ADJUST_RESIZE_DIRECT_BOTTOM)
			|| (m_resizeDirect == ADJUST_NONE && point.y > rightPoint.y&& point.x > leftPoint.x&& point.x < rightPoint.x))
		{
			SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_SIZENS));
			if (isLButtonDown)
			{
				if (m_resizeDirect == ADJUST_NONE)
				{
					m_resizeDirect = ADJUST_RESIZE_DIRECT_BOTTOM;
				}
				rightPoint.y = point.y;
				m_boxRect = CRect(leftPoint, rightPoint);
				m_target->Invalidate();
			}
		}
	}
}

void BoxSelectCompleteState::onDraw(CDC* drawDC, CDC* imageMemDC)
{
	drawBox(drawDC, imageMemDC);
	drawSizeText(drawDC, imageMemDC);
	if (m_isMouseInBox)
	{
		drawMagnifierBox(drawDC, imageMemDC, m_cursorPosition);
	}
	
	m_target->moveToolbar(m_boxRect.BottomRight());
}

void BoxSelectCompleteState::onNextState(State* nextState)
{
	nextState->m_boxRect = m_boxRect;
}

DrawLineState::DrawLineState(CMaskDlg* target)
	: State(target)
	, m_isMouseInBox(false)
{
	m_target->startTimer();
}

DrawLineState::~DrawLineState()
{
	m_target->stopTimer();
}

void DrawLineState::onDraw(CDC* drawDC, CDC* imageMemDC)
{
	drawBox(drawDC, imageMemDC);
	drawSizeText(drawDC, imageMemDC);
	drawCursor(drawDC);
}

void DrawLineState::onMouseMove(CPoint point, bool isLButtonDown)
{
	m_cursorPosition = point;
	m_isLButtonDown = isLButtonDown;
	CPoint leftPoint = m_boxRect.TopLeft();
	CPoint rightPoint = m_boxRect.BottomRight();
	if (point.x > leftPoint.x && point.x < rightPoint.x && point.y > leftPoint.y && point.y < rightPoint.y)
	{
		// 十字拖动图标
		while (ShowCursor(FALSE) >= 0)
			ShowCursor(FALSE);
		m_isMouseInBox = true;
		m_target->Invalidate();
	}
	else
	{
		while (ShowCursor(TRUE) < 0)
			ShowCursor(TRUE);
		SetClassLong(m_target->GetSafeHwnd(), GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_NO));
		if (m_isMouseInBox)
		{
			m_isMouseInBox = false;
			m_target->Invalidate();
		}
	}
}

void DrawLineState::drawCursor(CDC* drawDC)
{
	if (m_isMouseInBox)
	{
		Gdiplus::Graphics graphics(drawDC->m_hDC);
		Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));
		const int centerRectWidth = 4;
		const int centerRectX = m_cursorPosition.x - centerRectWidth / 2;
		const int centerRectY = m_cursorPosition.y - centerRectWidth / 2;

		// 画中间方块
		Gdiplus::Rect centerRectangleRect(centerRectX, centerRectY, centerRectWidth, centerRectWidth);
		graphics.FillRectangle(&brush, centerRectangleRect);

		// 画左边线
		const int leftLineWidth = centerRectWidth * 2;
		const int leftLineHight = centerRectWidth;
		const int offset = centerRectWidth;
		const int leftLineX = centerRectX - offset - leftLineWidth;
		const int leftLineY = centerRectY;
		Gdiplus::Rect leftLineRect(leftLineX, leftLineY, leftLineWidth, leftLineHight);
		graphics.FillRectangle(&brush, leftLineRect);

		// 画右边线
		const int rightLineWidth = leftLineWidth;
		const int rightLineHeight = leftLineHight;
		const int rightLineX = centerRectX + centerRectWidth + offset;
		const int rightLineY = leftLineY;
		Gdiplus::Rect rightLineRect(rightLineX, rightLineY, rightLineWidth, rightLineHeight);
		graphics.FillRectangle(&brush, rightLineRect);

		// 画上边线
		const int topLineWidth = centerRectWidth;
		const int topLineHeight = centerRectWidth * 2;
		const int topLineX = centerRectX;
		const int topLineY = centerRectY - offset - topLineHeight;
		Gdiplus::Rect topLineRect(topLineX, topLineY, topLineWidth, topLineHeight);
		graphics.FillRectangle(&brush, topLineRect);

		// 画下边线
		const int bottomLineWidth = topLineWidth;
		const int bottomLineHeight = topLineHeight;
		const int bottomLineX = topLineX;
		const int bottomLineY = centerRectY + centerRectWidth + offset;
		Gdiplus::Rect bottomRect(bottomLineX, bottomLineY, bottomLineWidth, bottomLineHeight);
		graphics.FillRectangle(&brush, bottomRect);

	}
}


// CMaskDlg 对话框

IMPLEMENT_DYNAMIC(CMaskDlg, CDialogEx)

CMaskDlg::CMaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MASK, pParent)
	, m_finishCut(false)
	, m_state(STATE_BEGIN)
	, m_startResize(false)
	, m_resizeDirect(ADJUST_NONE)
	, m_inBoxPoint(-1, -1)
	, m_curState(NULL)
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
	ON_MESSAGE(WM_USER_MESSAGE_DRAW, OnDrawLine)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
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

	if (m_curState != NULL)
	{
		delete m_curState;
	}

	new BeginState(this);
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
	if (m_memCDC.m_hDC != NULL)
	{
		m_curState->draw();
	}
}


void CMaskDlg::boxChanged()
{
	if (m_state == STATE_BOX_SELECT_COMPLETE)
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

void CMaskDlg::moveToolbar(CPoint point)
{
	// 修改toolbar 位置
	int toolbarWidth = m_toolbarDlg.getToolbarWidth();
	int toolbarHeight = m_toolbarDlg.getToolbarHeight();
	int x = point.x - toolbarWidth;
	int y = point.y + 10;
	m_toolbarDlg.SetWindowPos(NULL, x, y, toolbarWidth, toolbarHeight, 0);
	m_toolbarDlg.ShowWindow(SW_SHOW);
}

void CMaskDlg::reset()
{
	m_firstPoint = CPoint(0, 0);
	m_curPoint = CPoint(0, 0);
	m_state = STATE_BEGIN;
	m_resizeDirect = ADJUST_NONE;
	Invalidate();
}

void CMaskDlg::setState(State* state)
{
	m_curState = state;
}

CDC* CMaskDlg::getImageMemDC()
{
	return &m_memCDC;
}

void CMaskDlg::startTimer()
{
	SetTimer(TIMER_ID_DRAW, 10, NULL);
}

void CMaskDlg::stopTimer()
{
	KillTimer(TIMER_ID_DRAW);
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
	CRect boxRect = m_curState->m_boxRect;
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

	m_curState->onLButtonDown(point);
	
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMaskDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_curState->onMouseMove(point, nFlags & MK_LBUTTON);

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
	m_curState->onLButtonUp(point);
	
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
	CRect boxRect = m_curState->m_boxRect;
	int width = boxRect.Width();
	int height = boxRect.Height();
	std::cout << "----width:" << width << "height:" << height << std::endl;

	CPinDlg* pinDlg = new CPinDlg(width, height);
	pinDlg->Create(IDD_DIALOG_PIN_BACK, GetDesktopWindow());
	fillBoxImage(pinDlg->getCDC());
	::SetWindowPos(pinDlg->GetSafeHwnd(), HWND_TOPMOST, boxRect.left, boxRect.top, width, height, SWP_SHOWWINDOW);
	pinDlg->SetActiveWindow();
	pinDlg->Invalidate();
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_CUT_QUIT, 0, 0);

	return 0;

}

LRESULT CMaskDlg::OnDrawLine(WPARAM wParam, LPARAM lParam)
{
	std::cout << "OnDrawLine" << std::endl;
	m_curState->switchToNextState(STATE_DRAW_LINE);
	return 0;
}

LRESULT CMaskDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
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
			std::string strColor(W2A(m_curState->m_colorRGBValue)); //CString-->std::string

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




void CMaskDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	std::cout << "OnTimer" << std::endl;
	Invalidate();
	CDialogEx::OnTimer(nIDEvent);
}
