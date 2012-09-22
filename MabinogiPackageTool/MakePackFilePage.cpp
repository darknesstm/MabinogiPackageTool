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
	, m_nVersion(100)
{
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
    m_psp.pszHeaderTitle = _T("制作Pack文件");
    m_psp.pszHeaderSubTitle = _T("可以指定一个版本和文件夹用于创建一个Pack文件，注意选中的文件夹将作为Pack文件中的Data根文件夹。");

}

CMakePackFilePage::~CMakePackFilePage()
{
}

void CMakePackFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT_FOLDER, m_strInputFolder);
	DDX_Text(pDX, IDC_OUTPUT_FILE, m_strOutputFile);
	DDX_Text(pDX, IDC_VERSION, m_nVersion);
	DDX_Control(pDX, IDC_VERSION_SPIN, m_spinVersion);
}


BEGIN_MESSAGE_MAP(CMakePackFilePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BROWSER_INPUT, &CMakePackFilePage::OnClickedBrowserInput)
	ON_BN_CLICKED(IDC_BROWSER_OUTPUT, &CMakePackFilePage::OnClickedBrowserOutput)
END_MESSAGE_MAP()


// CMakePackFilePage 消息处理程序


BOOL CMakePackFilePage::OnSetActive()
{
	GetParentSheet()->SetWizardButtons(PSWIZB_FINISH);
	return CPropertyPage::OnSetActive();
}


void CMakePackFilePage::OnClickedBrowserInput()
{
	CFolderPickerDialog dlg(TEXT("选择打包文件夹"), 0, this);
	if (dlg.DoModal() == IDOK)
	{
		m_strInputFolder = dlg.GetPathName();
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
