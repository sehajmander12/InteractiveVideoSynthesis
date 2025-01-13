#ifndef INTERACTIVEWINDOW_H
#define INTERACTIVEWINDOW_H

#include <vector>

#include "openglwindow.h"

#include "simulationdata.h"
#include "simulationparams.h"
#include "readutility.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QScreen>
#include <QWidget>
#include <QKeyEvent>
#include <QTime>

using namespace std;

/**
 * class: InteractiveWindow
 * A Qt OpenGL program to simulate with the given feature/fixed points and
 * modal shape information. Information are stored in SimulationParams,
 * Triangulation and SimulationData.
 */
class InteractiveWindow : public OpenGLWindow
{
//    Q_OBJECT
public:
    InteractiveWindow(const string& dataDir, const string& name, const string& method, QWindow *parent = 0);
    ~InteractiveWindow();
    virtual void initialize() Q_DECL_OVERRIDE;
    virtual void render() Q_DECL_OVERRIDE;

private:
    void readTexture(string filename);
    void damp(int elapsedTime);

    // keyboard interaction as detailed in README
    void keyPressEvent(QKeyEvent* key) Q_DECL_OVERRIDE;

    float pixelToXCoord(int x);
    float pixelToYCoord(int y);

    // when the user drags with the mouse, a red line would show the start and
    // end point of the drag and modifies modal coordinates accordingly
    void mousePressEvent(QMouseEvent* mouse) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* mouse) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* mouse) Q_DECL_OVERRIDE;
    void exciteModes();
    void exciteMode(int i);
    void drawArrow();

    GLuint m_posAttr;
    GLuint m_texAttr;
    GLuint m_clickAttr;
    GLuint m_colorAttr;
    GLuint m_dispAttr;
    GLuint m_fixedAttr;

    GLuint m_wireframe;
    GLuint m_arrow;
    GLuint m_shape;

    QOpenGLTexture* tex;

    QOpenGLShaderProgram *m_program;

    SimulationData *sim;
    TriangulationData tri;
    SimulationParams *params;

    QTime time;
    int lastTime;
};

#endif // INTERACTIVEWINDOW_H
