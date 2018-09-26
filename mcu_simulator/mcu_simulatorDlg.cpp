
// mcu_simulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mcu_simulator.h"
#include "mcu_simulatorDlg.h"
#include "afxdialogex.h"
#include "json.h"
#include "data_transfer_by_rcf.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <io.h>    
#include <fcntl.h>  
#define		MCU_SYNC1			0xFA
#define		MCU_SYNC2			0xAF
enum 
{
	en_pos_head0,
	en_pos_head1,
	en_pos_len,
	en_pos_index,
	en_pos_ack,
	en_pos_cmd_head,
};
void InitConsoleWindow()
{
	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE * hf = _fdopen(hCrt, "w");
	*stdout = *hf;
}

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>

#define CMD_VALUE_LEN 50
using namespace std;
typedef unsigned char ubyte;
typedef map<wstring, vector<ubyte>> map_name_id;
//typedef map<wstring, mutex*> map_name_mutex;

static CImageList _tree_img_list;
static map_name_id _map_name_id;
static map_name_id _map_name_value,_map_name_value_snd;
//static map_name_mutex _map_name_mutex;
vector<wstring> _blured_itm_name_list;
mutex _value_blured_update;
static bool _is_running;
static bool _size_is_blured = false;
vector<ubyte> _vlink_layer(CMD_VALUE_LEN, 0);
static data_transfer_by_rcf _transfer_by_rcf;

//comm
static bool _comm_connected = false;
static HANDLE _hcomm=NULL;

#define WMSG_VDATA_FROM_HOST (WM_USER+100)

ubyte calc_check_sum(ubyte * pBuf, int iLen)
{
	int i = 0;
	ubyte check_sum = 0;

	check_sum = pBuf[0];
	for (i = 1; i < iLen; i++)
	{
		check_sum ^= pBuf[i];
	}
	return check_sum;
}

Cmcu_simulatorDlg::Cmcu_simulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmcu_simulatorDlg::IDD, pParent)
	, _str_search(_T(""))
	, _comm(_T(""))
	, _str_baudrate(_T(""))
	, _str_from_host(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmcu_simulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, _treectl_protocol_data);
	DDX_Text(pDX, IDC_EDIT1, _values[0]);
	DDX_Text(pDX, IDC_EDIT2, _values[1]);
	DDX_Text(pDX, IDC_EDIT3, _values[2]);
	DDX_Text(pDX, IDC_EDIT4, _values[3]);
	DDX_Text(pDX, IDC_EDIT5, _values[4]);
	DDX_Text(pDX, IDC_EDIT6, _values[5]);
	DDX_Text(pDX, IDC_EDIT7, _values[6]);
	DDX_Text(pDX, IDC_EDIT8, _values[7]);
	DDX_Text(pDX, IDC_EDIT9, _values[8]);
	DDX_Text(pDX, IDC_EDIT10, _values[9]);
	DDX_Text(pDX, IDC_EDIT11, _values[10]);
	DDX_Text(pDX, IDC_EDIT12, _values[11]);
	DDX_Text(pDX, IDC_EDIT13, _values[12]);
	DDX_Text(pDX, IDC_EDIT14, _values[13]);
	DDX_Text(pDX, IDC_EDIT15, _values[14]);
	DDX_Text(pDX, IDC_EDIT16, _values[15]);
	DDX_Text(pDX, IDC_EDIT17, _values[16]);
	DDX_Text(pDX, IDC_EDIT18, _values[17]);
	DDX_Text(pDX, IDC_EDIT19, _values[18]);
	DDX_Text(pDX, IDC_EDIT20, _values[19]);
	DDX_Text(pDX, IDC_EDIT21, _values[20]);
	DDX_Text(pDX, IDC_EDIT22, _values[21]);
	DDX_Text(pDX, IDC_EDIT23, _values[22]);
	DDX_Text(pDX, IDC_EDIT24, _values[23]);
	DDX_Text(pDX, IDC_EDIT25, _values[24]);
	DDX_Text(pDX, IDC_EDIT26, _values[25]);
	DDX_Text(pDX, IDC_EDIT27, _values[26]);
	DDX_Text(pDX, IDC_EDIT28, _values[27]);
	DDX_Text(pDX, IDC_EDIT29, _values[28]);
	DDX_Text(pDX, IDC_EDIT30, _values[29]);
	DDX_Text(pDX, IDC_EDIT31, _values[30]);
	DDX_Text(pDX, IDC_EDIT32, _values[31]);

	DDX_Control(pDX, IDOK2, _btn_exit);
	DDX_Control(pDX, IDOK, _btn_start);
	DDX_Control(pDX, IDCANCEL, _btn_stop);
	DDX_Control(pDX, IDC_RADIO2, _by_priority);
	DDX_Control(pDX, IDC_BUTTON2, _btn_search);
	DDX_Text(pDX, IDC_EDIT33, _str_search);
	DDX_CBString(pDX, IDC_COMBO2, _comm);
	DDX_CBString(pDX, IDC_COMBO1, _str_baudrate);
	DDX_Control(pDX, IDC_COMBO2, _ctl_comm);
	DDX_Control(pDX, IDC_COMBO1, _ctl_baudrate);
	DDX_Control(pDX, IDC_BUTTON3, _btn_connect);
	DDX_Text(pDX, IDC_EDIT34, _str_from_host);
	DDX_Control(pDX, IDC_EDIT34, _edt_vdata_from_host);
}

