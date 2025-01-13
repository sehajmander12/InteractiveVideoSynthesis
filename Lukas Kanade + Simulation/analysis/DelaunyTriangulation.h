#ifndef DELAUNYTRIANGULATION_H
#define DELAUNYTRIANGULATION_H

#include <vector>
 
#include <opencv2/imgproc/imgproc.hpp>

/**
 * class: DelaunyTriangulation
 * The class is an extension to OpenCV's Subdiv2D to add functionality for
 * creating and returning a list of triangles by vertex indices, and also
 * deal with the "points at infinity" that OpenCV's original Subdiv2D uses
 * to compute the triangulation.
 */ 
class DelaunyTriangulation : public cv::Subdiv2D
{
public:
    DelaunyTriangulation(const cv::Rect& rect);

    // find index of a vertex in the triangulation closest to the given point
    int getIndex(const cv::Point2f& pt);
    // returns whether a point is covered in the triangulated region
    bool contains(const cv::Point2f& pt) const;
    // insert vertices stored in Nx2 matrix into the triangulation, the
    // feature vs fixed difference does not really matter in the triangulation,
    // but is required down the pipeline
    void insertFeaturePoints(const cv::Mat& vtx);
    void insertFixedPoints(const cv::Mat& vtx);
    // get vertices as collection of point locations
    const std::vector<cv::Point2f>& getFeaturePoints() const;
    const std::vector<cv::Point2f>& getFixedPoints() const;
    // get dimensions of the area covered by the triangulation
    int getWidth() { return rect.width; }
    int getHeight() { return rect.height; }
    // returns a list of triangles, which are three vertex indices
    std::vector<std::vector<int>> getTriangles();

    // operator overload for writing to files
    friend std::ostream& operator<<(std::ostream& os, DelaunyTriangulation& tri);

private:
    // the region covered by the triangulation
    cv::Rect rect;
    // the vertex locations
    std::vector<cv::Point2f> featurePoints;
    std::vector<cv::Point2f> fixedPoints;
    // the memoization of the list of triangles
    std::vector<std::vector<int>> triangles;
    // used internally to track if the triangulation has been updated
    bool triangulated;
};

#endif // DELAUNYTRIANGULATION_H
