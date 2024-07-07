#include "drm_connector.h"

DrmConnector::DrmConnector(uint32_t connector_id, uint32_t gpu_fd)
    : m_fd(gpu_fd)
    , m_isConnected(false)
{
    m_connector = drmModeGetConnector(gpu_fd, connector_id);
    if (!m_connector) {
        qWarning("Failed to get connector %d", connector_id);
        return;
    }
    m_isConnected = (m_connector->connection == DRM_MODE_CONNECTED);

    // Get connector name
    const char *name = drmModeGetConnectorTypeName(m_connector->connector_type);
    m_name = QString::fromUtf8(name);

    for (int i = 0; i < m_connector->count_modes; ++i) {
        auto mode = m_connector->modes[i];
        qInfo("Mode %d: %dx%d@%d", i, mode.hdisplay, mode.vdisplay, mode.vrefresh);
    }
}


DrmConnector::~DrmConnector()
{
    drmModeFreeConnector(m_connector);
}

uint32_t DrmConnector::connectorId() const
{
    return m_connector->connector_id;
}

QString DrmConnector::name() const
{
    return m_name;
}