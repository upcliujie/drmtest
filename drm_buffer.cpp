#include "drm_buffer.h"

#include <QSize>

#include <xf86drm.h>
#include <xf86drmMode.h>

DrmBuffer::DrmBuffer(uint32_t gpu_fd, QSize size)
    : m_fd(gpu_fd)
{

}

DrmBuffer::~DrmBuffer()
{

}

uint32_t DrmBuffer::bufferId() const
{
    return m_bufferId;
}