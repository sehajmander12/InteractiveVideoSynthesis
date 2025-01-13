#include "FeatureSelector.h"
#include "fileutil.h"

#include <iostream>
#include <vector>

#include <QPainter>

#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
 
FeatureSelector::FeatureSelector(const cv::Mat& frame, QWidget *parent)
    : QDialog(parent), referenceFrame(frame), featuredImage(frame), selectingFeature(false)
{
}

FeatureSelector::~FeatureSelector()
{

}

void FeatureSelector::setFrame(const cv::Mat& frame) {

    originalW = frame.cols;
    originalH = frame.rows;
    cv::Mat f = frame.clone();

    if (frame.rows > maxH) {
        cv::resize(frame, f, cv::Size(float(frame.cols * maxH) / frame.rows, maxH));
    }
    if (f.cols > maxW) {
        cv::resize(f, f, cv::Size(maxW, float(f.rows * maxW) / f.cols));
    }
    referenceFrame = f.clone();
    featuredImage = f.clone();
    w = referenceFrame.cols;
    h = referenceFrame.rows;
    resetToBoundaryPoints();

    this->resize(w, h);
    this->repaint();
}

void FeatureSelector::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    drawFeaturePoints();
    QPainter paint(this);
    QImage qimage((uchar*)featuredImage.data, w, h, featuredImage.step, QImage::Format_RGB888);
    paint.drawImage(0, 0, qimage);
    paint.end();
}

void FeatureSelector::mousePressEvent(QMouseEvent *event) {
    float point[2] = { 1.f * event->x(), 1.f * event->y() };
    if (selectingFeature) {
        if (selectedFeaturePoints.cols != 2) {
            selectedFeaturePoints = cv::Mat(1,2,CV_32F, point);
        } else {
            selectedFeaturePoints.push_back(cv::Mat(1,2,CV_32F,point));
        }

    } else {
        fixedPoints.push_back(cv::Mat(1, 2, CV_32F, point));
    }
    repaint();
}

void FeatureSelector::keyPressEvent(QKeyEvent *event) {
    if (!QString::compare(event->text(), "d", Qt::CaseSensitive)) {
        if (!selectingFeature && fixedPoints.rows > 0) {
            fixedPoints.pop_back();
            repaint();
        } else if (selectedFeaturePoints.rows > 0) {
            selectedFeaturePoints.pop_back();
            repaint();
        }
    }
}

void FeatureSelector::setFeatureCount(int count) {
    cv::Mat grayFrame;
    cvtColor(referenceFrame, grayFrame, CV_RGB2GRAY);
    goodFeaturesToTrack(grayFrame, featurePoints, count, 0.01, 10);
    repaint();
}

void FeatureSelector::drawFeaturePoints() {
    featuredImage = referenceFrame.clone();
    if (fixedPoints.rows > 0 || featurePoints.rows > 0) {
        DelaunyTriangulation* subdiv = computeTriangulation();
        vector<cv::Vec6f> triangleList;
        subdiv->getTriangleList(triangleList);

        cv::Rect rect(0, 0, w, h);

        for (size_t i = 0; i < triangleList.size(); ++i) {
            cv::Vec6f tri = triangleList[i];
            cv::Point p1 = cv::Point(cvRound(tri[0]) , cvRound(tri[1]));
            p1.x *= float(w) / originalW;
            p1.y *= float(h) / originalH;
            cv::Point p2 = cv::Point(cvRound(tri[2]), cvRound(tri[3]));
            p2.x *= float(w) / originalW;
            p2.y *= float(h) / originalH;
            cv::Point p3 = cv::Point(cvRound(tri[4]), cvRound(tri[5]));
            p3.x *= float(w) / originalW;
            p3.y *= float(h) / originalH;
//            cout << p1 << " " << p2 << " " << p3 << endl;

            if (rect.contains(p1)) {
                if (rect.contains(p2)) {
                    cv::line(featuredImage, p1, p2, cv::Scalar(255,255,255), 1, CV_AA, 0);
                }
                if (rect.contains(p3)) {
                    cv::line(featuredImage, p3, p1, cv::Scalar(255,255,255), 1, CV_AA, 0);
                }
            }
            if (rect.contains(p2) && rect.contains(p3)) {
                cv::line(featuredImage, p2, p3, cv::Scalar(255,255,255), 1, CV_AA, 0);
            }
        }
    }

    for (int i = 0; i < featurePoints.rows; ++i) {
        cv::Point p(featurePoints.at<float>(i,0), featurePoints.at<float>(i,1));
        cv::circle(featuredImage, p, 3, cv::Scalar(0, 0, 255), -1);
    }

    for (int i = 0; i < selectedFeaturePoints.rows; ++i) {
        cv::Point p(selectedFeaturePoints.at<float>(i,0), selectedFeaturePoints.at<float>(i,1));
        cv::circle(featuredImage, p, 3, cv::Scalar(0, 0, 255), -1);
    }
    for (int i = 0; i < fixedPoints.rows; ++i) {
        cv::Point p(fixedPoints.at<float>(i,0), fixedPoints.at<float>(i,1));
        cv::circle(featuredImage, p, 3, cv::Scalar(255, 0, 0), -1);
    }
}

void FeatureSelector::resetToBoundaryPoints() {
    fixedPoints = (cv::Mat_<float>(4, 2) << w - 1.f, h - 1.f, 1.f, h - 1.f, w - 1.f, 1.f, 1.f, 1.f);
}

DelaunyTriangulation* FeatureSelector::computeTriangulation() {
    cv::Rect rect(0, 0, originalW, originalH);
    DelaunyTriangulation* triangulation = new DelaunyTriangulation(rect);

    triangulation->insertFeaturePoints(featurePoints.clone() * float(originalW) / w);
    triangulation->insertFeaturePoints(selectedFeaturePoints.clone() * float(originalW) / w);
    triangulation->insertFixedPoints(fixedPoints.clone() *float(originalW) / w);

    return triangulation;
}

void FeatureSelector::setSelectFeatures(bool set) {
    selectingFeature = set;
}

void FeatureSelector::saveFeaturePoints(string text) {
    DelaunyTriangulation* trianglation = computeTriangulation();
    FileUtil::saveFeaturePoints(text, originalW, originalH, trianglation);
    delete trianglation;
}

void FeatureSelector::addFeaturePoint(const cv::Mat &fp) {
    selectedFeaturePoints.push_back(fp);
}

void FeatureSelector::addFixedPoint(const cv::Mat &fp) {
    fixedPoints.push_back(fp);
}
