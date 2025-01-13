#include "DelaunyTriangulation.h"

#include <iostream>
 
using namespace std;

DelaunyTriangulation::DelaunyTriangulation(const cv::Rect& rect) : cv::Subdiv2D(rect), rect(rect), triangulated(false)
{
    featurePoints.clear();
    fixedPoints.clear();
    triangles.clear();
}

int DelaunyTriangulation::getIndex(const cv::Point2f& pt) {
    if (!rect.contains(pt)) return -1;
    int edge, vertex;
    int ret = locate(pt, edge, vertex);
    if (ret != cv::Subdiv2D::PTLOC_VERTEX) return -1;

    float epsilon = 0.0001f;
    for (unsigned long i = 0; i < featurePoints.size(); ++i) {
        if (pow(pt.x - featurePoints[i].x, 2) + pow(pt.y - featurePoints[i].y, 2) < epsilon) {
            return i;
        }
    }
    for (unsigned long i = 0; i < fixedPoints.size(); ++i) {
        if (pow(pt.x - fixedPoints[i].x, 2) + pow(pt.y - fixedPoints[i].y, 2) < epsilon) {
            return i + featurePoints.size();
        }
    }
    return -1;
}

bool DelaunyTriangulation::contains(const cv::Point2f& pt) const {
    return rect.contains(pt);
}


void DelaunyTriangulation::insertFeaturePoints(const cv::Mat& pts) {
    for (int i = 0; i < pts.rows; ++i) {
        cv::Point2f pt(pts.at<float>(i,0), pts.at<float>(i,1));
        this->insert(pt);
        featurePoints.push_back(pt);
    }
    triangulated = false;
}

void DelaunyTriangulation::insertFixedPoints(const cv::Mat& pts) {
    for (int i = 0; i < pts.rows; ++i) {
        cv::Point2f pt(pts.at<float>(i,0), pts.at<float>(i,1));
        this->insert(pt);
        fixedPoints.push_back(pt);
    }
    triangulated = false;
}

const vector<cv::Point2f>& DelaunyTriangulation::getFeaturePoints() const {
    return featurePoints;
}

const vector<cv::Point2f>& DelaunyTriangulation::getFixedPoints() const {
    return fixedPoints;
}

std::vector<std::vector<int>> DelaunyTriangulation::getTriangles() {
    if (triangulated) return triangles;

    vector<cv::Vec6f> tri;
    getTriangleList(tri);
    triangles.clear();

    for (unsigned long i = 0; i < tri.size(); ++i) {
        cv::Point2f v1(tri[i][0], tri[i][1]);
        if (!contains(v1)) continue;
        int i1 = getIndex(v1);
        if (i1 == -1) continue;

        cv::Point2f v2(tri[i][2], tri[i][3]);
        if (!contains(v2)) continue;
        int i2 = getIndex(v2);
        if (i2 == -1) continue;

        cv::Point2f v3(tri[i][4], tri[i][5]);
        if (!contains(v3)) continue;
        int i3 = getIndex(v3);
        if (i3 == -1) continue;

        vector<int> triangle;
        triangle.push_back(i1);
        triangle.push_back(i2);
        triangle.push_back(i3);

        triangles.push_back(triangle);
    }
    triangulated = true;
    return triangles;
}

ostream& operator<<(ostream& os, DelaunyTriangulation& tri) {
    const vector<cv::Point2f>& featurePoints = tri.getFeaturePoints();
    const vector<cv::Point2f>& fixedPoints = tri.getFixedPoints();

    os << featurePoints.size() << endl;
    for (unsigned long i = 0; i < featurePoints.size(); ++i) {
        os << featurePoints[i].x << "," << featurePoints.at(i).y << endl;
    }
    os << fixedPoints.size() << endl;
    for (unsigned long i = 0; i < fixedPoints.size(); ++i) {
        os << fixedPoints[i].x << "," << fixedPoints.at(i).y << endl;
    }

    vector<vector<int>> triangles = tri.getTriangles();
    os << triangles.size() << endl;
    for (unsigned long i = 0; i < triangles.size(); ++i) {
        os << triangles[i][0] << "," << triangles[i][1] << "," << triangles[i][2] << endl;
    }
    return os;
}
