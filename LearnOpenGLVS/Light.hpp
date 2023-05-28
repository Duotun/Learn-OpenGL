#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "Basic.hpp"

class lightApp : public BasicApp {
	
private:
	static lightApp* instancePtr;

public:
	lightApp();
	lightApp(int inWidth, int inHeight, Camera inCam);

	virtual void RunApplication();   //rewrite for the virual function
	//could extend its own function as well
	static lightApp* getInstance(int inWidth = 800, int inHeight = 600, Camera inCam = Camera())
	{
		if (instancePtr == nullptr)
		{
			instancePtr = new lightApp(inWidth, inHeight, inCam);		
		}
		return instancePtr;
	}

};


#endif // !LIGHT_H
