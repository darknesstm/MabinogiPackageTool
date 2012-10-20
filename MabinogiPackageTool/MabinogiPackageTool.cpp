
// MabinogiPackageTool.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MabinogiPackageTool.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "MabinogiPackageToolDoc.h"
#include "LeftView.h"

#include "ProgressDialog.h"
#include "MakePackFilePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMabinogiPackageToolApp

BEGIN_MESSAGE_MAP(CMabinogiPackageToolApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMabinogiPackageToolApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_MAKE_PACK_FILE, &CMabinogiPackageToolApp::OnFileMakePackFile)
END_MESSAGE_MAP()


// CMabinogiPackageToolApp 构造

CMabinogiPackageToolApp::CMabinogiPackageToolApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MabinogiPackageTool.AppID.2012"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CMabinogiPackageToolApp 对象

CMabinogiPackageToolApp theApp;


// CMabinogiPackageToolApp 初始化

BOOL CMabinogiPackageToolApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("海森堡"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_MabinogiPackageTYPE,
		RUNTIME_CLASS(CMabinogiPackageToolDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CLeftView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// 启动不需要新建一个文档
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}

	// 启用“DDE 执行”
	EnableShellOpen();
	//RegisterShellFileTypes(TRUE);


	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CMabinogiPackageToolApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	// 删除之前创建的所有临时文件
	CFileFind finder;
	BOOL bWorking = finder.FindFile(GetMyTempPath() + GetMyTempFilePrefix() + TEXT("*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CFile::Remove(finder.GetFilePath());
	}
	return CWinAppEx::ExitInstance();
}

// CMabinogiPackageToolApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// 用于运行对话框的应用程序命令
void CMabinogiPackageToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMabinogiPackageToolApp 自定义加载/保存方法

void CMabinogiPackageToolApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	GetContextMenuManager()->AddMenu(strName + TEXT("@LeftView"), IDR_LEFT_POPUP_EDIT);

	//HMENU hMenu = GetContextMenuManager()->GetMenuById(IDR_LEFT_POPUP_EDIT);
	//::SetMenuDefaultItem(hMenu, 0, TRUE);
}

void CMabinogiPackageToolApp::LoadCustomState()
{
}

void CMabinogiPackageToolApp::SaveCustomState()
{
}

// CMabinogiPackageToolApp 消息处理程序
CFont * CMabinogiPackageToolApp::GetDefaultFont()
{
	return &afxGlobalData.fontRegular;
}
HBRUSH CMabinogiPackageToolApp::GetDefaultBackground()
{
	return afxGlobalData.brBtnFace;
}

CString CMabinogiPackageToolApp::GetMyTempPath(void)
{
	if (m_strTempPath.GetLength() == 0)
	{
		// 其实也可以直接使用CString的缓冲区
		TCHAR szTempPath[MAX_PATH + 1] = {0};
		::GetTempPath(MAX_PATH, szTempPath);
		m_strTempPath = szTempPath;
	}
	return m_strTempPath;
}


CString CMabinogiPackageToolApp::GetMyTempFilePrefix(void)
{
	if (m_strTempFilePrefix.GetLength() == 0)
	{
		m_strTempFilePrefix.Format(TEXT("mpt_tmp_%u_"), (DWORD) AfxGetInstanceHandle());
	}
	return m_strTempFilePrefix;
}

void RecursiveFindFile(CString& strPath, vector<CString> &collector)
{
	CFileFind ff;
	BOOL bFind = ff.FindFile(strPath + TEXT("\\*.*"));

	while(bFind)
	{
		bFind = ff.FindNextFile();
		if (ff.IsDots())
			continue;

		if (ff.IsDirectory())
		{
			RecursiveFindFile(ff.GetFilePath(), collector);
		}
		else
		{
			collector.push_back(ff.GetFilePath());
		}
	}
}

UINT lambda_OnFileMakePackFile(CProgressMonitor *pMonitor, LPVOID pParam)
{
	CMakePackFilePage *pPage = (CMakePackFilePage*) pParam;
			
	pMonitor->BeginTask(TEXT("制作pack文件：") + pPage->m_strOutputFile, -1);

	// 先查找出所有的文件
	vector<CString> filePaths;
	RecursiveFindFile(pPage->m_strInputFolder, filePaths);
	int prefixLength = lstrlen(pPage->m_strInputFolder);

	PPACKOUTPUT output = pack_output(pPage->m_strOutputFile, pPage->m_ulVersion);

	for(auto iter = filePaths.begin();iter != filePaths.end();++iter)
	{
		CString& filePath = *iter;
	//for (CString& filePath : filePaths)
	//{
		pMonitor->SubTask(filePath);

		USES_CONVERSION;

		if (pMonitor->IsCanceled())
		{
			break;
		}
		s_pack_entry entry;
		CFile file(filePath, CFile::modeRead);
		CFileStatus fs;
		file.GetStatus(fs);

		SYSTEMTIME  st;  
		FILETIME  ft;

		fs.m_mtime.GetAsSystemTime(st); 
		::SystemTimeToFileTime(&st,  &ft);
		memcpy(&entry.ft[0], &ft, sizeof(FILETIME));
		memcpy(&entry.ft[1], &ft, sizeof(FILETIME));

		fs.m_ctime.GetAsSystemTime(st); 
		::SystemTimeToFileTime(&st,  &ft); 
		memcpy(&entry.ft[2], &ft, sizeof(FILETIME));
		memcpy(&entry.ft[3], &ft, sizeof(FILETIME));

		fs.m_atime.GetAsSystemTime(st); 
		::SystemTimeToFileTime(&st,  &ft); 
		memcpy(&entry.ft[4], &ft, sizeof(FILETIME));

		// 将全路径转换为相对路径
		LPCTSTR lpszRelativePath = ((LPCTSTR)filePath) + prefixLength + 1;
		lstrcpyA( entry.name, CT2A(lpszRelativePath));

#pragma warning( push )
#pragma warning( disable : 4244 ) // 禁止类型转换丢失的警告
		// 写入文件内容，其实可以使用内存文件映射提高效率
		// pack文件的格式基础上无法加入单个大于4G的文件
		if (fs.m_size < (1024i64 * 1024 * 1024 * 4))
		{
			char *buffer = new char[fs.m_size];
			file.Read(buffer, fs.m_size);
			file.Close();

			pack_output_put_next_entry(output, &entry);
			pack_output_write(output, (byte*)buffer, fs.m_size);
			pack_output_close_entry(output);
			delete[] buffer;
		} else {
			// 警告信息
		}
#pragma warning( pop )
	}

	if (pMonitor->IsCanceled())
	{
		pack_output_drop(output);
	}
	else
	{
		pack_output_close(output);
	}

	return 0;
}

