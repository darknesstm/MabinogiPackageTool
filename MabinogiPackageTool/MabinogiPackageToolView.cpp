
// MabinogiPackageToolView.cpp : CMabinogiPackageToolView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MabinogiPackageTool.h"
#endif

#include "MabinogiPackageToolDoc.h"
#include "MabinogiPackageToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMabinogiPackageToolView

IMPLEMENT_DYNCREATE(CMabinogiPackageToolView, CListView)

BEGIN_MESSAGE_MAP(CMabinogiPackageToolView, CListView)
	ON_WM_STYLECHANGED()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMabinogiPackageToolView 构造/析构

CMabinogiPackageToolView::CMabinogiPackageToolView()
{
	// TODO: 在此处添加构造代码

}

CMabinogiPackageToolView::~CMabinogiPackageToolView()
{
}

BOOL CMabinogiPackageToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= LVS_REPORT|LVS_SHAREIMAGELISTS;
	cs.dwExStyle |= LVS_EX_TRACKSELECT;
	return CListView::PreCreateWindow(cs);
}

HIMAGELIST CMabinogiPackageToolView::GetShellImageList(BOOL bLarge)
{
	TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory(szWinDir, MAX_PATH) == 0)
	{
		return NULL;
	}

	SHFILEINFO sfi;
	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo(szWinDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX |(bLarge ? 0 : SHGFI_SMALLICON));
	return hImageList;
}

void CMabinogiPackageToolView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	::SetWindowTheme(GetListCtrl().GetSafeHwnd(), L"Explorer", 0);

	TCHAR szCurDir [MAX_PATH + 1];
	if (GetCurrentDirectory(MAX_PATH, szCurDir) > 0)
	{
		SHFILEINFO sfi;
		GetListCtrl().SetImageList(CImageList::FromHandle((HIMAGELIST) SHGetFileInfo(
			szCurDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)), 0);
	}

	GetListCtrl().SetImageList(CImageList::FromHandle(GetShellImageList(TRUE)), LVSIL_NORMAL);
	GetListCtrl().SetImageList(CImageList::FromHandle(GetShellImageList(FALSE)), LVSIL_SMALL);

	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	GetListCtrl().InsertColumn(0, TEXT("文件名"), 0 , 100);
	GetListCtrl().InsertColumn(1, TEXT("类型"), 0 , 100);
	GetListCtrl().InsertColumn(2, TEXT("大小"), 0 , 100);
	GetListCtrl().InsertColumn(3, TEXT("压缩后大小"), 0 , 100);
	GetListCtrl().InsertColumn(4, TEXT("创建日期"), 0 , 100);
	GetListCtrl().InsertColumn(5, TEXT("最后写入日期"), 0 , 100);
	GetListCtrl().InsertColumn(6, TEXT("最后访问日期"), 0 , 100);
}

void CMabinogiPackageToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMabinogiPackageToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMabinogiPackageToolView 诊断

#ifdef _DEBUG
void CMabinogiPackageToolView::AssertValid() const
{
	CListView::AssertValid();
}

void CMabinogiPackageToolView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMabinogiPackageToolDoc* CMabinogiPackageToolView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMabinogiPackageToolDoc)));
	return (CMabinogiPackageToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMabinogiPackageToolView 消息处理程序
void CMabinogiPackageToolView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: 添加代码以响应用户对窗口视图样式的更改	
	CListView::OnStyleChanged(nStyleType,lpStyleStruct);	
}


void CMabinogiPackageToolView::OnUpdate(CView* pSender, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	if (pSender != this)
	{
		GetListCtrl().DeleteAllItems();

		CPackFolder * pFolder = GetDocument()->m_pSelectedFolder;
		if ( pFolder != 0)
		{
			for (size_t i = 0;i < pFolder->m_entries.size();i++)
			{
				shared_ptr<CPackEntry> spFile = pFolder->m_entries.at(i);
				SHFILEINFO shFilefo;
				SHGetFileInfo( spFile->m_strName ,FILE_ATTRIBUTE_NORMAL , &shFilefo, sizeof(shFilefo),
					SHGFI_TYPENAME|SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX );
				
				int nItem = GetListCtrl().InsertItem(0, spFile->m_strName, shFilefo.iIcon);
				GetListCtrl().SetItemText(nItem, 1, shFilefo.szTypeName);

				CString tmp;
				// 余下信息填充
			}
		}
	}
}
