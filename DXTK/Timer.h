#pragma once
#include "Types.h"

namespace dxtk
{
	class Timer
	{
	public:
		Timer();

		void stop();
		void start();
		float reset();
		float elapsed();
		bool elapsed(float secs) { return elapsed() >= secs; }

	protected:
		LARGE_INTEGER begin, end, freq;
		bool bStoped;
	};
}

