#pragma once

#define _CRT_SECURE_NO_WARNINGS
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
	#define PRINTF_SIZE_T "%llu"
#else
	#include <unistd.h>
	#define max(a,b) (((a) > (b)) ? (a) : (b))
	#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

// Time utils
typedef double TIMESTAMP;
TIMESTAMP GetTimeCurrent();
TIMESTAMP GetTimeElapsed(TIMESTAMP StartTime);

// String / Integer utils
size_t IntLen(unsigned int Value);

// Terminate
void TerminateProgram(const char* Message, unsigned int ExitCode);