#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "renderer.h"
#include "geometryengine.h"
#include "octree.h"

#include <vector>
#include <fstream>
#include <cmath>

#include <QQuickItem>
#include <QVector3D>
#include <QQuaternion>

struct Header{
    unsigned long long size;
    int thetaX, thetaY;
    double pos[3];
};

class Geometry : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
private:
    Renderer *renderer;

    QVector3D m_position = {0, 0, -10};
    QQuaternion m_rotation;

    int mouseStartX, mouseStartY;
    int mouseEndX, mouseEndY;

    int selectX1, selectY1;

public:
    Geometry();

    QVector3D position() const { return m_position; }
    void setPosition(const QVector3D &position);

    QQuaternion rotation() const { return m_rotation; }
    void setRotation(const QQuaternion &rotation);

    Q_INVOKABLE bool unsaved() const {return false;}
    Q_INVOKABLE bool canSave() const {return false;}
    Q_INVOKABLE void move(float x, float y, float z) {
        m_position += {x, y, z};
        qDebug() << m_position;
        if (renderer) renderer->setPosition(m_position);
    }

    Q_INVOKABLE void setMouse(int x, int y) {
        mouseStartX = x;
        mouseStartY = y;
    }

    Q_INVOKABLE void mouseMove(int x, int y) {
        if (renderer) {
            renderer->thetaX += mouseEndX-mouseStartX;
            renderer->thetaY += mouseEndY-mouseStartY;
            renderer->thetaX %= 360;
            renderer->thetaY %= 360;
        }
        mouseStartX = mouseEndX;
        mouseStartY = mouseEndY;
        mouseEndX = x;
        mouseEndY = y;
    }

    Q_INVOKABLE void select(int x, int y) {
        const auto ray = renderer->raycast(x, y);
        qDebug() << ray;
        for (auto &geom: renderer->geometries) {
            geom->octree->selectOne(ray);
        }
        selectX1 = x;
        selectY1 = y;
    }

    Q_INVOKABLE void selectBox(int x, int y) {
        int x1 = std::min(x, selectX1),
            y1 = std::min(y, selectY1),
            x2 = std::max(x, selectX1),
            y2 = std::max(y, selectY1);
        const auto b0 = renderer->raycast(x1, y1);
        const auto b1 = renderer->raycast(x2, y1);
        const auto b2 = renderer->raycast(x2, y2);
        const auto b3 = renderer->raycast(x1, y2);
        const auto n0 = QVector3D::crossProduct(b1, b0).normalized();
        qDebug() << "n0:" << n0;
        const auto n1 = QVector3D::crossProduct(b1, b2).normalized();
        qDebug() << "n1:" << n1;
        const auto n2 = QVector3D::crossProduct(b3, b2).normalized();
        qDebug() << "n2:" << n2;
        const auto n3 = QVector3D::crossProduct(b3, b0).normalized();
        qDebug() << "n3:" << n3;
        //const auto n4 = renderer->raycast(1280/2, 720/2);
        std::vector<QVector3D> selected;
        for (auto &geom: renderer->geometries) {
            geom->octree->select({n0, n1, n2, n3}, selected);
        }
        qDebug() << selected.size();
    }

public slots:
    void sync();
    void cleanup();

    void newProject() {
        qDebug() << "new project";
        renderer->reset();
        qDebug() << "reset";
        m_position = {0, 0, -10};
        qDebug() << "position";
        m_rotation = {0, 0, 0, 0};
        qDebug() << "rotation";
    }

    void open(const QString &filename) {
#ifdef WIN32
        // pesky '/' at the beginning, nobody liked that
        std::ifstream infile(filename.mid(1).toStdString(),
                             std::ifstream::in | std::ifstream::binary);
#else
        std::ifstream infile(filename.toStdString(),
                             std::ifstream::in | std::ifstream::binary);
#endif
        Header header;
        infile.read(reinterpret_cast<char*>(&header), sizeof(header));
        renderer->geometries.reserve(header.size);
        renderer->thetaX = header.thetaX;
        renderer->thetaY = header.thetaY;
        for (int i = 0; i < 3; ++i)
            m_position[i] = header.pos[i];
        qDebug() << header.size;
        for (unsigned long long i = 0; i < header.size; ++i) {
            renderer->geometries.push_back(std::make_shared<GeometryEngine>());
            renderer->geometries[i]->octree = Octree::deserialize(infile);
            renderer->geometries[i]->octree->loaded = true;
            qDebug() << renderer->geometries[i]->octree->halfsize;
        }
        infile.close();
    }

    void save(const QString &filename) {
#ifdef WIN32
        // pesky '/' at the beginning, nobody liked that
        std::ofstream outfile(filename.mid(1).toStdString(),
                              std::ofstream::out | std::ofstream::binary);
#else
        std::ofstream outfile(filename.toStdString(),
                              std::ofstream::out | std::ofstream::binary);
#endif
        Header header = {renderer->geometries.size(), renderer->thetaX, renderer->thetaY, {m_position.x(), m_position.y(), m_position.z()}};
        outfile.write(reinterpret_cast<char*>(&header), sizeof(header));
        for (const auto &g: renderer->geometries) {
            g->serialize(outfile);
        }
        outfile.close();
    }

    void importModel(const QString &filename) {
        if (filename.endsWith(".pts")) {
            loadPoints(filename);
        }
    }
    void exportModel(const QString &){}

    void undo(){}
    void redo(){}
    void cut(){}
    void copy(){}
    void paste(){}

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;
    void loadPoints(const QString &filename) {
#ifdef WIN32
        // pesky '/' at the beginning, nobody liked that
        std::ifstream infile(filename.mid(1).toStdString());
#else
        std::ifstream infile(filename.toStdString());
#endif
        std::vector<QVector3D> pts;
        if (infile.fail()) {
            // TODO handle this properly with a message or sth
            qDebug() << "whoops.. didn't know this was on";
            return;
        }
        bool firstline = true;
        float minx, maxx, miny, maxy, minz, maxz;
        for (std::string line; std::getline(infile, line); ) {
            float x, y, z;
            if (!sscanf(line.c_str(), "%f %f %f", &x, &y, &z)) continue;
            pts.emplace_back(x, y, z);
            if (firstline) {
                minx = maxx = x;
                miny = maxy = y;
                minz = maxz = z;
                firstline = false;
            } else {
                minx = std::min(x, minx);
                miny = std::min(y, miny);
                minz = std::min(z, minz);
                maxx = std::max(x, maxx);
                maxy = std::max(y, maxy);
                maxz = std::max(z, maxz);
            }
        }
        infile.close();
        // TODO: move this further along the pipeline to eliminate the extra loop
        // compiler possibly does that automatically tho
        // int i = 0;
        for (auto &p: pts) {
            p -= {minx, miny, minz};
            //p /= 0.1; // resolution, also should be handled later lol
            //p[0] = std::floor(p[0]/0.1);
            //p[1] = std::floor(p[1]/0.1);
            //p[2] = std::floor(p[2]/0.1);
            //p *= 0.1;
            //if (i++ % 1000 == 0) qDebug() << "after" << p;
        }
        qDebug() << renderer;
        renderer->addLayer(pts, std::max({maxx-minx, maxy-miny, maxz-minz}));
    }

signals:
    void positionChanged();
    void rotationChanged();
};

#endif // GEOMETRY_H