BEGIN_MESSAGE_MAP(Cmcu_simulatorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Cmcu_simulatorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Cmcu_simulatorDlg::OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &Cmcu_simulatorDlg::OnSelchangedProtocolTree)
	ON_BN_CLICKED(IDC_BUTTON1, &Cmcu_simulatorDlg::OnBnInputClicked)
	ON_BN_CLICKED(IDOK2, &Cmcu_simulatorDlg::OnBnClickedOk2)
	ON_NOTIFY(TVN_ITEMCHANGED, IDC_TREE1, &Cmcu_simulatorDlg::OnItemchangedTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &Cmcu_simulatorDlg::OnClickTree1)
	ON_BN_CLICKED(IDC_BUTTON2, &Cmcu_simulatorDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &Cmcu_simulatorDlg::OnBnClickedButton3)
	ON_COMMAND(IDC_RADIO2, &Cmcu_simulatorDlg::OnRadioRcf)
	ON_COMMAND(IDC_RADIO1, &Cmcu_simulatorDlg::OnRadioSerialPort)
	ON_MESSAGE(WMSG_VDATA_FROM_HOST, &Cmcu_simulatorDlg::OnWmsgVdataFromHost)
END_MESSAGE_MAP()
// Cmcu_simulatorDlg message handlers

BOOL Cmcu_simulatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_vlink_layer[en_pos_head0] = MCU_SYNC1;
	_vlink_layer[en_pos_head1] = MCU_SYNC2;
	InitConsoleWindow();
	_by_priority.SetCheck(1);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	_btn_stop.EnableWindow(FALSE);
	auto hicon = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE (IDI_ICON1));
	_btn_search.SetIcon(hicon);

	_tree_img_list.Create(16, 16, 0, 7, 7);
	_tree_img_list.SetBkColor(RGB(255, 255, 255));
	auto hicon_nm = LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON2));
	auto hicon_sl= LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDI_ICON3));
	_tree_img_list.Add(hicon_nm);
	_tree_img_list.Add(hicon_sl);
	_treectl_protocol_data.SetImageList(&_tree_img_list, TVSIL_NORMAL);
	// TODO: Add extra initialization here
	_ctl_comm.SetCurSel(2);
	_ctl_baudrate.SetCurSel(7);
	_ctl_comm.EnableWindow(FALSE);
	_ctl_baudrate.EnableWindow(FALSE);
	_btn_connect.EnableWindow(FALSE);
	::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_CLOSE,
		MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	_transfer_by_rcf.set_vadta_handle([this](vector<unsigned char>& vdata){
		_str_from_host.Empty();
		for (auto udata:vdata)
		{
			wchar_t str_data[20] = { 0 };
			wsprintf(str_data, L"%x ", udata);
			_str_from_host += str_data;
		}
		//UpdateData(FALSE);
		PostMessage(WMSG_VDATA_FROM_HOST);
	});
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cmcu_simulatorDlg::OnPaint()
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
HCURSOR Cmcu_simulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cmcu_simulatorDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
	if (_by_priority.GetCheck())
	{
		_btn_stop.EnableWindow(TRUE);
		_btn_start.EnableWindow(FALSE);
		printf("rcf is_checkd\n");
		thread td_rcf([this]{
			_is_running = true;
		    //_treectl_protocol_data.EnableWindow(FALSE);

			while (_is_running)
			{
				if (_size_is_blured)
				{
					_map_name_value_snd.clear();
					_treectl_protocol_data.EnableWindow(FALSE);
					_map_name_value_snd = _map_name_value;
					_treectl_protocol_data.EnableWindow(TRUE);
					_size_is_blured = false;
				}
				if (_blured_itm_name_list.size()>0)
				{
					_value_blured_update.lock();
					for (auto istr : _blured_itm_name_list)
					{
						auto isnd = _map_name_value_snd.find(istr);
						if (isnd != _map_name_value_snd.end())
						{
							auto irvalue = _map_name_value.find(istr);
							if (irvalue != _map_name_value.end())
							{
								memcpy(&*isnd->second.begin(), &*irvalue->second.begin(), irvalue->second.size());
							}
						}
					}
					_blured_itm_name_list.clear();
					_value_blured_update.unlock();
				}

				for (auto itvalue : _map_name_value_snd)
				{
					auto itcmd = _map_name_id.find(itvalue.first);
					if (itcmd!=_map_name_id.end())
					{
						//if (_map_name_mutex[itvalue.first]->try_lock())
						{
							auto cmd_len = itcmd->second.size();
							auto value_len = itvalue.second.size();
							memcpy(&*_vlink_layer.begin(), &*itcmd->second.begin(), cmd_len);//附件cmd
							memcpy(&*_vlink_layer.begin() + cmd_len, &*itvalue.second.begin(), value_len);

							bool be_success = _transfer_by_rcf.send_data_to_host(_vlink_layer);
							
							//usleep(100000);
							Sleep(100);
							//_map_name_mutex[itvalue.first]->unlock();
							if (!be_success)
							{
								_is_running = false;
								GetDlgItem(IDC_STATIC7)->SetWindowText(L"remote call error!");
								_btn_stop.EnableWindow(FALSE);
								_btn_start.EnableWindow(TRUE);
								break;
							}
						}
					}
				}
			}
			//_treectl_protocol_data.EnableWindow(TRUE);
		});
		td_rcf.detach();
	}
	else
	{
		if (!_comm_connected)
		{
			GetDlgItem(IDC_STATIC7)->SetWindowText(L"connect comm firstly,please!");
			return;
		}
		_btn_stop.EnableWindow(TRUE);
		_btn_start.EnableWindow(FALSE);
		thread td_serial_bus([this]{
			_is_running = true;
			_vlink_layer[en_pos_head0] = MCU_SYNC1;
			_vlink_layer[en_pos_head1] = MCU_SYNC2;
			while (_is_running)
			{
				if (_size_is_blured)
				{
					_map_name_value_snd.clear();
					_treectl_protocol_data.EnableWindow(FALSE);
					_map_name_value_snd = _map_name_value;
					_treectl_protocol_data.EnableWindow(TRUE);
					_size_is_blured = false;
				}
				if (_blured_itm_name_list.size() > 0)
				{
					_value_blured_update.lock();
					for (auto istr : _blured_itm_name_list)
					{
						auto isnd = _map_name_value_snd.find(istr);
						if (isnd != _map_name_value_snd.end())
						{
							auto irvalue = _map_name_value.find(istr);
							if (irvalue != _map_name_value.end())
							{
								memcpy(&*isnd->second.begin(), &*irvalue->second.begin(), irvalue->second.size());
							}
						}
					}
					_value_blured_update.unlock();
				}
				for (auto itvalue : _map_name_value_snd)
				{
					auto itcmd = _map_name_id.find(itvalue.first);
					if (itcmd != _map_name_id.end())
					{
						static ubyte frame_idex = 0;
						auto cmd_len = itcmd->second.size();
						auto value_len = itvalue.second.size();
						auto frame_len = cmd_len + value_len + 3;
						_vlink_layer[en_pos_len] = frame_len;
						_vlink_layer[en_pos_index] = frame_idex++;
						memcpy(&*_vlink_layer.begin() + en_pos_cmd_head, &*itcmd->second.begin(), cmd_len);//附件cmd
						memcpy(&*_vlink_layer.begin() + en_pos_cmd_head + cmd_len, &*itvalue.second.begin(), value_len);
						auto chksm = calc_check_sum(&*_vlink_layer.begin() + en_pos_len, frame_len);
						_vlink_layer[en_pos_len + frame_len] = chksm;
						//_transfer_by_rcf.send_data_to_host(_vlink_layer);//串口发送链路层数据
						DWORD dwBytesWritten = _vlink_layer.size();
						DWORD dwErrorFlags;
						COMSTAT ComStat;
						OVERLAPPED m_osWrite = { 0 };
						BOOL bWriteStat;
						bWriteStat = WriteFile(_hcomm, &*_vlink_layer.begin(), dwBytesWritten, &dwBytesWritten, &m_osWrite);
						if (!bWriteStat)
						{
							DWORD errcode = GetLastError();
							//wprintf(L"error code:%d\n", errcode);
							if (errcode == ERROR_IO_PENDING)
							{
								WaitForSingleObject(m_osWrite.hEvent, 1000);
							}
						}
						Sleep(100);
					}
				}

			}
		});
		td_serial_bus.detach();
	}

}


