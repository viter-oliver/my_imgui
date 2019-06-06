/*
* 简介：利用Boost发送http的Post/Get请求
* 作者：陈鲁勇
* 邮箱：727057301@qq.com
* CSDN：http://blog.csdn.net/csnd_ayo
* 码云：https://git.oschina.net/Mr_ChenLuYong
* github：http://github.com/chenluyong
* 创建时间：2017年3月12日 04:32:07
* VS版本：VS2013
* Boost版本：boost_1_61_0
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
	//可以解析下列三种类型的URL:
	//http://yunhq.sse.com.cn:32041/v1/sh1/snap/204001?callback=jQuery_test&select=name%2Clast%2Cchg_rate%2Cchange%2Camount%2Cvolume%2Copen%2Cprev_close%2Cask%2Cbid%2Chigh%2Clow%2Ctradephase
	//http://hq.sinajs.cn/list=sh204001
	//https://www.baidu.com
	//////////////////////////////////////////////////////////////////////////
	int HttpBase::parseUrl(const std::string& url, std::string& out_server,
		std::string& out_port, std::string& out_path) {
		const std::string& http___ = "http://";
		const std::string& https___ = "https://";
		std::string temp_data = url;

		// 截断http协议头
		if (temp_data.find(http___) == 0) {
			temp_data = temp_data.substr(http___.length());
		}
		else if (temp_data.find(https___) == 0) {
			temp_data = temp_data.substr(https___.length());
		}
		else {
			return HTTP_ERROR_HTTP_HEAD;
		}

		// 解析域名
		std::size_t idx = temp_data.find('/');
		// 解析 域名后的page
		if (std::string::npos == idx) {
			out_path = "/";
			idx = temp_data.size();
		}
		else {
			out_path = temp_data.substr(idx);
		}

		// 解析域名
		out_server = temp_data.substr(0, idx);

		// 解析端口
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
	* 建立Post请求
	* buildPostRequest
	*/
	int HttpBase::buildPostRequest(const std::string& server, const std::string& path,
		std::ostream& out_request) {
		// 分割path中的json数据
		std::string temp_path(path), temp_json;
		int json_pos_begin = temp_path.find(HTTP_JSON_BEGIN) + 1;
		int json_pos_end = temp_path.find(HTTP_JSON_END);
		if (json_pos_begin != std::string::npos) {
			// 计算json的长度
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
	* 建立Get请求
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