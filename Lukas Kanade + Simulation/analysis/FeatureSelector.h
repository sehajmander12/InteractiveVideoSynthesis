#ifndef FEATURESELECTOR_H
#define FEATURESELECTOR_H

#include <string>
#include <vector>

#include <QDialog>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QVector2D>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "DelaunyTriangulation.h"

using namespace std;

/**
 * class: FeatureSelector
 * The image component of the feature detection part of the pipeline, performs
 * the triangulation and allows user to click on the image to select points.
 */
class FeatureSelector : public QDialog
{
    Q_OBJECT

public:
    FeatureSelector(const cv::Mat& frame = cv::imread("tmp/wireman1.png"), QWidget *parent = 0);
    ~FeatureSelector();

    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent* event);

    void setFeatureCount(int count);
    void saveFeaturePoints(string name);

    void setFrame(const cv::Mat& frame);

    void addFeaturePoint(const cv::Mat& fp);
    void addFixedPoint(const cv::Mat& fp);
    void resetToBoundaryPoints();

    const int maxW = 1280;
    const int maxH = 720;

public slots:
    //  controls whether the selected points are features or fixed points
    void setSelectFeatures(bool set);

private:
    // draw functions for the triangulation on top of the reference frame
    void drawFeaturePoints();
    // computes the underlying Delauny Triangulation
    DelaunyTriangulation* computeTriangulation();

    // reference frame
    cv::Mat referenceFrame;
    // reference frame with the triangulation drawn on top
    cv::Mat featuredImage;

    int w, h;
    int originalW, originalH;

    // Nx2 matrices storing point locations
    cv::Mat featurePoints;
    cv::Mat selectedFeaturePoints;
    cv::Mat fixedPoints;

    bool selectingFeature;
};

#endif // FEATURESELECTOR_H
