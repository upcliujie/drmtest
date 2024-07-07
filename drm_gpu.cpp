#include "drm_gpu.h"

#include <unistd.h>
#include <fcntl.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

DrmGpu::DrmGpu(const QString &device)
{
    m_fd = open(device.toStdString().c_str(), O_RDWR | O_CLOEXEC);
    if (m_fd < 0) {
        qWarning("Failed to open %s", qPrintable(device));
        return;
    }
    init();
}

DrmGpu::~DrmGpu()
{
    close(m_fd);
}

int DrmGpu::fd() const
{
    return m_fd;
}

void DrmGpu::init()
{
    drmModeRes *resources = drmModeGetResources(m_fd);
    if (!resources) {
        qWarning("drmModeGetResources failed");
        return;
    }

    for (int i = 0; i < resources->count_connectors; ++i) {
        auto c = new DrmConnector(resources->connectors[i], m_fd);
        m_connectors.append(c);
    }

    drmModeFreeResources(resources);
}
