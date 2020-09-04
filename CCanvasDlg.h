#pragma once
#include <vector>

// CCanvasDlg 对话框

class CCanvasDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCanvasDlg)

public:
	CCanvasDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCanvasDlg();

private:
	float m_expandRatio;
	CPoint m_curMousePoint;
	CPoint m_curMoveOffset;
	bool m_ctrlKeyPress;
	CString m_imagePath;
	int m_delta;
	CImage m_image;
	bool m_loadSuccess;
	int m_toolbarWidth;
	int m_toolbarHeight;

public:
	void loadImage(const CString& path);
private:
	void drawImage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CANVAS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
