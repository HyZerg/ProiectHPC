#include "Scenario.h"


void GenerateDummyInputFile(const char* InputFile)
{
	FILE* File = fopen(InputFile, "w");
	if (File == NULL)
	{
		TerminateProgram("[ERR] Could not open output file.\n", 0);
		return; // ma enerveaza warn msvc
	}
		


	fprintf(File, "[SCENARIO_COUNT] = %%lu\n\n");
	fprintf(File, "[SCENARIO] = %%s\n");
	fprintf(File, "    [THREADS] = %%hu\n");
	fprintf(File, "    [SIZE_X_Y] = %%lu %%lu\n");
	fprintf(File, "    [MAX_TIME] = %%lf\n");
	fprintf(File, "    [SOURCE_TIME] = %%lf\n");
	fprintf(File, "    [STEP_TIME] = %%lf\n");
	fprintf(File, "    [SAVE_TICK] = %%lu\n");
	fprintf(File, "    [DAMPING] = %%lf\n");
	fprintf(File, "    [H] = %%lf\n");
	fprintf(File, "    [SOURCE] = %%d %%d %%d %%f %%lf\n");
	fprintf(File, "    [BLOCK] = %%d %%d %%d %%d\n");

	fclose(File);
}

#define NEXT_TOKEN_EXIT_IF_NULL						\
Token = strtok(NULL, " =\n");						\
if (Token == NULL)									\
{													\
	TerminateProgram("[ERR] Invalid input.\n", 0);	\
	return;											\
}


void LoadScenarioInfo(const char* InputFile, SCENARIO** Scenarios, size_t* ScenarioCount)
{
	FILE* File = fopen(InputFile, "r");
	if (File == NULL)
	{
		TerminateProgram("[ERR] Could not open output file.\n", 0);
		return; // ma enerveaza warn msvc
	}

	char Line[256] = { 0 };
	size_t ScenarioIndex = -1;
	size_t FileLine = 1;

	while (fgets(Line, 256, File))
	{
		char* Token = strtok(Line, " =\n");
		if (Token == NULL)
			continue;

		if (strcmp(Token, "[SCENARIO_COUNT]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;

			*ScenarioCount = (size_t)atoll(Token);
			*Scenarios = calloc(*ScenarioCount, sizeof(SCENARIO));
			if (*Scenarios == NULL)
				TerminateProgram("[ERR] Failed to allocate memory.\n", 0);
		}
		else if (strcmp(Token, "[SCENARIO]") == 0)
		{
			ScenarioIndex++;
			NEXT_TOKEN_EXIT_IF_NULL;

			strcpy((*Scenarios)[ScenarioIndex].Name, Token);
		}
		else if (strcmp(Token, "[THREADS]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].ThreadCount = (unsigned short)atoll(Token);
		}
		else if (strcmp(Token, "[SIZE_X_Y]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].SizeX = (unsigned int)atoll(Token);
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].SizeY = (unsigned int)atoll(Token);
		}
		else if (strcmp(Token, "[MAX_TIME]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].MAX_TIME = atof(Token);
		}
		else if (strcmp(Token, "[SOURCE_TIME]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].SOURCE_TIME = atof(Token);
		}
		else if (strcmp(Token, "[STEP_TIME]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].TIME_STEP = atof(Token);
		}
		else if (strcmp(Token, "[SAVE_TICK]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].SAVE_TIME = (unsigned int)atoll(Token);
		}
		else if (strcmp(Token, "[DAMPING]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].Damping = atof(Token);
		}
		else if (strcmp(Token, "[H]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].H = atof(Token);
		}
		else if (strcmp(Token, "[SOURCE]") == 0)
		{
			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].Source.X = (int)atoll(Token);

			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].Source.Y = (int)atoll(Token);

			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].Source.Radius = (int)atoll(Token);

			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].Source.Amplitude = (float)atof(Token);

			NEXT_TOKEN_EXIT_IF_NULL;
			(*Scenarios)[ScenarioIndex].Source.Frequency = (float)atof(Token);
		}
		else if (strcmp(Token, "[BLOCK]") == 0)
		{
			if ((*Scenarios)[ScenarioIndex].StructureCount >= MAX_STRUCTURES)
			{
				printf("Scenario: %llu    Line: %llu\n", ScenarioIndex, FileLine);
				TerminateProgram("[ERR] Scenario exceeds max blocks.\n", 0);
				
				return; // ma enerveaza warn msvc
			}

			#define THIS_BLOCK ((*Scenarios)[ScenarioIndex].Structures[(*Scenarios)[ScenarioIndex].StructureCount])

			NEXT_TOKEN_EXIT_IF_NULL;
			THIS_BLOCK.TopLeft.X = (int)atoll(Token);

			NEXT_TOKEN_EXIT_IF_NULL;
			THIS_BLOCK.TopLeft.Y = (int)atoll(Token);

			NEXT_TOKEN_EXIT_IF_NULL;
			THIS_BLOCK.BottomRight.X = (int)atoll(Token);

			NEXT_TOKEN_EXIT_IF_NULL;
			THIS_BLOCK.BottomRight.Y = (int)atoll(Token);

			#undef THIS_BLOCK

			(*Scenarios)[ScenarioIndex].StructureCount++;
		}

		FileLine++;
	}



	fclose(File);
}

void DeleteScenarioInfo(SCENARIO* Scenarios, size_t ScenarioCount)
{

}