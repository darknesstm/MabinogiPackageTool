#pragma once


// CProgressWnd
class CProgressMonitor
{
public:
	CProgressMonitor() {};
	virtual ~CProgressMonitor() {};

	virtual void BeginTask(LPCTSTR lpszName, int totalWork) = 0;
	virtual void Done() = 0;
	virtual bool IsCanceled() = 0;
	virtual void SetCanceled(bool value) = 0;
	virtual void SetTaskName(LPCTSTR lpszName) = 0;
	virtual void SubTask(LPCTSTR lpszName) = 0;
	virtual void Worked(int lpszName) = 0;
};

typedef UINT (*RunnableFunc)(CProgressMonitor *pMonitor, LPVOID pParam); 

class CProgressDialog : public CWnd
{
	DECLARE_DYNAMIC(CProgressDialog)

public:
	CProgressDialog(HWND hParentWnd, RunnableFunc fnRunnable, LPVOID pRunnableParam);
	virtual ~CProgressDialog();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL Create(HWND hParentWnd, LPCTSTR pszTitle);
public:

	int DoModal();

protected:
	HWND m_hParentWnd;
	RunnableFunc m_fnRunnable;
	LPVOID m_pRunnableParam;

	CStatic m_lblTaskName;
	CProgressCtrl m_wndProgress;
	CStatic m_lblSubTaskName;

public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


