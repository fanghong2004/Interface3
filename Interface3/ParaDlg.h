#pragma once



// CParaDlg 窗体视图

class CParaDlg : public CFormView
{
	DECLARE_DYNCREATE(CParaDlg)

protected:
	CParaDlg();           // 动态创建所使用的受保护的构造函数
	virtual ~CParaDlg();

public:
	enum { IDD = DIALOG_PARA };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRun();
};


