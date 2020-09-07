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

class Renderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Renderer();
    ~Renderer();

    void setPosition(const QVector3D &pos) { position = pos; };
    void setRotation(const QQuaternion &rot) { rotation = rot; };
    void setViewportSize(const QSize &size) { viewportSize = size; }
    void setWindow(QQuickWindow *w) { window = w; }
    void resize(int w, int h);
    void reset(){
        for (auto &geom: geometries) {
            delete geom;
        }
        geometries.clear();
    }
    void addLayer(const std::vector<QVector3D> &pts) {
        addLayer();
        geometries.back()->addPoints(pts);
    }
    void addLayer() {
        geometries.push_back(new GeometryEngine());
    }

protected:
    //void mousePressEvent(QMouseEvent *e) override;
    //void mouseReleaseEvent(QMouseEvent *e) override;
    //void timerEvent(QTimerEvent *e) override;

    //void initializeGL() override;
    //void paintGL() override;

    void initShaders();
    void initTextures();

public slots:
    void init();
    void paint();

private:
    QSize viewportSize;
    QOpenGLShaderProgram *program;
    QQuickWindow *window;
    QOpenGLTexture *farina;
    QMatrix4x4 projection;
    std::vector<GeometryEngine *> geometries; // consider using smart ptrs

    QVector3D position;
    QQuaternion rotation;
};

#endif // MAINWINDOW_H
