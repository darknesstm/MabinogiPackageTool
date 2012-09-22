// ProgressWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "MabinogiPackageTool.h"
#include "ProgressDialog.h"

#define IDC_CANCEL   10
#define IDC_TASK_NAME     11
#define IDC_SUB_TASK_NAME     12
#define IDC_PROGRESS 13

///////////////////////////////
class CProgressMonitorImpl : public CProgressMonitor
{
public:
	CProgressMonitorImpl(HWND hWndProgress, HWND hlblTaskName, HWND hlblSubTaskName)
	{
		m_hWndProgress = hWndProgress;
		m_hlblTaskName = hlblTaskName;
		m_hlblSubTaskName = hlblSubTaskName;
		m_isCanceled = false;
	};

	virtual ~CProgressMonitorImpl()
	{
	};

	void DoSetWindowText(HWND hWnd, LPCTSTR lpszName)
	{
		CRect rcWindow;
		::SendMessage(hWnd, WM_SETTEXT, 0, (WPARAM)lpszName);
		//::GetWindowRect(hWnd, &rcWindow);
		//InvalidateRect(hWnd, 0, TRUE);
	}

	virtual void BeginTask(LPCTSTR lpszName, int totalWork)
	{
		// 指针注意
		DoSetWindowText(m_hlblTaskName, lpszName);
		if (totalWork == UNKNOWN)
		{
			CWnd::ModifyStyle(m_hWndProgress, 0, PBS_MARQUEE, 0);
			::SendMessage(m_hWndProgress, PBM_SETMARQUEE, TRUE, 20);
		} 
		else
		{
			CWnd::ModifyStyle(m_hWndProgress, 0, PBS_SMOOTHREVERSE, 0);
			::SendMessage(m_hWndProgress, PBM_SETRANGE32, 0, totalWork);
		}

		m_nMaxValue = totalWork;
		m_nWorkedValue = 0;
	};
	virtual void Done()
	{
		if (m_nMaxValue > 0)
		{
			::SendMessage(m_hWndProgress, PBM_SETPOS, m_nMaxValue, 0);
		}
		else
		{
			::SendMessage(m_hWndProgress, PBM_SETMARQUEE, FALSE, 0);
		}

	};
	virtual bool IsCanceled() { return m_isCanceled; };
	virtual void SetCanceled(bool value) { m_isCanceled = value; };
	virtual void SetTaskName(LPCTSTR lpszName)
	{
		DoSetWindowText(m_hlblTaskName, lpszName);
	}
	virtual void SubTask(LPCTSTR lpszName)
	{
		DoSetWindowText(m_hlblSubTaskName, lpszName);
	}
	virtual void Worked(int work)
	{
		if (m_nMaxValue > 0)
		{
			m_nWorkedValue += work;
			if (m_nWorkedValue > m_nMaxValue)
			{
				m_nWorkedValue = m_nMaxValue;
			}
			::SendMessage(m_hWndProgress, PBM_SETPOS, m_nWorkedValue, 0);
		}
	}

private:
	bool m_isCanceled;
	int m_nMaxValue;
	int m_nWorkedValue;

	HWND m_hlblTaskName;
	HWND m_hlblSubTaskName;
	HWND m_hWndProgress;
};


///////////////////////////////

// CProgressWnd

IMPLEMENT_DYNAMIC(CProgressDialog, CWnd)

