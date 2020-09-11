#include "geometry.h"
#include "happly.h"

#include <QtQuick/qquickwindow.h>
#include <QtCore/QRunnable>

QQuickView *Geometry::view = nullptr;

Geometry::Geometry() : renderer(nullptr) {
    connect(this, &QQuickItem::windowChanged, this, &Geometry::handleWindowChanged);
}

void Geometry::handleWindowChanged(QQuickWindow *win) {
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Geometry::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Geometry::cleanup, Qt::DirectConnection);

        win->setColor(Qt::black);
    }
}

void Geometry::cleanup() {
    delete renderer;
    renderer = nullptr;
}

void Geometry::setPosition(const QVector3D &position) {
    m_position = position;
    emit positionChanged();
    if (renderer) renderer->setPosition(m_position); // FIXME: no
};

void Geometry::setRotation(const QQuaternion &rotation) {
    m_rotation = rotation;
    emit rotationChanged();
    if (renderer) renderer->setRotation(m_rotation);  // FIXME: no
};

class CleanupJob : public QRunnable {
public:
    CleanupJob(Renderer *renderer) : renderer(renderer) { }
    void run() override { delete renderer; }
private:
    Renderer *renderer;
};

void Geometry::releaseResources() {
    window()->scheduleRenderJob(new CleanupJob(renderer), QQuickWindow::BeforeSynchronizingStage);
    renderer = nullptr;
}

void Geometry::sync() {
    if (!renderer) {
        renderer = new Renderer();
        connect(window(), &QQuickWindow::beforeRendering, renderer, &Renderer::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, renderer, &Renderer::paint, Qt::DirectConnection);
        connect(window(), &QQuickWindow::afterRendering, window(), &QQuickWindow::update, Qt::DirectConnection);
    }
    renderer->resize(window()->width(), window()->height());
    renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    renderer->setWindow(window());
    renderer->setPosition(m_position);
    renderer->setRotation(m_rotation);
}

void Geometry::importModel(const QString &filename) {
    std::vector<QVector3D> pts;
    float minx=0, maxx=0, miny=0, maxy=0, minz=0, maxz=0;
    { // scoped so _pts can be freed
        std::vector<std::array<double, 3>> _pts;
        if (filename.endsWith(".pts")) {
            _pts = loadPoints(filename);
        } else if (filename.endsWith(".ply") || filename.endsWith(".obj") || filename.endsWith(".stl")){
            _pts = readPLY(filename);
        } else {
            qDebug() << "I'm sorry Dave I can't let you do that";
        }
        bool firstline = true;
        for (const auto &p: _pts) {
            pts.emplace_back(p[0], p[1], p[2]);
            if (firstline) {
                minx = maxx = p[0];
                miny = maxy = p[1];
                minz = maxz = p[2];
                firstline = false;
            } else {
                minx = std::min((float)p[0], minx);
                miny = std::min((float)p[1], miny);
                minz = std::min((float)p[2], minz);
                maxx = std::max((float)p[0], maxx);
                maxy = std::max((float)p[1], maxy);
                maxz = std::max((float)p[2], maxz);
            }
        }
    }
    // TODO: move this further along the pipeline to eliminate the extra loop
    // compiler possibly does that automatically tho
    // int i = 0;
    for (auto &p: pts) {
        p -= {minx, miny, minz};
    }
    qDebug() << renderer;
    renderer->addLayer(pts, std::max({maxx-minx, maxy-miny, maxz-minz}));
}

void Geometry::newProject() {
    view->setTitle("Farina");
    _canSave = false;
    qDebug() << "new project";
    renderer->reset();
    qDebug() << "reset";
    m_position = {0, 0, -10};
    qDebug() << "position";
    m_rotation = {0, 0, 0, 0};
    qDebug() << "rotation";
}

void Geometry::open(const QString &filename) {
#ifdef WIN32
    // pesky '/' at the beginning, nobody liked that
    std::ifstream infile(filename.mid(1).toStdString(),
                         std::ifstream::in | std::ifstream::binary);
    view->setTitle("Farina - " + filename.mid(1));
#else
    std::ifstream infile(filename.toStdString(),
                         std::ifstream::in | std::ifstream::binary);
    view->setTitle("Farina - " + filename);
#endif
    setSave(filename);
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

void Geometry::save() {
#ifdef WIN32
    // pesky '/' at the beginning, nobody liked that
    std::ofstream outfile(filename.mid(1).toStdString(),
                          std::ofstream::out | std::ofstream::binary);
    view->setTitle("Farina - " + filename.mid(1));
#else
    std::ofstream outfile(filename.toStdString(),
                          std::ofstream::out | std::ofstream::binary);
    view->setTitle("Farina - " + filename);
#endif
    Header header = {renderer->geometries.size(), renderer->thetaX, renderer->thetaY, {m_position.x(), m_position.y(), m_position.z()}};
    outfile.write(reinterpret_cast<char*>(&header), sizeof(header));
    for (const auto &g: renderer->geometries) {
        g->serialize(outfile);
    }
    outfile.close();
}

std::vector<std::array<double, 3>> Geometry::loadPoints(const QString &filename) {
#ifdef WIN32
    // pesky '/' at the beginning, nobody liked that
    std::ifstream infile(filename.mid(1).toStdString());
#else
    std::ifstream infile(filename.toStdString());
#endif
    std::vector<std::array<double, 3>> pts;
    if (infile.fail()) {
        // TODO handle this properly with a message or sth
        qDebug() << "whoops.. didn't know this was on";
        return pts;
    }
    for (std::string line; std::getline(infile, line); ) {
        double x, y, z;
        if (!sscanf(line.c_str(), "%lf %lf %lf", &x, &y, &z)) continue;
        pts.push_back({x, y, z});
    }
    infile.close();
    return pts;
}

std::vector<std::array<double, 3>> Geometry::readPLY(const QString &filename) {
#ifdef WIN32
    // pesky '/' at the beginning, nobody liked that
    happly::PLYData plyIn(filename.mid(1).toStdString());
#else
    happly::PLYData plyIn(filename.toStdString());
#endif
    if (plyIn.hasElement("vertices")) {
        return plyIn.getVertexPositions("vertices");
    } else if (plyIn.hasElement("vertex")) {
        return plyIn.getVertexPositions("vertex");
    } else {
        // TODO: handle properly
        qDebug() << "nothing here but us chickens";
        return std::vector<std::array<double, 3>>();
    }
}
