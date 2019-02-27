#pragma once
#define AFGUI_APP(cls) int main(int argc, char **argv) { return startApp<cls>(argc, argv); }
template <class AppClass>
int startApp(int argc, char **argv)
{
	try
	{

		AppClass* app = new AppClass(argc, argv);
		app->prepare();
		if (app->create_run())
		{
			app->destroy();
		}
		delete app;
		app = 0;
	}
	catch (...)
	{

		printf( "something is wrong!\n");
		throw;
	}

	return 0;
}