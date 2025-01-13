#include "fileutil.h"

#include <iostream>
#include <sstream>
#include <vector>

#include <QDir>
#include <QFileInfo>
#include <QString>

#include <opencv2/plot.hpp>

#include "utility.h"

using namespace std;

FileUtil::FileUtil()
{

}

void FileUtil::saveFeaturePoints(string name, int w, int h,
                                 DelaunyTriangulation* triangulation) {
    string outputName = "tmp/" + name + "-fp.txt";
    ofstream fs(outputName);
    if (!fs) {
        cout << "Cannot open output file " << outputName << endl;
        return;
    }

    fs << w << endl;
    fs << h << endl;
    fs << *triangulation;
    fs.close();
    cout << "Saved feature points to " << outputName << endl;
}

DelaunyTriangulation* FileUtil::readFeaturePoints(string filename) {
    ifstream fs(filename);
    if (!fs) {
        throw std::runtime_error("Cannot open input file " + filename);
    }

    int w, h;
    fs >> w;
    fs >> h;

    int nFeatures;
    fs >> nFeatures;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');

    cv::Rect rect(0,0,w,h);
    DelaunyTriangulation *result = new DelaunyTriangulation(rect);

    cv::Mat featurePoints;
    for (int i = 0; i < nFeatures; ++i) {
        string line;
        if (!getline(fs, line)) {
            throw "Cannot read enough feature points from " + filename;
        }

        istringstream ss(line);
        vector<float> coords;

        while (ss) {
          string s;
          if (!getline( ss, s, ',' )) break;
          coords.push_back(atof(s.c_str()));
        }
        float point[2] = {coords[0], coords[1]};

        featurePoints.push_back(cv::Mat(1,2,CV_32F,point));
    }
    result->insertFeaturePoints(featurePoints);

    int nFixed;
    fs >> nFixed;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');

    cv::Mat fixedPoints;
    for (int i = 0; i < nFixed; ++i) {
        string line;
        if (!getline(fs, line)) {
            throw "Cannot read enough feature points from " + filename;
        }

        istringstream ss(line);
        vector<float> coords;

        while (ss) {
          string s;
          if (!getline( ss, s, ',' )) break;
          coords.push_back(atof(s.c_str()));
        }
        float point[2] = {coords[0], coords[1]};

        fixedPoints.push_back(cv::Mat(1,2,CV_32F,point));
    }
    result->insertFixedPoints(fixedPoints);

    unsigned long nTriangles;
    fs >> nTriangles;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<vector<int>> triangles = result->getTriangles();
    assert(triangles.size() == nTriangles);

    fs.close();

    return result;
}


void FileUtil::saveFlowVec(string name, cv::Mat& xFlowVec,
                           cv::Mat& yFlowVec, float fps) {
    string outputName = "tmp/" + name + "-flowvec.txt";
    ofstream fs(outputName);
    if (!fs) {
        cout << "Cannot open output file " << outputName << endl;
        return;
    }

    fs << fps << endl;
    fs << xFlowVec.rows << endl;
    fs << xFlowVec.cols << endl;

    writeCvMat(fs, xFlowVec);
    writeCvMat(fs, yFlowVec);

    fs.close();
}

double FileUtil::readFlowVec(std::string filename, cv::Mat *xFlowVec,
                             cv::Mat *yFlowVec) {

    ifstream fs(filename);
    if (!fs) {
        cout << "Cannot open input file " << filename << endl;
        return -1.0;
    }

    double fps;
    fs >> fps;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');
    int nFrames, nFeatures;
    fs >> nFrames;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');
    fs >> nFeatures;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');

    *xFlowVec = cv::Mat::zeros(nFrames, nFeatures, CV_32F);
    *yFlowVec = cv::Mat::zeros(nFrames, nFeatures, CV_32F);

    readCvMat(fs, xFlowVec, nFrames, nFeatures);
    readCvMat(fs, yFlowVec, nFrames, nFeatures);

    return fps;
}

