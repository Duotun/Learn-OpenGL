#pragma once
#ifndef MODELTEST_H
#define MODELTEST_H

#include "Basic.hpp"

class modelApp : public BasicApp {

private:
	static modelApp* instancePtr;

public:
	modelApp();
	modelApp(int inWidth, int inHeight, Camera inCam);

	virtual void RunApplication();   //rewrite for the virual function
	//could extend its own function as well
	static modelApp* getInstance(int inWidth = 800, int inHeight = 600, Camera inCam = Camera())
	{
		if (instancePtr == nullptr)
		{
			instancePtr = new modelApp(inWidth, inHeight, inCam);
		}
		return instancePtr;
	}

};


#endif // ! MODELTEST_H
