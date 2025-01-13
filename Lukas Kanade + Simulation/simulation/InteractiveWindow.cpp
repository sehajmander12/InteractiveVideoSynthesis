#include "InteractiveWindow.h"

InteractiveWindow::InteractiveWindow(const string& dataDir, const string& name, const string& method, QWindow *parent)
    : OpenGLWindow(parent), m_program(0)
{
    params = new SimulationParams(dataDir, name, method);
    tri = readFeaturePositionFile(params->featurePosFile, params->rotateRef);
    sim = readFFT(params->fftFile, &tri, params);
    if (!sim) return;

    time = QTime();

    resize(params->width, params->height);
    setAnimating(true);
    stringstream title;
    title << "Frequency: " << sim->frequencies[params->selectedFreq]<<" "<<method;
    setTitle(QString::fromStdString(title.str()));
}

InteractiveWindow::~InteractiveWindow() {
}

static const char *vertexShaderSource =
    "#version 410 core\n"
    "in vec2 position;\n"
    "in vec2 texcoord;\n"
    "in float clicked;\n"
    "in vec3 color;\n"
    "in float disp;\n"
    "in float isFixed;\n"

    "out vec2 Texcoord;\n"
    "out float Clicked;\n"
    "out vec3 Color;\n"
    "out float Disp;\n"
    "out float Fixed;\n"

    "uniform bool arrow;\n"

    "void main() {\n"
    "    Texcoord = texcoord;\n"
    "    Clicked = clicked;\n"
    "    Color = color;\n"
    "    Disp = disp;\n"
    "    Fixed = isFixed;\n"
    "    gl_Position = vec4(position, 0, 1.0);\n"
    "    if (arrow) {\n"
    "        gl_Position[2] = 0.5;\n"
    "    }\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 410 core\n"

    "in vec2 Texcoord;\n"
    "in float Clicked;\n"
    "in vec3 Color;\n"
    "in float Disp;\n"
    "in float Fixed;\n"

    "layout(location = 0) out vec4 outColor;\n"

    "uniform sampler2D wiremanTex;\n"
    "uniform bool wireframe;\n"
    "uniform bool arrow;\n"
    "uniform bool shape;\n"

    "void main() {\n"
    "    outColor = texture(wiremanTex, Texcoord) * vec4(1.0, 1.0, 1.0, 1.0);\n"
    "    outColor[3] = 1.0;\n"
    "    if (wireframe) {\n"
    "        outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "        if (Clicked > 0.9) {\n"
    "            outColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
    "        }\n"
    "    }\n"
    "    if (arrow) {\n"
    "        outColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "    } else if (shape) {\n"
    "        outColor = vec4(Color, 1.0);\n"
    "    }\n"
    "    gl_FragDepth = 0.1 / (0.11 + Disp);\n"
    "    if (arrow) gl_FragDepth = 0.0;\n"
    "    //gl_FragDepth = gl_FragCoord.z + Disp;\n"
    "    //outColor = vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, 1.0);\n"
    "}\n";


void InteractiveWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_posAttr = m_program->attributeLocation("position");
    m_texAttr = m_program->attributeLocation("texcoord");
    m_clickAttr = m_program->attributeLocation("clicked");
    m_colorAttr = m_program->attributeLocation("color");
    m_dispAttr = m_program->attributeLocation("disp");
    m_fixedAttr = m_program->attributeLocation("isFixed");

    m_wireframe = m_program->uniformLocation("wireframe");
    m_arrow = m_program->uniformLocation("arrow");
    m_shape = m_program->uniformLocation("shape");
    readTexture(params->referenceFrameFile);

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    time.start();
}

