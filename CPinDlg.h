#pragma once


// CPinDlg 对话框

class CPinDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPinDlg)

public:
	CPinDlg(int width, int height, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPinDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PIN_BACK };
#endif

private:
	CDC m_cdc;
	int m_width;
	int m_height;
	CPoint m_lbtnDownPoint;
	CRect m_lastRect;
	CMenu m_MenuTrack;


public:
	CDC* getCDC();
	void DrawBorder(CDC* pDC);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnMenuItemPinDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
