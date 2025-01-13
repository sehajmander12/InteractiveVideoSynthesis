#ifndef TRIANGULATIONDATA_H
#define TRIANGULATIONDATA_H

#include "featurepoint.h"
#include "triangle.h"

/**
 * struct: TriangulationData
 * A wrapper struct to contain necessary structural information for the
 * triangulation. Stores arrays of the feature and fixed vertices, and
 * an index array for the triangles, and also store other parameters
 * for the vertices and dimensions of the arrays.
 */ 
typedef struct {
    FeaturePoint* featurePoints;
    FeaturePoint* fixedPoints;
    unsigned int* indexArray;

    vector<Triangle> triangles;

    int nFeatures;
    int nFixed;
    int nTriangles;

    int tex_width;
    int tex_height;
} TriangulationData;

#endif // TRIANGULATIONDATA_H
