#include "mesh.h"

void Mesh::addToMesh(const QVector3D &point) {
    points.push_back(point[0]);
    points.push_back(point[1]);
    points.push_back(point[2]);
    //colors.push_back((point[2]-minz)/(maxz-minz));
    //colors.push_back((point[2]-minz)/(maxz-minz));
    //colors.push_back((point[2]-minz)/(maxz-minz));
    _pSize = points.size();
    _cSize = colors.size();
}

void Mesh::addCubeToMesh(float x, float y, float z, float size, std::vector<bool> neighbors) {
    //qDebug() << x << y << z << size << neighbors;
    ++voxelCount;
    size /= 2;
    QVector3D p1(x-size, y-size, z+size);
    //QVector2D t1(0.0f, 0.0f);
    QVector3D p2(x+size, y-size, z+size);
    //QVector2D t2(0.33f, 0.0f);
    QVector3D p3(x+size, y+size, z+size);
    //QVector2D t3(0.33f, 0.5f);
    QVector3D p4(x-size, y+size, z+size);
    //QVector2D t4(0.0f, 0.5f);

    QVector3D p5(x+size, y-size, z-size);
    //QVector2D t5(0.33f, 0.5f);
    QVector3D p6(x-size, y-size, z-size);
    //QVector2D t6(1.0f, 0.5f);
    QVector3D p7(x-size, y+size, z-size);
    //QVector2D t7(1.0f, 1.0f);
    QVector3D p8(x+size, y+size, z-size);
    //QVector2D t8(0.33f, 1.0f);


    //front
    if (!neighbors[1]) {
        addToMesh(p1);
        addToMesh(p2);
        addToMesh(p3);

        addToMesh(p1);
        addToMesh(p3);
        addToMesh(p4);
    }
    //back
    if (!neighbors[0]) {
        addToMesh(p5);
        addToMesh(p6);
        addToMesh(p7);

        addToMesh(p5);
        addToMesh(p7);
        addToMesh(p8);
    }
    //right
    if (!neighbors[5]) {
        addToMesh(p2);
        addToMesh(p5);
        addToMesh(p8);

        addToMesh(p2);
        addToMesh(p8);
        addToMesh(p3);
    }
    //left
    if (!neighbors[4]) {
        addToMesh(p6);
        addToMesh(p1);
        addToMesh(p4);

        addToMesh(p6);
        addToMesh(p4);
        addToMesh(p7);
    }
    //top
    if (!neighbors[3]) {
        addToMesh(p4);
        addToMesh(p3);
        addToMesh(p8);

        addToMesh(p4);
        addToMesh(p8);
        addToMesh(p7);
    }
    //bottom
    if (!neighbors[2]) {
        addToMesh(p6);
        addToMesh(p5);
        addToMesh(p2);

        addToMesh(p6);
        addToMesh(p2);
        addToMesh(p1);
    }
}

void Mesh::clearMesh() {
    points.clear();
    colors.clear();
}

void Mesh::createMesh(QOpenGLBuffer &arrayBuf) {
    // TODO: this is trash fix it
    std::vector<float> buf;
    for (int i = 0; i < (int)points.size(); i+=3) {
        buf.push_back(points[i]);
        buf.push_back(points[i+1]);
        buf.push_back(points[i+2]);
        //buf.push_back(colors[i]);
        //buf.push_back(colors[i+1]);
        //buf.push_back(colors[i+2]);
    }
    arrayBuf.bind();
    arrayBuf.allocate(buf.data(), buf.size()*sizeof(float));
}
