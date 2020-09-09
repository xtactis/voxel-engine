#ifndef GEOMETRYENGLINE_H
#define GEOMETRYENGLINE_H

#include <vector>
#include <atomic>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector3D>

#include "octree.h"

struct GeometryEngine
{
    QOpenGLBuffer arrayBuf;
    std::shared_ptr<Octree> octree;

    bool _initialized{false};
    bool paused{false};

    GeometryEngine(float size=20);
    virtual ~GeometryEngine();
    void init();

    void drawCubeGeometry(QOpenGLShaderProgram *program);
    bool initialized() const;

    void addPoints(const std::vector<QVector3D> &pts) {
        // TODO: make octree take the pts in the constructor
        for (const auto &point: pts) {
            octree->addPoint(point);
        }
        octree->loaded = true;
        qDebug() << "size:" << octree->size();
    }
    void addPoint(float x, float y, float z) {
        octree->addPoint(x, y, z);
    }
    void addPoint(const QVector3D &point) {
        octree->addPoint(point);
    }
    //void addCube(float x, float y, float z);
    //void addCube(QVector3D);
    void draw() {
        if (octree->changed()) {
            octree->createMesh(arrayBuf);
        }
        octree->draw();
    }

    void serialize(std::ofstream &outfile) {
        octree->serialize(outfile);
    }

    void initCubeGeometry();
};

#endif // GEOMETRYENGLINE_H
