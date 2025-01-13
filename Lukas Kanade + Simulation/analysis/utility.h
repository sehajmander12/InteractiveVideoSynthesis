#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <opencv2/core/core.hpp>

class Utility
{
public:
    Utility();

    // utility functions to transform between Nx2 cv matrix and vector of Point2f
    static std::vector<cv::Point2f> cvMatToVectorPoint2f(const cv::Mat& points);
    static cv::Mat vectorPoint2fToCvMat(const std::vector<cv::Point2f> points);
};

#endif // UTILITY_H
 
