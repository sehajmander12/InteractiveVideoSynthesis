#include "utility.h"

using namespace std;

Utility::Utility()
{

}

vector<cv::Point2f> Utility::cvMatToVectorPoint2f(const cv::Mat& points) {
    vector<cv::Point2f> ret;
    for (int i = 0; i < points.rows; ++i) {
        cv::Point2f pt(points.at<float>(i,0), points.at<float>(i,1));
        ret.push_back(pt);
    }
    return ret;
}

cv::Mat Utility::vectorPoint2fToCvMat(const vector<cv::Point2f> points) {
    cv::Mat ret;
    for (unsigned long i = 0; i < points.size(); ++i) {
        float pt[2] = { points[i].x, points[i].y };
        ret.push_back(cv::Mat(1,2,CV_32F,pt));
    }
    return ret;
}
 
