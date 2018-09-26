#pragma once
#include <vector>
#include <functional>
typedef std::function<void(unsigned char* pcmd_buf,int len)> cmd_handle;

class idata_collect
{
	cmd_handle _cmd_handle;
public:
	idata_collect();
	~idata_collect();
	void link();
	void send_vdata_2_host(std::vector<unsigned char>& vadata);
	void set_cmd_handle(cmd_handle cmd_hdl)
	{
		std::swap(_cmd_handle, cmd_hdl);
	}
};

