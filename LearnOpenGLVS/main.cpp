//#include application headers will be enough
#include "Light.hpp"
//#include "DepthTest.hpp"
//#include "FrameBuffer.hpp"

const int width = 1024;
const int height = 768;

int main()
{
	Camera cam = Camera(glm::vec3(0.0, 0.0f, 3.0f));
	BasicApp* basicapp = lightApp::getInstance(1024, 768, cam);
	basicapp->RunApplication();

	return 0;
}