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

    QMatrix4x4 createMVmatrix() {
        QMatrix4x4 matrix;
        matrix.rotate(rotation);
        matrix.translate(position);
        matrix.rotate(thetaX, 0, 1, 0);
        matrix.rotate(thetaY, 1, 0, 0);

        return matrix;
    }

    QVector3D raycast(int mouseX, int mouseY) {
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

public slots:
    void init();
    void paint();
};

#endif // MAINWINDOW_H
