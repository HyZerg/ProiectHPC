#include "Scenario.h"

#define AT(X, Y) ((Y)*Scenario->SizeX + (X))

// ------ Compute Buffers ------
double* A;
double* B;
double* C;
size_t ArraySize;

struct tagPIXEL
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
};
typedef struct tagPIXEL PIXEL;

PIXEL* Pixels;
size_t PixelBufferSize;
// -----------------------------

// ------ Functiile Originale Refacute ------
int IsSource(SCENARIO* Scenario, int X, int Y, int SourceActive)
{
	return (SourceActive && (
		(Scenario->Source.Radius * Scenario->Source.Radius) >= 
		(Scenario->Source.X - X) * (Scenario->Source.X - X) + 
		(Scenario->Source.Y - Y) * (Scenario->Source.Y - Y) 
		));
}

void PulseSource(SCENARIO* Scenario, unsigned int Step, STRUCTURE* PulseRegion)
{
	// Step este incorect matematic in functia sin. Trebuia:
	//   sin(Step * TIME_STEP * M_PI / 4)

	int Y;
	#pragma omp parallel for
	for (Y = PulseRegion->TopLeft.Y; Y < PulseRegion->BottomRight.Y; Y++)
		for (int X = PulseRegion->TopLeft.X; X < PulseRegion->BottomRight.X; X++)
			if (IsSource(Scenario, X, Y, 1))
				C[AT(X, Y)] = Scenario->Source.Amplitude * sin(2 * M_PI * Scenario->Source.Frequency * Step * Scenario->TIME_STEP);
}

// ------------------------------------------


void PreallocComputeBuffers(SCENARIO* Scenarios, size_t ScenarioCount)
{
	size_t Largest2DArray = 0;

	for (size_t i = 0; i < ScenarioCount; i++)
	{
		if (Scenarios[i].SizeX * Scenarios[i].SizeY > Largest2DArray)
			Largest2DArray = Scenarios[i].SizeX * Scenarios[i].SizeY;
	}

	printf("Preallocation sizes:\n");
	printf("    Largest 2D Map:     %.2lf KB\n", Largest2DArray * sizeof(double) / 1024.0);
	printf("    Largest RGB Buffer: %.2lf KB\n", Largest2DArray * sizeof(PIXEL) / 1024.0);
	printf("\n");
	printf("    Total required: %.2lf MB\n", (Largest2DArray * sizeof(double) * 3 + Largest2DArray * sizeof(PIXEL)) / (1024.0 * 1024.0));
	printf("\n");

	A = calloc(Largest2DArray, sizeof(double));
	B = calloc(Largest2DArray, sizeof(double));
	C = calloc(Largest2DArray, sizeof(double));
	Pixels = calloc(Largest2DArray, sizeof(PIXEL));

	if (!A || !B || !C || !Pixels)
		TerminateProgram("[ERR] Failed to allocate buffers.\n", 0);

	PixelBufferSize = Largest2DArray * sizeof(PIXEL);
	ArraySize = Largest2DArray * sizeof(double);
}
void DeleteComputeBuffers()
{
	free(A);
	free(B);
	free(C);
	free(Pixels);
}


