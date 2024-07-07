#include <iostream>

extern "C"
{
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
}

struct framebuffer
{
    uint32_t size;
    uint32_t handle;
    uint32_t fb_id;
    uint32_t *vaddr;
};

static void create_fb(int fd, uint32_t width, uint32_t height, uint32_t color, framebuffer *buf)
{
    drm_mode_create_dumb create = {};
    drm_mode_map_dumb map = {};
    uint32_t i = 0;
    uint32_t fb_id = 0;

    create.width = width;
    create.height = height;
    create.bpp = 32;

    drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);

    drmModeAddFB(fd, width, height, 24, 32, create.pitch, create.handle, &fb_id);

    map.handle = create.handle;

    drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map);

    uint32_t *vaddr = (uint32_t *)mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);

    for (i = 0; i < (create.size / 4); i++)
        vaddr[i] = color;

    buf->vaddr = vaddr;
    buf->size = create.size;
    buf->handle = create.handle;
    buf->fb_id = fb_id;

    return;
}

static void release_fb(int fd, framebuffer *buf)
{
    drm_mode_destroy_dumb destroy = {};
    destroy.handle = buf->handle;

    drmModeRmFB(fd, buf->fb_id);
    munmap(buf->vaddr, buf->size);
    drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);

    return;
}

int main()
{
    int fd = 0;
    framebuffer buf[3];
    drmModeConnector *conn = nullptr;
    drmModeResPtr res = nullptr;
    drmModeModeInfoPtr mode = nullptr;
    uint32_t conn_id = 0;
    uint32_t crtc_id = 0;

    fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    res = drmModeGetResources(fd);

    for (int i = 0; i < res->count_connectors; i++) {
        conn = drmModeGetConnector(fd, res->connectors[i]);
        if (conn->connection == DRM_MODE_CONNECTED) {
            crtc_id = res->crtcs[i];
            conn_id = res->connectors[i];
            break;
        }
        drmModeFreeConnector(conn);
    }

    if (conn == nullptr) {
        std::cerr << "No connected connector found" << std::endl;
        return -1;
    }

    if (crtc_id == 0) {
        std::cerr << "No crtc found" << std::endl;
        return -1;
    }

    if (conn_id == 0) {
        std::cerr << "No connector found" << std::endl;
        return -1;
    }
    conn = drmModeGetConnector(fd, conn_id);
    std::cout << "hdisplay: " << conn->modes[0].hdisplay << ", vdisplay: " << conn->modes[0].vdisplay << std::endl;

    create_fb(fd, conn->modes[0].hdisplay, conn->modes[0].vdisplay, 0x00ff0000, &buf[0]);
    create_fb(fd, conn->modes[0].hdisplay, conn->modes[0].vdisplay, 0x0000ff00, &buf[1]);
    create_fb(fd, conn->modes[0].hdisplay, conn->modes[0].vdisplay, 0x000000ff, &buf[2]);
    drmModeSetCrtc(fd, crtc_id, buf[0].fb_id, 0, 0, &conn_id, 1, &conn->modes[0]);
    drmModeSetCrtc(fd, crtc_id, buf[1].fb_id, 0, 0, &conn_id, 1, &conn->modes[0]);
    drmModeSetCrtc(fd, crtc_id, buf[2].fb_id, 0, 0, &conn_id, 1, &conn->modes[0]);

    int i = 0;
    while (true) {
        drmModePageFlip(fd, crtc_id, buf[i].fb_id, DRM_MODE_PAGE_FLIP_EVENT, &conn_id);
        usleep(1000000);
        if (++i == 3) {
            i = 0;
        }
    }

    release_fb(fd, &buf[0]);
    release_fb(fd, &buf[1]);
    release_fb(fd, &buf[2]);

    drmModeFreeConnector(conn);
    drmModeFreeResources(res);

    close(fd);
    return 0;
}