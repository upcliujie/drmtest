#include "drm_buffer.h"

#include <QSize>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

DrmBuffer::DrmBuffer(uint32_t gpu_fd, QSize size)
    : m_fd(gpu_fd)
    , m_size(size)
{
    init();
}

DrmBuffer::~DrmBuffer()
{
    drmModeRmFB(m_fd, m_bufferId);
    drm_mode_destroy_dumb destroy = {};
    destroy.handle = m_handle;
    drmIoctl(m_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
    munmap(m_image.bits(), m_image.sizeInBytes());
}

uint32_t DrmBuffer::bufferId() const
{
    return m_bufferId;
}

void DrmBuffer::init()
{
    drm_mode_create_dumb create = {};
    drm_mode_map_dumb map = {};

    create.width = m_size.width();
    create.height = m_size.height();
    create.bpp = 32;

    drmIoctl(m_fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);

    drmModeAddFB(m_fd, create.width, create.height, 24, 32, create.pitch, create.handle, &m_bufferId);

    map.handle = create.handle;
    drmIoctl(m_fd, DRM_IOCTL_MODE_MAP_DUMB, &map);

    m_image = QImage(m_size, QImage::Format_ARGB32_Premultiplied);
    m_image.fill(Qt::red);

    uint32_t *vaddr = (uint32_t*)mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, map.offset);
    if (vaddr == MAP_FAILED) {
        qWarning("mmap failed");
        return;
    }
    for (int y = 0; y < m_size.height(); ++y) {
        for (int x = 0; x < m_size.width(); ++x) {
            vaddr[y * m_size.width() + x] = 0xff0000ff;
        }
    }


    m_handle = create.handle;
}

