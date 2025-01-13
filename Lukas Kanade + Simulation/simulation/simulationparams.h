#ifndef SIMULATIONPARAMS_H
#define SIMULATIONPARAMS_H

#include "triangle.h"
#include <opencv2/highgui/highgui.hpp>

/**
 * struct: SimulationParams
 * A wrapper struct to contain necessary parameters for the simulation.
 */
typedef struct SimulationParams {
    // drawing related params - dimensions and whether to draw as rotated
    bool rotateRef;
    float aspect_ratio;
    float pixel_magnitude;
    float width;
    float height;

    // drawing related params - drawing wireframe or modal shape viz
    bool wireframe;
    bool shape;
    bool shapeX;

    // whether all modes are normalized to have the same total magnitude
    // when reading modal shapes from file
    bool normalized;

    // interaction related params changed in mouse events
    bool pressed;
    float releaseTime;
    float clickedX, clickedY;
    float draggedX, draggedY;
    Triangle* clickedTriangle;
    int clickedIndex;

    // simulation related params for increasing/decreasing the spatial and
    // temporal scales of the simulation
    float damping;
    float move_scale;
    float force_scale;
    float time_scale;

    // housekeeping params for reading information from files
    string featurePosFile;
    string fftFile;
    string referenceFrameFile;

    // the frequency currently being simulated
    int selectedFreq;
    // whether the simulation is adding up all modes' deformations or just
    // the selected frequency
    bool composite;

    SimulationParams(const string& dataDir, const string& name, const string& method);
} SimulationParams;

#endif // SIMULATIONPARAMS_H
