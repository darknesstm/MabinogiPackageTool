// MakePackFilePage.cpp : 实现文件
//

#include "stdafx.h"
#include "MabinogiPackageTool.h"
#include "MakePackFilePage.h"
#include "afxdialogex.h"


// CMakePackFilePage 对话框

IMPLEMENT_DYNAMIC(CMakePackFilePage, CPropertyPage)

CMakePackFilePage::CMakePackFilePage()
	: CPropertyPage(CMakePackFilePage::IDD)
	, m_strInputFolder(_T(""))
	, m_strOutputFile(_T(""))
	, m_ulVersion(100)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    m_psp.pszHeaderTitle = _T("制作Pack文件");
    m_psp.pszHeaderSubTitle = _T("可以指定一个版本和文件夹用于创建一个Pack文件，注意选中的文件夹将作为Pack文件中的Data根文件夹。");

}

CMakePackFilePage::~CMakePackFilePage()
{
}

bool ValidateFileOrFolderPath(/*CWnd *pControl, */CString &path, bool isFolder)
{
	if (path.IsEmpty())
	{
		AfxMessageBox(TEXT("请选择文件或文件夹"));
		//pControl->SetFocus();
		return false;
	}

	CFileFind finder;
	BOOL bWorking = finder.FindFile(path);
	if (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (isFolder != (bool)finder.IsDirectory())
		{
			AfxMessageBox(TEXT("选择的不是文件或文件夹：") + path);
			//pControl->SetFocus();
			return false;
		}
	} else {
		AfxMessageBox(TEXT("无法找到文件或文件夹：") + path);
		//pControl->SetFocus();
		return false;
	}
	return true;
}

//void AFXAPI DDV_FilePath(CDataExchange* pDX, CString &value, bool isFolder)
//{
//	if (value.IsEmpty())
//	{
//		AfxMessageBox(TEXT("请选择文件或文件夹"));
//		pDX->Fail();
//	}
//	CFileFind finder;
//	BOOL bWorking = finder.FindFile(value);
//	if (bWorking)
//	{
//		bWorking = finder.FindNextFile();
//		if (isFolder != (bool)finder.IsDirectory())
//		{
//			AfxMessageBox(TEXT("选择的不是文件或文件夹：" + value));
//			pDX->Fail();
//		}
//	} else {
//		AfxMessageBox(TEXT("无法找到文件或文件夹：" + value));
//		pDX->Fail();
//	}
//}

void CMakePackFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT_FOLDER, m_strInputFolder);
	DDX_Text(pDX, IDC_OUTPUT_FILE, m_strOutputFile);
	DDX_Text(pDX, IDC_VERSION, m_ulVersion);

	DDV_MinMaxULongLong(pDX, m_ulVersion, 0, 4294967295l);

	//DDV_FilePath(pDX, m_strInputFolder, true);
	//DDV_FilePath(pDX, m_strOutputFile, false);
}


BEGIN_MESSAGE_MAP(CMakePackFilePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BROWSER_INPUT, &CMakePackFilePage::OnClickedBrowserInput)
	ON_BN_CLICKED(IDC_BROWSER_OUTPUT, &CMakePackFilePage::OnClickedBrowserOutput)
END_MESSAGE_MAP()


// CMakePackFilePage 消息处理程序


BOOL CMakePackFilePage::OnSetActive()
{
	//GetParentSheet()->SetWizardButtons(PSWIZB_FINISH);
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_FINISH);
	return CPropertyPage::OnSetActive();
}


void CMakePackFilePage::OnClickedBrowserInput()
{
	CString strOutput;
	if (theApp.GetShellManager()->BrowseForFolder(strOutput, this, 0, TEXT("选择打包文件夹"), 
		BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_NEWDIALOGSTYLE|BIF_USENEWUI) != FALSE)
	{
		m_strInputFolder = strOutput;
		UpdateData(FALSE);
	}
}


void CMakePackFilePage::OnClickedBrowserOutput()
{
	CFileDialog dlg(FALSE, TEXT("pack"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		TEXT("Mabinogi Resources Files (*.pack)|*.pack||"), this);
	if (dlg.DoModal() == IDOK)
	{
		m_strOutputFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
}


BOOL CMakePackFilePage::OnWizardFinish()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (UpdateData())
	{
		// 保证m_strInputFolder没有最后的'\\'
		if (!m_strInputFolder.IsEmpty() && m_strInputFolder.Right(1) == TEXT('\\'))
		{
			m_strInputFolder.Truncate(m_strInputFolder.GetLength() - 1);
		}

		return ValidateFileOrFolderPath(m_strInputFolder, true);
	}
	return FALSE;
	
}


BOOL CMakePackFilePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;
}
