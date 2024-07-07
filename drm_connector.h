#pragma once

#include <QObject>

#include <xf86drm.h>
#include <xf86drmMode.h>

class DrmConnector : public QObject
{
    Q_OBJECT
public:

    DrmConnector(uint32_t connector_id, uint32_t gpu_fd);
    ~DrmConnector();

    uint32_t connectorId() const;
    inline bool isConnected() const{
        return m_isConnected;
    };

    QString name() const;

private:
    drmModeConnectorPtr m_connector = nullptr;
    uint32_t m_fd;
    bool m_isConnected;
    QString m_name;
};