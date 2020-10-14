
// ImageCatDlg.h: 头文件
//

#pragma once
#include <vector>
#include "CCanvasDlg.h"
#include "CMaskDlg.h"

// CImageCatDlg 对话框
class CImageCatDlg : public CDialogEx
{
// 构造
public:
	CImageCatDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGECAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	CDC m_screenMemDC;
	CBitmap m_screenDCbitmap;
	CString m_imagePath;
	std::vector<CString> m_ImageNameArray;
	int m_curentImageIndex;
	CToolBar m_toolBar;
	CImageList m_toolbarlist;		//图标链表
	int m_toolbarWidth;
	int m_toolbarHeight;
	CCanvasDlg m_canvas;
	CMaskDlg m_mask;
	int m_screenWidth;
	int m_screenHeight;
	CString m_toolbarTips;
	 
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void onToolbarBtnOpen();
	afx_msg void onToolbarBtnSave();
	afx_msg void onToolbarBtnDelete();
	afx_msg void onToolbarBtnMirror();
	afx_msg void onToolbarBtnRotateCW();
	afx_msg void onToolbarBtnCut();
	afx_msg long OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCutQuit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNextImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrevImage(WPARAM wParam, LPARAM lParam);
	BOOL  OnDisplay(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
	void initToolbar();
	CString getCommandLineArg();
	void storageAllImageNameFromPath(CString path);
	bool isSupportFileFormatImage(CString fileName);
	void setCurrentImageIndex();
	void nextImage();
	void prevImage();
	void cutImage();
	void quitCutImage();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMove(int x, int y);
};
