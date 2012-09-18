#pragma once

class CProgressMonitor;
class CRunnableWithProgress
{
public:
	CRunnableWithProgress(void);
	virtual ~CRunnableWithProgress(void);

	virtual int Run(CProgressMonitor *pMonitor, DWORD_PTR param) = 0;

};

