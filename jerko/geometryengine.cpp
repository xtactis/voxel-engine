#include "geometryengine.h"

#include <QVector3D>
#include <QVector2D>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

GeometryEngine::GeometryEngine(float size)
{
    octree = std::make_shared<Octree>(size);
    //arrayBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);

    //initCubeGeometry();
    //qDebug() << arrayBuf.size();
    //qDebug() << octree.mesh.points.size();
    //qDebug() << octree.mesh.points;
}

GeometryEngine::~GeometryEngine()
{
    qDebug() << "destruct";
    arrayBuf.destroy();
    qDebug() << "arraybuf";
}

bool GeometryEngine::initialized() const {
    return _initialized;
}

void GeometryEngine::init() {
    if (!octree->loaded) return;
    qDebug() << "Creating buffer";
    qDebug() << arrayBuf.create();

    // creating mesh
    octree->createMesh(arrayBuf);

    qDebug() << octree->maxDepth;
    qDebug() << octree->maxDepth;
    qDebug() << octree->halfsize;
    qDebug() << octree->resolution;
    qDebug() << octree->size();

    _initialized = true;
}

void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program)
{
    if (!_initialized) {
        init();
    }
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();

    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, 0);

    // Offset for color
    offset += sizeof(QVector3D);

    //int colorLocation = program->attributeLocation("a_color");
    //program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, 7*sizeof(float));
    //program->enableAttributeArray(colorLocation);

    glDrawArrays(GL_TRIANGLES, 0, octree->mesh.points.size()/3);
}
