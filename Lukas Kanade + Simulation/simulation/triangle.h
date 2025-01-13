#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdio.h>
#include <vector>

#include "featurepoint.h"

using namespace std;
 
/**
 * Represents a triangle in the simulation, and can determine whether the mouse
 * falls within the triangle.
 * class: Triangle
 */
class Triangle {
public:
    Triangle(FeaturePoint* t1, FeaturePoint* t2, FeaturePoint* t3);

    bool inBoundingBox(float x, float y);
    vector<float> barycentric(float x, float y);
    bool contains(vector<float> barycentric);
    bool contains(float x, float y);
    bool isFixed();
    bool hasVertex(FeaturePoint fp);

    float maxX, maxY, minX, minY;

    FeaturePoint *t1, *t2, *t3;

    friend std::ostream & operator<<(std::ostream & out, Triangle & v);

private:
    void getBoundingBox();
};

#endif // TRIANGLE_H
