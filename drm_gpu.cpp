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
    drmModeFreeResources(m_resources);
    close(m_fd);
}

int DrmGpu::fd() const
{
    return m_fd;
}

void DrmGpu::init()
{
    m_resources = drmModeGetResources(m_fd);
    if (!m_resources) {
        qWarning("drmModeGetResources failed");
        return;
    }

    for (int i = 0; i < m_resources->count_connectors; ++i) {
        auto c = new DrmConnector(m_resources->connectors[i], m_fd);
        m_connectors.append(c);
    }

}

QList<DrmConnector*> DrmGpu::connectors() const
{
    return m_connectors;
}

void DrmGpu::setBuffer(DrmConnector *connector, DrmBuffer *buffer)
{
    if (!connector || !buffer) {
        return;
    }

    drmModeSetCrtc(m_fd, m_resources->crtcs[0], buffer->bufferId(), 0, 0, &connector->connector()->connector_id, 1, &connector->connector()->modes[0]);
    drmModePageFlip(m_fd, m_resources->crtcs[0], buffer->bufferId(), DRM_MODE_PAGE_FLIP_EVENT, buffer);
}