
// license_generatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "license_generator.h"
#include "license_generatorDlg.h"
#include "afxdialogex.h"
#include "aes.h"
#include "get_web_time.h"
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Clicense_generatorDlg dialog

string ikey = "max&maj20190815x";
unsigned char iv[] = { 103, 35, 148, 239, 76, 213, 47, 118, 255, 222, 123, 176, 106, 134, 98, 92 };
string cur_dir;
string getAppCurpath(void)
{
	string curPath;
	TCHAR filename[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, filename, MAX_PATH);
	curPath = filename;
	string::size_type idx = curPath.rfind(L'\\');
	curPath.erase(idx + 1);
	return curPath;
}

Clicense_generatorDlg::Clicense_generatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Clicense_generatorDlg::IDD, pParent)
	, m_valid_days(180)
	, m_counts(100)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Clicense_generatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VALID_DAY, m_valid_days);
	DDX_Text(pDX, IDC_COUNT, m_counts);
}

BEGIN_MESSAGE_MAP(Clicense_generatorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Clicense_generatorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Clicense_generatorDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// Clicense_generatorDlg message handlers
BOOL Clicense_generatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	cur_dir = getAppCurpath();
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Clicense_generatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Clicense_generatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Clicense_generatorDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
	UpdateData(TRUE);
	DWORD cur_sec_consume = GetTimeFromServer(NULL);
	if (!cur_sec_consume)
	{
		MessageBox("fail to connect to network!", "warning");
		return;
	}
	const int time_scale=24 * 60 * 60;
	DWORD valid_sec = m_valid_days * time_scale;
	AESModeOfOperation moo;
	moo.set_key((unsigned char*)ikey.c_str());
	moo.set_mode(MODE_CBC);
	moo.set_iv(iv);
	vector<string> license_out;
	license_out.resize(m_counts);
	vector<BYTE> ptext;
	ptext.resize(32);
	memcpy(&ptext[0], &cur_sec_consume, 4);
	memcpy(&ptext[8], &valid_sec, 4);
	vector<BYTE> ctext;
	ctext.resize(32);
	for (DWORD ix = 0; ix < m_counts;ix++)
	{
		memcpy(&ptext[4], &ix, 4);
		moo.Encrypt(&ptext[0], 32, &ctext[0]);
		hex_to_char(ctext, license_out[ix]);
	}
	char output_file[0x100];
	sprintf_s(output_file, "licenselist_%u.txt", cur_sec_consume);
	string output_file_path = cur_dir + output_file;
	ofstream fout;
	fout.open(output_file_path, ios::out | ios::trunc);
	char rtn = '\n';
	for (auto& lic:license_out)
	{
		fout.write(&lic[0], lic.size());
		fout.write(&rtn, 1);
	}
	fout.close();
	
	GetDlgItem(IDC_STATE)->SetWindowText(output_file);
}


void Clicense_generatorDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
