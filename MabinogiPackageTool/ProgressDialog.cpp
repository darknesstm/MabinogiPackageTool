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

CProgressDialog::CProgressDialog(HWND hParentWnd, RunnableFunc fnRunnable, LPVOID pRunnableParam)
{
	m_hParentWnd = hParentWnd;
	m_fnRunnable = fnRunnable;
	m_pRunnableParam = pRunnableParam;

	Create(hParentWnd, TEXT("进度"));
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
	LPVOID pRunnableParam;
};

UINT RunnableThreadFuc(LPVOID pParam)
{
	UINT result = 0;
	SThreadParamter *p = (SThreadParamter*) pParam;
	try
	{
		if(p->fnRunnable != nullptr)
		{
			result = p->fnRunnable(nullptr, p->pRunnableParam);
		}
	}
	catch (...)
	{
	}

	p->isThreadEnd = true;

	return result;
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
						WS_BORDER|WS_CAPTION|WS_POPUP|WS_SIZEBOX,     // style
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


	OnSize(0,390, 130);
	return TRUE;
}

int CProgressDialog::DoModal()
{
	::EnableWindow(m_hParentWnd, FALSE);
	EnableWindow(TRUE);

	CenterWindow();
	ShowWindow(SW_SHOW);

	// 启动线程
	SThreadParamter threadParam;
	threadParam.isThreadEnd = false;
	threadParam.fnRunnable = m_fnRunnable;
	threadParam.pMonitor = NULL;
	threadParam.pRunnableParam = m_pRunnableParam;
	CWinThread *pThread = AfxBeginThread(RunnableThreadFuc, (LPVOID)&threadParam);

	// 拦截消息循环
	MSG msg;
	while (!threadParam.isThreadEnd) 
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
	// 进度条保持固定高度为30
	// 四周缩进5
	if (cx > 10 && cy > 40)
	{
		if (::IsWindow(m_lblTaskName.m_hWnd))
		{
			m_lblTaskName.MoveWindow(5, 5, cx - 10, (cy - 10 - 30) / 2 - 5);
		}
	
		if (::IsWindow(m_wndProgress.m_hWnd))
		{
			m_wndProgress.MoveWindow(5, (cy - 10 - 30) / 2, cx - 10, 30);
		}
	
		if (::IsWindow(m_lblSubTaskName.m_hWnd))
		{
			m_lblSubTaskName.MoveWindow(5, (cy - 10 - 30) / 2 + 35, cx - 10, (cy - 10 - 30) / 2 - 5);
		}
	}

	

	//m_progress.MoveWindow();
}
