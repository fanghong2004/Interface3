
// Interface3View.h : CInterface3View 类的接口
//

#pragma once


class CInterface3View : public CView
{
protected: // 仅从序列化创建
	CInterface3View();
	DECLARE_DYNCREATE(CInterface3View)

// 特性
public:
	CInterface3Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CInterface3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Interface3View.cpp 中的调试版本
inline CInterface3Doc* CInterface3View::GetDocument() const
   { return reinterpret_cast<CInterface3Doc*>(m_pDocument); }
#endif

