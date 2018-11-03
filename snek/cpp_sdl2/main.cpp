#include "app.hpp"
#include "error.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	try
	{
		Snek::App app;
		app.run();
	}
	catch (Snek::Error &error)
	{
		std::cerr << "Snek fatal error: " << error.what() << std::endl;
		return 1;
	}

	return 0;
}
