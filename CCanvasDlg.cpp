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
	m_scrDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
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
	std::cout << "CCanvasDlg::loadImage.......";
	m_imagePath = path;
	m_expandRatio = 1.0f;
	m_curMoveOffset = CPoint(0, 0);
	m_curMousePoint = CPoint(-1, -1);
	m_delta = 0;
	m_image.Destroy();

	HRESULT result = m_image.Load(m_imagePath);
	if (result == 0)
	{
		CString suffix = m_imagePath.Right(m_imagePath.GetLength() - m_imagePath.ReverseFind('.'));
		if (suffix == _T(".png"))
		{
			setPngAlpha(&m_image);
		}
		m_loadSuccess = true;
	}
	else
	{
		m_loadSuccess = false;
	}
	Invalidate();
}

void CCanvasDlg::rotation(int angle)
{
	if (m_loadSuccess)
	{
		CImage image;
		imageRotation(&image, &m_image, angle);
		m_image.Destroy();
		m_image.Attach(image.Detach());
		Invalidate();
	}
}

void CCanvasDlg::saveFile(const CString& fileName)
{
	if (m_loadSuccess)
	{
		m_image.Save(fileName);
	}
}

CDC* CCanvasDlg::getMemDC()
{
	return &m_memDC;
}

// 绘制图片
void CCanvasDlg::drawImage()
{
	if (m_imagePath == _T(""))
	{
		CDialogEx::OnPaint();
		return;
	}
	CPaintDC dc(this); // device context for painting

	CRect		rect;
	GetClientRect(&rect);

	int rectWidth = rect.Width();
	int rectHeight = rect.Height();
	std::cout << "rectHeight:" << rectHeight << "rectWidth:" << rectWidth << std::endl;

	int imageWidth = m_imageFail.GetWidth();
	int imageHeight = m_imageFail.GetHeight();
	if (m_loadSuccess)
	{
		imageWidth = m_image.GetWidth();
		imageHeight = m_image.GetHeight();
	}

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

	

	std::cout << "imageWidht:" << imageWidth << "imageHeight:" << imageHeight << std::endl;
	imageHeight = imageHeight * ratio * m_expandRatio;
	imageWidth = imageWidth * ratio * m_expandRatio;

	int screenOrgX = (rectWidth - imageWidth) / 2;
	int screenOrgY = (rectHeight - imageHeight) / 2;


	CPoint offset = m_curMoveOffset;
	std::cout << "offset.x:" << offset.x << "offset.y:" << offset.y << std::endl;
	std::cout << "screenOrgX:" << screenOrgX << "screenOrgY:" << screenOrgY << std::endl;
		
	screenOrgX += offset.x;
	screenOrgY += offset.y;

	CDC memDC;
	CBitmap memBitmap;
	memDC.CreateCompatibleDC(NULL);
	memBitmap.CreateCompatibleBitmap(&dc, rectWidth, rectHeight);
	CBitmap* pOldBit = memDC.SelectObject(&memBitmap);
	memDC.FillSolidRect(0, 0, rectWidth, rectHeight, RGB(255, 255, 255));

	SetStretchBltMode(memDC, STRETCH_HALFTONE);
	if (m_loadSuccess)
	{
		m_image.Draw(memDC, screenOrgX, screenOrgY, imageWidth, imageHeight, 0, 0, m_image.GetWidth(), m_image.GetHeight());
	}
	else
	{
		m_imageFail.Draw(memDC, screenOrgX, screenOrgY, imageWidth, imageHeight, 0, 0, m_imageFail.GetWidth(), m_imageFail.GetHeight());
	}
	dc.BitBlt(0, 0, rectWidth, rectHeight, &memDC, 0, 0, SRCCOPY);
		
	{
		if (m_memDC.m_hDC == NULL)
		{
			m_memDC.CreateCompatibleDC(&m_scrDC);
		}
		m_memBitmap.DeleteObject();
		m_memBitmap.CreateCompatibleBitmap(&m_scrDC, rectWidth, rectHeight);
		m_memDC.SelectObject(m_memBitmap);
		m_memDC.BitBlt(0, 0, rectWidth, rectHeight, &memDC, 0, 0, SRCCOPY);		/// 记录数据
	}


	memBitmap.DeleteObject();
	memDC.DeleteDC();
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

void CCanvasDlg::imageRotation(CImage* dst, CImage* src, int angle)
{
	double alpha =  angle / 180.0 * 3.141592653;
	int ww, Dx, Dy, bpd;
	double centerx, centery, sintheta, costheta;
	double X1, Y1, X2, Y2, theta, xx, yy, rr;
	BYTE** list, * sc, * lp;
	int x, y;
	Dx = src->GetWidth();
	Dy = src->GetHeight();
	sc = (BYTE*)malloc(2 * (Dx * src->GetBPP() + 31) / 32 * 4);	//申请工作单元 
	list = (BYTE**)malloc(Dy * sizeof(BYTE*));				//对原位图建立二维数组 
	for (int i = 0; i < Dy; i++)
	{
		list[i] = (BYTE*)src->GetPixelAddress(0, i);
	}
	centerx = Dx / 2.0 + 0.5;									//计算位图中心位置 
	centery = Dy / 2.0 + 0.5;
	rr = sqrt(centerx * centerx + centery * centery);			//计算对角线长度 

	theta = atan(centery / centerx);

	X1 = fabs(rr * cos(alpha + theta)) + 0.5;
	Y1 = fabs(rr * sin(alpha + theta)) + 0.5;
	X2 = fabs(rr * cos(alpha - theta)) + 0.5;
	Y2 = fabs(rr * sin(alpha - theta)) + 0.5;

	if (X2 > X1)	X1 = X2;								//得外接矩形宽度 
	if (Y2 > Y1)	Y1 = Y2;								//外接矩形高度 
	ww = (int)(2 * X1);
	int hh = (int)(2 * Y1);

	dst->Destroy();

	dst->Create(ww, hh, src->GetBPP());		//建立结果位图 
	bpd = src->GetBPP() / 8;
	sintheta = sin(alpha);
	costheta = cos(alpha);

	for (int j = (int)(centery - Y1), Yd = 0; j <= (centery + Y1) && Yd < hh; j++, Yd++)
	{
		if (src->GetBPP() == 8)
			memset(sc, 255, ww);							//256色位图像素行置背景值 
		else
			memset(sc, 255, ww * bpd);						//真彩色位图像素行置背景值 

		for (int i = (int)(centerx - X1), Xd = 0; i <= centerx + X1; i++, Xd += bpd)
		{
			xx = centerx + costheta * (i - centerx) + sintheta * (j - centery);
			yy = centery - sintheta * (i - centerx) + costheta * (j - centery);
			x = (int)(xx + 0.5);
			y = (int)(yy + 0.5);
			if (x < 0 || x >= src->GetWidth() || y < 0 || y >= src->GetHeight())
				continue;
			if (x == src->GetWidth())		x--;
			if (y == src->GetHeight())	y--;
			memcpy(&sc[Xd], &list[y][x * bpd], bpd);		//从源位图复制像素数据 
		}
		lp = (BYTE*)dst->GetPixelAddress(0, Yd);			//处理结果总结果位图 
		memcpy(lp, sc, ww * bpd);
	}
	free(list);											//释放工作单元 
	free(sc);
}

void CCanvasDlg::setPngAlpha(CImage* pImg)
{
	int width = pImg->GetWidth();
	int height = pImg->GetHeight();
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			unsigned char* pucColor = reinterpret_cast<unsigned char*>(pImg->GetPixelAddress(i, j));
			pucColor[0] = pucColor[0] * pucColor[3] / 255;
			pucColor[1] = pucColor[1] * pucColor[3] / 255;
			pucColor[2] = pucColor[2] * pucColor[3] / 255;
		}
	}	
}

