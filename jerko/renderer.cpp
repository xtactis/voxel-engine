#include "renderer.h"

#include <QDebug>
#include <QtGui/QOpenGLContext>
#include <QtCore/QRunnable>

#include <chrono>
#include <cmath>

Renderer::Renderer() : program(0) {
    initializeOpenGLFunctions();
    glClearColor(0,0,0,.5);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST); // z-buffer test
    //glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    geometries.push_back(new GeometryEngine());
}

Renderer::~Renderer()
{
    delete program;
}

void Renderer::init()
{
    if (!program) {
        QSGRendererInterface *rif = window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        program = new QOpenGLShaderProgram();
        initShaders();
        initTextures();
    }
}

void Renderer::initShaders()
{
    // Compile vertex shader
    if (!program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vert")) {
        qDebug() << "couldn't open vertex shader";
        // TODO: handle this properly bitch
        //close();
    }

    // Compile fragment shader
    if (!program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.frag"))
        qDebug() << "couldn't open fragment shader";
        //close();

    // Link shader pipeline
    if (!program->link())
        qDebug() << "couldn't link shaders";
        //close();
}

void Renderer::initTextures()
{
    // TODO: reimplement this, we need farina and also textures in general

    // Load cube.png image
    farina = new QOpenGLTexture(QImage(":/farina.png").mirrored());

    // Set nearest filtering mode for texture minification
    farina->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    farina->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    farina->setWrapMode(QOpenGLTexture::Repeat);
}

void Renderer::resize(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 2.0, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void Renderer::paint()
{
    window->beginExternalCommands();

    program->bind();

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    farina->bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(position);
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program->setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 which contains farina.png
    //program->setUniformValue("texture", 0);
    program->setUniformValue("a_color", QColor(138, 192, 255));

    // Draw cube geometry
    for (const auto &geom: geometries) {
        if (!geom->initialized()) {
            geom->init();
        }
        geom->drawCubeGeometry(program);
    }

    program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    window->resetOpenGLState();

    window->endExternalCommands();
}
