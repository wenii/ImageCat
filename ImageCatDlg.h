
// ImageCatDlg.h: 头文件
//

#pragma once
#include <vector>

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
	// 放大因子
	float m_expandRatio;
	bool m_lButtonDown;
	CPoint m_lButtonDownPoint;
	CPoint m_curMousePoint;
	bool m_ctrlKeyPress;
	CString m_imagePath;
	std::vector<CString> m_ImageNameArray;
	int m_curentImageIndex;
	 
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void drawImage();
	void getImageNameFromPath(CString path);
	bool isFileFormatImage(CString fileName);
	void setCurrentImageIndex();
	void nextImage();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
