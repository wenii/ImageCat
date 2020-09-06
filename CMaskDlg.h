#pragma once


// CMaskDlg 对话框

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

public:
	void setCDC(CDC* cdc);

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
};
