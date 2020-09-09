#include "geometry.h"

#include <QtQuick/qquickwindow.h>
#include <QtCore/QRunnable>

Geometry::Geometry() : renderer(nullptr) {
    connect(this, &QQuickItem::windowChanged, this, &Geometry::handleWindowChanged);
}

void Geometry::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Geometry::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Geometry::cleanup, Qt::DirectConnection);

        // Ensure we start with cleared to black. The squircle's blend mode relies on this.
        win->setColor(Qt::black);
    }
}

void Geometry::cleanup()
{
    delete renderer;
    renderer = nullptr;
}

void Geometry::setPosition(const QVector3D &position) {
    qDebug() << "geometry set Position" << position;
    m_position = position;
    emit positionChanged();
    if (renderer) renderer->setPosition(m_position); // FIXME: no
};

void Geometry::setRotation(const QQuaternion &rotation) {
    m_rotation = rotation;
    emit rotationChanged();
    if (renderer) renderer->setRotation(m_rotation);  // FIXME: no
};

class CleanupJob : public QRunnable
{
public:
    CleanupJob(Renderer *renderer) : renderer(renderer) { }
    void run() override { delete renderer; }
private:
    Renderer *renderer;
};

void Geometry::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(renderer), QQuickWindow::BeforeSynchronizingStage);
    renderer = nullptr;
}

void Geometry::sync()
{
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
