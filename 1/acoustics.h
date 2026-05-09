#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#define EXIT_ERROR(msg) 		\
{								\
	fprintf(stderr,msg);		\
	fflush(stderr);				\
	exit(EXIT_FAILURE);			\
}

// Numarul maxim admis de scenarii si numarul maxim admis de structuri pentru domeniul unui scenariu
#define MAX_SCENARIOS		16
#define MAX_STRUCTURES		16


// Indicatori pentru laturile structurilor dreptunghiulare
#define N_EDGE			1
#define W_EDGE			2
#define S_EDGE			3
#define E_EDGE			4
#define NW_CORNER		5
#define NE_CORNER		6
#define SE_CORNER		7
#define SW_CORNER		8

typedef struct
{
	int x;			// linia sursei acustice
	int y;			// coloana sursei acustice
	int radius;		// raza sursei sferice
	double p_amp;		// valoarea pulsatiei sursei acustice
} source_t;

typedef struct {int c_points[4][2];} structure_t;

typedef struct
{
	int OMP_THREADS;	// numarul de thread-uri OMP (default 4)
	int SAVE_TIME;		// numarul de pasi de discretizare dupa care trebuie salvate rezultatele

	int nx;				// numarul de linii ale domeniului
	int ny;				// numarul de coloane ale domeniului
	
	int nr_struct;							// numarul de structuri din domeniu
	structure_t structure[MAX_STRUCTURES];	// structurile din domeniu

	source_t source;			// sursa acustica
	double H;					// pasul de discretizare spatiala
	double MAX_TIME;			// timpul maxim pentru rulare (in secunde)
	double TIME_STEP;			// pasul de discretizare temporala

	// VTK I/O optimization
	void* Section;
	size_t SectionSize;
} scenario_t;


// Scenariile
extern size_t ScenarioCount;
extern scenario_t Scenarios[MAX_SCENARIOS];

// File I/O
void ImportData(const char* FileName);
void ExportDataVTK(size_t ScenarioIndex, size_t StepIndex);
void ExportDataGNUPlot(const char* FileName, size_t ScenarioIndex, const struct timespec* Elapsed);