void FileUtil::saveFFT(string name, const cv::Mat& xFFTReal, const cv::Mat& xFFTImag,
                       const cv::Mat &yFFTReal, const cv::Mat &yFFTImag,
                       const vector<double>& frequencies) {
    string outputName = "tmp/" + name + "-fft.txt";
    ofstream fs(outputName);
    if (!fs) {
        cout << "Cannot open output file " << outputName << endl;
        return;
    }

    int nFreqs = xFFTReal.rows;
    assert((int)frequencies.size() == nFreqs);
    int nFeatures = xFFTReal.cols;

    fs << nFreqs << endl;
    fs << nFeatures << endl;

    cv::Mat xMagnitudes, yMagnitudes;
    cv::Mat freqs;

    for (int i = 0; i < nFreqs; ++i) {
        fs << frequencies[i];
        freqs.push_back(frequencies[i]);
        cv::Mat xMags, yMags;
        cv::magnitude(xFFTReal.row(i), xFFTImag.row(i), xMags);
        cv::magnitude(yFFTReal.row(i), yFFTImag.row(i), yMags);
        xMagnitudes.push_back(cv::sum(xMags)[0]);
        yMagnitudes.push_back(cv::sum(yMags)[0]);
        fs << "," << cv::sum(xMags)[0] << "," << cv::sum(yMags)[0] << endl;
    }

    writeCvMat(fs, xFFTReal);
    writeCvMat(fs, xFFTImag);
    writeCvMat(fs, yFFTReal);
    writeCvMat(fs, yFFTImag);

    fs.close();
}

vector<double> FileUtil::readFFT(std::string filename,
                                 cv::Mat *xFFTReal, cv::Mat *xFFTImag,
                                 cv::Mat *yFFTReal, cv::Mat *yFFTImag,
                                 vector<double>& xMags, vector<double>& yMags) {
    vector<double> frequencies;
    ifstream fs(filename);
    if (!fs) {
        cout << "Cannot open input file " << filename << endl;
        return frequencies;
    }

    int nFreqs;
    fs >> nFreqs;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');

    int nFeatures;
    fs >> nFeatures;
    fs.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!xMags.empty()) {
        xMags.clear();
    }
    if (!yMags.empty()) {
        yMags.clear();
    }

    cv::Mat mags;
    readCvMat(fs, &mags, nFreqs, 3);
    for (int i = 0; i < nFreqs; ++i) {
        frequencies.push_back(mags.at<float>(i,0));
        xMags.push_back(mags.at<float>(i,1));
        yMags.push_back(mags.at<float>(i,2));
    }

    readCvMat(fs, xFFTReal, nFreqs, nFeatures);
    readCvMat(fs, xFFTImag, nFreqs, nFeatures);
    readCvMat(fs, yFFTReal, nFreqs, nFeatures);
    readCvMat(fs, yFFTImag, nFreqs, nFeatures);

    return frequencies;
}

void FileUtil::writeCvMat(ofstream &fs, const cv::Mat &mat) {
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols - 1; ++j) {
            fs << mat.at<float>(i,j) << ",";
        }
        fs << mat.at<float>(i, mat.cols - 1) << endl;
    }
}

void FileUtil::readCvMat(ifstream& fs, cv::Mat *mat, int nRow, int nCol) {
    *mat = cv::Mat::zeros(nRow, nCol, CV_32F);
    for (int i = 0; i < nRow; ++i) {
        string line;
        if (!getline(fs, line)) {
            throw "Cannot read enough cv mat";
        }

        istringstream ss(line);
        int count = 0;
        while (ss) {
          string s;
          if (!getline( ss, s, ',' )) break;
          mat->at<float>(i, count) = atof(s.c_str());
          ++count;
        }
        assert(count == nCol);
    }
}

bool FileUtil::fileExists(const string& path) {
    QFileInfo check_file(QString::fromStdString(path));
    if (check_file.exists() && check_file.isFile()) {
        return true;
    }
    return false;
}

bool FileUtil::dirExists(const string& path) {
    QFileInfo check_file(QString::fromStdString(path));
    if (check_file.exists() && check_file.isDir()) {
        return true;
    }
    return false;
}

void FileUtil::createDir(const string &path) {
    QDir().mkdir(QString::fromStdString(path));
}
