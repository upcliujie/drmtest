#pragma once

#include <QObject>
#include <QImage>

class DrmBuffer : public QObject
{
    Q_OBJECT
public:
    DrmBuffer(uint32_t gpu_fd, QSize size);
    ~DrmBuffer();

    uint32_t bufferId() const;
    void init();
    inline QSize size() const {
        return m_size;
    };

    inline QImage image() const {
        return m_image;
    };

private:
    uint32_t m_fd = 0;
    uint32_t m_bufferId = 0;
    QSize m_size;
    QImage m_image;

    uint32_t m_handle = 0;
};