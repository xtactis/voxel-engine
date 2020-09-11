#ifndef OCTREE_H
#define OCTREE_H

/* TODO
cast rays from all 4 corners
get the 5 (6) planes of the frustum
find the normals of each plane
iterate through all (?) points and check if they're inside the frustum
 - the check is done by taking the dot product of the point and all of the normals
 - if all of the dot products are >0 the point is inside the frustum
selected points go in the "selected" buffer
if delete is pressed they get deleted from the mesh
all of the deleted points get put in the undo stack
the octree is modified so a new mesh should be generated
*/

#include "mesh.h"

#include <vector>
#include <bitset>
#include <atomic>
#include <array>
#include <fstream>

#include <QVector3D>
#include <QOpenGLBuffer>

struct Octree {
    bool root;
    std::bitset<8> children;
    std::array<std::shared_ptr<Octree>, 8> childrenNodes;
    std::vector<QVector3D> points;

    int maxDepth = 0;
    static double resolution;
    float halfsize; //actually halfsize lol
    Mesh mesh;

    std::atomic<bool> loaded = false;

    Octree(float s) : root(false), halfsize(s/2) {}

    std::shared_ptr<Octree> update();

    void getPoints(std::vector<QVector3D> &pts) const;

    int addPoint(const QVector3D &point, int depth=0);
    int addPoint(float x, float y, float z, const QVector3D &orig, int depth=0);

    bool checkLeaf(float x, float y, float z) const;

    void createMeshHelper(Mesh &m, double x=0, double y=0, double z=0);
    void createMesh(QOpenGLBuffer &arrayBuf);

    bool changed() { return true; }

    int size() const;

    static std::shared_ptr<Octree> deserialize(std::ifstream &infile);
    void serialize(std::ofstream &outfile);

    bool intersect(const QVector3D &ray, float x, float y, float z);
    void selectOne(const QVector3D &ray, float x=0, float y=0, float z=0);
    void select(const std::array<QVector3D, 4> &normals, std::vector<QVector3D> &selected, float x=0, float y=0, float z=0);
};

#endif // OCTREE_H
