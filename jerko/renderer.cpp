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

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    qDebug() << this;
    geometries.push_back(std::make_shared<GeometryEngine>());
}

Renderer::~Renderer() {
    delete program;
}

void Renderer::resize(int w, int h) {
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear = 1.0, zFar = 200.0, fov = 45.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);
}

void Renderer::reset() {
    paused = true;
    geometries.clear();
    paused = false;
}

void Renderer::addLayer(const std::vector<QVector3D> &pts, float size) {
    addLayer(size);
    geometries.back()->addPoints(pts);
}

void Renderer::addLayer(float size) {
    geometries.push_back(std::make_shared<GeometryEngine>(size));
}

void Renderer::initShaders() {
    if (!program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vert")) {
        qDebug() << "couldn't open vertex shader";
        // TODO: handle this properly bitch
    }
    if (!program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.frag"))
        qDebug() << "couldn't open fragment shader";
    if (!program->link())
        qDebug() << "couldn't link shaders";
}

void Renderer::initTextures() {
    // TODO: reimplement this, we need farina and also textures in general

    farina = new QOpenGLTexture(QImage(":/farina.png").mirrored());
    farina->setMinificationFilter(QOpenGLTexture::Nearest);
    farina->setMagnificationFilter(QOpenGLTexture::Linear);
    farina->setWrapMode(QOpenGLTexture::Repeat);
}

QMatrix4x4 Renderer::createMVmatrix() {
    QMatrix4x4 matrix;
    matrix.rotate(rotation);
    matrix.translate(position);
    matrix.rotate(thetaX, 0, 1, 0);
    matrix.rotate(thetaY, 1, 0, 0);

    return matrix;
}

QVector3D Renderer::raycast(int mouseX, int mouseY) {
    //qDebug() << "raycast";
    //qDebug() << mouseX << mouseY;
    float x = (2.0f * mouseX) / 1280 - 1.0f;
    float y = (2.0f * mouseY) / 720 - 1.0f;
    QVector4D ray_clip = {x, y, 1.0f, 1.0f};
    QVector4D ray_eye = projection.inverted() * ray_clip;
    ray_eye = {ray_eye.toVector2D(), 1.0, 0.0};
    QVector3D ray_wor = (createMVmatrix().inverted() * ray_eye).toVector3D();
    //qDebug() << ray_wor.normalized();
    return ray_wor.normalized();
}

void Renderer::paint() {
    if (paused) return;
    window->beginExternalCommands();
    program->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    farina->bind();

    QMatrix4x4 matrix = createMVmatrix();

    program->setUniformValue("mv_matrix", matrix);
    program->setUniformValue("p_matrix", projection);

    program->setUniformValue("a_color", QColor(255, 255, 255));

    for (const auto &geom: geometries) {
        geom->drawCubeGeometry(program);
    }

    program->release();

    window->resetOpenGLState();
    window->endExternalCommands();
}

void Renderer::init() {
    if (!program) {
        QSGRendererInterface *rif = window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL || rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        program = new QOpenGLShaderProgram();
        initShaders();
        initTextures();
    }
}
