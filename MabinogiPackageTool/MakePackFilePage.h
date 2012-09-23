#pragma once
#include "afxcmn.h"


// CMakePackFilePage 对话框

class CMakePackFilePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMakePackFilePage)

public:
	CMakePackFilePage();
	virtual ~CMakePackFilePage();

// 对话框数据
	enum { IDD = IDD_MAKEPACKFILEPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
public:
	CString m_strInputFolder;
	CString m_strOutputFile;
	unsigned long m_ulVersion;
//	CSpinButtonCtrl m_spinVersion;
public:
	afx_msg void OnClickedBrowserInput();
	afx_msg void OnClickedBrowserOutput();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnInitDialog();
};
