#ifndef READUTILITY_H
#define READUTILITY_H

#include <complex>
#include <fstream>
#include <sstream>
#include <vector>

#include "triangulationdata.h"
#include "simulationdata.h"
#include "simulationparams.h"

#include "featurepoint.h"
#include "triangle.h"
 

/****************************************************************************************************************/
/************************************************ Read Functions ************************************************/
FeaturePoint& getPoint(int i, int nFeatures, FeaturePoint* featurePoints, FeaturePoint* fixedPoints);

TriangulationData readFeaturePositionFile(string filename, bool rotateRef);

SimulationData* readFFT(const string& filename, TriangulationData* tri, SimulationParams* params);
/************************************************ Read Functions ************************************************/
/****************************************************************************************************************/


#endif // READUTILITY_H
