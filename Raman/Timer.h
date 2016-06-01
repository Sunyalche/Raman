#pragma once
#include "Util.h"

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void ReStart();
	void Stop();
	double GetDeltaBySec();
	double GetDeltaByNanoSec();



	double mSecondsPerCount;
	__int64 prevTime;
	__int64 delta;
};

