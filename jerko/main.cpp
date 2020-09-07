#include "renderer.h"
#include "geometry.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QtQuick/QQuickView>
#include <QQuickStyle>
#include <QQmlEngine>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSwapInterval(0);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    a.setApplicationName("Farina");
    a.setApplicationVersion("0.1");

    QQuickStyle::setStyle("Fusion");

    qmlRegisterType<Geometry>("Voxels", 1, 0, "Geometry");

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    view.engine()->connect(view.engine(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

    return a.exec();
}
