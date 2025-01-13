#ifndef OPENGLWARPER_H
#define OPENGLWARPER_H

#include <vector>

#include "OpenGLWindow.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QKeyEvent>

#include <opencv2/highgui.hpp>

using namespace std;

/**
 * class: OpenGLWarper
 * A Qt OpenGL program to warp a reference image at framesDir/NAMEN.png, where
 * N is refFrame, to a new image based on the triangulation and positions data,
 * and saves the output image into framesDir/warped/NAME-warpedN.png, where N
 * is saveFrame.
 */ 
class OpenGLWarper : public OpenGLWindow
{
public:
    OpenGLWarper(const string& framesDir, const string& name, int refFrame, int saveFrame,
                 const std::vector<std::vector<int>>& triangles,
                 const std::vector<cv::Point2f>& texcoords,
                 const std::vector<cv::Point2f>& positions,
                 QWindow* parent = 0);
    ~OpenGLWarper();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

    void setPositions(const std::vector<cv::Point2f>& newPos) {
        positions = newPos;
        render();
    }
    void setTexcoords(const std::vector<cv::Point2f>& tex) {
        texcoords = tex;
        render();
    }

    string framesDir;
    string name;

    bool rendered;

private:
    void readTexture();
    void saveToFile();

    void drawTriangulation();

    GLuint m_posAttr;
    GLuint m_texAttr;
    GLuint m_wireBool;

    float w, h;

    QOpenGLTexture* tex;
    int refFrame;
    int saveFrame;

    QOpenGLShaderProgram *m_program;

    int nTriangles;
    unsigned int *indexArray;
    std::vector<cv::Point2f> texcoords;
    std::vector<cv::Point2f> positions;

    // controls whether to draw the triangulation wireframe on top of warped
    // image
    bool wireframe;
};

#endif // OPENGLWARPER_H
