#pragma once


// CPreviewPane

class CPreviewPane : public CDockablePane
{
	DECLARE_DYNAMIC(CPreviewPane)

public:
	CPreviewPane();
	virtual ~CPreviewPane();

	void UpdateFonts();

protected:
	DECLARE_MESSAGE_MAP()

	CEdit m_txtControl;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void SetTextContent(LPCTSTR content);
};


