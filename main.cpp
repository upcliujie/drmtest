
#include "drm_gpu.h"

#include <QDebug>

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    DrmGpu gpu("/dev/dri/card0");
    QList<DrmConnector*> connectors = gpu.connectors();
    for (auto connector : connectors) {
        if (connector->isConnected()) {
            qDebug() << "Connector" << connector->name() << "is connected";
            DrmBuffer *buffer = new DrmBuffer(gpu.fd(), QSize(connector->connector()->modes[0].vdisplay, connector->connector()->modes[0].hdisplay));
            gpu.setBuffer(connector, buffer);
        }
    }

    return app.exec();
}