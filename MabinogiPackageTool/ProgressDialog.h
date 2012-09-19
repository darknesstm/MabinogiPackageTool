#pragma once


// CProgressWnd
class CProgressMonitor
{
public:
	CProgressMonitor();
	virtual ~CProgressMonitor();

};

class CRunnable
{
public:
	CRunnable();
	virtual ~CRunnable();

	virtual int Run(CProgressMonitor *Monitor) = 0;

};



class CProgressDialog : public CWnd
{
	DECLARE_DYNAMIC(CProgressDialog)

public:
	CProgressDialog(HWND hParentWnd, CRunnable *runnable);
	virtual ~CProgressDialog();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL Create(HWND hParentWnd, LPCTSTR pszTitle);
public:
	/**
	 * 
	 * @param block ÊÇ·ñÊÇ¶ÂÈû²Ù×÷
	 */
	int DoModal();

protected:
	HWND m_hParentWnd;
	CRunnable *m_pRunnable;

	CStatic m_lblTaskName;
	CProgressCtrl m_wndProgress;
	CStatic m_lblSubTaskName;

public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


