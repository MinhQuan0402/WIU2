
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

class Application
{
public:
	Application();
	~Application();
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);


	static int m_consoleHeight;
	static int m_consoleWidth;
	static bool enableCursor;
	static bool showCursor;
private:

	//Declare a window object
	StopWatch m_timer;
};

#endif