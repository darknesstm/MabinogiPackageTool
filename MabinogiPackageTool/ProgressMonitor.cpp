#include "stdafx.h"
#include "ProgressMonitor.h"


CProgressMonitor::CProgressMonitor(void)
{
}


CProgressMonitor::~CProgressMonitor(void)
{
}


void CProgressMonitor::BeginTask(LPCTSTR lpszName, int nTotalWork)
{
}


void CProgressMonitor::Done(void)
{
}


bool CProgressMonitor::IsCanceled(void)
{
	return false;
}


void CProgressMonitor::SetCanceled(bool value)
{
}


void CProgressMonitor::SetTaskName(LPCTSTR lpszName)
{
}


void CProgressMonitor::SubTask(LPCTSTR lpszName)
{
}


void CProgressMonitor::Worked(int work)
{
}
