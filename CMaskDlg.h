#pragma once

#include "CCutToolbarDlg.h"
#include <list>
#include "CPinDlg.h"
#include "ImageCat.h"
// CMaskDlg 对话框
#define WM_USER_MESSAGE_CUT_QUIT (WM_USER + 1)
#define WM_USER_MESSAGE_SAVE_TO_MEM (WM_USER + 2)
#define WM_USER_MESSAGE_SAVE_TO_FILE (WM_USER + 3)
#define WM_USER_MESSAGE_PIN (WM_USER + 4)
#define WM_USER_MESSAGE_DRAW (WM_USER + 5)

enum {
	STATE_BEGIN,						// 起始状态
	STATE_BOX_SELECT,					// 框选状态
	STATE_BOX_SELECT_COMPLETE,			// 调整阶段
	STATE_DRAW_LINE,				// 画笔绘画状态
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

enum 
{
	TIMER_ID_DRAW = 1
};

class State;
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





private:
	CDC m_memCDC;
	CBitmap m_screenDCbitmap;
	CPoint m_firstPoint;
	CPoint m_curPoint;
	CPoint m_moveBeginPoint;
	bool m_finishCut;
	int m_state;
	bool m_startResize;
	int m_resizeDirect;
	CCutToolbarDlg m_toolbarDlg;
	CPoint m_inBoxPoint;
	CString m_colorRGB;
	int m_screenWidth;
	int m_screenHeight;
	CPoint m_movePoint;
	State* m_curState;
	CPoint m_boxLeftTopPoint;
	CPoint m_boxRightBottmPoint;

public:
	void snapshot();
	void boxChanged();
	void moveToolbar(CPoint point);
	void reset();
	void setState(State* state);
	CDC* getImageMemDC();
	void startTimer();
	void stopTimer();
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
	afx_msg LRESULT OnDrawLine(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


// 状态
class State
{
	friend class CMaskDlg;
public:
	virtual void onMouseMove(CPoint point, bool isLButtonDown) {};
	virtual void onLButtonDown(CPoint point) {};
	virtual void onLButtonUp(CPoint point) {};
	virtual void onDraw(CDC* drawDC, CDC* imageMemDC) {}
	virtual void onNextState(State* nextState){}

public:
	void draw();
protected:
	void switchToNextState(int state);
	void drawBox(CDC* drawDC, CDC* imageMemDC);
	void drawSizeText(CDC* drawDC, CDC* imageMemDC);
	void drawMagnifierBox(CDC* drawDC, CDC* imageMemDC, CPoint cursorPoint);
public:
	State(CMaskDlg* target);
	virtual ~State() {}

public:
	CRect m_boxRect;
	CPoint m_lbuttonDownPoint;
	CPoint m_cursorPosition;		// 鼠标位置
	bool m_isLButtonDown;			// 鼠标左键是否按下
	CMaskDlg* m_target;				// 状态目标
	CString m_colorRGBValue;		// 颜色值
};

class BeginState : public State
{
public:
	BeginState(CMaskDlg* target);
public:
	virtual void onLButtonDown(CPoint point);
	virtual void onNextState(State* nextState);
};

class BoxSelectState : public State
{
public:
	BoxSelectState(CMaskDlg* target);
public:
	virtual void onMouseMove(CPoint point, bool isLButtonDown);
	virtual void onLButtonUp(CPoint point);
	virtual void onNextState(State* nextState);
	virtual void onDraw(CDC* drawDC, CDC* imageMemDC);

};

class BoxSelectCompleteState : public State
{
public:
	BoxSelectCompleteState(CMaskDlg* target);

public:
	void adjustBoxRect();
public:
	virtual void onLButtonDown(CPoint point);
	virtual void onLButtonUp(CPoint point);
	virtual void onMouseMove(CPoint point, bool isLButtonDown);
	virtual void onDraw(CDC* drawDC, CDC* imageMemDC);
	virtual void onNextState(State* nextState);

public:
	int m_resizeDirect;
	CPoint m_moveBeginPoint;
	bool m_isMouseInBox;
};

struct Line
{
	Line(CPoint beginPoint, CPoint endPoint)
		: m_beginPoint(beginPoint)
		, m_endPoint(endPoint)
	{}
	CPoint m_beginPoint;
	CPoint m_endPoint;
};
class DrawLineState : public State
{
public:
	DrawLineState(CMaskDlg* target);
	~DrawLineState();
public:
	virtual void onDraw(CDC* drawDC, CDC* imageMemDC);
	virtual void onMouseMove(CPoint point, bool isLButtonDown);
	virtual void onLButtonDown(CPoint point);
	virtual void onLButtonUp(CPoint point);

private:
	void drawCursor(CDC* drawDC);
	void drawLine(CDC* drawDC);

private:
	bool m_isMouseInBox;
	std::list<std::list<Line>> m_lineListList;
	std::list<Line> m_lineList;
};

