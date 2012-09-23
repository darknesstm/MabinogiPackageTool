
// LeftView.cpp : CLeftView 类的实现
//

#include "stdafx.h"
#include "MabinogiPackageTool.h"

#include "MabinogiPackageToolDoc.h"
#include "LeftView.h"

#include <Uxtheme.h>
#include "ProgressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CLeftView::OnTvnSelchanged)
	ON_WM_CONTEXTMENU()

	ON_NOTIFY_REFLECT(NM_RCLICK, &CLeftView::OnNMRClick)
	ON_COMMAND(ID_EDIT_EXTRACT_TO, &CLeftView::OnEditExtractTo)
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


void CLeftView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_LEFT_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


void CLeftView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_TREEVIEW* pHdr = (NM_TREEVIEW*) pNMHDR;

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	// 同时选中节点
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
	   GetTreeCtrl().Select(hItem, TVGN_CARET);
	}

	ClientToScreen(&point);

	OnContextMenu(this, point);
	*pResult = 0;
}


void CLeftView::OnEditExtractTo()
{
	// 解压整个文件夹
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem != nullptr)
	{
		CFolderPickerDialog dlg(0, 0, this);
		if (dlg.DoModal() == IDOK)
		{
			static CString path;
			static CPackFolder *pFolder;

			path = dlg.GetPathName();
			pFolder = (CPackFolder*) GetTreeCtrl().GetItemData(hItem);
			CProgressDialog dlg(theApp.GetMainWnd()->GetSafeHwnd(), [](CProgressMonitor *pMonitor, LPVOID pParam) -> UINT
			{
				pMonitor->BeginTask(TEXT("解压到：") + path, -1);
				CLeftView *pLeftView = (CLeftView*) pParam;
				pLeftView->ExtractTo(pFolder, path + TEXT("\\") + pFolder->m_strName, pMonitor);
				return 0;
			}, this, true);

			dlg.DoModal();
		}
	}
}


void CLeftView::ExtractTo(CPackFolder *pFolder, CString strPath, CProgressMonitor *pMonitor)
{
	for (auto spEntry : pFolder->m_entries)
	{
		CString outputPath = strPath + TEXT("\\") + spEntry->m_strName;
		if (pMonitor != nullptr)
		{
			if (pMonitor->IsCanceled())
			{
				return;
			}
			else
			{
				USES_CONVERSION;
				pMonitor->SubTask(outputPath);
			}
		}
		
		spEntry->WriteToFile(outputPath);
	}

	for (auto spFolder : pFolder->m_children)
	{
		CString outputPath = strPath  + TEXT("\\") + spFolder->m_strName;
		if (pMonitor != nullptr)
		{
			if (pMonitor->IsCanceled())
			{
				return;
			}
			else
			{
				pMonitor->SubTask(outputPath);
			}
		}
		ExtractTo(spFolder.get(), outputPath, pMonitor);
	}
}
