#pragma once
#ifndef FrameBuffer_H
#include "Basic.hpp"

class FrameBufferApp : public BasicApp {

private:
	static FrameBufferApp* instancePtr;

public:
	FrameBufferApp();
	FrameBufferApp(int inWidth, int inHeight, Camera inCam);

	virtual void RunApplication();   //rewrite for the virual function
	//could extend its own function as well
	static FrameBufferApp* getInstance(int inWidth = 800, int inHeight = 600, Camera inCam = Camera())
	{
		if (instancePtr == nullptr)
		{
			instancePtr = new FrameBufferApp(inWidth, inHeight, inCam);
		}
		return instancePtr;
	}

};

#endif // !FrameBuffer_H
