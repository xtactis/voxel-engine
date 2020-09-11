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

    std::atomic<bool> _initialized{false};
    std::atomic<bool> paused{false};

    GeometryEngine(float size=20);
    virtual ~GeometryEngine();
    bool init();

    void drawCubeGeometry(QOpenGLShaderProgram *program);
    bool initialized() const;

    void addPoints(const std::vector<QVector3D> &pts);
    void addPoint(float x, float y, float z);
    void addPoint(const QVector3D &point);

    void serialize(std::ofstream &outfile);

    void initCubeGeometry();
};

#endif // GEOMETRYENGLINE_H
