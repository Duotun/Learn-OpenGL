#pragma once
#ifndef HDRAPP_H
#define HDRAPP_H

#include "Basic.hpp"

void renderQuad(VAO& inVAO, VBO& inVBO);
void renderCube(VAO& inVAO, VBO& inVBO);

class HDRApp : public BasicApp {

private:
	static HDRApp* instancePtr;

public:
	HDRApp();
	HDRApp(int inWidth, int inHeight, Camera inCam);

	virtual void RunApplication();   //rewrite for the virual function
	//could extend its own function as well
	static HDRApp* getInstance(int inWidth = 800, int inHeight = 600, Camera inCam = Camera())
	{
		if (instancePtr == nullptr)
		{
			instancePtr = new HDRApp(inWidth, inHeight, inCam);
		}
		return instancePtr;
	}

};


#endif // !HDRAPP_H
