#include "TextureApp.hpp"

int main()
{
	TextureApp app(1000, 600);
	try {
		app.run();
	}
	catch (const std::exception& e)  //catch whether the app runs successfully
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return 0;
}