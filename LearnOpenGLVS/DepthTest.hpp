#pragma once
#ifndef DEPTHTEST_H
#define DEPTHTEST_H

#include "Basic.hpp"

class depthApp : public BasicApp {

private:
	static depthApp* instancePtr;

public:
	depthApp();
	depthApp(int inWidth, int inHeight, Camera inCam);

	virtual void RunApplication();   //rewrite for the virual function
	//could extend its own function as well
	static depthApp* getInstance(int inWidth = 800, int inHeight = 600, Camera inCam = Camera())
	{
		if (instancePtr == nullptr)
		{
			instancePtr = new depthApp(inWidth, inHeight, inCam);
		}
		return instancePtr;
	}

};


#endif // ! DEPTHTEST_H
