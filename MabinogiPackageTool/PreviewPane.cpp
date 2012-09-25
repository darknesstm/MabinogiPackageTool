// PreviewPane.cpp : 实现文件
//

#include "stdafx.h"
#include "MabinogiPackageTool.h"
#include "PreviewPane.h"


// CPreviewPane

IMPLEMENT_DYNAMIC(CPreviewPane, CDockablePane)

CPreviewPane::CPreviewPane()
{

}

CPreviewPane::~CPreviewPane()
{
}


BEGIN_MESSAGE_MAP(CPreviewPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CPreviewPane 消息处理程序




int CPreviewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_txtControl.Create(WS_VISIBLE|WS_CHILD|ES_READONLY|ES_MULTILINE|WS_VSCROLL|WS_HSCROLL, rectDummy, this, 1);

	UpdateFonts();

	return 0;
}


void CPreviewPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区:
	m_txtControl.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}


void CPreviewPane::SetTextContent(LPCTSTR content)
{
	m_txtControl.SetWindowText(content);
}

void CPreviewPane::UpdateFonts()
{
	m_txtControl.SetFont(theApp.GetDefaultFont());
}