#include "my_application.h"
#include "main.h"

my_application::my_application(int argc, char **argv)
	:application(argc,argv)
{
}


my_application::~my_application()
{
}
AFGUI_APP(my_application)