#include "WindowApplication.hpp"


int main()
{
	GLFWApplication app;
	try {
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return 0;
}