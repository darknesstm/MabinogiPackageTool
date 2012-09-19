// ProgressWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "MabinogiPackageTool.h"
#include "ProgressDialog.h"

#define IDC_CANCEL   10
#define IDC_TASK_NAME     11
#define IDC_SUB_TASK_NAME     12
#define IDC_PROGRESS 13

CProgressMonitor::CProgressMonitor()
{
}

CProgressMonitor::~CProgressMonitor()
{
}

// CProgressWnd

IMPLEMENT_DYNAMIC(CProgressDialog, CWnd)

CProgressDialog::CProgressDialog(HWND hParentWnd)
{
	Create(hParentWnd, TEXT("进度"));
	m_hParentWnd = hParentWnd;
}

CProgressDialog::~CProgressDialog()
{
	if (IsWindow(m_hWnd))
	{
		DestroyWindow();
	}
}


BEGIN_MESSAGE_MAP(CProgressDialog, CWnd)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

struct SThreadParamter
{
	bool isThreadEnd;
	RunnableFunc fnRunnable;
	CProgressMonitor *pMonitor;
	LPVOID pParam;
};

UINT RunnableThreadFuc(LPVOID pParam)
{
	SThreadParamter *p = (SThreadParamter*) pParam;
	try
	{
		if(p->fnRunnable != nullptr)
		{
			p->fnRunnable(nullptr, pParam);
		}
	}
	catch (...)
	{
	}

	p->isThreadEnd = true;

	return 0;
}
// CProgressWnd 消息处理程序


BOOL CProgressDialog::Create(HWND hParentWnd, LPCTSTR pszTitle)
{
	CString csClassName = AfxRegisterWndClass(CS_OWNDC|CS_HREDRAW|CS_VREDRAW,
                                              ::LoadCursor(NULL, IDC_APPSTARTING),
                                              CBrush(::GetSysColor(COLOR_BTNFACE)));

	BOOL bSuccess = CreateEx(WS_EX_DLGMODALFRAME, // Extended style
                        csClassName,                       // Classname
                        pszTitle,                          // Title
						WS_BORDER|WS_CAPTION|WS_POPUP,     // style
                        0,0,                               // position - updated soon.
                        390,130,                           // Size - updated soon
                        hParentWnd,            // handle to parent
                        0,                                 // No menu
                        NULL);    
    if (!bSuccess) 
		return FALSE;

	bSuccess = m_lblTaskName.Create(TEXT(""), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, IDC_TASK_NAME);
	if (!bSuccess) 
		return FALSE;

	bSuccess = m_lblSubTaskName.Create(TEXT(""), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, IDC_TASK_NAME);
	if (!bSuccess) 
		return FALSE;

	bSuccess = m_wndProgress.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, IDC_PROGRESS);
	if (!bSuccess) 
		return FALSE;


	//m_wndProgress.SendMessage(PBM_SETMARQUEE, TRUE, 200);

	return TRUE;
}

int CProgressDialog::DoModal(RunnableFunc fnRunnable, LPVOID pParam)
{
	::EnableWindow(m_hParentWnd, FALSE);
	EnableWindow(TRUE);

	CenterWindow();
	ShowWindow(SW_SHOW);

	// 启动线程
	SThreadParamter param;
	param.isThreadEnd = false;
	param.fnRunnable = fnRunnable;
	param.pMonitor = NULL;
	param.pParam = pParam;
	CWinThread *pThread = AfxBeginThread(RunnableThreadFuc, (LPVOID)&param);

	// 拦截消息循环
	MSG msg;
	while (!param.isThreadEnd) 
	{
		if (::PeekMessage(&msg, NULL,0,0,PM_NOREMOVE))
		{
			if (!AfxGetApp()->PumpMessage()) 
			{
				::PostQuitMessage(0);
				return 0;
			}
		}
	}

	DestroyWindow();

	DWORD dwExitCode;
	GetExitCodeThread(pThread->m_hThread, &dwExitCode);
	return dwExitCode;
}


void CProgressDialog::OnClose()
{
	CWnd::OnClose();
}


void CProgressDialog::OnDestroy()
{
	CWnd::OnDestroy();
	::EnableWindow(m_hParentWnd, TRUE);
	::BringWindowToTop(m_hParentWnd);
}


void CProgressDialog::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// 从上到下依次是 任务名 进度条 子任务名

	//m_progress.MoveWindow();
}
