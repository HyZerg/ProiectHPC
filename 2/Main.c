#include "Scenario.h"


int main()
{
	//GenerateDummyInputFile("Input.txt");
	SCENARIO* Scenarios;
	size_t ScenarioCount;

	LoadScenarioInfo("Input.txt", &Scenarios, &ScenarioCount);
	printf("Loaded %llu scenarios.\n", ScenarioCount);

	PreallocComputeBuffers(Scenarios, ScenarioCount);

	for (size_t i = 0; i < ScenarioCount; i++)
		ComputeScenario(&Scenarios[i], Scenarios[i].Name);
	
	DeleteComputeBuffers();

	return 0;
}