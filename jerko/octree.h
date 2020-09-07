#ifndef OCTREE_H
#define OCTREE_H

#include "mesh.h"

#include <vector>
#include <bitset>

#include <QVector3D>
#include <QOpenGLBuffer>

struct Octree {
    bool root;
    std::bitset<8> children;
    Octree *childrenNodes[8];

    int maxDepth = 0;
    double resolution;
    float halfsize; //actually halfsize lol
    Mesh mesh;

    Octree(float s, double res = 0.1) : resolution(res), halfsize(s/2) {
        root = false;
    }

    void addPoint(const QVector3D &point, int depth=0) {
        addPoint(point.x(), point.y(), point.z(), depth);
    }

    int addPoint(float x, float y, float z, int depth=0) {
        maxDepth = std::max(depth, maxDepth);
        char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
        children[ind] = 1;
        if (root) {
            if (halfsize > resolution)
                maxDepth = std::max(maxDepth, childrenNodes[(int)ind]->addPoint(x-halfsize*(x>halfsize), y-halfsize*(y>halfsize), z-halfsize*(z>halfsize), ++depth));
        } else {
            root = true;
            for (int i = 0; i < 8; ++i) {
                childrenNodes[i] = new Octree(halfsize);
            }
        }
        return maxDepth;
    }

    bool checkLeaf(float x, float y, float z) {
        if (root) {
            return checkLeaf(x-halfsize*(x>halfsize), y-halfsize*(y>halfsize), z-halfsize*(z>halfsize));
        }
        char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
        return children[ind];
    }

    void createMeshHelper(Mesh &mesh, double x=0, double y=0, double z=0) {
        if (!root) {
            mesh.addCubeToMesh(x, y, z, halfsize*2, std::vector<bool>(6, 0));
            return;
        }
        for (int i = 0; i < 8; ++i) {
            if (children[i])
                childrenNodes[i]->createMeshHelper(mesh, x+halfsize*(i&1), y+halfsize*!!(i&2), z+halfsize*!!(i&4));
        }
    }

    void createMesh(QOpenGLBuffer &arrayBuf) {
        mesh.clearMesh();
        createMeshHelper(mesh);
        mesh.createMesh(arrayBuf);
    }

    void draw() {
        //mesh.drawMesh();
    }

    bool changed() {
        return true;
    }

    int size() {
        int ret = 1+4+1+8*8;
        if (!root) return ret;
        for (int i = 0; i < 8; ++i) {
            if (children[i])
                ret += childrenNodes[i]->size();
        }
        return ret;
    }
};

#endif // OCTREE_H
