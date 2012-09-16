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
END_MESSAGE_MAP()



// CPreviewPane 消息处理程序




int CPreviewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
