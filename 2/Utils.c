#include "Utils.h"

size_t IntLen(unsigned int Value)
{
	size_t CharCount = 0;

	do
	{
		CharCount++;
		Value /= 10;
	} while (Value > 0);

	return CharCount;
}


#ifdef _WIN32
	TIMESTAMP GetTimeCurrent()
	{
		LARGE_INTEGER LI;
		QueryPerformanceCounter(&LI);
		return ((double)LI.QuadPart * 100) / 1000000000;
	}
#else
	TIMESTAMP GetTimeCurrent()
	{
		struct timespec Start;
		clock_gettime(CLOCK_MONOTONIC_RAW, &Start);
		return ((double)(Start.tv_sec * 1000000000 + Start.tv_nsec) / 1000000000);
	}
#endif

TIMESTAMP GetTimeElapsed(TIMESTAMP StartTime)
	{ return GetTimeCurrent() - StartTime; }

void TerminateProgram(const char* Message, unsigned int ExitCode)
{
	if (Message)
	{
		fprintf(stderr, Message);
		fflush(stderr);
	}

	#ifdef _WIN32
		ExitProcess(ExitCode);
	#else
		exit(ExitCode);
	#endif
}