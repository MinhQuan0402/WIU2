#ifndef _TIME_H
#define _TIME_H

class Time
{
private:
	Time(void) = default;
	~Time(void) = default;

public:
	static float deltaTime;
	static float time;
	static float fixedDeltaTime;
};
#endif

