#pragma once

#include "drm_connector.h"

#include <QObject>

class DrmGpu : public QObject
{
    Q_OBJECT

public:
    DrmGpu(const QString &device);
    ~DrmGpu();

    int fd() const;
    QList<DrmConnector*> connectors() const;
    void setBuffer(DrmConnector *connector, DrmBuffer *buffer);

private:
    void init();

    drmModeRes *m_resources;

    int m_fd;

    QList<DrmConnector*> m_connectors;

};