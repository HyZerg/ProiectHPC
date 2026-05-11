#include "Scenario.h"


int main(int ArgCount, char** Args)
{
        if (ArgCount < 1)
          TerminateProgram("No input file specified.\n", 0);
          
          
	//GenerateDummyInputFile("Input.txt");
	SCENARIO* Scenarios;
	size_t ScenarioCount;

	LoadScenarioInfo(Args[1], &Scenarios, &ScenarioCount);
	printf("Loaded " PRINTF_SIZE_T " scenarios.\n", ScenarioCount);

	PreallocComputeBuffers(Scenarios, ScenarioCount);

	for (size_t i = 0; i < ScenarioCount; i++)
		ComputeScenario(&Scenarios[i], Scenarios[i].Name);
	
	DeleteComputeBuffers();

	return 0;
}
