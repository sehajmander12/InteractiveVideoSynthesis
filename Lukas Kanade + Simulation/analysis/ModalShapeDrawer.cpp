#include "ModalShapeDrawer.h"
#include "ComplexUtility.h"
#include "fileutil.h"

#include <iostream>
#include <cstdint>

using namespace std;

ModalShapeDrawer::ModalShapeDrawer(const string& framesDir, const string& name,
                                   const std::vector<std::vector<int>>& triangles,
                                   const std::vector<cv::Point2f>& positions, int nFeatures,
                                   const cv::Mat& modeReal, const cv::Mat& modeImag,
                                   double frequency, double max, bool isX,
                                   double w, double h, QWindow* parent)
    : OpenGLWindow(parent), rendered(false),
      name(name), freq(frequency), isX(isX), w(w), h(h), m_program(0)
{
 
    string dirName = "/yShape";
    if (isX) {
        dirName = "/xShape";
    }
    if (!FileUtil::endsWith(framesDir, dirName)) {
        if (!FileUtil::dirExists(framesDir + dirName)) {
            FileUtil::createDir(framesDir + dirName);
        }
        this->framesDir = framesDir + dirName;
    }

    // construct triangle index array
    nTriangles = triangles.size();
    indexArray = new unsigned int[nTriangles * 3];
    for (int i = 0; i < nTriangles; ++i) {
        indexArray[3*i] = triangles[i][0];
        indexArray[3*i + 1] = triangles[i][1];
        indexArray[3*i + 2] = triangles[i][2];
    }

    nPoints = positions.size();
    this->positions = new GLfloat[nPoints * 5];
    for (int i = 0; i < nPoints; ++i) {
        this->positions[5*i] = (positions[i].x / w) * 2 - 1;
        this->positions[5*i + 1] = (positions[i].y / h) * 2 - 1;

        if (i >= nFeatures) {
            this->positions[5*i + 2] = 0.f;
            this->positions[5*i + 3] = 0.f;
            this->positions[5*i + 4] = 0.f;

        } else {
            complex<float> c(modeReal.at<float>(i), modeImag.at<float>(i));
            vector<float> color = complexToColor(c, max);
            this->positions[5*i + 2] = color[0];
            this->positions[5*i + 3] = color[1];
            this->positions[5*i + 4] = color[2];
        }
    }

    resize(w, h);
}

ModalShapeDrawer::~ModalShapeDrawer() {
    delete indexArray;
    delete positions;
}

static const char *vertexShaderSource =
    "#version 410 core\n"
    "in vec2 position;\n"
    "in vec3 color;\n"

    "out vec3 Color;\n"

    "void main() {\n"
    "    Color = color;\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 410 core\n"
    "in vec3 Color;\n"

    "layout(location = 0) out vec4 outColor;\n"

    "void main() {\n"
    "    outColor = vec4(Color, 1.0);\n"
    "}\n";


void ModalShapeDrawer::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_posAttr = m_program->attributeLocation("position");
    m_colorAttr = m_program->attributeLocation("color");

    glGenFramebuffers(1, &fbo);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, w, h);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

}

void ModalShapeDrawer::render()
{
    glViewport(0, 0, w, h);

    glClearColor(0,0,0,1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nPoints * 5 * sizeof(GLfloat), positions, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "bound vao " << err << endl;
    }
    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2 * sizeof(float)));

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_colorAttr);
    glBufferData(GL_ARRAY_BUFFER, nPoints * 5 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "Before drawing " << err << endl;
    }

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 nTriangles * sizeof(unsigned int) * 3,
                 indexArray, GL_STATIC_DRAW);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawElements(GL_TRIANGLES,      // mode
                   nTriangles * 3,        // count
                   GL_UNSIGNED_INT,            // type
                   (void*)0           // element array buffer offset
                   );

    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &vao);
    glBlitFramebuffer(0,0,w,h,0,0,w,h,GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "Drawing warping to fbo " << err << endl;
    }

    m_program->release();

    if (!rendered) {
        //after drawing
        GLubyte* data = new GLubyte[int(w*h*3)];
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE,data);

        err = glGetError();
        if (err != GL_NO_ERROR) {
            cout << "Error reading pixels " << err << endl;
        }

        cv::Mat image = cv::Mat(h, w, CV_8UC3, data);
        stringstream output;
        if (isX) {
            output << framesDir << "/" << name << "-x-" << freq << ".png";
        } else {
            output << framesDir << "/" << name << "-y-" << freq << ".png";
        }
        cv::imwrite(output.str(), image);
        delete[] data;
        // Return to onscreen rendering:
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);

        rendered = true;
    }
}
