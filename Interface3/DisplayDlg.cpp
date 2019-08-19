// DisplayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Interface3.h"
#include "DisplayDlg.h"


// CDisplayDlg

IMPLEMENT_DYNCREATE(CDisplayDlg, CFormView)

CDisplayDlg::CDisplayDlg()
	: CFormView(CDisplayDlg::IDD)
{

}

CDisplayDlg::~CDisplayDlg()
{
}

void CDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDisplayDlg, CFormView)
END_MESSAGE_MAP()


// CDisplayDlg 诊断

#ifdef _DEBUG
void CDisplayDlg::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDisplayDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDisplayDlg 消息处理程序
