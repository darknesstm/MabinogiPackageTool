
// LeftView.cpp : CLeftView 类的实现
//

#include "stdafx.h"
#include "MabinogiPackageTool.h"

#include "MabinogiPackageToolDoc.h"
#include "LeftView.h"

#include <Uxtheme.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CLeftView::OnTvnSelchanged)
END_MESSAGE_MAP()


// CLeftView 构造/析构

CLeftView::CLeftView()
{
	// TODO: 在此处添加构造代码
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= TVS_HASBUTTONS | TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);
}


void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	SHFILEINFO ssfi;
	::SetWindowTheme(GetTreeCtrl().GetSafeHwnd(), L"Explorer", 0);

	TCHAR szCurDir [MAX_PATH + 1];
	if (GetCurrentDirectory(MAX_PATH, szCurDir) > 0)
	{
		SHFILEINFO sfi;
		GetTreeCtrl().SetImageList(CImageList::FromHandle((HIMAGELIST) SHGetFileInfo(
			szCurDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)), 0);
	}

    if ( SHGetFileInfo( szCurDir,
                        0,
                        &ssfi,
                        sizeof( SHFILEINFO ),
                        SHGFI_ICON | 
                        SHGFI_SMALLICON ) )
    {
		m_iIconFolder = ssfi.iIcon;

		// we only need the index from the system image list
		DestroyIcon( ssfi.hIcon ); 
    } 
	else
	{
		m_iIconFolder = 0;
	}



    if ( SHGetFileInfo( szCurDir,
                        0,
                        &ssfi,
                        sizeof( SHFILEINFO ),
                        SHGFI_ICON | SHGFI_OPENICON |
                        SHGFI_SMALLICON ) )
    {
		m_iIconFolderOpen = ssfi.iIcon;

		// we only need the index of the system image list
		DestroyIcon( ssfi.hIcon );
    } 
	else
	{
		m_iIconFolderOpen = 0;
	}



	shared_ptr<CPackFolder> m_spRoot = GetDocument()->GetRoot();
	if (m_spRoot.get())
	{
		InsertItem( TVI_ROOT, m_spRoot);
	}

}

void CLeftView::InsertItem( HTREEITEM hParentItem, shared_ptr<CPackFolder> spFolder )
{
	HTREEITEM hItem = GetTreeCtrl().InsertItem(spFolder->m_strName, hParentItem);
	GetTreeCtrl().SetItemImage(hItem, m_iIconFolder, m_iIconFolderOpen);
	GetTreeCtrl().SetItemData(hItem, (DWORD_PTR)spFolder.get());
	for (size_t i = 0; i < spFolder->m_children.size();i++)
	{
		InsertItem(hItem, spFolder->m_children.at(i));
	}
}
// CLeftView 诊断

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CMabinogiPackageToolDoc* CLeftView::GetDocument() // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMabinogiPackageToolDoc)));
	return (CMabinogiPackageToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView 消息处理程序


void CLeftView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	GetDocument()->m_pSelectedFolder = (CPackFolder*)GetTreeCtrl().GetItemData(GetTreeCtrl().GetSelectedItem());
	GetDocument()->UpdateAllViews(this);
	*pResult = 0;
}
