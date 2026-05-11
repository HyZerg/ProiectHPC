#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

// Time utils
typedef double TIMESTAMP;
TIMESTAMP GetTimeCurrent();
TIMESTAMP GetTimeElapsed(TIMESTAMP StartTime);

// String / Integer utils
size_t IntLen(unsigned int Value);

// Terminate
void TerminateProgram(const char* Message, unsigned int ExitCode);