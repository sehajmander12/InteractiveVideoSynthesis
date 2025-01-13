#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <fstream>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "DelaunyTriangulation.h"

/**
 * class: FileUtil
 * Utility class for reading and saving generated data files.
 */ 
class FileUtil
{
public:
    FileUtil();

    // reading and saving feature pont file as NAME-fp.txt, includes
    // dimensions, point locations (separating feature and fixed point
    // for the pipeline) and triangle list
    static void saveFeaturePoints(std::string name, int w, int h,
                                  DelaunyTriangulation* triangulation);
    static DelaunyTriangulation* readFeaturePoints(std::string filename);

    // reading and saving optical flow vector as NAME-flowvec.txt, includes
    // the frame-per-second of the video and the vectors
    static void saveFlowVec(std::string name, cv::Mat &xFlowVec,
                            cv::Mat &yFlowVec, float fps = 60.0);
    static double readFlowVec(std::string filename, cv::Mat *xFlowVec,
                              cv::Mat *yFlowVec);

    // reading and saving FFTs, or modal shapes, of every feature points as
    // NAME-fft.txt, includes the frequencies, the total magnitudes of x and y
    // modal shapes, and individual modal shape complex values
    static void saveFFT(std::string name, const cv::Mat& xFFTReal, const cv::Mat& xFFTImag,
                        const cv::Mat& yFFTReal, const cv::Mat& yFFTImag,
                        const std::vector<double>& frequencies);
    static std::vector<double> readFFT(std::string filename,
                                       cv::Mat *xFFTReal, cv::Mat *xFFTImag,
                                       cv::Mat *yFFTReal, cv::Mat *yFFTImag,
                                       std::vector<double>& xMags, std::vector<double>& yMags);

    // helper function to read and write a comma separated matrix from a file
    static void writeCvMat(std::ofstream& fs, const cv::Mat& mat);
    static void readCvMat(std::ifstream& fs, cv::Mat *mat, int nRow, int nCol);

    // helper functions that uses QDir to do filesystem checks and mkdir
    static bool fileExists(const std::string& path);
    static bool dirExists(const std::string& path);
    static void createDir(const std::string& path);

    // helper function to check if a string ends in a suffix
    static bool endsWith(const std::string& str, const std::string& suffix) {
        return std::equal(str.cbegin() + str.size() - suffix.size(), str.cend(), suffix.cbegin());
    }
};

#endif // FILEUTIL_H
