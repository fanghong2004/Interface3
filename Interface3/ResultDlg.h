#pragma once



// CResultDlg 窗体视图

class CResultDlg : public CFormView
{
	DECLARE_DYNCREATE(CResultDlg)

protected:
	CResultDlg();           // 动态创建所使用的受保护的构造函数
	virtual ~CResultDlg();

public:
	enum { IDD = DIALOG_Result };
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


