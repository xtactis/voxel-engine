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
    constexpr static double resolution {0.1};
    float halfsize; //actually halfsize lol
    Mesh mesh;

    std::atomic<bool> loaded = false;

    Octree(float s) : root(false), halfsize(s/2) {}

    void addPoint(const QVector3D &point, int depth=0) {
        addPoint(point.x(), point.y(), point.z(), depth);
    }

    int addPoint(float x, float y, float z, int depth=0) {
        maxDepth = std::max(depth, maxDepth);
        char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
        children[ind] = 1;
        if (!root) {
            for (int i = 0; i < 8; ++i) {
                childrenNodes[i] = std::make_shared<Octree>(halfsize);
            }
        }
        if (halfsize > resolution) {
            maxDepth = std::max(maxDepth, childrenNodes[(int)ind]->addPoint(x-halfsize*(x>halfsize), y-halfsize*(y>halfsize), z-halfsize*(z>halfsize), depth+1));
        } else {
            points.emplace_back(x, y, z);
        }
        root = true;
        return maxDepth;
    }

    bool checkLeaf(float x, float y, float z) {
        if (root) {
            return checkLeaf(x-halfsize*(x>=halfsize), y-halfsize*(y>=halfsize), z-halfsize*(z>=halfsize));
        }
        char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
        return children[ind];
    }

    void createMeshHelper(Mesh &m, double x=0, double y=0, double z=0) {
        //qDebug() << x << y << z << halfsize;
        if (!root) {
            //qDebug() << x << y << z << resolution;
            m.addCubeToMesh(x, y, z, halfsize*2, std::vector<bool>(6, false));
            return;
        }
        bool noRoots = true;
        for (int i = 0; i < 8; ++i) {
            if (children[i] && childrenNodes[i]->root) {
                noRoots = false;
            }
        }
        if (children.to_ulong() == (1<<8)-1 && noRoots) {
            m.addCubeToMesh(x, y, z, halfsize*2, std::vector<bool>(6, false));
            return;
        }
        for (int i = 0; i < 8; ++i) {
            if (children[i]) {
                childrenNodes[i]->createMeshHelper(m, x+halfsize*(i&1), y+halfsize*!!(i&2), z+halfsize*!!(i&4));
            }
        }
    }

    void createMesh(QOpenGLBuffer &arrayBuf) {
        mesh.clearMesh();
        createMeshHelper(mesh);
        mesh.createMesh(arrayBuf);
        qDebug() << "pts: " << mesh.points.size();
    }

    void draw() {
        //mesh.drawMesh();
    }

    bool changed() {
        return true;
    }

    int size() {
        int ret = 1+2*4+1+8*8;
        if (!root) return ret;
        for (int i = 0; i < 8; ++i) {
            if (children[i])
                ret += childrenNodes[i]->size();
        }
        return ret;
    }

    static std::shared_ptr<Octree> deserialize(std::ifstream &infile) {
        float halfsize;
        infile.read(reinterpret_cast<char*>(&halfsize), sizeof(halfsize));
        std::shared_ptr<Octree> node = std::make_shared<Octree>(halfsize*2);
        infile.read(reinterpret_cast<char*>(&node->root), sizeof(node->root));
        if (!node->root) {
            unsigned long long size;
            infile.read(reinterpret_cast<char*>(&size), sizeof(size));
            node->points.reserve(size);
            infile.read(reinterpret_cast<char*>(node->points.data()), size*sizeof(*node->points.data()));
            return node;
        }
        infile.read(reinterpret_cast<char*>(&node->children), sizeof(node->children));
        for (int i = 0; i < 8; ++i) {
            if (node->children[i]) {
                node->childrenNodes[i] = Octree::deserialize(infile);
            }
        }
        return node;
    }

    void serialize(std::ofstream &outfile) {
        outfile.write(reinterpret_cast<char*>(&halfsize), sizeof(halfsize));
        outfile.write(reinterpret_cast<char*>(&root), sizeof(root));
        if (!root) {
            unsigned long long size = points.size();
            outfile.write(reinterpret_cast<char*>(&size), sizeof(size));
            outfile.write(reinterpret_cast<char*>(points.data()), size*sizeof(*points.data()));
            return;
        }
        outfile.write(reinterpret_cast<char*>(&children), sizeof(children));
        for (int i = 0; i < 8; ++i) {
            if (children[i]) {
                childrenNodes[i]->serialize(outfile);
            }
        }
    }
};

#endif // OCTREE_H