void Cmcu_simulatorDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnCancel();
	_is_running = false;
	_btn_stop.EnableWindow(FALSE);
	_btn_start.EnableWindow(TRUE);

}


void Cmcu_simulatorDlg::OnSelchangedProtocolTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	//printf("%d:%s\n", __LINE__, __FUNCTION__);
	*pResult = 0;
}

#include <fstream>
using namespace Json;
using namespace std;
void insert_node_to_treeview(CTreeCtrl& tc, Value& jnt, HTREEITEM hparent,vector<ubyte>* hparent_msg_id=NULL)
{
	Value& name = jnt["name"];
	wchar_t ws[100];
	swprintf(ws,100, L"%hs", name.asCString());
	auto hcurnode = tc.InsertItem(ws, hparent, TVI_LAST);
	tc.SetItemImage(hcurnode, 0, 1);
	vector<ubyte> msg_id;
	if (hparent_msg_id)
	{
		msg_id.assign(hparent_msg_id->begin(), hparent_msg_id->end());
	}
	msg_id.push_back(jnt["id"].asInt());
	_map_name_id[ws] = msg_id;
	tc.Expand(hparent, TVE_EXPAND);
	Value& child = jnt["child"];
	if (!child.isNull())
	{
		int isz = child.size();
		for (auto ix = 0; ix < isz; ix++)
		{
			Value& chd = child[ix];
			insert_node_to_treeview(tc, chd, hcurnode,&msg_id);
		}
	}
}
void Cmcu_simulatorDlg::OnBnInputClicked()
{
	// TODO: Add your control notification handler code here
	CFileDialog cdg(TRUE,NULL,NULL, OFN_HIDEREADONLY, L"(*.json)||");
	if (cdg.DoModal() == IDOK)
	{
		CString protocol_path = cdg.GetPathName();
		GetDlgItem(IDC_STATIC4)->SetWindowText(protocol_path);
		ifstream ifs;
		ifs.open(protocol_path.GetString());
		if (ifs.is_open())
		{
			Value root;
			Reader reader;
			if (reader.parse(ifs,root,false))
			{
				Value& vchild = root["child"];
				int isz = vchild.size();
				for (int id = 0; id < isz;id++)
				{
					Value& ichild = vchild[id];
					insert_node_to_treeview(_treectl_protocol_data, ichild, TVI_ROOT);
				}
			}
		}	
	}
}

