#include "Timer.h"

namespace dxtk
{
	Timer::Timer()
	{
		QueryPerformanceFrequency(&freq);

		ZeroMemory(&begin, sizeof(begin));
		ZeroMemory(&end, sizeof(end));

		bStoped = false;
	}

	void Timer::stop()
	{
		if(!bStoped)
		{
			QueryPerformanceCounter(&end);
			bStoped = true;
		}
	}

	void Timer::start()
	{
		if(bStoped)
		{
			uint64_t llElapsed = end.QuadPart - begin.QuadPart;
			QueryPerformanceCounter(&begin);
			begin.QuadPart -= llElapsed;
			bStoped = false;
		}
		else
		{
			QueryPerformanceCounter(&begin);
		}
	}

	float Timer::reset()
	{
		uint64_t llElapsed = 0;

		if(bStoped)
		{
			llElapsed = end.QuadPart - begin.QuadPart;
			QueryPerformanceCounter(&begin);
			bStoped = false;
		}
		else
		{
			QueryPerformanceCounter(&end);
			llElapsed = end.QuadPart - begin.QuadPart;
			begin = end;
		}
		return float(llElapsed / (double)freq.QuadPart);
	}

	float Timer::elapsed()
	{
		int64_t llElapsed;

		if(bStoped)
		{
			llElapsed = end.QuadPart - begin.QuadPart;
		}
		else
		{
			QueryPerformanceCounter(&end);
			llElapsed = end.QuadPart - begin.QuadPart;
		}
		return float(llElapsed / (double)freq.QuadPart);
	}
}
