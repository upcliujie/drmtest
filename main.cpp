

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <QCoreApplication>
#include <QImage>
#include <QDebug>

struct framebuffer
{
    uint32_t size;
    uint32_t handle;
    uint32_t fb_id;
    uint32_t *vaddr;
};

static void create_fb(int fd, uint32_t width, uint32_t height, const QImage& img, framebuffer *buf)
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

    QImage image = img.scaled(width, height, Qt::KeepAspectRatio).convertToFormat(QImage::Format_ARGB32_Premultiplied);

    uint32_t *vaddr = (uint32_t *)mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            vaddr[i++] = image.pixel(x, y);
        }
    }

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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // get image file name from command line
    if (argc < 2) {
        qDebug() << "Usage: " << argv[0] << " <image file>";
        return -1;
    }

    QImage img(argv[1]);

    int fd = 0;
    framebuffer buf;
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
        return -1;
    }

    if (crtc_id == 0) {
        return -1;
    }

    if (conn_id == 0) {
        return -1;
    }
    conn = drmModeGetConnector(fd, conn_id);

    create_fb(fd, conn->modes[0].hdisplay, conn->modes[0].vdisplay, img, &buf);
    drmModeSetCrtc(fd, crtc_id, buf.fb_id, 0, 0, &conn_id, 1, &conn->modes[0]);
    drmModePageFlip(fd, crtc_id, buf.fb_id, DRM_MODE_PAGE_FLIP_EVENT, &conn_id);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&](){
        release_fb(fd, &buf);
        drmModeFreeConnector(conn);
        drmModeFreeResources(res);
        close(fd);
    });


    return app.exec();
}