HTREEITEM  finditem(CTreeCtrl& tc, HTREEITEM  item, CString strtext)
{
	HTREEITEM  hfind;
	//空树，直接返回NULL
	if (item == NULL)
		return  NULL;
	//遍历查找
	while (item != NULL)
	{
		//当前节点即所需查找节点
		if (tc.GetItemText(item) == strtext)
			return   item;
		//查找当前节点的子节点

		if (tc.ItemHasChildren(item))
		{
			item = tc.GetChildItem(item);
			//递归调用查找子节点下节点
			hfind = finditem(tc,item, strtext);
			if (hfind)
			{
				return  hfind;
			}
			else   //子节点中未发现所需节点，继续查找兄弟节点
				item = tc.GetNextSiblingItem(tc.GetParentItem(item));
		}
		else{   //若无子节点，继续查找兄弟节点
			item = tc.GetNextSiblingItem(item);
		}
	}
	return item;
}

void Cmcu_simulatorDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	GetDlgItem(IDC_STATIC7)->SetWindowText(_T(""));

	if (_str_search.GetLength() > 0)
	{
		wprintf(L"input:%s\n", _str_search.GetBuffer());
		auto itm_tar = finditem(_treectl_protocol_data, _treectl_protocol_data.GetRootItem(), _str_search);
		if (itm_tar!=NULL)
		{
			_treectl_protocol_data.SelectItem(itm_tar);
			_treectl_protocol_data.SetFocus();
		}
		else
		{
			CString strShow = _T("fail to find ");
			strShow += _str_search;
			GetDlgItem(IDC_STATIC7)->SetWindowText(strShow);
		}
		_str_search.Empty();
		UpdateData(FALSE);
	}
}