CProgressDialog::CProgressDialog(HWND hParentWnd, RunnableFunc fnRunnable, LPVOID pRunnableParam, bool bCancelable)
{
	m_hParentWnd = hParentWnd;
	m_fnRunnable = fnRunnable;
	m_pRunnableParam = pRunnableParam;
	m_bCancelable = bCancelable;
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
	ON_WM_CTLCOLOR()
//	ON_COMMAND(IDCANCEL, &CProgressDialog::OnCancel)
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
			result = p->fnRunnable(p->pMonitor, p->pRunnableParam);
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
											  GetGlobalData()->brBtnFace);

	BOOL bSuccess = CreateEx(WS_EX_DLGMODALFRAME, // Extended style
                        csClassName,                       // Classname
                        pszTitle,                          // Title
						WS_BORDER|WS_CAPTION|WS_POPUP,     // style
                        0,0,                               // position - updated soon.
                        500,130,                           // Size - updated soon
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

	bSuccess = m_btnCancel.Create(TEXT("取消"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, IDCANCEL);
	if (!bSuccess) 
		return FALSE;

	m_lblTaskName.SetFont(&GetGlobalData()->fontRegular);
	m_lblSubTaskName.SetFont(&GetGlobalData()->fontRegular);
	m_btnCancel.SetFont(&GetGlobalData()->fontRegular);

	if (!m_bCancelable)
	{
		m_btnCancel.EnableWindow(false);
	}

	Layout();

	return TRUE;
}

int CProgressDialog::DoModal()
{
	::EnableWindow(m_hParentWnd, FALSE);
	EnableWindow(TRUE);

	CenterWindow();
	ShowWindow(SW_SHOW);

	// 启动线程
	CProgressMonitorImpl monitor(m_wndProgress.GetSafeHwnd(), m_lblTaskName.GetSafeHwnd(), m_lblSubTaskName.GetSafeHwnd());
	SThreadParamter threadParam;
	threadParam.isThreadEnd = false;
	threadParam.fnRunnable = m_fnRunnable;
	threadParam.pMonitor = &monitor;
	threadParam.pRunnableParam = m_pRunnableParam;
	CWinThread *pThread = AfxBeginThread(RunnableThreadFuc, (LPVOID)&threadParam);

	// 拦截消息循环
	MSG msg;
	while (!threadParam.isThreadEnd) 
	{
    
		if (::PeekMessage(&msg, NULL,0,0,PM_NOREMOVE))
		{
			if (m_bCancelable)
			{
				if ((msg.message == WM_CHAR) && (msg.wParam == VK_ESCAPE))
				{
					monitor.SetCanceled(true);
				}

				if (msg.message == WM_LBUTTONUP)
				{
					CRect rect;
					m_btnCancel.GetWindowRect(rect);
					if (rect.PtInRect(msg.pt))
					{
						monitor.SetCanceled(true);
					}
				}
			}

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
	if (!IsIconic())
	{
		Layout();
	}
}


void CProgressDialog::Layout(void)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	static int s_Margin = 5;
	static int s_ProgressHeight = 25;
	static int s_Spaceing = 5;

	static int s_ButtonWidth = 80;
	static int s_ButtonHeight = 25;

	if (rcClient.Width() > (s_Margin * 2) && rcClient.Height() > (s_Margin * 2 + s_ProgressHeight))
	{
		CRect rcControl;
		rcControl.SetRect(rcClient.left + s_Margin, rcClient.top + s_Margin, 
				rcClient.right- s_Margin, 
				rcClient.top + (rcClient.Height() - (s_Margin * 2 + s_ProgressHeight) - (s_Spaceing * 2)) / 2);

		if (::IsWindow(m_lblTaskName.m_hWnd))
		{
			m_lblTaskName.MoveWindow(rcControl);
		}
	
		rcControl.MoveToY(rcControl.bottom + s_Spaceing);
		rcControl.bottom = rcControl.top + s_ProgressHeight;
		if (::IsWindow(m_wndProgress.m_hWnd))
		{
			m_wndProgress.MoveWindow(rcControl);
		}
	
		rcControl.MoveToY(rcControl.bottom + s_Spaceing);
		rcControl.bottom = rcClient.bottom - s_Margin;
		rcControl.right -= s_ButtonWidth + s_Spaceing;
		if (::IsWindow(m_lblSubTaskName.m_hWnd))
		{
			m_lblSubTaskName.MoveWindow(rcControl);
		}

		rcControl = CRect(CPoint(rcClient.right - s_Margin - s_ButtonWidth, rcClient.bottom - s_ButtonHeight - s_Margin), CSize(s_ButtonWidth, s_ButtonHeight));
		if (::IsWindow(m_btnCancel.m_hWnd))
		{
			m_btnCancel.MoveWindow(rcControl);
		}
	}

}


HBRUSH CProgressDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC: //对所有静态文本控件的设置
	{
		pDC->SetBkMode(TRANSPARENT); 
		//设置背景为透明
		
		return GetGlobalData()->brBtnFace;
	}
	default:
		break;
	}
	return hbr;
}


//void CProgressDialog::OnCancel()
//{
//	
//}