void CMabinogiPackageToolApp::OnFileMakePackFile()
{
	CMakePackFilePage page;
	CPropertySheet sheet(TEXT("制作Pack文件向导"), theApp.GetMainWnd());
	
	sheet.AddPage(&page);
	
	sheet.m_psh.dwFlags |= PSH_WIZARD97;
	sheet.m_psh.hInstance = AfxGetInstanceHandle();

	sheet.SetWizardMode();
	sheet.SetActivePage(&page);
	if (sheet.DoModal() ==  ID_WIZFINISH)
	{
		//CProgressDialog dlg(GetMainWnd()->GetSafeHwnd(), [](CProgressMonitor *pMonitor, LPVOID pParam) -> UINT
		//{
		//	CMakePackFilePage *pPage = (CMakePackFilePage*) pParam;
		//	
		//	pMonitor->BeginTask(TEXT("制作pack文件：") + pPage->m_strOutputFile, -1);

		//	// 先查找出所有的文件
		//	vector<CString> filePaths;
		//	RecursiveFindFile(pPage->m_strInputFolder, filePaths);
		//	int prefixLength = lstrlen(pPage->m_strInputFolder);

		//	PPACKOUTPUT output = pack_output(pPage->m_strOutputFile, pPage->m_ulVersion);


		//	for (CString& filePath : filePaths)
		//	{
		//		pMonitor->SubTask(filePath);

		//		USES_CONVERSION;

		//		if (pMonitor->IsCanceled())
		//		{
		//			break;
		//		}
		//		s_pack_entry entry;
		//		CFile file(filePath, CFile::modeRead);
		//		CFileStatus fs;
		//		file.GetStatus(fs);

		//		SYSTEMTIME  st;  
		//		FILETIME  ft;

		//		fs.m_mtime.GetAsSystemTime(st); 
		//		::SystemTimeToFileTime(&st,  &ft);
		//		memcpy(&entry.ft[0], &ft, sizeof(FILETIME));
		//		memcpy(&entry.ft[1], &ft, sizeof(FILETIME));

		//		fs.m_ctime.GetAsSystemTime(st); 
		//		::SystemTimeToFileTime(&st,  &ft); 
		//		memcpy(&entry.ft[2], &ft, sizeof(FILETIME));
		//		memcpy(&entry.ft[3], &ft, sizeof(FILETIME));

		//		fs.m_atime.GetAsSystemTime(st); 
		//		::SystemTimeToFileTime(&st,  &ft); 
		//		memcpy(&entry.ft[4], &ft, sizeof(FILETIME));

		//		// 将全路径转换为相对路径
		//		LPCTSTR lpszRelativePath = ((LPCTSTR)filePath) + prefixLength + 1;
		//		lstrcpyA( entry.name, CT2A(lpszRelativePath));

		//		
		//		// 写入文件内容，其实可以使用内存文件映射提高效率
		//		char *buffer = new char[fs.m_size];
		//		file.Read(buffer, fs.m_size);
		//		pack_output_put_next_entry(output, &entry);
		//		pack_output_write(output, (byte*)buffer, fs.m_size);
		//		pack_output_close_entry(output);
		//		free(buffer);
		//		file.Close();
		//	}

		//	if (pMonitor->IsCanceled())
		//	{
		//		pack_output_drop(output);
		//	}
		//	else
		//	{
		//		pack_output_close(output);
		//	}

		//	return 0;
		//}, &page, true);
		CProgressDialog dlg(GetMainWnd()->GetSafeHwnd(), lambda_OnFileMakePackFile,  &page, true);
		dlg.DoModal();
	}

	//CFolderPickerDialog fdlg(TEXT("选择一个"));

	//CProgressDialog dlg(GetMainWnd()->GetSafeHwnd(), [](CProgressMonitor *pMonitor, LPVOID pParam) -> UINT{
	//	pMonitor->BeginTask(TEXT("开始一个进度"), 100);
	//	for (int i = 0; i < 100; i++)
	//	{
	//		if (pMonitor->IsCanceled())
	//		{
	//			break;
	//		}
	//		CString tmp;
	//		tmp.Format(TEXT("- %d -"), i );
	//		pMonitor->SubTask(tmp);
	//		pMonitor->Worked(1);
	//		//pMonitor->Done();
	//		Sleep(50);
	//	}

	//	Sleep(1000);
	//	return 0;
	//}, NULL, false);

	//dlg.DoModal();
}