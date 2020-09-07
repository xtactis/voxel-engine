#ifndef GEOMETRYENGLINE_H
#define GEOMETRYENGLINE_H

#include <vector>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector3D>

#include "octree.h"

class GeometryEngine
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();
    void init();

    void drawCubeGeometry(QOpenGLShaderProgram *program);
    bool initialized() const;

    void addPoints(const std::vector<QVector3D> &pts) {
        // TODO: make octree take the pts in the constructor
        for (const auto &point: pts) {
            octree.addPoint(point);
        }
    }
    void addPoint(float x, float y, float z) {
        octree.addPoint(x, y, z);
    }
    void addPoint(const QVector3D &point) {
        octree.addPoint(point);
    }
    //void addCube(float x, float y, float z);
    //void addCube(QVector3D);
    void draw() {
        if (octree.changed()) {
            octree.createMesh(arrayBuf);
        }
        octree.draw();
    }

private:
    void initCubeGeometry();

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
    QOpenGLBuffer colorBuf;
    Octree octree;

    bool _initialized;
};

#endif // GEOMETRYENGLINE_H
