#pragma once


// CDlgGetLicense dialog

class CDlgGetLicense : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGetLicense)

public:
	CDlgGetLicense(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGetLicense();

// Dialog Data
	enum { IDD = IDD_DLGGETLICENSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	CString m_cslicense;
};
