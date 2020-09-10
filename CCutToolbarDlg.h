#pragma once


// CCutToolbarDlg 对话框

class CCutToolbarDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCutToolbarDlg)

public:
	CCutToolbarDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCutToolbarDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CUT_TOOL_BAR };
#endif

private:
	CToolBar m_toolBar;
	CImageList m_toolbarlist;
	int m_toolbarWidth;
	int m_toolbarHeight;
	CString m_toolbarTips;

private:
	void initToolbar();

public:
	int getToolbarWidth();
	int getToolbarHeight();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void onToolbarBtnSaveToMem();
	afx_msg void onToolbarBtnCancel();
	afx_msg void onToolbarBtnSaveToFile();
	afx_msg void onToolbarBtnPin();
	BOOL  OnDisplay(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
};
