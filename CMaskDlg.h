﻿#pragma once

#include "CCutToolbarDlg.h"
#include <list>
#include "CPinDlg.h"
#include "ImageCat.h"
// CMaskDlg 对话框
#define WM_USER_MESSAGE_CUT_QUIT (WM_USER + 1)
#define WM_USER_MESSAGE_SAVE_TO_MEM (WM_USER + 2)
#define WM_USER_MESSAGE_SAVE_TO_FILE (WM_USER + 3)
#define WM_USER_MESSAGE_PIN (WM_USER + 4)
class CMaskDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMaskDlg)

public:
	CMaskDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMaskDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MASK };
#endif

	enum {
		STATE_BEGIN,		// 起始状态
		STATE_BOX_SELECT,	// 框选状态
		STATE_BOX_ADJUST,	// 调整阶段
	};

	enum {
		ADJUST_NONE,
		ADJUST_MOVE,
		ADJUST_RESIZE_DIRECT_RIGHT,
		ADJUST_RESIZE_DIRECT_LEFT,
		ADJUST_RESIZE_DIRECT_TOP,
		ADJUST_RESIZE_DIRECT_BOTTOM,
		ADJUST_RESIZE_DIRECT_TOP_RIGHT,
		ADJUST_RESIZE_DIRECT_TOP_LEFT,
		ADJUST_RESIZE_DIRECT_BOTTOM_RIGHT,
		ADJUST_RESIZE_DIRECT_BOTTOM_LEFT
	};

private:
	CDC* m_memCDC;
	CPoint m_firstPoint;
	CPoint m_curPoint;
	CPoint m_moveBeginPoint;
	bool m_finishCut;
	int m_state;
	bool m_startResize;
	int m_resizeDirect;
	CCutToolbarDlg m_toolbarDlg;

public:
	void setCDC(CDC* cdc);
	void boxChanged();
	void reset();
private:
	CRect getBoxRect();
	void fillBoxImage(CDC* cdc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSaveToMem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSaveToFile(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPin(WPARAM wParam, LPARAM lParam);
};
