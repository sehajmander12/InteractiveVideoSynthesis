#include "ModalShapeProducer.h"

#include <ctime>
#include <iostream>
#include <vector>

#include <QVBoxLayout>
#include <QDir>

#include "DelaunyTriangulation.h"
#include "ModalShapeDrawer.h"

using namespace std;

ModalShapeProducer::ModalShapeProducer(FrequencySelector* freq, QWidget *parent)
    : QWidget(parent), freqSelector(freq), name("wireman"), framesDir("tmp")
{
    progress = new QLabel("Producing modal shapes");
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(progress);
    this->setLayout(layout);
}
 
void ModalShapeProducer::drawWarpedFrames() {
    string flowvecFile = framesDir + "/" + name + "-flowvec.txt";
    cv::Mat xFlowvec, yFlowvec;
    double fps = FileUtil::readFlowVec(flowvecFile, &xFlowvec, &yFlowvec);
    if (fps < 0.0) {
        throw "FPS read from flow vec file " + flowvecFile + " is negative";
    }
    int nFrames = xFlowvec.rows;

    // initialize the warper
    DelaunyTriangulation *triangulation = FileUtil::readFeaturePoints("tmp/" + name + "-fp.txt");
    vector<cv::Point2f> texcoords;
    vector<cv::Point2f> positions;
    for (auto p : triangulation->getFeaturePoints()) {
        texcoords.push_back(p);
        positions.push_back(p);
    }
    for (auto p : triangulation->getFixedPoints()) {
        texcoords.push_back(p);
        positions.push_back(p);
    }
    for (int i = 0; i < nFrames; ++i) {
        for (int j = 0; j < xFlowvec.cols; ++j) {
            texcoords[j].x += xFlowvec.at<float>(i,j);
            texcoords[j].y += yFlowvec.at<float>(i,j);
        }
        OpenGLWarper* warper = new OpenGLWarper(
                    framesDir + "/frames", name, i+1, i+1,
                    triangulation->getTriangles(),
                    texcoords, texcoords);
        warper->show();

        while(!warper->rendered) {}
        warper->close();
        delete warper;
    }
}

void ModalShapeProducer::drawModalShapes() {
    string fftFile = framesDir + "/" + name + "-fft.txt";
    if (!FileUtil::fileExists(fftFile)) return;

    cv::Mat xFFTReal, xFFTImag, yFFTReal, yFFTImag;
    vector<double> xMags, yMags;
    vector<double> frequencies = FileUtil::readFFT(fftFile,
                                             &xFFTReal, &xFFTImag,
                                             &yFFTReal, &yFFTImag,
                                             xMags, yMags);

    // initialize the warper
    DelaunyTriangulation *triangulation = FileUtil::readFeaturePoints("tmp/" + name + "-fp.txt");
    vector<cv::Point2f> texcoords;
    vector<cv::Point2f> positions;
    for (auto p : triangulation->getFeaturePoints()) {
        texcoords.push_back(p);
        positions.push_back(p);
    }
    for (auto p : triangulation->getFixedPoints()) {
        texcoords.push_back(p);
        positions.push_back(p);
    }

    for (unsigned long i = 0; i < frequencies.size(); ++i) {
        float xMax = -1.f;
        float yMax = -1.f;
        for (unsigned long j = 0; j < triangulation->getFeaturePoints().size(); ++j) {
            float xMag = sqrt(pow(xFFTReal.at<float>(i,j),2) + pow(xFFTImag.at<float>(i,j),2));
            float yMag = sqrt(pow(yFFTReal.at<float>(i,j),2) + pow(yFFTImag.at<float>(i,j),2));
            if (xMax < xMag) xMax = xMag;
            if (yMax < yMag) yMax = yMag;
        }

        stringstream ss;
        ss << "Frequency " << frequencies[i];

        ModalShapeDrawer* xDrawer = new ModalShapeDrawer(
                framesDir, name, triangulation->getTriangles(),
                positions, triangulation->getFeaturePoints().size(),
                xFFTReal.row(i), xFFTImag.row(i),
                frequencies[i], xMax, true,
                triangulation->getWidth(), triangulation->getHeight());
        xDrawer->setTitle(QString::fromStdString("X-shape at " + ss.str()));
        xDrawer->show();

        while(!xDrawer->rendered) {}
        xDrawer->close();
        delete xDrawer;

        ModalShapeDrawer* yDrawer = new ModalShapeDrawer(
                framesDir, name, triangulation->getTriangles(),
                positions, triangulation->getFeaturePoints().size(),
                yFFTReal.row(i), yFFTImag.row(i),
                frequencies[i], yMax, false,
                triangulation->getWidth(), triangulation->getHeight());
        yDrawer->setTitle(QString::fromStdString("Y-shape at " + ss.str()));
        yDrawer->show();

        while(!yDrawer->rendered) {}
        yDrawer->close();
        delete yDrawer;
    }
}

void ModalShapeProducer::computeFFT() {
    cv::Mat xFlowvec, yFlowvec;
    double fps = FileUtil::readFlowVec(framesDir + "/" + name + "-flowvec.txt", &xFlowvec, &yFlowvec);
    if (fps < 0.0) {
        throw "FPS read from flow vec file " + framesDir + "/" + name + "-flowvec.txt negative";
    }

    int nFrames = xFlowvec.rows;
    vector<double> frequencies(nFrames / 2);
    for (int i = 0; i < nFrames / 2; ++i) {
        frequencies[i] = fps / nFrames * i;
    }

    xFlowvec = xFlowvec.t();
    yFlowvec = yFlowvec.t();

    cv::Mat xFFT, yFFT;

    cv::Mat xFFTs[2];
    cv::Mat yFFTs[2];

    cv::dft(xFlowvec, xFFT, cv::DFT_ROWS + cv::DFT_COMPLEX_OUTPUT);
    cv::dft(yFlowvec, yFFT, cv::DFT_ROWS + cv::DFT_COMPLEX_OUTPUT);

    cv::split(xFFT, xFFTs);
    cv::split(yFFT, yFFTs);

    cv::Range rR = cv::Range::all();
    cv::Range cR(0, nFrames / 2);
    FileUtil::saveFFT(name, xFFTs[0](rR, cR).t(), xFFTs[1](rR, cR).t(), yFFTs[0](rR, cR).t(), yFFTs[1](rR, cR).t(), frequencies);
}

void ModalShapeProducer::endSession() {
    freqSelector->setName(name);
    freqSelector->setDataDir(framesDir);
    freqSelector->init();
    freqSelector->show();

    this->close();
}
