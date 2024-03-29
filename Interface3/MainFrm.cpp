
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Interface3.h"

#include "MainFrm.h"
#include "SelectView.h"
#include "DisplayView.h"
#include "UserDlg.h"
#include "ParaDlg.h"
#include "ResultDlg.h"
#include "DisplayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()

	//ON_MESSAGE响应的是自定义消息
	//产生NM_X消息，自动调用OnMyChange函数
	ON_MESSAGE(NM_A, OnMyChange)
	ON_MESSAGE(NM_B, OnMyChange)
	ON_MESSAGE(NM_C, OnMyChange)
	ON_MESSAGE(NM_D, OnMyChange)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	//设置图标
	SetClassLong(m_hWnd, GCL_HICON, (LONG)AfxGetApp()->LoadIconW(IDI_ICON_Title));

	//设置标题
	SetTitle(TEXT("版本1.1"));

	//设置窗口大小
	MoveWindow(0,0,800,500);

	//设置居中
	CenterWindow();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	//return CFrameWnd::OnCreateClient(lpcs, pContext);

	//拆成一行两列
	m_spliter.CreateStatic(this, 1, 2);

	//左侧和右侧的显示内容
	m_spliter.CreateView(0, 0, RUNTIME_CLASS(CSelectView),CSize(200,500), pContext);
	m_spliter.CreateView(0, 1, RUNTIME_CLASS(CDisplayView),CSize(600,500), pContext);

	return TRUE;
}

LRESULT CMainFrame::OnMyChange(WPARAM wParam, LPARAM lParam)
{
	CCreateContext Context;
	if(wParam==NM_A)
	{
		//MessageBox(TEXT("模型介绍挂载"));
		//挂载界面
		Context.m_pNewViewClass = RUNTIME_CLASS(CUserDlg);
		Context.m_pCurrentFrame = this;
		Context.m_pLastView = (CFormView* )m_spliter.GetPane(0, 1);
		m_spliter.DeleteView(0, 1);
		m_spliter.CreateView(0, 1, RUNTIME_CLASS(CUserDlg), CSize(600, 500), &Context);
		CUserDlg* pNewView = (CUserDlg* )m_spliter.GetPane(0,1);
		m_spliter.RecalcLayout();
		pNewView->OnInitialUpdate();
		m_spliter.SetActivePane(0, 1);
	}
	else if(wParam==NM_B)
	{
		//MessageBox(TEXT("参数设置挂载"));
		Context.m_pNewViewClass = RUNTIME_CLASS(CParaDlg);
		Context.m_pCurrentFrame = this;
		Context.m_pLastView = (CFormView* )m_spliter.GetPane(0, 1);
		m_spliter.DeleteView(0, 1);
		m_spliter.CreateView(0, 1, RUNTIME_CLASS(CParaDlg), CSize(600, 500), &Context);
		CParaDlg* pNewView = (CParaDlg* )m_spliter.GetPane(0,1);
		m_spliter.RecalcLayout();
		pNewView->OnInitialUpdate();
		m_spliter.SetActivePane(0, 1);
	}
	else if(wParam==NM_C)
	{
		//MessageBox(TEXT("参数设置挂载"));
		Context.m_pNewViewClass = RUNTIME_CLASS(CResultDlg);
		Context.m_pCurrentFrame = this;
		Context.m_pLastView = (CFormView* )m_spliter.GetPane(0, 1);
		m_spliter.DeleteView(0, 1);
		m_spliter.CreateView(0, 1, RUNTIME_CLASS(CResultDlg), CSize(600, 500), &Context);
		CResultDlg* pNewView = (CResultDlg* )m_spliter.GetPane(0,1);
		m_spliter.RecalcLayout();
		pNewView->OnInitialUpdate();
		m_spliter.SetActivePane(0, 1);
	}
	else if(wParam==NM_D)
	{
		//MessageBox(TEXT("参数设置挂载"));
		Context.m_pNewViewClass = RUNTIME_CLASS(CDisplayDlg);
		Context.m_pCurrentFrame = this;
		Context.m_pLastView = (CFormView* )m_spliter.GetPane(0, 1);
		m_spliter.DeleteView(0, 1);
		m_spliter.CreateView(0, 1, RUNTIME_CLASS(CDisplayDlg), CSize(600, 500), &Context);
		CDisplayDlg* pNewView = (CDisplayDlg* )m_spliter.GetPane(0,1);
		m_spliter.RecalcLayout();
		pNewView->OnInitialUpdate();
		m_spliter.SetActivePane(0, 1);
	}

	return 0;
}