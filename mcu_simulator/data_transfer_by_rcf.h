/**
  ========================================================================
  Copyright(c) 2017-2018 ****
  author£ºDell  
  version£ºV1.0.0
  data:12/1/2017 
  modifier:                          
  reason:

  ========================================================================
  file:
  description:
  
  ========================================================================
**/

#pragma once
#include <vector>
#include <functional>
using namespace std;

class data_transfer_by_rcf
{
	function<void(vector<unsigned char>&)> _handle_vadata;
public:
	data_transfer_by_rcf();
	~data_transfer_by_rcf();
	void set_vadta_handle(function<void(vector<unsigned char>&)> hd_vdata)
	{
		swap(_handle_vadata, hd_vdata);
	}
	bool send_data_to_host(std::vector<unsigned char>&vdata);
	void dispatch_vdata_to_client(std::vector<unsigned char>&vdata);

};