void Cmcu_simulatorDlg::OnBnClickedOk2()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

void Cmcu_simulatorDlg::OnItemchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
	// TODO: Add your control notification handler code here
	ubyte new_state = pNMTVItemChange->uStateNew;
	ubyte old_state = pNMTVItemChange->uStateOld;
	new_state &= 0x0f;
	old_state &= 0x0f;
	auto cur_item = pNMTVItemChange->hItem;
	auto itemtext = _treectl_protocol_data.GetItemText(cur_item);
	wprintf(L"new state:0x%x,old state:0x%x itemtext:%s\n", new_state, old_state, itemtext.GetBuffer());
	auto iif = _map_name_id.find(itemtext.GetBuffer());
	if (iif != _map_name_id.end())
	{
		BOOL bchk = _treectl_protocol_data.GetCheck(cur_item);
		auto iiif = _map_name_value.find(itemtext.GetBuffer());
		if (new_state == 0x2 && old_state == 0 && bchk)//got focus
		{
			if (iiif!=_map_name_value.end())
			{
				GetDlgItem(IDC_STATIC20)->SetWindowText(itemtext);
				memcpy(_values, &*iiif->second.begin(), sizeof(_values));
				UpdateData(FALSE);
			}
		}
		else
		if (new_state == 0x00&&old_state==0x2 && bchk)//leave focus
		{
			if (iiif != _map_name_value.end())
			{
				UpdateData(TRUE);
				memcpy(&*iiif->second.begin(), _values, sizeof(_values));
				
			}
		}
		else
		if (new_state ==old_state)
		{
			_size_is_blured = true;
			if (bchk)
			{
				vector<ubyte> vvalue(32, 0);
				memset(_values, 0, 32);
				UpdateData(FALSE);//data to control to show
				//GetDlgItem(IDC_STATIC20)->SetWindowText(itemtext);
				_map_name_value[itemtext.GetBuffer()] = vvalue;
			    //_map_name_mutex.insert(make_pair( itemtext.GetBuffer(), new mutex()));
			}
			else
			{
				//GetDlgItem(IDC_STATIC20)->SetWindowText(_T(""));
				_map_name_value.erase(iiif);
				/*auto imt = _map_name_mutex.find(itemtext.GetBuffer());
				if (imt!=_map_name_mutex.end())
				{
					delete imt->second;
					_map_name_mutex.erase(imt);
				}*/
			}
		}
	}
	
	//MessageBox(itemText);
	*pResult = 0;
}


void Cmcu_simulatorDlg::OnClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CPoint opt;
	UINT nFlag;
	GetCursorPos(&opt);
	_treectl_protocol_data.ScreenToClient(&opt);
	auto slitem = _treectl_protocol_data.HitTest(opt);
	if (slitem!=NULL)
	{
		//_treectl_protocol_data.SelectDropTarget(slitem);
	}
	*pResult = 0;
}



BOOL Cmcu_simulatorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN==pMsg->message&&VK_RETURN==pMsg->wParam)
	{
		auto hfocus = GetFocus();
		if (hfocus == GetDlgItem(IDC_EDIT33))//search
		{
			OnBnClickedButton2();
		}
		else
		{
			auto ctl_id = hfocus->GetDlgCtrlID();
			
			if (ctl_id>=IDC_EDIT1&&ctl_id<=IDC_EDIT32)
			{
				auto isel = _treectl_protocol_data.GetSelectedItem();
				if (isel!=NULL&&_treectl_protocol_data.GetCheck(isel))
				{
					UpdateData(TRUE);
					auto txtnm=_treectl_protocol_data.GetItemText(isel);
					if (_is_running)
					{
						wstring strTxtNum = txtnm.GetBuffer();
						auto iburedname = find_if(_blured_itm_name_list.begin(), _blured_itm_name_list.end(), 
							[strTxtNum](wstring& item)->bool{
							return item==strTxtNum;
						});
						if (iburedname==_blured_itm_name_list.end())
						{
							_value_blured_update.lock();
							_blured_itm_name_list.push_back(txtnm.GetBuffer());
							_value_blured_update.unlock();
						}
					}
					//auto imx = _map_name_mutex.find(txtnm.GetBuffer());
					//if (imx!=_map_name_mutex.end())
					{
						//imx->second->lock();
						auto ivalue = _map_name_value.find(txtnm.GetBuffer());
						memcpy(&*ivalue->second.begin(), _values, sizeof(_values));
						//imx->second->unlock();
					}
					printf("valid input\n");
					auto idx = ctl_id - IDC_EDIT1;
					printf("value%d is inputted\n", idx);

				}
				else
				{
					printf("invalid\n");
				}
				
			}
		}
	}
	return  0;// CDialogEx::PreTranslateMessage(pMsg);
}


