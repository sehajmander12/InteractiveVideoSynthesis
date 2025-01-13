#include "simulationparams.h"

SimulationParams::SimulationParams(const string& dataDir, const string& name, const string& method )
    : featurePosFile(dataDir + "/" + name + "-fp.txt"),
      fftFile(dataDir + "/" + name + method + "-selected-fft.txt"),
      referenceFrameFile(dataDir + "/frames/" + name + "1.png")
{
    selectedFreq = 0;
    composite = false;

    clickedTriangle = nullptr;
    clickedIndex = -1;
    clickedX = -1.f;
    clickedY = -1.f;
    pressed = false;
    releaseTime = 0.f;
    draggedX = 0.f;
    draggedY = 0.f;
    normalized = true;

    damping = 1.f;
    move_scale = 0.2f;
    if(method=="lk")
    {
        force_scale = 500000.f;
    }
    else if(method=="berthy")
    {
        force_scale = 50000.f;
    }
    else if(method=="raft")
    {
        force_scale = 75000.f;
    }
    else if(method=="lkberthy")
    {
        force_scale = 50000.f;
    }
    time_scale = 0.001f;

    cv::Mat frame = cv::imread(referenceFrameFile);
    rotateRef = false;
    wireframe = false;
    shape = false;
    width = frame.cols * 0.8;
    height = frame.rows * 0.8;
    aspect_ratio = height / width;
    pixel_magnitude = 1.f / height;
}
