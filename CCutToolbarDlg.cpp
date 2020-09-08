// CCutToolbarDlg.cpp: 实现文件
//

#include "pch.h"
#include "ImageCat.h"
#include "CCutToolbarDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CMaskDlg.h"

// CCutToolbarDlg 对话框

IMPLEMENT_DYNAMIC(CCutToolbarDlg, CDialogEx)

CCutToolbarDlg::CCutToolbarDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CUT_TOOL_BAR, pParent)
	, m_toolbarWidth(0)
	, m_toolbarHeight(0)
{

}

CCutToolbarDlg::~CCutToolbarDlg()
{
}

void CCutToolbarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCutToolbarDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_TOOL_BAR_BTN_YES, onToolbarBtnSaveToMem)
	ON_COMMAND(ID_TOOL_BAR_BTN_CANCEL, onToolbarBtnCancel)
	ON_COMMAND(ID_TOOL_BAR_BTN_SAVE_FILE, onToolbarBtnSaveToFile)
	ON_COMMAND(ID_TOOL_BAR_BTN_PIN, onToolbarBtnPin)
END_MESSAGE_MAP()


// CCutToolbarDlg 消息处理程序


BOOL CCutToolbarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	initToolbar();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CCutToolbarDlg::initToolbar()
{
	if (m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_ANY | CBRS_TOOLTIPS))
	{

		static UINT BASED_CODE DockTool[] = { ID_TOOL_BAR_BTN_PIN, ID_TOOL_BAR_BTN_SAVE_FILE, ID_TOOL_BAR_BTN_CANCEL, ID_TOOL_BAR_BTN_YES };
		
		m_toolbarlist.Create(32, 32, ILC_COLOR24, 0, 0);

		CBitmap bitmapPin;
		bitmapPin.LoadBitmapW(IDB_BITMAP_PIN);
		m_toolbarlist.Add(&bitmapPin, (CBitmap*)NULL);

		CBitmap bitmapSave;
		bitmapSave.LoadBitmapW(IDB_BITMAP_SAVE);
		m_toolbarlist.Add(&bitmapSave, (CBitmap*)NULL);

		CBitmap bitmapCancel;
		bitmapCancel.LoadBitmapW(IDB_BITMAP_CANCEL);
		m_toolbarlist.Add(&bitmapCancel, (CBitmap*)NULL);

		CBitmap bitmapYes;
		bitmapYes.LoadBitmapW(IDB_BITMAP_YES);
		m_toolbarlist.Add(&bitmapYes, (CBitmap*)NULL);


		//设置工具栏按钮图片
		m_toolBar.GetToolBarCtrl().SetImageList(&m_toolbarlist);
		//设置工具栏按钮大小， 和按钮中位图大小
		SIZE sbutton, sImage;
		sbutton.cx = 48;
		sbutton.cy = 48;
		sImage.cx = 32;
		sImage.cy = 32;
		m_toolBar.SetSizes(sbutton, sImage);
		m_toolBar.SetButtons(DockTool, (UINT)4);

		m_toolbarWidth = 4 * 48;
		m_toolbarHeight = 48;
	}
}

int CCutToolbarDlg::getToolbarWidth()
{
	return m_toolbarWidth;
}

int CCutToolbarDlg::getToolbarHeight()
{
	return m_toolbarHeight;
}


void CCutToolbarDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_toolBar.GetSafeHwnd() != NULL)
	{
		m_toolBar.SetWindowPos(NULL, 0, 0, m_toolbarWidth, m_toolbarHeight, 0);
	}
	// TODO: 在此处添加消息处理程序代码
}


BOOL CCutToolbarDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return CDialogEx::OnEraseBkgnd(pDC);
}

void CCutToolbarDlg::onToolbarBtnSaveToMem()
{
	std::cout << "onToolbarBtnSaveToMem" << std::endl;
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_SAVE_TO_MEM, 0, 0);
}

void CCutToolbarDlg::onToolbarBtnCancel()
{
	std::cout << "onToolbarBtnCancel" << std::endl;
	::SendMessage(this->GetParent()->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_CUT_QUIT, 0, 0);
}

void CCutToolbarDlg::onToolbarBtnSaveToFile()
{
	std::cout << "onToolbarBtnSaveToFile" << std::endl;
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_SAVE_TO_FILE, 0, 0);
}

void CCutToolbarDlg::onToolbarBtnPin()
{
	std::cout << "onToolbarBtnPin" << std::endl;
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_USER_MESSAGE_PIN, 0, 0);
}
