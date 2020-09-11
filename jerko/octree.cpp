#include "octree.h"

double Octree::resolution = 0.1;

std::shared_ptr<Octree> Octree::update() {
    auto newOt = std::make_shared<Octree>(halfsize*2);
    std::vector<QVector3D> pts;
    getPoints(pts);
    for (const auto &p: pts) {
        newOt->addPoint(p);
    }
    newOt->loaded = true;
    return newOt;
}

void Octree::getPoints(std::vector<QVector3D> &pts) const {
    for (const auto &p: points)
        pts.push_back(p);
    if (!root) return;
    for (int i = 0; i < 8; ++i) {
        if (children[i])
            childrenNodes[i]->getPoints(pts);
    }
}

int Octree::addPoint(const QVector3D &point, int depth) {
    return addPoint(point.x(), point.y(), point.z(), point, depth);
}

int Octree::addPoint(float x, float y, float z, const QVector3D &orig, int depth) {
    maxDepth = std::max(depth, maxDepth);
    char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
    children[ind] = 1;
    if (!root) {
        for (int i = 0; i < 8; ++i) {
            childrenNodes[i] = std::make_shared<Octree>(halfsize);
        }
    }
    if (halfsize > resolution) {
        maxDepth = std::max(maxDepth, childrenNodes[(int)ind]->addPoint(x-halfsize*(x>halfsize), y-halfsize*(y>halfsize), z-halfsize*(z>halfsize), orig, depth+1));
    } else {
        points.emplace_back(orig);
    }
    root = true;
    return maxDepth;
}

bool Octree::checkLeaf(float x, float y, float z) const {
    if (root) {
        return checkLeaf(x-halfsize*(x>=halfsize), y-halfsize*(y>=halfsize), z-halfsize*(z>=halfsize));
    }
    char ind = (x>halfsize)|(y>halfsize)<<1|(z>halfsize)<<2;
    return children[ind];
}

void Octree::createMeshHelper(Mesh &m, double x, double y, double z) {
    //qDebug() << x << y << z << halfsize;
    if (!root) {
        //qDebug() << x << y << z << resolution;
        m.addCubeToMesh(x, y, z, halfsize*2, std::vector<bool>(6, false));
        return;
    }
    for (int i = 0; i < 8; ++i) {
        if (children[i]) {
            childrenNodes[i]->createMeshHelper(m, x+halfsize*(i&1), y+halfsize*!!(i&2), z+halfsize*!!(i&4));
        }
    }
}

void Octree::createMesh(QOpenGLBuffer &arrayBuf) {
    qDebug() << "create mesh?";
    mesh.clearMesh();
    createMeshHelper(mesh);
    mesh.createMesh(arrayBuf);
    qDebug() << "pts: " << mesh.points.size();
}

int Octree::size() const {
    int ret = 1+2*4+1+8*8+points.size()*3*sizeof(float);
    if (!root) return ret;
    for (int i = 0; i < 8; ++i) {
        if (children[i])
            ret += childrenNodes[i]->size();
    }
    return ret;
}

std::shared_ptr<Octree> Octree::deserialize(std::ifstream &infile) {
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

void Octree::serialize(std::ofstream &outfile) {
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

bool Octree::intersect(const QVector3D &ray, float x, float y, float z) {
    // FIXME: this just doesn't work because I'm dumb
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    const QVector3D invdir = -ray;
    const QVector3D bounds[2] = {{x-halfsize, y-halfsize, z-halfsize}, {x+halfsize, y+halfsize, z+halfsize}};
    int sign[3] = {invdir.x() < 0, invdir.y() < 0, invdir.z() < 0};
    tmin = (bounds[sign[0]].x()) * invdir.x();
    tmax = (bounds[1-sign[0]].x()) * invdir.x();
    tymin = (bounds[sign[1]].y()) * invdir.y();
    tymax = (bounds[1-sign[1]].y()) * invdir.y();

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[sign[2]].z()+10) * invdir.z();
    tzmax = (bounds[1-sign[2]].z()+10) * invdir.z();

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

void Octree::selectOne(const QVector3D &ray, float x, float y, float z) {
    // FIXME: I think this would work if intersect worked
    if (!intersect(ray, x, y, z)) return;
    if (!root) {
        for (const auto &p: points) qDebug() << p;
        return;
    }
    for (int i = 0; i < 8; ++i) {
        if (children[i]) {
            childrenNodes[i]->selectOne(ray, x+halfsize*(i&1), y+halfsize*!!(i&2), z+halfsize*!!(i&4));
        }
    }
}

void Octree::select(const std::array<QVector3D, 4> &normals, std::vector<QVector3D> &selected, float x, float y, float z) {
    // FIXME: this is just trash idk what I was thinking
    // fr tho I think the normals aren't being calculated properly
    // also the raycast vectors seem to not care about position, idk why
    if (!root) {
        for (const auto &n: normals) {
            const auto &&dp = QVector3D::dotProduct(n, {x, y, z});
            qDebug() << dp << x << y << z << n;
            if (dp > 0)
                return;
        }
        for (const auto &p: points) {
            selected.emplace_back(p);
        }
        return;
    }
    for (int i = 0; i < 8; ++i) {
        if (children[i]) {
            childrenNodes[i]->select(normals, selected, x+halfsize*(i&1), y+halfsize*!!(i&2), z+halfsize*!!(i&4));
        }
    }
}
