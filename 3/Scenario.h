#pragma once
#include "Utils.h"

#define MAX_STRUCTURES 8

struct tagSTRUCTURE
{
	struct
	{
		int X;
		int Y;
	}TopLeft, BottomRight;
};
typedef struct tagSTRUCTURE STRUCTURE;

struct tagSOURCE
{
	int X;
	int Y;
	int Radius;
	float Amplitude;
	double Frequency;
};
typedef struct tagSOURCE SOURCE;

struct tagSCENARIO
{
	char Name[64];

	int SizeX;
	int SizeY;

	unsigned short ThreadCount;
	unsigned short SAVE_TIME;
	
	SOURCE Source;
	
	double Damping;
	double H;
	double MAX_TIME;
	double TIME_STEP;
	double SOURCE_TIME;

	size_t StructureCount;
	STRUCTURE Structures[MAX_STRUCTURES];
};
typedef struct tagSCENARIO SCENARIO;

// Read program input.
void LoadScenarioInfo(const char* InputFile, SCENARIO** Scenarios, size_t* ScenarioCount);
void DeleteScenarioInfo(SCENARIO* Scenarios, size_t ScenarioCount);
void GenerateDummyInputFile(const char* InputFile);

// Buffers & Memory.
void PreallocComputeBuffers(SCENARIO* Scenarios, size_t ScenarioCount);
void DeleteComputeBuffers();

// Set OutputDirectory = NULL for no output.
void ComputeScenario(SCENARIO* Scenario, const char* OutputFileName);
