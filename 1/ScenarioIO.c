#include "acoustics.h"

// -- UTILS --
size_t IntLen(int Val)
{
	char Buffer[32];
	return strlen(itoa(Val, Buffer, 10));
}
// -----------

// Pentru orice motiv, autorul a generat seturi de intrare cu coordonate fl64[0, 1] si scaleaza
//  in functia de import la planul i32[0, nx] / i32[0, ny].
#define INPUT_VALUE_INTERPOLATION_X(StrValue) (int)(atof(StrValue) * Scenarios[CurrentScenario].nx)
#define INPUT_VALUE_INTERPOLATION_Y(StrValue) (int)(atof(StrValue) * Scenarios[CurrentScenario].ny)

void ImportData(const char* FileName)
{
	FILE* InputFile = fopen(FileName, "r");
	if (!InputFile)
		EXIT_ERROR("[EROARE] Deschiderea fisierului de input esuata.\n");

	char Line[256] = { 0 };
	size_t CurrentScenario = -1;
	size_t CurrentStructure = -1;

	// expect: "[abcd] = 1234\n"
	while (fgets(Line, 256, InputFile) != NULL)
	{
		const char* LValue = strtok(Line, " =\n");
		const char* RValue = strtok(NULL, " =\n");
		const char* RValue2 = strtok(NULL, " =\n");

		if (!LValue || !RValue)
			continue;

		#define IF_LVALUE_CMP(FieldName)\
			if (strcmp(LValue, FieldName) == 0)

		#define IF_LVALUE_CMP_SAFESCENARIO(FieldName)\
			if (strcmp(LValue, FieldName) == 0 && (CurrentScenario < MAX_SCENARIOS))

		#define IF_LVALUE_CMP_SAFESCENARIO_SAFESTRUCT(FieldName)\
			if (strcmp(LValue, FieldName) == 0 && (CurrentScenario < MAX_SCENARIOS) && (CurrentStructure < MAX_STRUCTURES))

		IF_LVALUE_CMP("[NUM_SCENARIOS]")
		{
			ScenarioCount = atoi(RValue);
			if (ScenarioCount > MAX_SCENARIOS)
				EXIT_ERROR("[EROARE] Invalid input: [NUM_SCENARIOS] > MAX_SCENARIOS\n");
		}
		else IF_LVALUE_CMP("[SCENARIO]")
		{
			CurrentScenario = atoi(RValue);
			if (CurrentScenario >= ScenarioCount)
				EXIT_ERROR("[EROARE] Invalid input: [SCENARIO] out of bounds\n");
		}
		else IF_LVALUE_CMP_SAFESCENARIO("[OMP_THREADS]")
			Scenarios[CurrentScenario].OMP_THREADS = atoi(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[SIZEX]")
			Scenarios[CurrentScenario].nx = atoi(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[SIZEY]")
			Scenarios[CurrentScenario].ny = atoi(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[H]")
			Scenarios[CurrentScenario].H = atof(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[MAX_TIME]")
			Scenarios[CurrentScenario].MAX_TIME = atof(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[TIME_STEP]")
			Scenarios[CurrentScenario].TIME_STEP = atof(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[SAVE_TIME]")
			Scenarios[CurrentScenario].SAVE_TIME = atoi(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[SRC_X]")
			Scenarios[CurrentScenario].source.x = INPUT_VALUE_INTERPOLATION_X(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[SRC_Y]")
			Scenarios[CurrentScenario].source.y = INPUT_VALUE_INTERPOLATION_Y(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[SRC_RADIUS]")
			Scenarios[CurrentScenario].source.radius = atof(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[SRC_AMPLITUDE]")
			Scenarios[CurrentScenario].source.p_amp = atof(RValue);
		else IF_LVALUE_CMP_SAFESCENARIO("[NUM_STRUCTURES]")
		{
			Scenarios[CurrentScenario].nr_structures = atoi(RValue);
			if (Scenarios[CurrentScenario].nr_structures > MAX_STRUCTURES)
				EXIT_ERROR("[EROARE] Invalid input: [NUM_STRUCTURES] > MAX_STRUCTURES\n");
		}
		else IF_LVALUE_CMP_SAFESCENARIO("[STRUCTURE]")
		{
			CurrentStructure = atoi(RValue);
			if (CurrentStructure >= CurrentStructure)
				EXIT_ERROR("[EROARE] Invalid input: [SCENARIO] out of bounds\n");
		}
		else IF_LVALUE_CMP_SAFESCENARIO_SAFESTRUCT("[P1]")
		{
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[0][0] = INPUT_VALUE_INTERPOLATION_X(RValue);
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[0][1] = INPUT_VALUE_INTERPOLATION_Y(RValue2);
		}
		else IF_LVALUE_CMP_SAFESCENARIO_SAFESTRUCT("[P2]")
		{
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[1][0] = INPUT_VALUE_INTERPOLATION_X(RValue);
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[1][1] = INPUT_VALUE_INTERPOLATION_Y(RValue2);
		}
		else IF_LVALUE_CMP_SAFESCENARIO_SAFESTRUCT("[P3]")
		{
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[2][0] = INPUT_VALUE_INTERPOLATION_X(RValue);
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[2][1] = INPUT_VALUE_INTERPOLATION_Y(RValue2);
		}
		else IF_LVALUE_CMP_SAFESCENARIO_SAFESTRUCT("[P4]")
		{
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[3][0] = INPUT_VALUE_INTERPOLATION_X(RValue);
			Scenarios[CurrentScenario].structure[CurrentStructure].c_points[3][1] = INPUT_VALUE_INTERPOLATION_Y(RValue2);
		}
		else if (LValue) printf("[WARN]: Invalid LValue input: %s\n", LValue);
		

		#undef IF_LVALUE_CMP
		#undef IF_LVALUE_CMP_SAFESCENARIO
		#undef IF_LVALUE_CMP_SAFESCENARIO_SAFESTRUCT
	}

	fclose(InputFile);
}

// Set FileName = NULL for printf().
void ExportDataGNUPlot(const char* FileName, size_t ScenarioIndex, const struct timespec* Elapsed)
{
	if (FileName == NULL)
		return printf("Scenario [%2lu]:  Area[%d]  Time[%4.10lf]\n", ScenarioIndex, 
			          Scenarios[ScenarioIndex].nx * Scenarios[ScenarioIndex].ny,
			          (long)Elapsed.tv_sec * 1000000000 + Elapsed.tv_nsec);
		

	FILE* OutputFile = fopen(FileName, "a");
	if (OutputFile == NULL)
		EXIT_ERROR("[EROARE] Deschiderea fisierului de output esuata.\n");

	fprintf(OutputFile, "Scenario [%2lu]:  Area[%d]  Time[%4.10lf]\n", ScenarioIndex,
						Scenarios[ScenarioIndex].nx * Scenarios[ScenarioIndex].ny,
						(double)Elapsed->tv_sec + Elapsed->tv_nsec / 1000000000);

	fclose(OutputFile);
}

// Precompute identical vtk section.
// Foarte urat acest format :(
void PrepareVTKSection(size_t ScenarioIndex, void** Section, size_t* SectionSize)
{
	#define VTK_HEADER "# vtk DataFile Version 2.0\nThis is a test file for the vtk format file export\nASCII\nDATASET UNSTRUCTURED_GRID\n\n"
	#define VTK_POINTS_SUBSECTION "POINTS  double\n"	// + numar
	#define VTK_CELLS_SUBSECTION "\nCELLS  \n"			// + 2 numere
	#define VTK_CELL_TYPES_SUBSECTION "\nCELL_TYPES \n"	// + numar
	#define VTK_CELL_DATA_SUBSECTION "\nCELL_DATA \n"	// + numar
	#define VTK_SCALARS_SUBSECTION "SCALARS u FLOAT\n"
	#define VTK_LOOKUP_TABLE_SUBSECTION "LOOKUP_TABLE values_table\n"

	*SectionSize = strlen(VTK_HEADER
						  VTK_POINTS_SUBSECTION 
						  VTK_CELLS_SUBSECTION 
						  VTK_CELL_TYPES_SUBSECTION 
						  VTK_CELL_DATA_SUBSECTION 
						  VTK_SCALARS_SUBSECTION 
						  VTK_LOOKUP_TABLE_SUBSECTION);

	// VTK_POINTS_SUBSECTION
	{
		*SectionSize += IntLen((Scenarios[ScenarioIndex].nx + 1) * ((Scenarios[ScenarioIndex].ny + 1));

		// "%1.6lf" sunt numere [0, 1] deci: 1 digit, 1 . , 6 frac  --> 2*8
		// 3 numere separate de 2 ' ' si 1 '\n'						--> 3
		// ultimul numar -> dimensiunea Z dar 2D					--> 1
		*SectionSize += (2 * 8 + 3 + 1) * (Scenarios[ScenarioIndex].nx + 1) * ((Scenarios[ScenarioIndex].ny + 1);
	}

	// Nu inteleg ce reprezinta CELLS si probabil valorile sunt proaste din proiect.
	// VTK_CELLS_SUBSECTION
	{
		*SectionSize += IntLen((Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny)) +
						IntLen((Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny) * 5);

		for (int i = 0; i < Scenarios[ScenarioIndex].ny; i++)
			for (int j = 0; j < Scenarios[ScenarioIndex].nx; j++)
				*SectionSize += strlen("4    \n") + 
				                IntLen(j + i * Scenarios[ScenarioIndex].nx + i) + 
				                IntLen(j + i * Scenarios[ScenarioIndex].nx + i + 1) + 
							    IntLen(j + (i + 1) * Scenarios[ScenarioIndex].nx + i + 2) +
								IntLen(j + (i + 1) * Scenarios[ScenarioIndex].nx + i + 1);
	}

	//VTK_CELL_TYPES_SUBSECTION
	{
		*SectionSize += IntLen((Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny));
		*SectionSize += strlen("9 ") * (Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny);
		*SectionSize += 1; // '\n'
	}

	//VTK_CELL_DATA_SUBSECTION
	{
		*SectionSize += IntLen((Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny));
	}

	//VTK_LOOKUP_TABLE_SUBSECTION
	{
		// "%1.6lf" sunt numere [0, 1] deci: 1 digit, 1 . , 6 frac  --> 8
		// '\n'														--> 1
		//*SectionSize += (Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny) * (8 + 1)

		// AICI ESTE PARTEA DINAMICA SIMULATA
	}
	
	*Section = malloc(*SectionSize);
	if (*Section == NULL)
		EXIT_ERROR("[EROARE] Spatiu insuficient pentru prealocare.\n");

	char Buffer[64];
	size_t Cursor = 0;

	#define MEMCPY_STR(Str)										\
		memcpy(((char*)(*Section) + Cursor), Str, strlen(Str));	\
		Cursor += strlen(Str);

	// VTK_HEADER
	MEMCPY_STR(VTK_HEADER);

	// VTK_POINTS_SUBSECTION
	sprintf(Buffer, "POINTS %d double\n", (Scenarios[ScenarioIndex].nx + 1) * ((Scenarios[ScenarioIndex].ny + 1));
	MEMCPY_STR(Buffer);

	for (int i = 0; i < Scenarios[ScenarioIndex].ny; i++)
		for (int j = 0; j < Scenarios[ScenarioIndex].nx; j++)
		{
			sprintf(Buffer, "%1.6lf %1.6lf 0\n", (double)i / Scenarios[ScenarioIndex].ny, (double)j / Scenarios[ScenarioIndex].nx);
			MEMCPY_STR(Buffer);
		}

	// VTK_CELLS_SUBSECTION
	sprintf(Buffer, "\nCELLS %d %d\n", (Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny), 5 * (Scenarios[ScenarioIndex].nx) * ((Scenarios[ScenarioIndex].ny));
	MEMCPY_STR(Buffer);

	for (int i = 0; i < Scenarios[ScenarioIndex].ny; i++)
		for (int j = 0; j < Scenarios[ScenarioIndex].nx; j++)
		{
			sprintf(Buffer, "4 %ld %ld %ld %ld\n",
					j + i * Scenarios[ScenarioIndex].nx + i,
					j + i * Scenarios[ScenarioIndex].nx + i + 1,
					j + (i + 1) * Scenarios[ScenarioIndex].nx + i + 2,
					j + (i + 1) * Scenarios[ScenarioIndex].nx + i + 1);

			MEMCPY_STR(Buffer);
		}

	// VTK_CELL_TYPES_SUBSECTION
	sprintf(Buffer, "\nCELL_TYPES %d\n", Scenarios[ScenarioIndex].nx * Scenarios[ScenarioIndex].ny);
	MEMCPY_STR(Buffer);
	for (int i = 0; i < Scenarios[ScenarioIndex].ny; i++)
		for (int j = 0; j < Scenarios[ScenarioIndex].nx; j++)
			MEMCPY_STR("9 ");

	// VTK_CELL_DATA_SUBSECTION
	sprintf(Buffer, "\nCELL_DATA %d\n", Scenarios[ScenarioIndex].nx * Scenarios[ScenarioIndex].ny);
	MEMCPY_STR(Buffer);


	// VTK_SCALARS_SUBSECTION
	MEMCPY_STR(VTK_SCALARS_SUBSECTION);


	// VTK_LOOKUP_TABLE_SUBSECTION
	MEMCPY_STR(VTK_LOOKUP_TABLE_SUBSECTION);

	// AICI ESTE PARTEA DINAMICA SIMULATA
	/*
	for (int i = 0; i < Scenarios[ScenarioIndex].ny; i++)
		for (int j = 0; j < Scenarios[ScenarioIndex].nx; j++)
		{
			sprintf(Buffer, "1.6lf\n", uc[i][j]);
			MEMCPY_STR(Buffer);
		}
	*/


	#undef VTK_HEADER
	#undef VTK_POINTS_SUBSECTION
	#undef VTK_CELLS_SUBSECTION
	#undef VTK_CELL_TYPES_SUBSECTION
	#undef VTK_CELL_DATA_SUBSECTION
	#undef VTK_SCALARS_SUBSECTION
	#undef VTK_LOOKUP_TABLE_SUBSECTION
}

void ExportDataVTK(size_t ScenarioIndex, size_t StepIndex)
{
	if (Scenarios[ScenarioIndex].Section == NULL)
		PrepareVTKSection(ScenarioIndex, &(Scenarios[ScenarioIndex].Section), &(Scenarios[ScenarioIndex].SectionSize));

	char FileName[256] = { 0 };

	FILE* OutputFile = fopen(FileName, "w+");
	if (OutputFile == NULL)
		EXIT_ERROR("[EROARE] Deschiderea fisierului de output esuata.\n");

	fwrite(Scenarios[ScenarioIndex].Section, Scenarios[ScenarioIndex].SectionSize, 1, OutputFile);

	for (int i = 0; i < Scenarios[ScenarioIndex].ny; i++)
		for (int j = 0; j < Scenarios[ScenarioIndex].nx; j++)
		{
			if (!in_structure(i, j))
				fprintf(OutputFile, "%1.6lf\n", uc[i][j]);
			else
				fprintf(OutputFile, "%1.6lf\n", Scenarios[ScenarioIndex].source.p_amp);
		}

	fclose(OutputFile);
}