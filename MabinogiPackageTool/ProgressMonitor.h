#pragma once
class CProgressMonitor
{
public:
	CProgressMonitor(void);
	virtual ~CProgressMonitor(void);

	virtual void BeginTask(LPCTSTR lpszName, int nTotalWork);
	virtual void Done(void);
	virtual bool IsCanceled(void);
	virtual void SetCanceled(bool value);
	virtual void SetTaskName(LPCTSTR lpszName);
	virtual void SubTask(LPCTSTR lpszName);
	virtual void Worked(int work);
};

