
// MabinogiPackageToolDoc.cpp : CMabinogiPackageToolDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MabinogiPackageTool.h"
#endif

#include "MabinogiPackageToolDoc.h"

#include <propkey.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMabinogiPackageToolDoc

IMPLEMENT_DYNCREATE(CMabinogiPackageToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CMabinogiPackageToolDoc, CDocument)
END_MESSAGE_MAP()


// CMabinogiPackageToolDoc 构造/析构

CMabinogiPackageToolDoc::CMabinogiPackageToolDoc()
{
	// TODO: 在此添加一次性构造代码
	m_pPackInput = NULL;
	m_spRoot = shared_ptr<CPackFolder>(new CPackFolder);
}

CMabinogiPackageToolDoc::~CMabinogiPackageToolDoc()
{
}

BOOL CMabinogiPackageToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMabinogiPackageToolDoc 序列化

void CMabinogiPackageToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
		m_pPackInput = pack_input(ar.GetFile()->GetFilePath());

		Parse();

	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMabinogiPackageToolDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMabinogiPackageToolDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMabinogiPackageToolDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMabinogiPackageToolDoc 诊断

#ifdef _DEBUG
void CMabinogiPackageToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMabinogiPackageToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMabinogiPackageToolDoc 命令

shared_ptr<CPackFolder> CMabinogiPackageToolDoc::GetRoot()
{
	return m_spRoot;
}


void CMabinogiPackageToolDoc::DeleteContents()
{
	if (m_pPackInput)
	{
		pack_input_close(m_pPackInput);
		m_pPackInput = NULL;
	}

	m_spRoot->Clean();
}


void CMabinogiPackageToolDoc::Parse(void)
{
	m_pSelectedFolder = NULL;
	m_spRoot->Clean();
	if (m_pPackInput)
	{
		// TODO 从文件中读取顶层文件夹名
		m_spRoot->m_strName = TEXT("Data");

		// 解析成树状结构
		size_t count = pack_input_get_entry_count(m_pPackInput);
		for (size_t i = 0; i < count; i++)
		{
			USES_CONVERSION;
			PPACKENTRY pEntry = pack_input_get_entry(m_pPackInput, i);
			
			static LPCTSTR lpszTokens = TEXT("/\\");
			int pos = 0;
			// 分割路径
			vector<CString> paths;
			CString resToken;
			CString fullName = CA2W(pEntry->name);
			resToken = fullName.Tokenize(lpszTokens, pos);
			while (resToken != TEXT(""))
			{
				paths.push_back(resToken);
				resToken = fullName.Tokenize(lpszTokens, pos);
			}

			CString fileName = *paths.rbegin();
			paths.pop_back();

			shared_ptr<CPackFolder> spFolder = m_spRoot;
			for (size_t j = 0;j < paths.size();j++)
			{
				CString path = paths.at(j);
				spFolder = spFolder->FindOrCreateFolder(path);
			}

			shared_ptr<CPackEntry> entry(new CPackEntry);
			entry->index = i;
			entry->m_pInput = m_pPackInput;
			entry->m_strName = fileName;
			spFolder->m_entries.push_back(entry);

		}

		m_pSelectedFolder = m_spRoot.get();
	}
}