void InteractiveWindow::keyPressEvent(QKeyEvent* key) {
    stringstream title;
    switch (key->key()) {
        case Qt::Key_W:
            params->wireframe = !params->wireframe;
            break;
        case Qt::Key_S:
            params->shape = !params->shape;
            break;
        case Qt::Key_X:
            params->shapeX = true;
            break;
        case Qt::Key_Y:
            params->shapeX = false;
            break;
        case Qt::Key_Up:
            params->selectedFreq = (params->selectedFreq + 1) % sim->nModes;
            title << "Frequency: " << sim->frequencies[params->selectedFreq];
            this->setTitle(QString::fromStdString(title.str()));
            break;
        case Qt::Key_Down:
            params->selectedFreq = (params->selectedFreq - 1) % sim->nModes;
            if (params->selectedFreq < 0) {
                params->selectedFreq += sim->nModes;
            }
            title << "Frequency: " << sim->frequencies[params->selectedFreq];
            this->setTitle(QString::fromStdString(title.str()));
            break;
        case Qt::Key_E:
            for (int i = 0; i < sim->nModes; ++i) {
                sim->modalCoords[i] *= 1.5f;
            }
            break;
        case Qt::Key_R:
            for (int i = 0; i < sim->nModes; ++i) {
                sim->modalCoords[i] /= 1.5f;
            }
            break;
        case Qt::Key_A:
                params->time_scale *= 1.05f;
                break;
            case Qt::Key_Z:
            params->time_scale /= 1.05f;
            break;
        case Qt::Key_C:
            params->composite = !params->composite;
            if (params->composite) {
                this->setTitle("Composite Movements");
            } else {
                title << "Frequency: " << sim->frequencies[params->selectedFreq];
                this->setTitle(QString::fromStdString(title.str()));
            }
            break;
    case Qt::Key_M:
        params->move_scale *= 10.0f;
        cout << "Move: " << params->move_scale << endl;
        break;
    case Qt::Key_N:
        params->move_scale /= 10.0f;
        cout << "Move: " << params->move_scale << endl;
        break;
    case Qt::Key_F:
        params->force_scale *= 10.0f;
        cout << "Force: " << params->force_scale << endl;
        break;
    case Qt::Key_G:
        params->force_scale /= 10.0f;
        cout << "Force: " << params->force_scale << endl;
        break;
    case Qt::Key_D:
        if (params->damping < 1.f) params->damping *= 5.f;
        cout << "Damp: " << params->damping << endl;
        break;
    case Qt::Key_T:
        if (params->damping > 0.001f) params->damping /= 5.f;
        cout << "Damp: " << params->damping << endl;
        break;
    case Qt::Key_Delete:
        if (params->damping > 0.f) {
            params->damping = 0.f;
        } else {
            params->damping = 0.01f;
        }
        cout << "Damp: " << params->damping << endl;
        break;
    }
}

void InteractiveWindow::damp(int elapsedTime) {
       for (int i = 0; i < sim->nModes; ++i) {
        sim->modalCoords[i] *= exp(- params->damping * (elapsedTime - lastTime) * 0.001);
//        cout<<elapsedTime<<" "<<lastTime<<endl;
    }
    lastTime = elapsedTime;

}
void InteractiveWindow::exciteMode(int i) {
    complex<float> xMode(0.f, 0.f);
    complex<float> yMode(0.f, 0.f);
    float fps = 0.f;
    if (!params->clickedTriangle->t1->getFixed()) {
        xMode += params->clickedTriangle->t1->getXMode(i);
        yMode += params->clickedTriangle->t1->getYMode(i);
        fps++;
    }
    if (!params->clickedTriangle->t2->getFixed()) {
        xMode += params->clickedTriangle->t2->getXMode(i);
        yMode += params->clickedTriangle->t2->getYMode(i);
        fps++;
    }
    if (!params->clickedTriangle->t3->getFixed()) {
        xMode += params->clickedTriangle->t3->getXMode(i);
        yMode += params->clickedTriangle->t3->getYMode(i);
        fps++;
    }
    xMode /= fps;
    yMode /= fps;

    complex<float> projection = xMode * params->draggedX + yMode * params->draggedY;

    float phase = - arg(projection) + M_PI / 2.f;
    float energy = abs(projection) * params->force_scale;
    sim->modalCoords[i] = polar(energy, phase);
}

void InteractiveWindow::exciteModes() {
    for (int i = 0; i < sim->nModes; ++i) {
        exciteMode(i);
    }
}

