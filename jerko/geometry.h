#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "renderer.h"

#include <vector>
#include <fstream>

#include <QQuickItem>
#include <QVector3D>
#include <QQuaternion>

class Geometry : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
private:
    Renderer *renderer;

    QVector3D m_position = {0, 0, -10};
    QQuaternion m_rotation;

public:
    Geometry();

    QVector3D position() const { return m_position; }
    void setPosition(const QVector3D &position);

    QQuaternion rotation() const { return m_rotation; }
    void setRotation(const QQuaternion &rotation);

    Q_INVOKABLE bool unsaved() const {return false;}
    Q_INVOKABLE bool canSave() const {return false;}

public slots:
    void sync();
    void cleanup();

    void newProject() {
        renderer->reset();
        m_position = {0, 0, -10};
        m_rotation = {0, 0, 0, 0};
    };
    void open(const QString &){};
    void save(const QString &){};

    void importModel(const QString &filename) {
        if (filename.endsWith(".pts")) {
            loadPoints(filename);
        }
    };
    void exportModel(const QString &){};

    void undo(){};
    void redo(){};
    void cut(){};
    void copy(){};
    void paste(){};

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
        for (std::string line; std::getline(infile, line); ) {
            float x, y, z;
            sscanf(line.c_str(), "%f %f %f", &x, &y, &z);
            pts.emplace_back(x, y, z);
        }
        infile.close();
        renderer->addLayer(pts);
    }

signals:
    void positionChanged();
    void rotationChanged();
};

#endif // GEOMETRY_H

