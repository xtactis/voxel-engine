#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "geometryengine.h"

#include <vector>

#include <QtQuick/qquickwindow.h>
#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QVector3D>
#include <QQuaternion>

QT_BEGIN_NAMESPACE
namespace Ui { class Renderer; }
QT_END_NAMESPACE

struct Renderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    QSize viewportSize;
    QOpenGLShaderProgram *program;
    QQuickWindow *window;
    QOpenGLTexture *farina;
    QMatrix4x4 projection;
    std::vector<std::shared_ptr<GeometryEngine>> geometries;

    QVector3D position;
    QQuaternion rotation;

    bool paused{false};
    int thetaX{0}, thetaY{0};

    Renderer();
    ~Renderer();
    void setPosition(const QVector3D &pos) { position = pos; };
    void setRotation(const QQuaternion &rot) { rotation = rot; };
    void setViewportSize(const  QSize &size) { viewportSize = size; }
    void setWindow(QQuickWindow *w) { window = w; }
    void resize(int w, int h);
    void reset(){
        paused = true;
        geometries.clear();
        paused = false;
    }
    void addLayer(const std::vector<QVector3D> &pts, float size) {
        addLayer(size);
        geometries.back()->addPoints(pts);
    }
    void addLayer(float size = 20) {
        geometries.push_back(std::make_shared<GeometryEngine>(size));
    }

    void initShaders();
    void initTextures();

public slots:
    void init();
    void paint();
};

#endif // MAINWINDOW_H
