#pragma once


// CScreenCutDlg 对话框

class CScreenCutDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenCutDlg)

public:
	CScreenCutDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CScreenCutDlg();

private:
	CDC m_screenMemDC;
	CBitmap m_screenDCbitmap;
	int m_screenWidth;
	int m_screenHeight;

public:
	void snapshot();
	CDC* getScreenMemDC();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SCREEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
