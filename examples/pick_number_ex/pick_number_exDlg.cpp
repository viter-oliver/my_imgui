
// pick_number_exDlg.cpp : implementation file
//

#include "stdafx.h"
#include <fstream>
#include "get_web_time.h"
#include "pick_number_ex.h"
#include "pick_number_exDlg.h"
#include "afxdialogex.h"
#include "DlgGetLicense.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


Cpick_number_exDlg::Cpick_number_exDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cpick_number_exDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cpick_number_exDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cpick_number_exDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Cpick_number_exDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Cpick_number_exDlg::OnBnClickedCancel)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &Cpick_number_exDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// Cpick_number_exDlg message handlers
//static SYSTEMTIME scur_time;
string reg_path = "pick_number_for_max";
string ikey = "max&maj20190815x";
unsigned char iv[] = { 103, 35, 148, 239, 76, 213, 47, 118, 255, 222, 123, 176, 106, 134, 98, 92 };
DWORD get_time_span(vector<BYTE> plainText)
{
	if (plainText.size()!=32)
	{
		return 0;
	}
	DWORD *prelease_time = (DWORD*)&plainText[0];
	DWORD *prelease_numb = (DWORD*)&plainText[4];
	DWORD *time_span = (DWORD*)&plainText[8];
	DWORD valid_time_point = *prelease_time + *time_span;
	return valid_time_point;
}
BOOL Cpick_number_exDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		// Set small icon
	//SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
	auto sec_consume = GetTimeFromServer(NULL);
	if (!sec_consume)
	{
		MessageBox("fail to connect to network!", "warning");
		exit(0);
	}
	//get license from reg or dlg 
	HKEY hKey;
#define  MAX_LC_LEN 0x100
	BYTE license[MAX_LC_LEN] = {0};
	DWORD lc_len=MAX_LC_LEN;
	//打开键
	auto lRet = RegOpenKeyEx(HKEY_CURRENT_USER, reg_path.c_str(), 0, KEY_READ, &hKey);
	bool be_get_license = false;
	AESModeOfOperation moo;
	moo.set_key((unsigned char*)ikey.c_str());
	moo.set_mode(MODE_CBC);
	moo.set_iv(iv);
	vector<unsigned char> plainText;
	if(lRet == ERROR_SUCCESS) 
	{

	    //读取键值
		if (RegQueryValueEx(hKey, "license", 0, 0, license, &lc_len) == ERROR_SUCCESS)
		{
			be_get_license = true;
			plainText.resize(lc_len);
			moo.Decrypt(license, lc_len, &plainText[0]);
			DWORD valid_time_point = get_time_span(plainText);
			if (valid_time_point < sec_consume)
			{
				be_get_license=false;
			}
		}
			    
	    //关闭键
	    RegCloseKey(hKey);
	}
	if (!be_get_license)
	{
		CDlgGetLicense req_license;
		auto retn = req_license.DoModal();
		if (retn == IDCANCEL)
		{
			exit(0);
		}
		else
		{
			printf("get it!\n");
			string slicense = req_license.m_cslicense.GetBuffer(0);
			if (slicense.size()!=64)
			{
				MessageBox("Application will exit for invalid license number!", "Warning");
				exit(0);
			}
			vector<unsigned char> hexLicense;
			//vector<unsigned char> plainText;
			char_to_hex(slicense, hexLicense);
			plainText.resize(hexLicense.size());
			
			moo.Decrypt(&hexLicense[0], hexLicense.size(), &plainText[0]);
			
			DWORD valid_time_point = get_time_span(plainText);
			if (valid_time_point < sec_consume)
			{
				MessageBox("The license number has expired, please apply for another license number ", "Warning");
				exit(0);
			}
			DWORD state;
			lRet = RegCreateKeyEx(HKEY_CURRENT_USER, reg_path.c_str(), 0, NULL, 0, KEY_WRITE, NULL, &hKey, &state);
			if (lRet == ERROR_SUCCESS)
			{
				RegSetValueEx(hKey, "license", 0, REG_BINARY, &hexLicense[0], 32);
				RegCloseKey(hKey);
			}
			//string plain_text=encDec()
		}
	}

	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cpick_number_exDlg::OnPaint()
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
HCURSOR Cpick_number_exDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool is_numb(char c)
{
	return c >= '0'&&c <= '9';
}

void Cpick_number_exDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString output_file;
	GetDlgItem(IDC_EDIT1)->GetWindowText(output_file);
	ifstream fin;
	fin.open(m_filepath);
	ofstream fout;
	fout.open(output_file.GetString(), ios::out | ios::trunc);
#define buff_len 0x100
	char cbuff[buff_len];
	while (fin.getline(cbuff, buff_len))
	{
		int clen = strlen(cbuff);
		for (int ix = 0; ix < clen;)
		{
			if (is_numb(cbuff[ix]))
			{
				int iy = ix + 1;
				int imx = ix + 30;
				int ib = ix + 10;
				for (; iy < imx; iy++)
				{
					if (!is_numb(cbuff[iy]))
					{
						ix = iy + 1;
						break;
					}
				}
				if (iy > ib)
				{
					cbuff[iy] = '\n';
					if (cbuff[iy - 11] == '1')
					{
						fout.write(&cbuff[iy - 11], 12);
					}
					ix = iy;
				}

			}
			else
			{
				ix++;
			}
		}
	}
	fin.close();
	fout.close();
	//CDialogEx::OnOK();
}


void Cpick_number_exDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void Cpick_number_exDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	TCHAR szFileName[MAX_PATH] = TEXT("");
	DragQueryFile(hDropInfo, 0, szFileName, sizeof(szFileName));
	GetDlgItem(IDC_LICENSE_NUMB)->SetWindowText(szFileName);
	m_filepath = szFileName;;
	string file_output = m_filepath + "_o";
	GetDlgItem(IDC_EDIT1)->SetWindowText(file_output.c_str());
	CDialogEx::OnDropFiles(hDropInfo);
}


void Cpick_number_exDlg::OnBnClickedButton1()//open new file
{
	// TODO: Add your control notification handler code here
	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = "file:\0*.*\0\0";
	char strFileName[MAX_PATH] = { 0 };
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = strFileName;
	ofn.nMaxFile = sizeof(strFileName);
	ofn.lpstrTitle = "Loading file...";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		GetDlgItem(IDC_LICENSE_NUMB)->SetWindowText(strFileName);
		m_filepath = strFileName;;
		string file_output = m_filepath + "_o";
		GetDlgItem(IDC_EDIT1)->SetWindowText(file_output.c_str());
		//add_file_to_mfiles_list(string(strFileName));
	}
}
