#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "renderer.h"
#include "geometryengine.h"
#include "octree.h"

#include <vector>
#include <fstream>
#include <cmath>

#include <QQuickItem>
#include <QVector3D>
#include <QQuaternion>
#include <QtQuick/QQuickView>

struct Header{
    unsigned long long size;
    int thetaX, thetaY;
    double pos[3];
};

class Geometry : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
private:
    Renderer *renderer;

    QVector3D m_position = {0, 0, -10};
    QQuaternion m_rotation;

    int mouseStartX, mouseStartY;
    int mouseEndX, mouseEndY;

    int selectX1, selectY1;

    QString filename;
    bool _canSave{false};

public:
    static QQuickView *view;

public:
    Geometry();

    QVector3D position() const { return m_position; }
    void setPosition(const QVector3D &position);

    QQuaternion rotation() const { return m_rotation; }
    void setRotation(const QQuaternion &rotation);

    Q_INVOKABLE void setSave(const QString &filename) {
        this->filename = filename;
        _canSave = true;
    }

    Q_INVOKABLE bool unsaved() const {return false;}
    Q_INVOKABLE bool canSave() const {return _canSave;}
    Q_INVOKABLE void move(float x, float y, float z) {
        m_position += {x, y, z};
        qDebug() << m_position;
        if (renderer) renderer->setPosition(m_position);
    }

    Q_INVOKABLE void pan(int x, int y) {
        // TODO: nicer panning that actually looks at the scene
        const QVector2D delta(x-mouseStartX, mouseStartY-y);
        setPosition(m_position+delta/32);
        mouseStartX = x;
        mouseStartY = y;
    }

    Q_INVOKABLE void setMouse(int x, int y) {
        mouseStartX = x;
        mouseStartY = y;
    }

    Q_INVOKABLE void mouseMove(int x, int y) {
        if (renderer) {
            renderer->thetaX += mouseEndX-mouseStartX;
            renderer->thetaY += mouseEndY-mouseStartY;
            renderer->thetaX %= 360;
            renderer->thetaY %= 360;
        }
        mouseStartX = mouseEndX;
        mouseStartY = mouseEndY;
        mouseEndX = x;
        mouseEndY = y;
    }

    Q_INVOKABLE void select(int x, int y) {
        const auto ray = renderer->raycast(x, y);

        return;
        // stopping here because selection doesn't work
        // FIX THIS

        for (auto &geom: renderer->geometries) {
            geom->octree->selectOne(ray);
        }
        selectX1 = x;
        selectY1 = y;
    }

    Q_INVOKABLE void selectBox(int x, int y) {
        int x1 = std::min(x, selectX1),
            y1 = std::min(y, selectY1),
            x2 = std::max(x, selectX1),
            y2 = std::max(y, selectY1);
        const auto b0 = renderer->raycast(x1, y1);
        const auto b1 = renderer->raycast(x2, y1);
        const auto b2 = renderer->raycast(x2, y2);
        const auto b3 = renderer->raycast(x1, y2);
        const auto n0 = QVector3D::crossProduct(b1, b0).normalized();
        //qDebug() << "n0:" << n0;
        const auto n1 = QVector3D::crossProduct(b1, b2).normalized();
        //qDebug() << "n1:" << n1;
        const auto n2 = QVector3D::crossProduct(b3, b2).normalized();
        //qDebug() << "n2:" << n2;
        const auto n3 = QVector3D::crossProduct(b3, b0).normalized();
        //qDebug() << "n3:" << n3;
        //const auto n4 = renderer->raycast(1280/2, 720/2);

        return;
        // stopping here because selection doesn't work
        // FIX THIS

        std::vector<QVector3D> selected;
        for (auto &geom: renderer->geometries) {
            geom->octree->select({n0, n1, n2, n3}, selected);
        }
        qDebug() << selected.size();
    }

public slots:
    void sync();
    void cleanup();

    void newProject();
    void open(const QString &filename);
    void save();

    void importModel(const QString &filename);
    void exportModel(const QString &){}

    void undo(){}
    void redo(){}
    void cut(){}
    void copy(){}
    void paste(){}

    std::vector<std::array<double, 3>> readPLY(const QString &filename);

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;
    std::vector<std::array<double, 3>> loadPoints(const QString &filename);

signals:
    void positionChanged();
    void rotationChanged();
};

#endif // GEOMETRY_H

