#pragma once

#include <QObject>


class DrmBuffer : public QObject
{
    Q_OBJECT
public:
    DrmBuffer(uint32_t gpu_fd, QSize size);
    ~DrmBuffer();

    uint32_t bufferId() const;

private:
    uint32_t m_fd;
    uint32_t m_bufferId;
};