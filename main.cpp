
#include "drm_gpu.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    DrmGpu gpu("/dev/dri/card0");

    return app.exec();
}