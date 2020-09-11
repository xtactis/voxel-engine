#include "geometryengine.h"

#include <QVector3D>
#include <QVector2D>

struct VertexData {
    QVector3D position;
    QVector2D texCoord;
};

GeometryEngine::GeometryEngine(float size) {
    octree = std::make_shared<Octree>(size);
}

GeometryEngine::~GeometryEngine() {
    arrayBuf.destroy();
}

bool GeometryEngine::initialized() const {
    return _initialized;
}

bool GeometryEngine::init() {
    if (!octree->loaded) return false;
    qDebug() << arrayBuf.create();

    octree->createMesh(arrayBuf);

    qDebug() << octree->maxDepth;
    qDebug() << octree->maxDepth;
    qDebug() << octree->halfsize;
    qDebug() << octree->resolution;
    qDebug() << octree->size();

    return _initialized = true;
}

void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program) {
    if (!_initialized) {
        if (!init()) return;
    }
    arrayBuf.bind();

    quintptr offset = 0;

    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, 0);

    glDrawArrays(GL_TRIANGLES, 0, octree->mesh.points.size()/3);
}

void GeometryEngine::addPoints(const std::vector<QVector3D> &pts) {
    // TODO: make octree take the pts in the constructor
    for (const auto &point: pts) {
        octree->addPoint(point);
    }
    octree->loaded = true;
    qDebug() << "size:" << octree->size();
}

void GeometryEngine::addPoint(float x, float y, float z) {
    octree->addPoint(x, y, z, {x, y, z});
}

void GeometryEngine::addPoint(const QVector3D &point) {
    octree->addPoint(point);
}

void GeometryEngine::serialize(std::ofstream &outfile) {
    octree->serialize(outfile);
}
