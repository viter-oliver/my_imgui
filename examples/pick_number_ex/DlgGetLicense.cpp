// DlgGetLicense.cpp : implementation file
//

#include "stdafx.h"
#include "pick_number_ex.h"
#include "DlgGetLicense.h"
#include "afxdialogex.h"


// CDlgGetLicense dialog

IMPLEMENT_DYNAMIC(CDlgGetLicense, CDialogEx)

CDlgGetLicense::CDlgGetLicense(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgGetLicense::IDD, pParent)
	, m_cslicense(_T(""))
{

}

CDlgGetLicense::~CDlgGetLicense()
{
}

void CDlgGetLicense::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LICENSE_NUMB, m_cslicense);
}


BEGIN_MESSAGE_MAP(CDlgGetLicense, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgGetLicense::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgGetLicense message handlers


void CDlgGetLicense::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