void InteractiveWindow::drawArrow() {

    glUniform1i(m_arrow, 1);

    float points[] = {
        params->clickedX, params->clickedY, 0.f, 0.f,
        params->clickedX + params->draggedX, params->clickedY + params->draggedY, 0.f, 0.f
    };

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "setup drawing arrow" << endl;
    }

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vao);

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(m_posAttr);

    glEnableVertexAttribArray(m_texAttr);
    glVertexAttribPointer(m_texAttr, 2, GL_FLOAT, GL_FALSE,
                          4*sizeof(float), (void*)(2*sizeof(float)));


    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    if (glGetError() != GL_NO_ERROR) {
        cout << "Bound draw arrow data" << endl;
    }

    glDrawArrays(GL_LINES, 0, 2);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glUniform1i(m_arrow, 0);

    glDisable(GL_LINE_SMOOTH);
    glLineWidth(1.f);

    if (glGetError() != GL_NO_ERROR) {
        cout << "Drawing arrow to fbo" << endl;
    }
}

void InteractiveWindow::render()
{
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    if (params->wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    m_program->setUniformValue(m_wireframe, params->wireframe);
    m_program->setUniformValue(m_arrow, false);
    m_program->setUniformValue(m_shape, params->shape);

    int elapsedTime = time.elapsed();
    if (params->pressed && params->clickedTriangle->t1) {
        exciteModes();

    } else {
        damp(elapsedTime);
    }

    int nPoints = sim->tri->nFeatures + sim->tri->nFixed;
    int nF = 10;
    GLfloat vertices[nPoints *  nF];
    for (int i = 0; i < nPoints; ++i) {
        FeaturePoint& p = getPoint(i, sim->tri->nFeatures, sim->tri->featurePoints, sim->tri->fixedPoints);
        if (params->pressed) {
            p.fixPosition(sim->tri->nFeatures, sim->frequencies, sim->modalCoords,
                          sim->tri->featurePoints, sim->tri->fixedPoints,
                          params->move_scale, params->selectedFreq, params->composite);
        } else {
            p.updatePosition((elapsedTime) * 1.f * params->time_scale, sim->tri->nFeatures, sim->frequencies, sim->modalCoords,
                             sim->tri->featurePoints, sim->tri->fixedPoints,
                             params->move_scale, params->selectedFreq, params->composite);
        }
    }

    for (int i = 0; i < nPoints; ++i) {
        int index = (i + sim->tri->nFeatures) % nPoints;
        FeaturePoint& p = getPoint(index, sim->tri->nFeatures, sim->tri->featurePoints, sim->tri->fixedPoints);

        vertices[nF * index] = p.x;
        vertices[nF * index + 1] = p.y;

        vertices[nF * index + 2] = p.getU();
        vertices[nF * index + 3] = p.getV();

        if (params->clickedTriangle && params->clickedTriangle->hasVertex(p)) {
            vertices[nF * index + 4] = 1.f;
        } else {
            vertices[nF * index + 4] = 0.f;
        }

        if (params->shapeX) {
            vertices[nF * index + 5] = p.getXColor(params->selectedFreq)[0];
            vertices[nF * index + 6] = p.getXColor(params->selectedFreq)[1];
            vertices[nF * index + 7] = p.getXColor(params->selectedFreq)[2];
        } else {
            vertices[nF * index + 5] = p.getYColor(params->selectedFreq)[0];
            vertices[nF * index + 6] = p.getYColor(params->selectedFreq)[1];
            vertices[nF * index + 7] = p.getYColor(params->selectedFreq)[2];
        }

        vertices[nF * index + 8] = sqrt(pow(p.dx, 2) + pow(p.dy, 2));
        if (p.getFixed()) {
            vertices[nF * index + 9] = 1.f;
        } else {
            vertices[nF * index + 9] = 0.f;
        }
    }

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
//    glBindBuffer(GL_ARRAY_BUFFER, vao);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
//        cout << "bound vao " << err << endl;
    }
    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, nF * sizeof(float), 0);
    glVertexAttribPointer(m_texAttr, 2, GL_FLOAT, GL_FALSE, nF * sizeof(float), (void*) (2 * sizeof(float)));
    glVertexAttribPointer(m_clickAttr, 1, GL_FLOAT, GL_FALSE, nF * sizeof(float), (void*) (4 * sizeof(float)));
    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, GL_FALSE, nF * sizeof(float), (void*) (5 * sizeof(float)));
    glVertexAttribPointer(m_dispAttr, 1, GL_FLOAT, GL_FALSE, nF * sizeof(float), (void*) (8 * sizeof(float)));
    glVertexAttribPointer(m_fixedAttr, 1, GL_FLOAT, GL_FALSE, nF * sizeof(float), (void*) (9 * sizeof(float)));

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_texAttr);
    glEnableVertexAttribArray(m_clickAttr);
    glEnableVertexAttribArray(m_colorAttr);
    glEnableVertexAttribArray(m_dispAttr);
    glEnableVertexAttribArray(m_fixedAttr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "Before drawing " << err << endl;
    }

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sim->tri->nTriangles * sizeof(unsigned int) * 3,
                 &sim->tri->indexArray[0], GL_STATIC_DRAW);

    tex->bind();

    glDrawElements(GL_TRIANGLES,      // mode
                   sim->tri->nTriangles * 3,        // count
                   GL_UNSIGNED_INT,            // type
                   (void*)0           // element array buffer offset
                   );

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &vao);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        cout << "Drawing warping to fbo " << err << endl;
    }

    if (params->pressed && params->clickedTriangle->t1) {
        exciteModes();
        drawArrow();
    }
    m_program->release();
}

