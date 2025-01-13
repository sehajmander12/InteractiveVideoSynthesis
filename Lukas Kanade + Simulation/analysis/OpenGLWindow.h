#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QtGui/QWindow>
#include <QOpenGLFunctions_4_1_Core>

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;

/**
 * class: OpenGLWindow
 * A Qt OpenGL program that extends QWindow and QOpenGLFunctions for ease of
 * using OpenGL functions and setting up OpenGL context/window for rendering.
 * The source code is modified from Qt 5.8's OpenGL window example at
 * http://doc.qt.io/qt-5/qtgui-openglwindow-example.html
 */ 
class OpenGLWindow : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();

    virtual void render(QPainter *painter);
    virtual void render();

    virtual void initialize();

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_update_pending;
    bool m_animating;

    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;
};


#endif // OPENGLWINDOW_H
