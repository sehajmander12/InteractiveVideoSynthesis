#ifndef FEATUREPOINT_H
#define FEATUREPOINT_H

#include <complex>
#include <iostream>
#include <vector>
 
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "ComplexUtility.h"

using namespace std;

/**
 * class: FeaturePoint
 * Represents a fixed or feature point during simulation. Stores the modal
 * shapes of the vertex, its original texture coordinates as rest state, and
 * computes deformation from the rest state based on the given information.
 */
class FeaturePoint {
public:
    // default initializer
    FeaturePoint() : x(0.f), y(0.f), dx(0.f), dy(0.f), id(0), u(0.f), v(0.f), x0(0.f), y0(0.f), isFixed(false) {}
    // initialize the rest state coordinates
    FeaturePoint(float u, float v, int id, bool rotateRef) : dx(0.f), dy(0.f), id(id), u(u), v(v), isFixed(false) {
        if (rotateRef) {
            y0 = 1.f - 2.f * u;
            y = y0;
            x0 = 1.f - 2.f * v;
            x = x0;
        } else {
            x0 = 2.f * u - 1.f;
            x = x0;
            y0 = 1.f - 2.f * v;
            y = y0;
        }
    }

    // computes deformation from the rest state based on the given time and modal
    // coordinates. If the vertex is a fixed point, the deformation would be the
    // average of the neighboring feature points; if the vertex is a feature point,
    // the deformation would be the sum of the modal shapes deformations as the
    // given time and modal coordinates
    void updatePosition(float time, int nFeatures, const vector<float>& frequencies, const vector<complex<float>>& modalCoords,
                        FeaturePoint*& featurePoints, FeaturePoint*& fixedPoints, float additionalScale, int selectedFreq = -1, bool composite = false);
    void fixPosition(int nFeatures, const vector<float>& frequencies, const vector<complex<float>>& modalCoords,
                     FeaturePoint*& featurePoints, FeaturePoint*& fixedPoints, float additionalScale, int selectedFreq = -1, bool composite = false);

    // adds neighbor's index to be used during position update
    void addNeighbors(int p) { neighbors.push_back(p); }
    const vector<int>& getNeighbors() { return neighbors; }

    complex<float> getXMode(unsigned int i) {
        if (i >= xModes.size()) throw "index for getXPhase exceeds vector size";
        return xModes[i];
    }
    complex<float> getYMode(unsigned int i) {
        if (i >= yModes.size()) throw "index for getYPhase exceeds vector size";
        return yModes[i];
    }
    float getXPhase(unsigned int i) {
        if (i >= xModes.size()) throw "index for getXPhase exceeds vector size";
        return phase(xModes[i]);
    }
    float getYPhase(unsigned int i) {
        if (i >= yModes.size()) throw "index for getYPhase exceeds vector size";
        return phase(yModes[i]);
    }
    float getXMagnitude(unsigned int i) {
        if (i >= xModes.size()) throw "index for getXMagnitude exceeds vector size";
        return abs(xModes[i]);
    }
    float getYMagnitude(unsigned int i) {
        if (i >= yModes.size()) throw "index for getYMagnitude exceeds vector size";
        return abs(yModes[i]);
    }

    float getU() { return u; }
    float getV() { return v; }
    float getX0() { return x0; }
    float getY0() { return y0; }
    const vector<complex<float>>& getXModes() { return xModes; }
    void addXMode(complex<float> xp) { xModes.push_back(xp); }
    const vector<complex<float>>& getYModes() { return yModes; }
    void addYMode(complex<float> yp) { yModes.push_back(yp); }
    bool getFixed() { return isFixed; }
    void setFixed(bool f) { this->isFixed = f; }
    void setNModes(int n) { this->nModes = n; }

    // compute the vertex's modal shape color. black for fixed points.
    void computeColors(vector<float> xMaxs, vector<float> yMaxs);
    vector<float> getXColor(unsigned int i) {
        if (i >= xColors.size()) throw "index for getXColor exceeds vector size";
        return xColors[i];
    }
    vector<float> getYColor(unsigned int i) {
        if (i >= yColors.size()) throw "index for getXColor exceeds vector size";
        return yColors[i];
    }

    float x, y;
    float dx, dy;
    int id;
    int nModes;

    friend std::ostream & operator<<(std::ostream & out, FeaturePoint & v);
    vector<vector<float>> xColors, yColors;

private:
    float u, v;
    float x0, y0;
    vector<complex<float>> xModes, yModes;

    bool isFixed;
    vector<int> neighbors;

};


#endif // FEATUREPOINT_H
