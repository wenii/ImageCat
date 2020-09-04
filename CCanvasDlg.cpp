// CCanvasDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImageCat.h"
#include "CCanvasDlg.h"
#include "afxdialogex.h"


// CCanvasDlg 对话框

IMPLEMENT_DYNAMIC(CCanvasDlg, CDialogEx)

CCanvasDlg::CCanvasDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CANVAS, pParent)
	, m_expandRatio(1.0f)
	, m_ctrlKeyPress(false)
	, m_delta(0)
	, m_loadSuccess(false)
	, m_toolbarWidth(0)
	, m_toolbarHeight(0)
{

}

CCanvasDlg::~CCanvasDlg()
{
}

void CCanvasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCanvasDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CCanvasDlg 消息处理程序


void CCanvasDlg::OnPaint()
{
	drawImage();
}


void CCanvasDlg::loadImage(const CString& path)
{
	m_imagePath = path;
	m_expandRatio = 1.0f;
	m_curMoveOffset = CPoint(0, 0);
	m_delta = 0;
	m_image.Destroy();

	HRESULT result = m_image.Load(m_imagePath);
	if (result == 0)
	{
		m_loadSuccess = true;
	}
	else
	{
		m_loadSuccess = false;
	}
	Invalidate();
}

// 绘制图片
void CCanvasDlg::drawImage()
{
	if (m_imagePath == _T(""))
	{
		return;
	}
	CPaintDC dc(this); // device context for painting

	CRect		rect;
	GetClientRect(&rect);

	int rectWidth = rect.Width();
	int rectHeight = rect.Height();
	std::cout << "rectHeight:" << rectHeight << "rectWidth:" << rectWidth << std::endl;


	if (m_loadSuccess)
	{
		int orgImageWidth = m_image.GetWidth();
		int orgImageHeight = m_image.GetHeight();

		int imageWidth = m_image.GetWidth();
		int imageHeight = m_image.GetHeight();



		float widthRatio = 1.0f;
		if (imageWidth > rectWidth)
		{
			widthRatio = rectWidth / (float)imageWidth;
		}
		float heightRatio = 1.0f;
		if (imageHeight > rectHeight)
		{
			heightRatio = rectHeight / (float)imageHeight;
		}

		const float ratio = widthRatio < heightRatio ? widthRatio : heightRatio;

		//if (imageWidth > imageHeight)   
		//{
		//	if (imageWidth > rectWidth)
		//	ratio = rectWidth / (float)imageWidth;
		//}
		//else 
		//{
		//	if(imageHeight > rectHeight)
		//	ratio = rectHeight / (float)imageHeight;
		//}

		std::cout << "imageWidht:" << imageWidth << "imageHeight:" << imageHeight << std::endl;
		imageHeight = imageHeight * ratio * m_expandRatio;
		imageWidth = imageWidth * ratio * m_expandRatio;

		int screenOrgX = (rectWidth - imageWidth) / 2;
		int screenOrgY = (rectHeight - imageHeight) / 2;


		CPoint offset = m_curMoveOffset;
		std::cout << "offset.x:" << offset.x << "offset.y:" << offset.y << std::endl;
		std::cout << "screenOrgX:" << screenOrgX << "screenOrgY:" << screenOrgY << std::endl;
		//if (screenOrgX < 0) {
		screenOrgX += offset.x;
		//}
		//if (screenOrgY < 0) {
		screenOrgY += offset.y;
		//}

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
	else
	{
		CDC memDC;
		CBitmap memBitmap;
		memDC.CreateCompatibleDC(NULL);
		memBitmap.CreateCompatibleBitmap(&dc, rectWidth, rectHeight);
		CBitmap* pOldBit = memDC.SelectObject(&memBitmap);
		memDC.FillSolidRect(0, 0, rectWidth, rectHeight, RGB(255, 255, 255));
		dc.BitBlt(0, 0, rectWidth, rectHeight, &memDC, 0, 0, SRCCOPY);
		memBitmap.DeleteObject();
		memDC.DeleteDC();

		//设置字体
		dc.SetTextColor(RGB(255, 0, 0));

		dc.TextOutW(rectWidth / 2, rectHeight / 2, _T("不支持的图片格式！"));


	}
}



void CCanvasDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	// TODO: 在此处添加消息处理程序代码
	Invalidate();
}


BOOL CCanvasDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CCanvasDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nFlags & MK_LBUTTON) {
		m_curMoveOffset += point - m_curMousePoint;
		m_curMousePoint = point;
		Invalidate();
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CCanvasDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}


void CCanvasDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_curMousePoint = point;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CCanvasDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_expandRatio = 1.0f;
	m_curMoveOffset = CPoint(0, 0);
	m_delta = 0;
	Invalidate();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
