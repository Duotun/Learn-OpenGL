#pragma once
#ifndef BLINNPHONDEMO_H
#define BLINNPHONDEMO_H

#include "Basic.hpp"

class BlinnPhonApp : public BasicApp {

private:
	static BlinnPhonApp* instancePtr;

public:
	BlinnPhonApp();
	BlinnPhonApp(int inWidth, int inHeight, Camera inCam);

	virtual void RunApplication();   //rewrite for the virual function
	//could extend its own function as well
	static BlinnPhonApp* getInstance(int inWidth = 800, int inHeight = 600, Camera inCam = Camera())
	{
		if (instancePtr == nullptr)
		{
			instancePtr = new BlinnPhonApp(inWidth, inHeight, inCam);
		}
		return instancePtr;
	}

};




#endif // !BLINNPHONDEMO_H
