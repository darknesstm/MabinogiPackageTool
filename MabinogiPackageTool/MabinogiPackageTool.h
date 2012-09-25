
// MabinogiPackageTool.h : MabinogiPackageTool 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CMabinogiPackageToolApp:
// 有关此类的实现，请参阅 MabinogiPackageTool.cpp
//

class CMabinogiPackageToolApp : public CWinAppEx
{
public:
	CMabinogiPackageToolApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	CFont * GetDefaultFont();
	HBRUSH GetDefaultBackground();
	CString GetMyTempPath(void);
	CString GetMyTempFilePrefix(void);

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
private:
	CString m_strTempPath;
	CString m_strTempFilePrefix;
	
public:
	afx_msg void OnFileMakePackFile();
};

extern CMabinogiPackageToolApp theApp;