void Cmcu_simulatorDlg::OnBnClickedButton3()//connect
{
	// TODO: Add your control notification handler code here
	if (_comm_connected)
	{
		CloseHandle(_hcomm);
		_hcomm = INVALID_HANDLE_VALUE;
		_comm_connected = false;
		_btn_connect.SetWindowText(_T("connect"));
	}
	else
	{
		UpdateData();
		_hcomm = CreateFile(_comm, GENERIC_READ | GENERIC_WRITE, 0, NULL, \
			OPEN_EXISTING, /*FILE_ATTRIBUTE_NORMAL |*/ FILE_FLAG_OVERLAPPED, NULL);
		if (_hcomm==INVALID_HANDLE_VALUE)
		{
			MessageBox(L"fail to open com!");
			return;
		}
		_btn_connect.SetWindowText(_T("disconnect"));
		_comm_connected = true;
		DCB commParam;
		if (!GetCommState(_hcomm, &commParam))
		{
			return;
		}

		commParam.BaudRate = _wtoi(_str_baudrate.GetBuffer());				// 设置波特率 
		commParam.fBinary = TRUE;					// 设置二进制模式，此处必须设置TRUE
		commParam.fParity = TRUE;					// 支持奇偶校验 
		commParam.ByteSize = 8;						// 数据位,范围:4-8 
		commParam.Parity = NOPARITY;				// 校验模式
		commParam.StopBits = ONESTOPBIT;			// 停止位 

		commParam.fOutxCtsFlow = FALSE;				// No CTS output flow control 
		commParam.fOutxDsrFlow = FALSE;				// No DSR output flow control 
		commParam.fDtrControl = DTR_CONTROL_ENABLE;
		// DTR flow control type 
		commParam.fDsrSensitivity = FALSE;			// DSR sensitivity 
		commParam.fTXContinueOnXoff = TRUE;			// XOFF continues Tx 
		commParam.fOutX = FALSE;					// No XON/XOFF out flow control 
		commParam.fInX = FALSE;						// No XON/XOFF in flow control 
		commParam.fErrorChar = FALSE;				// Disable error replacement 
		commParam.fNull = FALSE;					// Disable null stripping 
		commParam.fRtsControl = RTS_CONTROL_ENABLE;
		// RTS flow control 
		commParam.fAbortOnError = FALSE;			// 当串口发生错误，并不终止串口读写

		if (!SetCommState(_hcomm, &commParam))
		{
			wprintf(L"GetCommState failed\n");
			return ;
		}

		//设置串口读写时间
		COMMTIMEOUTS CommTimeOuts;
		GetCommTimeouts(_hcomm, &CommTimeOuts);
		CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
		CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
		CommTimeOuts.ReadTotalTimeoutConstant = 0;
		CommTimeOuts.WriteTotalTimeoutMultiplier = 50;
		CommTimeOuts.WriteTotalTimeoutConstant = 1000;

		if (!SetCommTimeouts(_hcomm, &CommTimeOuts))
		{
			return ;
		}

		//指定端口监测的事件集
		SetCommMask(_hcomm, EV_RXCHAR);

		//分配设备缓冲区
		::SetupComm(_hcomm, 4096, 4096);

		//初始化缓冲区中的信息
		::PurgeComm(_hcomm, PURGE_TXCLEAR | PURGE_RXCLEAR);
	}

}


void Cmcu_simulatorDlg::OnRadioRcf()
{
	// TODO: Add your command handler code here
	_ctl_comm.EnableWindow(FALSE);
	_ctl_baudrate.EnableWindow(FALSE);
	_btn_connect.EnableWindow(FALSE);
}


void Cmcu_simulatorDlg::OnRadioSerialPort()
{
	// TODO: Add your command handler code here
	_ctl_comm.EnableWindow(TRUE);
	_ctl_baudrate.EnableWindow(TRUE);
	_btn_connect.EnableWindow(TRUE);
}


afx_msg LRESULT Cmcu_simulatorDlg::OnWmsgVdataFromHost(WPARAM wParam, LPARAM lParam)
{
	//UpdateData(FALSE);
	_edt_vdata_from_host.SetWindowText(_str_from_host);
	return 0;
}
