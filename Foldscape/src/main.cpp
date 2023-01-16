#include "application.h"

int Main()
{
	foldscape::Application app;
	try
	{
		app.Init(L"Foldscape", 1000, 700);
		app.Run();
	}
	catch (const std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), "Error", MB_OK);
	}
	return 0;
}

int wmain(int argc, char* argv[])
{
	return Main();
}

int WINAPI wWinMain(HINSTANCE __in hInstanec, HINSTANCE __in_opt hPrevInstance, LPWSTR szCmdLine, int nCmdShow)
{
	return Main();
}