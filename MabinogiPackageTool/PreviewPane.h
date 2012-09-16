#pragma once


// CPreviewPane

class CPreviewPane : public CDockablePane
{
	DECLARE_DYNAMIC(CPreviewPane)

public:
	CPreviewPane();
	virtual ~CPreviewPane();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


