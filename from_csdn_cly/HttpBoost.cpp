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
 
#include "HttpBoost.h"

#include <boost/bind.hpp>
 
#include "http_mark.h"
namespace cly{
	HttpBoost::HttpBoost(boost::asio::io_service& io_service)
		: resolver_(io_service), socket_(io_service) 
	{
	}


	HttpBoost::~HttpBoost() {
	}


	int HttpBoost::post(const std::string& url) {
		handle_request_resolve(url, HttpBase::buildPostRequest);
		return HTTP_SUCCESS;
	}


	int HttpBoost::get(const std::string& url) {
		handle_request_resolve(url, HttpBase::buildGetRequest);
		return HTTP_SUCCESS;
	}


	void HttpBoost::handle_request_resolve(const std::string& url, pBuildRequest func) {
		try {
			responseData_.clear();
			// 解析URL
			std::string server, port, path;
			parseUrl(url, server, port, path);

			std::ostream request_stream(&request_);

			// 合成请求
			func(server, path, request_stream);

			// 解析服务地址\端口
			boost::asio::ip::tcp::resolver::query query(server, port);
			resolver_.async_resolve(query,
				boost::bind(&HttpBoost::handle_resolve, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
		}
		catch (std::exception& e) {
			socket_.close();
			std::cout << "Exception: " << e.what() << "\n";
		}
		return;
	}


	void HttpBoost::handle_resolve(const boost::system::error_code& err,
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
		if (err) {
			std::cout << "Error: " << err << "\n";
			return;
		}

		// 尝试连接
		boost::asio::async_connect(socket_, endpoint_iterator,
			boost::bind(&HttpBoost::handle_connect, this,
			boost::asio::placeholders::error));
	}


	void HttpBoost::handle_connect(const boost::system::error_code& err) {
		if (err) {
			std::cout << "Error: " << err << "\n";
			return;
		}

		// 发送request请求
		boost::asio::async_write(socket_, request_,
			boost::bind(&HttpBoost::handle_write_request, this,
			boost::asio::placeholders::error));
	}


	void HttpBoost::handle_write_request(const boost::system::error_code& err) {
		if (err) {
			std::cout << "Error: " << err << "\n";
			return;
		}

		// 异步持续读数据到response_，直到接收协议符号 \r\n 为止
		boost::asio::async_read_until(socket_, response_, "\r\n",
			boost::bind(&HttpBoost::handle_read_status_line, this,
			boost::asio::placeholders::error));
	}


	void HttpBoost::handle_read_status_line(const boost::system::error_code& err) {
		if (err) {
			std::cout << "Error: " << err << "\n";
			return;
		}

		// 解析buff
		std::istream response_stream(&response_);
		unsigned int status_code;
		std::string http_version, status_message;
		response_stream >> http_version;
		response_stream >> status_code;
		std::getline(response_stream, status_message);

		// 核对是否是正确返回
		if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
			std::cout << "error response\n";
			return;
		}
		if (status_code != 200) {
			std::cout << "服务器响应的状态码: " << status_code << "\n";
			return;
		}

		// 读取响应头,直到接收协议符号 \r\n\r\n 为止
		boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
			boost::bind(&HttpBoost::handle_read_headers, this,
			boost::asio::placeholders::error));
	}


	void HttpBoost::handle_read_headers(const boost::system::error_code& err) {
		if (err) {
			std::cout << "Error: " << err << "\n";
			return;
		}
		// 输出响应头
		std::istream response_stream(&response_);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r") {
#ifdef _DEBUG
			std::cout << header << "\n";
#endif 
		}
#ifdef _DEBUG
		std::cout << "\n";
#endif 

		// 写完所有剩余的内容
		if (response_.size() > 0) {
			boost::asio::streambuf::const_buffers_type cbt = response_.data();
			responseData_ += std::string(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
#ifdef _DEBUG
			std::cout << &response_;
#endif 
		}

		// 开始读取剩余所有内容
		boost::asio::async_read(socket_, response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&HttpBoost::handle_read_content, this,
			boost::asio::placeholders::error));
	}

	void HttpBoost::handle_read_content(const boost::system::error_code& err) {
		if (!err) {
			// 输出读到的数据
			boost::asio::streambuf::const_buffers_type cbt = response_.data();
			responseData_ += std::string(boost::asio::buffers_begin(cbt), boost::asio::buffers_end(cbt));
#ifdef _DEBUG
			std::cout << &response_;
#endif 

			// 继续读取剩余内容，直到读到EOF
			boost::asio::async_read(socket_, response_,
				boost::asio::transfer_at_least(1),
				boost::bind(&HttpBoost::handle_read_content, this,
				boost::asio::placeholders::error));
		}
		else if (err != boost::asio::error::eof) {
			std::cout << "Error: " << err << "\n";
		}
		else {
			socket_.close();
			resolver_.cancel();
			std::cout << "读取响应数据完毕." << std::endl;
		}
	}


	std::string post(std::string url) {
		boost::asio::io_service io;
		HttpBoost c(io);
		c.post(url);
		io.run();
		return c.getResponse();
	}

	std::string get(std::string url) {
		boost::asio::io_service io;
		HttpBoost c(io);
		c.get(url);
		io.run();
		return c.getResponse();
	}
}