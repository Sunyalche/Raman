#include "Timer.h"

Timer::Timer() : prevTime(0), delta(0)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

void Timer::Start()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	prevTime = currTime;
}

void Timer::ReStart()
{
	delta = 0;
	Timer::Start();
}

void Timer::Stop()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	delta += currTime - prevTime;
}

double Timer::GetDeltaBySec()
{
	return delta * mSecondsPerCount;
}

double Timer::GetDeltaByNanoSec()
{
	return GetDeltaBySec() * 10e9;
}


Timer::~Timer()
{
}
