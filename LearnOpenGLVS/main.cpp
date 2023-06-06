//#include application headers will be enough
#include "HDR.hpp";
//#include "Light.hpp"
//#include "DepthTest.hpp"
//#include "FrameBuffer.hpp"

const int width =  800; //1024;
const int height = 600;  //768;

int main()
{
	Camera cam = Camera(glm::vec3(0.0, 0.0f, -2.0f));
	BasicApp* basicapp = HDRApp::getInstance(800, 600, cam);
	basicapp->RunApplication();

	return 0;
}