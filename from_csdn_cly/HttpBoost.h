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
 
#ifndef __CLY_HTTPBOOST_H__
#define __CLY_HTTPBOOST_H__
 
#include "HttpBase.h"
 
#include <boost/asio.hpp>
 
namespace cly {
 
class HttpBoost :
	public HttpBase {

	boost::asio::ip::tcp::resolver resolver_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::streambuf request_;
	boost::asio::streambuf response_;
	std::string responseData_;
public:
 
	HttpBoost(boost::asio::io_service& io_service);
 
	virtual ~HttpBoost();
 
	/*
	* 发送Post请求
	*/
	virtual int post(const std::string& url);
 
	/*
	* 发送get请求
	*/
	virtual int get(const std::string& url);
 
	virtual std::string getResponse(void) {
		return responseData_;
	}
	
private:
	void handle_request_resolve(const std::string& url, pBuildRequest func);
	void handle_resolve(const boost::system::error_code& err,
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
 
	void handle_connect(const boost::system::error_code& err);
 
	void handle_write_request(const boost::system::error_code& err);
	
	void handle_read_status_line(const boost::system::error_code& err);
	
	void handle_read_headers(const boost::system::error_code& err);
 
	void handle_read_content(const boost::system::error_code& err);
};
// Post请求 消息范例 url:80/openapi/getversion[Post请求数据表格]
std::string post(std::string url);
 
 
 
std::string get(std::string url);
 
 
}
#endif // __CLY_HTTPBOOST_H__
