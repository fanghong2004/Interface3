#pragma once



// CDisplayDlg 窗体视图

class CDisplayDlg : public CFormView
{
	DECLARE_DYNCREATE(CDisplayDlg)

protected:
	CDisplayDlg();           // 动态创建所使用的受保护的构造函数
	virtual ~CDisplayDlg();

public:
	enum { IDD = DIALOG_Display };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};


