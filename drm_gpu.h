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

private:
    void init();

    int m_fd;
    QList<DrmConnector*> m_connectors;

};