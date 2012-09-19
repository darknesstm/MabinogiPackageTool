#pragma once


// CProgressWnd
class CProgressMonitor
{
public:
	CProgressMonitor();
	virtual ~CProgressMonitor();

};

typedef int (*RunnableFunc)(CProgressMonitor *pMonitor, LPVOID pParam); 

class CProgressDialog : public CWnd
{
	DECLARE_DYNAMIC(CProgressDialog)

public:
	CProgressDialog(HWND hParentWnd);
	virtual ~CProgressDialog();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL Create(HWND hParentWnd, LPCTSTR pszTitle);
public:

	int DoModal(RunnableFunc fnRunnable, LPVOID pParam);

protected:
	HWND m_hParentWnd;

	CStatic m_lblTaskName;
	CProgressCtrl m_wndProgress;
	CStatic m_lblSubTaskName;

public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