float InteractiveWindow::pixelToXCoord(int x) {
    return ((2.f * x) / (1.f * params->width)) - 1.f;
}

float InteractiveWindow::pixelToYCoord(int y) {
    return ((2.f * (params->height - y)) / (1.f * params->height)) - 1.f;
}

void InteractiveWindow::mousePressEvent(QMouseEvent *mouse) {
    params->clickedX = pixelToXCoord(mouse->pos().x());
    params->clickedY = pixelToYCoord(mouse->pos().y());
    int clickedI = 0;
    for (unsigned long i = 0; i < sim->tri->triangles.size(); ++i) {
        Triangle& tri = sim->tri->triangles[i];
        if (tri.inBoundingBox(params->clickedX, params->clickedY) && tri.contains(params->clickedX, params->clickedY)) {
            clickedI = i;
            break;
        }
    }
    if (sim->tri->triangles[clickedI].t1 && !sim->tri->triangles[clickedI].isFixed()) {
        params->pressed = true;
        params->clickedIndex = clickedI;
        params->clickedTriangle = &sim->tri->triangles[clickedI];
        params->draggedX = 0.f;
        params->draggedY = 0.f;
    }
}

void InteractiveWindow::mouseMoveEvent(QMouseEvent *mouse) {
    if (params->pressed) {
        params->draggedX = (pixelToXCoord(mouse->pos().x()) - params->clickedX);
        params->draggedY = (pixelToYCoord(mouse->pos().y()) - params->clickedY);
    }
}

void InteractiveWindow::mouseReleaseEvent(QMouseEvent *mouse) {
    Q_UNUSED(mouse);
    params->pressed = false;
    params->clickedX = -1.f;
    params->clickedY = -1.f;
    params->draggedX = 0.f;
    params->draggedY = 0.f;
    params->clickedTriangle = nullptr;
//    time.restart();
}

void InteractiveWindow::readTexture(string filename) {
    glEnable(GL_TEXTURE_2D);
    tex = new QOpenGLTexture(QImage(filename.c_str()));

    tex->setMinificationFilter(QOpenGLTexture::Nearest);
    tex->setMagnificationFilter(QOpenGLTexture::Linear);
    tex->setWrapMode(QOpenGLTexture::Repeat);

    if (glGetError() != GL_NO_ERROR) {
        cout << "Read texture" << endl;
    }
}
