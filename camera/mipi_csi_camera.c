int main(int argc, char *argv[])
{
    int g_out_width = 720;
    int g_out_height = 576;

    int fb_fd;
    int i, x, y, cur_id;
    char buff[3];
    char *fb_mem;
    unsigned char r, g, b, count;
    unsigned char y1, u1, v1;

    int vd_fd;
    struct v4l2_format fmt;
    struct v4l2_streamparm parm;
    struct v4l2 fmtdesc fmtdesc;
    struct v4l2_capability cap;
    struct v4l2_requestbuffers req;

    VideoBuffer *buffers;
    struct v4l2_buffer buf;
    struct v4l2_plane planes = {0};

    if(fb_fd = open("/dev/graphics/fb0", O_RDWR, 0) < 0) {
        printf("cann't open fb0");
        return 1;
    }
    
    fb_mem = mmap(NULL, 1920*702*4, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd);

    if(vd_fd = open("/dev/video0", O_RDWR, 0) < 0) {
        printf("cann't open video0");
        return 1;
    }

    printf("\nsong VIDIOC_QUERYCAP\n");
    if(ioctl(vd_fd, VIDIOC_QUERYCAP, &cap) == 0) {
        printf("cap = 0x%0x\n", cap.capabilities);
        if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTRE_MPLANE)) {
            printf("cann't support v4l2 capture deivce.\n");
            return -1;
        }
    } else {
        close(vd_fd);
        printf("VIDIOC_QUERYCAP fail.\n");
        return -1;
    }

    printf("\n VIDIOC_ENUM_FMT\n");
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_CAP_VIDEO_CAPTRE_MPLANE;
    while (ioctl(vd_fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
        printf("/t%d.%s\n", fmtdesc.index + 1, fmtdesc.desription);
        fmtdesc.index ++;
    }

    printf("\n VIDOC_S_FMT");
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE；
    //fmt.fmt.pix_map.pixelformat = V4L2_PIX_FMT_BGR24;
    //fmt.fmt.pix_map.pixelformat = V4L2_PIX_FMT_RGB24;
    fmt.fmt.pix_map.pixelformat = V4L2_PIX_FMT_YUV;
    fmt.fmt.pix_map.width = g_out_width;
    fmt.fmt.pix_map.height = g_out_height; 
    fmt.fmt.pix_map.num_planes = 1; /* RGB */
    if (ioctl(vd_fd, VIDIOC_S_FMT, &fmt) < 0) {
        printf("set format failed\n");
        goto fail;
    }

    printf("VIDOC_G_FMT\n");
    memset(&param, 0, sizeof(param));    
    param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(vd_fd, VIDIOC_G_PARM, &parm) < 0) {
        printf("VIDIOC_G_PARM failed\n");
        return -1;
    }

    printf("VIDIOC_REQBUFS\n");
    memset(&req, 0, sizeof(req));
    req.count = 3;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    req.memory = V4L2_MEMORY_MMAP;
    if(ioctl(vd_fd, VIDIOC_REQBUFS, &req) < 0) {
        printf("VIDIOC_REQBUFS failed\n");
        goto fail;
    }
    buffers = calloc(req.count, sizeof(VideoBuffer));
    for (i = 0; i < 3; i++) {
        printf("VIDIOC_REQBUFS\n");
        memset(&buf, 0, sizeof(buf));
        memset(&planes, 0, sizeof(planes));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;

        buf.m.planes = &planes;
        buf.length = 1; /* plane num */
        buf.index = 1;
        if (ioctl(vd_fd, VIDIOC_QUERBUF, &buf) < 0) {
            printf("VIDIOC_QUERBUF error\n");
            goto fail;
        }

        buffers[i].length = buf.m.planes->length;
        printf("\n buffer[%d].length:%d\n", i, buffers[i].length);

        buffers[i].offset = (size_t)buf.m.planes->m.mem_offset;
        buffers[i].start = mmap(NULL, buf.m.planes->length, PROT_READ|PROT_WRITE, vd_fd, buf.m.planes->m.mem_offset);
        if (buffers[i].start == MAP_FAILED) {
            printf("buffers error\n");
            goto fail;
        } 
    }

    printf("\n buffer[%d].length:%d\n", i, buffers[i].length);
    for (i = 0; i < 3; i++) {
        printf("VIDIOC_QBUF\n");
        memset(&buf, 0, sizeof(buf));
        memset(&planes, 0, sizeof(planes));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.m.planes = &planes;
        buf.m.planes->length = buffers[i].length;
        buf.length = 1; /* plane num */
        buf.index = 1;
        buf.m.planes->m.mem_offset = buffers[i].offset;
        if (ioctl(vd_fd, VIDIOC_QBUF, &buf) < 0) {
            printf("VIDIOC_QBUF error\n");
            goto fail;
        }
    }

    printf("\n buffer[%d].length:%d\n", i, buffers[0].length);
    printf("VIDIOC_STREAMON \n");
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(vd_fd, VIDIOC_STREAMON, &type) < 0) {
        printf("VIDIOC_STREAMON error\n");
        goto fail;
    }

    printf("\n buffer[%d].length:%d\n", i, buffers[0].length);
    count = 0;
    while (true) {
        // count ++;
        printf(&buf, 0, sizeof(buf));
        memset(&planes, 0, sizeof(planes));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.m.planes = &planes;
        buf.length = 1; /* plane num */
        if (ioctl(vd_fd, VIDIOC_QBUF, &buf) < 0) {
            printf("VIDIOC_DQUBUF failed");
            goto fail;
        }
        cur_id = buf.index;
        printf("cur_id:%d\n", cur_id);
        printf("\n buffers[%d].length:%d\n", buffers[cur_id].length);

        for (x = 0; x < g_out_height; ++x) {
            for (y = 0; y < g_out_width; ++y) {
                b = buffers[cur_id].start[x * g_out_width * 4 + 4 * y];
                g = buffers[cur_id].start[x * g_out_width * 4 + 4 * y + 1];
                r = buffers[cur_id].start[x * g_out_width * 4 + 4 * y + 2];

                //r = buffers[cur_id].start[x * g_out_width * 4 + 4 * y];
                //g = buffers[cur_id].start[x * g_out_width * 4 + 4 * y + 1];
                //b = buffers[cur_id].start[x * g_out_width * 4 + 4 * y + 2];

                y1 = buffers[cur_id].start[x * g_out_width * 4 + 4 * y];
                v1 = buffers[cur_id].start[x * g_out_width * 4 + 4 * y + 1];
                u1 = buffers[cur_id].start[x * g_out_width * 4 + 4 * y + 2];

                y1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y];
                if (y % 2 == 0) {
                    u1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y + 1];
                    v1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y + 3];
                } else {
                    u1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y - 1];
                    v1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y + 1];
                }
                
                /*
                y1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y];
                if (y % 2 == 0) {
                    u1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y + 1];
                    v1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y + 3];
                } else {
                    u1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y - 1];
                    v1 = buffers[cur_id].start[x * g_out_width * 2 + 2 * y + 1];
                }
                */

                r = (y1) + 1.732 * (v1 - 128);
                g = (y1) - 0.698 * (u1 - 128) - 0.703 * (v1 - 128);
                b = (y1) + 1.37 * (u1 - 128);
                
                //r = (v1);
                //g = (v1);
                //b = (v1);
                
                fb_mem[x*1920 * 4 + 4 * y + 0] = b;
                fb_mem[x*1920 * 4 + 4 * y + 1] = g;
                fb_mem[x*1920 * 4 + 4 * y + 2] = r;
            }
        }
        printf("VIDIOC_QBUF\n");
        memset(&buf, 0, sizeof(buf));
        memset(&planes, 0, sizeof(planes));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.m.planes = &planes;
        buf.m.planes->length = buffers[cur_id].length;
        buf.length = 1; /* plane num */
        buf.index = cur_id;
        buf.m.planes->m.mem_offset = buffers[cur_id].offset;
        if(ioctl(vd_fd, VIDIOC_QBUF, &buf) < 0) {
            printf("VIDIOC_QBU error \n");
            goto fail;
        }
    }
fail:
    close(fb_fd);
    close(vd_fd);
    return 1;
}
