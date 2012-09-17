
// MabinogiPackageToolView.h : CMabinogiPackageToolView 类的接口
//

#pragma once


class CMabinogiPackageToolView : public CListView
{
protected: // 仅从序列化创建
	CMabinogiPackageToolView();
	DECLARE_DYNCREATE(CMabinogiPackageToolView)

// 特性
public:
	CMabinogiPackageToolDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CMabinogiPackageToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	HIMAGELIST GetShellImageList(BOOL bLarge);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
//	afx_msg void OnCancelMode();
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // MabinogiPackageToolView.cpp 中的调试版本
inline CMabinogiPackageToolDoc* CMabinogiPackageToolView::GetDocument() const
   { return reinterpret_cast<CMabinogiPackageToolDoc*>(m_pDocument); }
#endif

