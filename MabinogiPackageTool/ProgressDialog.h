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
	CProgressDialog(CWnd *pParentWnd, CRunnable *runnable);
	virtual ~CProgressDialog();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL Create(CWnd* pParent, LPCTSTR pszTitle);
public:
	int DoModal(void);
protected:
	CRunnable *m_pRunnable;
	CProgressCtrl m_progress;
public:
	afx_msg void OnClose();
};