void CCanvasDlg::loadPngResource(CImage* pImg, UINT nID)
{
	pImg->Destroy();

	// 查找资源
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nID), _T("png"));
	if (hRsrc == NULL) return ;

	// 加载资源
	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);
	if (hImgData == NULL)
	{
		::FreeResource(hImgData);
		return;
	}

	// 锁定内存中的指定资源
	LPVOID lpVoid = ::LockResource(hImgData);

	LPSTREAM pStream = NULL;
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);

	// 解除内存中的指定资源
	::GlobalUnlock(hNew);

	// 从指定内存创建流对象
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
	if (ht != S_OK)
	{
		GlobalFree(hNew);
	}
	else
	{
		// 加载图片
		pImg->Load(pStream);

		GlobalFree(hNew);
	}
	// 释放资源
	::FreeResource(hImgData);
}


void CCanvasDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nFlags & MK_LBUTTON) {
		if (m_curMousePoint != CPoint(-1, -1))
		{
			m_curMoveOffset += point - m_curMousePoint;
			m_curMousePoint = point;
			Invalidate();
		}
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


BOOL CCanvasDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	loadPngResource(&m_imageFail, IDB_PNG_FAIL);
	setPngAlpha(&m_imageFail);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CCanvasDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
		//屏蔽ESC关闭窗体/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	
	return CDialogEx::PreTranslateMessage(pMsg);
}
