
// mcu_simulatorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// Cmcu_simulatorDlg dialog
class Cmcu_simulatorDlg : public CDialogEx
{
// Construction
public:
	Cmcu_simulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MCU_SIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	// all protocol data will be loaded by this variable
	CTreeCtrl _treectl_protocol_data;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BYTE _values[32];
	afx_msg void OnSelchangedProtocolTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnInputClicked();
	afx_msg void OnBnClickedOk2();
	CButton _btn_exit;
	CButton _btn_start;
	CButton _btn_stop;
	afx_msg void OnItemchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton _by_priority;
	afx_msg void OnClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton _btn_search;
	CString _str_search;
	afx_msg void OnBnClickedButton2();
	virtual BOOL PreTranslateMessage(MSG* pMsg) ;
	afx_msg void OnBnClickedButton3();
	CString _comm;
	CString _str_baudrate;
	CComboBox _ctl_comm;
	CComboBox _ctl_baudrate;
	afx_msg void OnRadioRcf();
	afx_msg void OnRadioSerialPort();
	CButton _btn_connect;
	CString _str_from_host;
protected:
	afx_msg LRESULT OnWmsgVdataFromHost(WPARAM wParam, LPARAM lParam);
public:
	CEdit _edt_vdata_from_host;
};
