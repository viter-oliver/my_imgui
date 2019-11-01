
// license_generatorDlg.h : header file
//

#pragma once


// Clicense_generatorDlg dialog
class Clicense_generatorDlg : public CDialogEx
{
// Construction
public:
	Clicense_generatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LICENSE_GENERATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	DWORD m_valid_days;
	DWORD m_counts;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

};
