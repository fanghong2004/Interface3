// SelectView.cpp : 实现文件
//

#include "stdafx.h"
#include "Interface3.h"
#include "SelectView.h"
#include "MainFrm.h"


// CSelectView

IMPLEMENT_DYNCREATE(CSelectView, CTreeView)

CSelectView::CSelectView()
{

}

CSelectView::~CSelectView()
{
}

BEGIN_MESSAGE_MAP(CSelectView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CSelectView::OnTvnSelchanged)
END_MESSAGE_MAP()


// CSelectView 诊断

#ifdef _DEBUG
void CSelectView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CSelectView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSelectView 消息处理程序


void CSelectView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
	//初始化 树视图
	//获取树控件
	m_treeCtrl = &GetTreeCtrl();


	//1、准备图片集合
	
	HICON icon = AfxGetApp()->LoadIconW(IDI_ICON_Head);

	m_imageList.Create(30, 30, ILC_COLOR32, 1, 1);
	m_imageList.Add(icon);

	m_treeCtrl->SetImageList(&m_imageList, TVSIL_NORMAL);


	//2、添加节点
	m_treeCtrl->InsertItem(TEXT("模型介绍"), 0, 0, NULL);
	m_treeCtrl->InsertItem(TEXT("参数设置"), 0, 0, NULL);
	m_treeCtrl->InsertItem(TEXT("计算结果"), 0, 0, NULL);
	m_treeCtrl->InsertItem(TEXT("绘制图形"), 0, 0, NULL);
	// TODO: 在此添加专用代码和/或调用基类
}


void CSelectView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	//@TN
		//*pResult = 0;
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	//获取当前选中项

	HTREEITEM item = m_treeCtrl->GetSelectedItem();
	CString str = m_treeCtrl->GetItemText(item);
	//MessageBox(str);

	if(str==TEXT("模型介绍"))
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), NM_A, (WPARAM)NM_A, (LPARAM)0);
	}
	else if(str==TEXT("参数设置"))
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), NM_B, (WPARAM)NM_B, (LPARAM)0);
	}
	else if(str==TEXT("计算结果"))
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), NM_C, (WPARAM)NM_C, (LPARAM)0);
	}
	else if(str==TEXT("绘制图形"))
	{
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), NM_D, (WPARAM)NM_D, (LPARAM)0);
	}
}
