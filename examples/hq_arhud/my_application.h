#pragma once
#include "application.h"

class my_application :
	public auto_future::application
{
      bool _be_uart_comm;
public:
	my_application(int argc, char **argv);
	void resLoaded();
	void onUpdate();
     void before_swap();
	virtual ~my_application();

};

