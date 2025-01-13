#ifndef MODALSHAPEDRAWER_H
#define MODALSHAPEDRAWER_H

#include <vector>
#include <complex>

#include "OpenGLWindow.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QKeyEvent>

#include <opencv2/highgui.hpp>

using namespace std;

/** 
 * class: ModalShapeDrawer
 * A Qt OpenGL program to visualize modal shapes as described in the report.
 * The program draws a plane based on the triangulation read from 
 * framesDir/NAME-fp.txt, and the color of each vertex depends on the modal
 * shape at the vertex read from framesDir/NAME-fft.txt. The resulting image
 * is saved as framesDir/xShape/NAME-x-FREQ.png for x-direction modal shapes,
 * or replaced x with y for y-direction modal shapes.
 */
class ModalShapeDrawer : public OpenGLWindow
{
public:
    ModalShapeDrawer(const string& framesDir, const string& name,
                     const std::vector<std::vector<int>>& triangles,
                     const std::vector<cv::Point2f>& positions, int nFeatures,
                     const cv::Mat& modeReal, const cv::Mat& modeImag,
                     double frequency, double max, bool isX, double w, double h, QWindow* parent = 0);
    ~ModalShapeDrawer();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

    bool rendered;

private:
    string framesDir;
    string name;
    double freq;
    bool isX;

    GLuint m_posAttr;
    GLuint m_colorAttr;
    GLuint fbo, rbo;

    float w, h;

    QOpenGLShaderProgram *m_program;

    int nTriangles, nPoints;
    unsigned int *indexArray;
    GLfloat* positions;
};

#endif // MODALSHAPEDRAWER_H
