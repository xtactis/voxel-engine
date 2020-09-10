#ifndef MESH_H
#define MESH_H

#include <vector>

#include <QVector3D>
#include <QVector2D>
#include <QOpenGLBuffer>
#include <QDebug>

struct Mesh {
    std::vector<float> points;
    std::vector<float> colors;
    unsigned int vaoIds[1], vboIds[2];
    int _pSize = 0, _cSize = 0;

    int voxelCount = 0; // for stats only

    void addToMesh(const QVector3D &point);
    void addCubeToMesh(float x, float y, float z, float size, std::vector<bool> neighbors);

    void clearMesh();
    void createMesh(QOpenGLBuffer &arrayBuf);
};

#endif // MESH_H
