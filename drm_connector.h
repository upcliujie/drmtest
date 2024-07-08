#pragma once

#include "drm_buffer.h"

#include <QObject>

#include <xf86drm.h>
#include <xf86drmMode.h>

class DrmConnector : public QObject
{
    Q_OBJECT
public:

    DrmConnector(uint32_t connector_id, uint32_t gpu_fd);
    ~DrmConnector();

    inline drmModeConnectorPtr connector() const {
        return m_connector;
    };

    inline bool isConnected() const{
        return m_isConnected;
    };

    inline QString name() const{
        return m_name;
    };



private:
    drmModeConnectorPtr m_connector = nullptr;
    uint32_t m_fd;
    bool m_isConnected;
    QString m_name;
    DrmBuffer *m_buffer = nullptr;
};