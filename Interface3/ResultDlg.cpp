// ResultDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Interface3.h"
#include "ResultDlg.h"


// CResultDlg

IMPLEMENT_DYNCREATE(CResultDlg, CFormView)

CResultDlg::CResultDlg()
	: CFormView(CResultDlg::IDD)
{

}

CResultDlg::~CResultDlg()
{
}

void CResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CResultDlg, CFormView)
END_MESSAGE_MAP()


// CResultDlg 诊断

#ifdef _DEBUG
void CResultDlg::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CResultDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CResultDlg 消息处理程序
