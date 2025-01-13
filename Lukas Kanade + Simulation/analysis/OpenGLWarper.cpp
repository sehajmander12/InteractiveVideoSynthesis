#include "OpenGLWarper.h"
#include "fileutil.h"

#include <iostream>
#include <cstdint>

using namespace std;

OpenGLWarper::OpenGLWarper(const string& framesDir, const string& name, int refFrame, int saveFrame,
                           const std::vector<std::vector<int>>& triangles,
                           const std::vector<cv::Point2f>& texcoords,
                           const std::vector<cv::Point2f>& positions,
                           QWindow* parent)
    : OpenGLWindow(parent), framesDir(framesDir), name(name), rendered(false),
      refFrame(refFrame), saveFrame(saveFrame), m_program(0), texcoords(texcoords), positions(positions),
      wireframe(true)
{
    string dirName = "/warped";
    if (!FileUtil::endsWith(framesDir, dirName)) {
        if (!FileUtil::dirExists(framesDir + dirName)) {
            FileUtil::createDir(framesDir + dirName);
        }
    }

    // construct triangle index array
    nTriangles = triangles.size();
    indexArray = new unsigned int[nTriangles * 3];
    for (int i = 0; i < nTriangles; ++i) {
        indexArray[3*i] = triangles[i][0];
        indexArray[3*i + 1] = triangles[i][1];
        indexArray[3*i + 2] = triangles[i][2];
    }
}

OpenGLWarper::~OpenGLWarper() {
    tex->destroy();
    delete indexArray;
}

static const char *vertexShaderSource =
    "#version 410 core\n"
    "in vec2 position;\n"
    "in vec2 texcoord;\n"

    "out vec2 Texcoord;\n"
    "uniform bool wireframe;\n"

    "void main() {\n"
    "    Texcoord = texcoord;\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 410 core\n"

    "in vec2 Texcoord;\n"
    "uniform sampler2D tex;\n"
    "uniform bool wireframe;\n"

    "layout(location = 0) out vec4 outColor;\n"

    "void main() {\n"
    "    if (wireframe) {\n"
    "        outColor = vec4(1,1,1,1.0);\n"
    "    } else {\n"
    "        outColor = texture(tex, Texcoord);\n"
    "    }\n"
    "}\n";


void OpenGLWarper::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_posAttr = m_program->attributeLocation("position");
    m_texAttr = m_program->attributeLocation("texcoord");
    m_wireBool = m_program->uniformLocation("wireframe");

    readTexture();
}

void OpenGLWarper::render()
{
    glViewport(0, 0, w, h);

    glClearColor(0,0,0,1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();
    if (wireframe) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_program->setUniformValue(m_wireBool, true);
        drawTriangulation();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_program->setUniformValue(m_wireBool, false);
    drawTriangulation();
    m_program->release();

    saveToFile();
}

void OpenGLWarper::readTexture() {
    glEnable(GL_TEXTURE_2D);
    stringstream ss;
    ss << framesDir << "/" << name << refFrame << ".png";
    QImage ref(QString::fromStdString(ss.str()));
    tex = new QOpenGLTexture(ref.mirrored());

    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::Repeat);

    if (glGetError() != GL_NO_ERROR) {
        cout << "Read texture " << saveFrame << endl;
    }

    w = tex->width();
    h = tex->height();

    resize(w, h);
}

void OpenGLWarper::saveToFile() {
    //after drawing
    vector<uint8_t> data(w*h*3, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE,&data[0]);

    cv::Mat image = cv::Mat(h, w, CV_8UC3, &data[0]);
    stringstream output;
    if (!wireframe) {
        output << framesDir << "/warped/" << name << "-warped" << saveFrame << ".png";
    } else {
        output << framesDir << "/warped/" << name << "-warped-wire" << saveFrame << ".png";
    }
    cv::imwrite(output.str(), image);
    // Return to onscreen rendering:
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

    rendered = true;
}

void OpenGLWarper::drawTriangulation() {
    int nPoints = texcoords.size();
    int nF = 4;
    GLfloat vertices[nPoints *  nF];
    for (int i = 0; i < nPoints; ++i) {
        vertices[nF * i] = (positions[i].x / w) * 2 - 1;
        vertices[nF * i + 1] = (positions[i].y / h) * 2 - 1;

        vertices[nF * i + 2] = texcoords[i].x / w;
        vertices[nF * i + 3] = 1.0 - texcoords[i].y / h;
    }

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "bound vao " << err << " at frame " << saveFrame << endl;
    }
    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, nF * sizeof(float), 0);
    glVertexAttribPointer(m_texAttr, 2, GL_FLOAT, GL_FALSE, nF * sizeof(float), (void*) (2 * sizeof(float)));

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_texAttr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

    tex->bind();
    glDrawElements(GL_TRIANGLES,      // mode
                   nTriangles * 3,        // count
                   GL_UNSIGNED_INT,            // type
                   (void*)0           // element array buffer offset
                   );

    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "Drawing warping to fbo " << err << endl;
    }
}
