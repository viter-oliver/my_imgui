/*
* ��飺����Boost����http��Post/Get����
* ���ߣ���³��
* ���䣺727057301@qq.com
* CSDN��http://blog.csdn.net/csnd_ayo
* ���ƣ�https://git.oschina.net/Mr_ChenLuYong
* github��http://github.com/chenluyong
* ����ʱ�䣺2017��3��12�� 04:32:07
* VS�汾��VS2013
* Boost�汾��boost_1_61_0
*/
 
#include "HttpBase.h"
#include "http_mark.h"
 
#define HTTP_JSON_BEGIN ("[")
#define HTTP_JSON_END ("]")
namespace cly
{

	HttpBase::HttpBase() {
	}


	HttpBase::~HttpBase() {
	}

	//////////////////////////////////////////////////////////////////////////
	//���Խ��������������͵�URL:
	//http://yunhq.sse.com.cn:32041/v1/sh1/snap/204001?callback=jQuery_test&select=name%2Clast%2Cchg_rate%2Cchange%2Camount%2Cvolume%2Copen%2Cprev_close%2Cask%2Cbid%2Chigh%2Clow%2Ctradephase
	//http://hq.sinajs.cn/list=sh204001
	//https://www.baidu.com
	//////////////////////////////////////////////////////////////////////////
	int HttpBase::parseUrl(const std::string& url, std::string& out_server,
		std::string& out_port, std::string& out_path) {
		const std::string& http___ = "http://";
		const std::string& https___ = "https://";
   		std::string temp_data = url;

		// �ض�httpЭ��ͷ
		if (temp_data.find(http___) == 0) {
			temp_data = temp_data.substr(http___.length());
		}
		else if (temp_data.find(https___) == 0) {
			temp_data = temp_data.substr(https___.length());
		}
		else {
			return HTTP_ERROR_HTTP_HEAD;
		}

		// ��������
		std::size_t idx = temp_data.find('/');
		// ���� �������page
		if (std::string::npos == idx) {
			out_path = "/";
			idx = temp_data.size();
		}
		else {
			out_path = temp_data.substr(idx);
		}

		// ��������
		out_server = temp_data.substr(0, idx);

		// �����˿�
		idx = out_server.find(':');
		if (std::string::npos == idx) {
			out_port = "80";
		}
		else {
			out_port = out_server.substr(idx + 1);
			out_server = out_server.substr(0, idx);
		}

		return HTTP_SUCCESS;
	}



	/*
	* ����Post����
	* buildPostRequest
	*/
	int HttpBase::buildPostRequest(const std::string& server, const std::string& path,
		std::ostream& out_request) {
		// �ָ�path�е�json����
		std::string temp_path(path), temp_json;
		int json_pos_begin = temp_path.find(HTTP_JSON_BEGIN) + 1;
		int json_pos_end = temp_path.find(HTTP_JSON_END);
		if (json_pos_begin != std::string::npos) {
			// ����json�ĳ���
			int temp_json_lenth = std::string::npos;
			if (json_pos_end != temp_json_lenth) {
				temp_json_lenth = (json_pos_end - json_pos_begin);
			}
			temp_json = temp_path.substr(json_pos_begin, temp_json_lenth);
			temp_path = temp_path.substr(0, (json_pos_begin - 1));
		}


		out_request << "POST " << temp_path.c_str() << " HTTP/1.0\r\n";
		out_request << "Host: " << server.c_str() << "\r\n";
		out_request << "Content-Length: " << temp_json.length() << "\r\n";
		out_request << "Content-Type: application/x-www-form-urlencoded\r\n";
		out_request << "Accept: */*\r\n";
		out_request << "Connection: close\r\n\r\n";
		out_request << temp_json.c_str();
		return HTTP_SUCCESS;
	}

	/*
	* ����Get����
	* buildGetRequest
	*/
	int HttpBase::buildGetRequest(const std::string& server, const std::string& path,
		std::ostream& out_request) {
		out_request << "GET " << path.c_str() << " HTTP/1.0\r\n";
		out_request << "Host: " << server.c_str() << "\r\n";
		out_request << "Accept: */*\r\n";
		out_request << "Connection: close\r\n\r\n";
		return HTTP_SUCCESS;
	}
}