#ifndef SIMULATIONDATA_H
#define SIMULATIONDATA_H

#include <complex>
#include <vector>

#include "triangulationdata.h"

/**
 * struct: SimulationData
 * A wrapper struct to contain necessary structural information for the
 * simulation. 
 */
typedef struct {
	// frequencies of the vibration modes
    std::vector<float> frequencies;
	// triangulation data with vertices and triangle list
    TriangulationData* tri;
    // modal coordinates that changes through time during simulation
    std::vector<std::complex<float>> modalCoords;

    int nModes;
} SimulationData;

#endif // SIMULATIONDATA_H