void ComputeScenario(SCENARIO* Scenario, const char* OutputFileName)
{
	FILE* OutputFile = NULL;
	if (OutputFileName != NULL)
	{
		OutputFile = fopen(OutputFileName, "wb");
		if (OutputFile == NULL)
			TerminateProgram("[ERR] Unable to open output file.\n", 0);

		printf("Scenario [%s]:\n", OutputFileName);
	}
	else printf("Scenario with no output file:\n");

	time_t ConsoleWriteTime = 0;
	int SourceActive = 1;

	TIMESTAMP TotalSimulationTime = 0;
	TIMESTAMP TotalOutputComputeTime = 0;

	// Source Region
	STRUCTURE SourceRegion;
	SourceRegion.TopLeft.Y = (Scenario->Source.Y > Scenario->Source.Radius) ?
		                     (Scenario->Source.Y - Scenario->Source.Radius) : 0;

	SourceRegion.BottomRight.Y = (Scenario->Source.Y + Scenario->Source.Radius < Scenario->SizeY) ?
		                         (Scenario->Source.Y + Scenario->Source.Radius) : Scenario->SizeY;

	SourceRegion.TopLeft.X = (Scenario->Source.X > Scenario->Source.Radius) ?
							 (Scenario->Source.X - Scenario->Source.Radius) : 0;

	SourceRegion.BottomRight.X = (Scenario->Source.X + Scenario->Source.Radius < Scenario->SizeX) ?
								 (Scenario->Source.X + Scenario->Source.Radius) : Scenario->SizeX;


	double Coeficient = (Scenario->TIME_STEP * Scenario->TIME_STEP) / (Scenario->H * Scenario->H);
	double Dampping = Scenario->Damping;
	
	omp_set_num_threads(Scenario->ThreadCount);
	printf("    Running with %d threads\n", (int)Scenario->ThreadCount);

	for (unsigned int Step = 0; Step < (unsigned int)(Scenario->MAX_TIME / Scenario->TIME_STEP); Step++)
	{
		TIMESTAMP StartSimulationTime = GetTimeCurrent();

		// Compute Node (inner)
		int Y;
		#pragma omp parallel for private(Y)
		for (Y = 1; Y < Scenario->SizeY - 1; Y++)
			for (int X = 1; X < Scenario->SizeX - 1; X++)
				C[AT(X, Y)] = (B[AT(X, Y)] + Coeficient * (B[AT(X - 1, Y)] - 4 * B[AT(X, Y)] + B[AT(X + 1, Y)] + B[AT(X, Y - 1)] + B[AT(X, Y + 1)])) + (B[AT(X, Y)] - A[AT(X, Y)]) * Dampping;

		// Compute Node (borders)
		#pragma omp parallel sections
		{
			// Top Row
			#pragma omp section
			{
				C[AT(0, 0)] = (2 * B[AT(0, 0)] +
					          Coeficient * (-4 * B[AT(0, 0)] + B[AT(1, 0)] + B[AT(0, 1)]) - 
					          A[AT(0, 0)]) * Dampping;


				for (int X = 1; X < Scenario->SizeX - 1; X++)
					C[AT(X, 0)] = (2 * B[AT(X, 0)] + 
					              Coeficient * (B[AT(X - 1, 0)] - 4 * B[AT(X, 0)] + B[AT(X + 1, 0)] + B[AT(X, 1)]) - 
					              A[AT(X, 0)]) * Dampping;
				

				C[AT(Scenario->SizeX - 1, 0)] = (2 * B[AT(Scenario->SizeX - 1, 0)] + 
					                            Coeficient * (-4 * B[AT(Scenario->SizeX - 1, 0)] + B[AT(Scenario->SizeX - 2, 0)] + B[AT(Scenario->SizeX - 1, 1)]) - 
					                            A[AT(Scenario->SizeX - 1, 0)]) * Dampping;
			}

			// Bottom Row
			#pragma omp section
			{
				C[AT(0, Scenario->SizeY - 1)] = (2 * B[AT(0, Scenario->SizeY - 1)] +
											    Coeficient * (-4 * B[AT(0, Scenario->SizeY - 1)] + B[AT(1, Scenario->SizeY - 1)] + B[AT(0, Scenario->SizeY - 2)]) -
											    A[AT(0, Scenario->SizeY - 1)]) * Dampping;

				for (int X = 1; X < Scenario->SizeX - 1; X++)
					C[AT(X, Scenario->SizeY - 1)] = (2 * B[AT(X, Scenario->SizeY - 1)] + Coeficient * (B[AT(X - 1, Scenario->SizeY - 1)] - 4 * B[AT(X, Scenario->SizeY - 1)] + B[AT(X + 1, Scenario->SizeY - 1)] + B[AT(X, Scenario->SizeY - 2)]) - A[AT(X, Scenario->SizeY - 1)]) * Dampping;
			
				C[AT(Scenario->SizeX - 1, Scenario->SizeY - 1)] = (2 * B[AT(Scenario->SizeX - 1, Scenario->SizeY - 1)] +
																  Coeficient * (B[AT(Scenario->SizeX - 2, Scenario->SizeY - 1)] - 4 * B[AT(Scenario->SizeX - 1, Scenario->SizeY - 1)] + B[AT(Scenario->SizeX - 1, Scenario->SizeY - 2)]) -
																  A[AT(Scenario->SizeX - 1, Scenario->SizeY - 1)]) * Dampping;
			}

			// Left Column
			#pragma omp section
			{
				for (int Y = 1; Y < Scenario->SizeY - 1; Y++)
					C[AT(0, Y)] = (2 * B[AT(0, Y)] + Coeficient * (-4 * B[AT(0, Y)] + B[AT(0 + 1, Y)] + B[AT(0, Y - 1)] + B[AT(0, Y + 1)]) - A[AT(0, Y)]) * Dampping;
			}

			// Right Column
			#pragma omp section
			{
				for (int Y = 1; Y < Scenario->SizeY - 1; Y++)
					C[AT(Scenario->SizeX - 1, Y)] = (2 * B[AT(Scenario->SizeX - 1, Y)] + Coeficient * (B[AT(Scenario->SizeX - 2, Y)] - 4 * B[AT(Scenario->SizeX - 1, Y)] + B[AT(Scenario->SizeX - 1, Y - 1)] + B[AT(Scenario->SizeX - 1, Y + 1)]) - A[AT(Scenario->SizeX - 1, Y)]) * Dampping;
			}
		}

		if (Step * Scenario->TIME_STEP < Scenario->SOURCE_TIME)
			PulseSource(Scenario, Step, &SourceRegion);
		

		// Zero Memory Structures
		int i; // size_t does not work for openmp
		#pragma omp parallel for private(i)
		for (i = 0; i < Scenario->StructureCount; i++)
			for (int Y = Scenario->Structures[i].TopLeft.Y; Y < Scenario->Structures[i].BottomRight.Y; Y++)
				memset(&C[AT(Scenario->Structures[i].TopLeft.X, Y)], 0,
					   sizeof(double) * (Scenario->Structures[i].BottomRight.X - Scenario->Structures[i].TopLeft.X));

		// Zero Memory A
		//memset(A, 0, ArraySize);

		TotalSimulationTime += GetTimeElapsed(StartSimulationTime);

		// I/O
		if ((OutputFileName != NULL) && (Step % Scenario->SAVE_TIME == 0))
		{
			TIMESTAMP StartComputeTime = GetTimeCurrent();

			int Y;
			#pragma omp parallel for private(Y)
			for (Y = 0; Y < Scenario->SizeY; Y++)
				for (int X = 0; X < Scenario->SizeX; X++)
				{
					Pixels[AT(X, Y)].R = (unsigned char)(max(0, C[AT(X, Y)]) / Scenario->Source.Amplitude * 255);
					Pixels[AT(X, Y)].G = 0;
					Pixels[AT(X, Y)].B = (unsigned char)(min(0, C[AT(X, Y)]) / Scenario->Source.Amplitude * 255);
				}

			// Gray out structures
			int i;
			#pragma omp parallel for private(i)
			for (i = 0; i < Scenario->StructureCount; i++)
				for (int Y = Scenario->Structures[i].TopLeft.Y; Y < Scenario->Structures[i].BottomRight.Y; Y++)
					memset(&Pixels[AT(Scenario->Structures[i].TopLeft.X, Y)], 0xFFFFFFFF, sizeof(PIXEL) * (Scenario->Structures[i].BottomRight.X - Scenario->Structures[i].TopLeft.X));

			fwrite(Pixels, PixelBufferSize, 1, OutputFile);

			TotalOutputComputeTime += GetTimeElapsed(StartComputeTime);
		}

		if (ConsoleWriteTime < time(0))
		{
			printf("\r    Generated %.02lf / %.02lf seconds      ", Scenario->TIME_STEP * Step, Scenario->MAX_TIME);
			ConsoleWriteTime = time(0);
		}

		// Swap Buffers
		double* Temp = A;
		A = B;
		B = C;
		C = Temp;
	}

	printf("\r    Generated %.02lf / %.02lf seconds      ", Scenario->MAX_TIME, Scenario->MAX_TIME);
	printf("\n    Output has " PRINTF_SIZE_T " frames (%dx%d)\n", (size_t)(Scenario->MAX_TIME / Scenario->TIME_STEP)/Scenario->SAVE_TIME,
													   Scenario->SizeX, Scenario->SizeY);
	printf("    Total output file size %.02lfMB\n", (double)(PixelBufferSize* ((unsigned int)(Scenario->MAX_TIME / Scenario->TIME_STEP) / Scenario->SAVE_TIME)) / (1024 * 1024));
	printf("    Simulation time:     %.09lf (%03.02lf%%)\n", TotalSimulationTime, 100 * (TotalSimulationTime) / (TotalSimulationTime + TotalOutputComputeTime));
	printf("    Output compute time: %.09lf (%03.02lf%%)\n\n", TotalOutputComputeTime, 100 * (TotalOutputComputeTime) / (TotalSimulationTime + TotalOutputComputeTime));

	if (OutputFileName != NULL)
		fclose(OutputFile);
}
