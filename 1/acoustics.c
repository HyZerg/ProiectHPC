#include "acoustics.h"

size_t ScenarioCount = MAX_SCENARIOS;
scenario_t Scenarios[MAX_SCENARIOS];

int main(int argc, char* argv[])
{
	if (argc != 2)
		EXIT_ERROR("[EROARE] Numar incorect de argumente. Folosire: ./acoustics <input_file_name>\n\n");

	ImportData(argv[1]);



	for (size_t i = 0; i < ScenarioCount; i++)
	{
		omp_set_num_threads(Scenarios[i].OMP_THREADS);

		struct timespec Start;
		clock_gettime(CLOCK_MONOTONIC_RAW, &Start);

		

		load_scenario();
		init_scenario();
		s_compute_acoustics();

		struct timespec End;
		clock_gettime(CLOCK_MONOTONIC_RAW, &End);

		End.tv_sec -= Start.tv_sec;
		if (End.tv_nsec >= Start.tv_nsec)
			End.tv_nsec -= Start.tv_nsec;
		else End.tv_nsec = End.tv_nsec + 1000000000 - Start.tv_nsec;
		ExportDataGNUPlot(NULL, i, End);

		unload_scenario();
	}

	return 0;
}