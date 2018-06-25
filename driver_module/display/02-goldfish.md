## 显示系统驱动程序
在模拟器使用驱动程序是Goldfish和FrameBuffer驱动程序，使用硬件抽象层Gralloc模块。Gralloc模块既可以被模拟器使用，也可以被实际硬件使用。

## 1.Goldfish的FrameBuffer驱动程序
Goldfish中的FrameBuffer驱动程序保存文件`drivers/video/goldfishfb.c`中。此文件主要实现如下:

```c
//验证函数
static int goldfish_fb_check_var(struct fd_var_screeninfo*var,struct fb_info*info){
	if((var->rotate&1)!=(info->var.rotate&1)){
		if((var->xres!=info->var.yres)||(var->yres!=info->var.xres)||(var->xres_virtual!=info->var.yres)||
		(var->yres_virtual>info->var.xres*2)||
		(var->yres_virtual<info->var.xres){
			return -EINVAL;
		}
	}else{
		if((var->xres!=info->var.xres)||
		(var->yres!=info->var.yres)||
		(var->xres_virtual!=info->var.xres)||
		(var->yres_virtual>info->var.yres*2)||
		(var->yres_virtual<info->var.yres)){
			return -EINVAL;
		}
	}
	if((var->xoffset!=info->var.xoffset)||
		(var->bits_per_pixel!=info->var.bits_per_pixel)||
		(var->grayscale!=info->var.grayscale)){
		return -EINVAL;
	}
	return 0;
}
//程序初始化函数
static int goldfish_fb_probe(struct platform_device*pdev){
	int ret;
	struct resource*r;
	struct goldfish_fb*fb;
	size_t framesize;
	uint32_t width,height;
	dma_addr_t fbpaddr;
	fb=kzalloc(sizeof(*fb),GFP_KERNEL);
	if(fb=NULL){
		ret=-ENOMEM;
		goto err_fb_alloc_failed;
	}
	spin_lock_init(&fb_lock);
	init_waitqueue_head(&fb->wait);
	platform_set_drvdata(pdev,fb);
	r=platform_get_resource(pdev,IORESOURCE_MEM,0);
	if(r==NULL){
		ret=-ENODEV;
		goto err_no_io_base;
	}
	fb->reg_base=IO_ADDRESS(r->start-IO_START);
	fb->irq=platform_get_irq(pdev,0);
	if(fb->irq<0){
		ret=-ENODEV;
		goto -err_no_irq;
	}
	width=readl(fb->reg_base+FB_GET_WIDTH);
	height=readl(fb->reg_base+FB_GET_HEIGHT);
	fb->fb.fbops=&goldfish_fb_ops;
	fb->fb.flags=FBINFO_FLAG_DEFAULT;
	fb->pseudo_palette=fb->cmap;
	//strncpy(fb->fb.fix.id,clcd_name,sizeof(fb->fb.fix.id));
	fb->fb.fix.type =FB_TYPE_PAKCED_PIXELS;
	fb->fb.fix.visual=FB_VISUAL_TRUESOLOR;
	fb->fb.fix.line_lenght=width*2;//RGB565每个像素占用16位，两个字节
	fb->fb.fix.accel=FB_ACCEL_NONE;
	fb->fb.fix.ypanstep=1;
	
	fb->fb.var.xres=width;//实际显示区域
	fb->fb.var.yres=height;
	fb->fb.var.xres_virtual=width;//虚拟显示区域
	fb->fb.var.yres_virtual=height*2;
	fb->fb.var.bits_per_pixel=16;
	fb->fb.var.activate=FB_ACTIVATE_NOW;
	fb->fb.var.height=readl(fb->reg_base+FB_GET_PHYS_HEIGHT);
	fb->fb.var.width=readl(fb->reg_base+FB_GET_PHYS_WIDTH);

	fb->fb.var.red.offset=11;
	fb->fb.var.red.length=5;
	fb->fb.var.green.offset=5;
	fb->fb.var.green.length=6;
	fb->fb.var.blue.offset=0;
	fb->fb.var.blue.length=5;
	//显示缓冲区大小
	framesize=width*height*2*2；
	//进行内存映射
	fb->fb.screen_base=dma_alloc_writecombine(&pdev->dev,framesize,&fbpaddr,GFP_KERNEL);
	printk("allocating frame buffer %d*%d ,got %p \n",width,height,fb->fb.screen_base);
	if(fb->fb.screen_base==0){
		ret=-ENOMEM;
		goto err_alloc_screen_base_failed;
	}
	fb->fb.fix.smem_start=fbpaddr;
	fb->fb.fix.smem_len=framesize;
	ret=fb_set_var(&fb->fb,&fb->fb.var);
	if(ret){
		goto err_fb_set_var_failed;
	}
	ret=request_irq(fb->irq,goldfish_fb_interrupt,IRQF_SHARED,pdev->name,fb);
	if(ret){
		goto err_request_irq_failed;
	}
	writel(FB_INT_BASE_UPDATE_DONE,fb->req_base+FB_INT_ENABLE);
	goldfish_fb_pan_display(&fb->fb.var,&fb->fb);//update base
	ret=register_framebuffer(&fb->fb);
# ifdef CONFIG_ANDROID_POWER
	fb->early_suspend.suspend=goldfish_fb_early_suspend;
	fb->early_suspend.resume=goldfish_fb_late_resume;
	android_register_early_suspend(&fb->early_suspend);
#endif
}
```
上述通过FrameBuffer驱动程序实现了对RGB565颜色空间的支持，其中虚拟显示的y值是实际显示的2倍，这样就实现了双缓冲功能。

## 1.2 使用Gralloc模块的驱动程序
不同的硬件有不同的硬件图形加速设备和缓冲内存实现方法。Android Gralloc动态抽象的任务是消除不同 的设备之间的差别，在上层看来都是同样的方法和对象。在Moudle层隐藏缓冲区操作细节。Android使用了动态链接聚gralloc.xxx.so来封装底层实现细节。

默认Gralloc模块的实现源码保存在`hardware/libhardware/modules/gralloc/`，Android Gralloc模块主要有如下三个实现文件:

* `gralloc.cpp`：其中实现了`gralloc_module_t`模块和`alloc_device_t`设备。
* `mapper.cpp`:其中实现了工具函数。
* `framebuffer.cpp`:其中实现了`alloc_device_t`设备.

### 1.2.1 文件gralloc.cpp
定义函数gralloc_device_open()，模块打开函数.

```c
int gralloc_device_open(const hw_module_t*module,const char*name,hw_device_t**device){
	int status=-EINVAL;
	if(!strcmp(name,GRALLOC_HARDWARE_ID)){
		gralloc_context*dev;
		dev=(gralloc_context_t*)malloc(sizeof(*dev));
		//初始化状态
		memset(dev,0,sizeof(*dev));
		//初始化procs
		dev->device.common.tag=HARDWARE_DEVICE_TAG;
		dev->device.common.version=0;
		dev->device.common.module=const_cat<hw_module_t*>(module);
		dev->common->close=gralloc_close;
		dev->device.alloc=gralloc_alloc；
		dev->device.free=gralloc_free;
		status=0;
	}else{
		//打开framebuffer_device_t设备
		status=fb_device_open(module,name,device);
	}
	return status;
}
```
定义函数`gralloc_alloc_framebuffer_locked()`：
```c
static int gralloc_alloc_framebuffer_locked(alloc_device_t*dev,size_t size,int usage,buffer_handle_t *pHandle){
	private module_t*m=reinterpret_cast<private_moudle_t*>(dev->common.module);
	//分配framebuffer
	if(m->framebuffer==NULL){
		//initialize the framebuffer,the framebuffer is mapped once and forever
		int err=mapFrameBufferLocked(m);
		if(err<0){
			return err;
		}
	}
}
```
定义函数`gralloc_alloc_buffer()`
```c
static int gralloc_alloc_buffer(alloc_device_t*dev,size_t size,int usage,buffer_handle_t*pHandle){
    int err=0;
    int flags=0;
    int fd=-1;
    void*base=0;
    int offset=0;
    int lockState=0;
    size=roundUpToPageSize(size);
#if HAVE_ANDROID_OS //should probably define HAVE_PMEM somewhere
    if(usage & GRALLOC_USAGE_HW_TEXTURE){
        //enable pmem in that case,so our software GL can fallback to 
        //to copybit module
        flags|=private_handle_t::PRIV_FLAGS_USES_PMEM;
    }
    if(usage & GRALLOC_USAGE_HW_2D){
        flags|=private_handle_t::PRIV_FLAGS_USERS_PMEM;
    }
    if((flags & private_handle_t::PRIV_FLAGS_USES_PMEM)==0){
try_ashmem:
        fd=ashmem_create_region("gralloc-buffer",size);
        if(fd<0){
            LOGE("couldn't create ashem(%s)\n",strerror(-errno));
            err=-errno;
        }
    }else{
        private_module_t* m=reinterpret_cast<private_module_t*>(dev->common.module);
        err=init_pmem_area(m);
        if(err==0){
            //PMEM 缓冲总是mmaped
            base=m->pmem_master_base;
            lockState|=private_handle_t::LOCK_STATE_MAPPED;
            offset=aAllocator.allocate(size);
            if(offset<0){
                //没有pmem记忆
                err=-ENOMEM;
            }else{
                struct pmem_region stub={offset,size};
                //创造
                fd=open("/dev/pmem",O_RDWR,0);
                err=fd<0?fd:0;
                //并且连接它
                if(err==0)
                    err=ioctl(fd,PMEM_CONNECT,m->pmem_master);
                //并且安排可用客户端过程
                if(err==0)
                    err=ioctl(fd,PMEM_MAP,&stub);
                if(err<0){
                    err=-errno;
                    close(fd);
                    sAllocator.deallocate(offset);
                    fd=-1;
                }
                memset((char*)base+offset,0,size);
            }
        }else{
            if((usage & GRALLOC_USAGE_HW_2D)==0){
                //访问这不会请求PMEM，因此可能尝试其他方式
                flags&=~privte_handle_t::PRIV_FLAGS_USES_PMEM;
                err=0;
                goto try_ashmem;
            }else{
                LOGE("couldn't open pmem (%s)",strerror(-errno));
                err=-errno;
            }
        }
    }
#else // HAVE ANDROID_OS
    if(err==0){
        private_handle_t *hnd=new private_handle_t(fd,size,flags);
        hnd->offset=offset;
        hnd->base=int(base)+offset;
        hnd->lockState=lockState;
        *pHandle=hnd;
    }
#endif
    LOG_IF(err,"gralloc failed err=%s",strerror(-err));
    return err;
}
```
上述代码流程:

* 如果系统没有PMEM，则直接赋值fd=ashmem_create_region("gralloc-buffer",size);
* 如果有PMEM，则init_pmem_area(m);
* 获取本次需要的size:offset=sAllocator.allocate(size);
* 建立PMEM region:struct pmem_region sub={offset,size};
* 重新打开:fd=open("/dev/pmem",O_RDWR,0);
* 链接空间:err=ioctl(fd,PMEM_CONNECT,m->pmem_master);
* 获取句柄:private_handle_t *hnd=new private_handle_t(fd,size,flags);
在文件gralloc.cpp中，结构类型private_handle_t扩展了gralloc_module_t结构体，次机构提在gralloc_priv.h中定义:

```c
struct private_module_t{
    gralloc_module_t base;
    private_handle_t*framebuffer;
    uint32_t flags;
    uint32_t numBuffers;
    uint32_t bufferMask;
    pthread_mutex_t lock;
    buffer_handle_t currentBuffer;
    int pmem_master;
    void* pmem_master_base;
    struct fb_var_screeninfo info;
    struct fb_fix_screeninfo finfo;
    float xdpi;
    float ydpi;
    float fps;
    enum{
        PRIV_USAGE_LOCKED_FOR_POST=0x80000000
};
```

### 1.2.2 文件master.cpp
文件master.cpp中定义结构体gralloc_module_t具体实现。
定义函数`gralloc_register_buffer()`,建立一个新的private_handle_t对象，如果不是本进程调用则赋初值.

```c
int gralloc_register_buffer(gralloc_module_t const*module,buffer_handle_t handle){
    if(private_handle_t::validate(handle)<0){
        return -EINVAL;
    }
    private_handle_t*hnd=(private_handle_t*)handle;
    if(hnd->pid!=getpid()){
        hnd->base=0;
        hnd->lockState=0;
        hnd->writeOwner=0;
    }
    return 0;
}
```
定义函数`gralloc_unregiser_buffer()`，功能是通过validate判断handle是否合法

```c
int gralloc_unregiser_buffer(gralloc_module_t const*module,buffer_handle_t handle){
    if(private_handle_t::validate(handle)<0){
        return -EINVAL;
    }
    private_handle_t*hnd=(private_handle_t*)handle;

    LOGE_IF(hnd->lockState & private_handle_t::LOCK_STATE_READ_MASK,"(unregiser) handle %p still locked (state=%08x)",hnd,hnd->lockState);
    //never unmap buffers that were created in this process
    if(hnd->pid!=getpid()){
        if(hnd->lockState&private_handle_t::LOCK_STATE_MAPPED){
            gralloc_unmap(module,handle);
        }
        hnd->base=0;
        hnd->lockState=0;
        hnd->writeOwner=0;
    }
    return 0;
}
```

### 1.2.3 文件framebuffer.cpp
文件framebuffer.cpp用于实现设备framebuffer_device_t,其核心代码和Donut之前版本的EGLDisplaySurface.cpp文件实现类似，不同的是文件framebuffer.cpp中使用双缓冲的实现方式.

```c
int fb_device_open(hw_module_t const*module,const char*name,hw_device_t**device){
    int status=-EINVAL;
    if(!strcmp(name,GRALLOC_HARDWARE_FB0)){
        alloc_device_t *gralloc_device;
        status = gralloc_open(module,&gralloc_device);
        if(status<0)
            return status;
        //初始化状态
        fb_context_t* dev=(fb_context_t*)malloc(sizeof(*dev));
        memset(dev,0,sizeof(*dev));

        //初始化procs
        dev->device.common.tag=HARDWARE_DEVICE_TAG;
        dev->device.common.version=0;
        dev->deivce.common.module=const_cast<hw_module_t*>(module);
        dev->device.common.close=fb_close;
        dev->device.setSwapInterval=fb_setSwapInterval;
        dev->device.post=fb_post;
        dev->device.setUpdateRect=0;

        private_module_t* m=(private_module_t*)module;
        status=mapFrameBuffer(m);//映射FrameBuffer设备
        if(status>=0){//填充设备framebuffer_device_t的各个内容
            int stride=m->finfo.line_lenght/(m->info.bits_per_pixel>>3);
            const_cast<uint32_t&>(dev->device.flags)=0;
            const_cast<uint32_t&>(dev->device.width)=m->info.xres;
            const_cast<uint32_t&>(dev->device.height)=m->info.yres;
            const_cast<int&>(dev->device.stride)=stride;
            const_cast<int&>(dev->device.format)=HAL_PIXEL_FORMAT_RGB_565;
            const_cast<float&>(dev->device.xdpi)=m->xdpi;
            const_cast<float&>(dev->device.ydpi)=m->ydpi;
            const_cast<float&>(dev->device.fps)=m->fps;
            const_cast<int&>(dev->device.minSwapInterval)=1;
            const_cast<float&>(dev->device.maxSwapInterval)=1;
            *device=&dev->device.common;

        }
    }
    return status;
}
```

定义函数`mapFrameBufferLocked()`,功能实现打开FramBuffer设备正真功能.

```c
int mapFrameBufferLocked(struct private_module_t*module){
    //已经初始化...
    if(module->framebuffer){
        return 0;
    }
    char const* const device_template[]={
        "/dev/graphics/fb%u",
        "/dev/fb%u",
        0
    };
    int fd=-1;
    int i=0;
    char name[64];
    while((fd==-1)&&device_template[i]){
        snprintf(name,64,device_template[i],0);
        fd=open(name,O_RDWR,0);
        i++;
    }
    if(fd<0)
        return -errno;
    struct fb_fix_screeninfo finfo;
    if(ioctl(fd,FBIOGET_FSCREENINFO,&finfo)==-1)
        return -errno;
    struct fb_var_screeninfo info;
    if(ioctl(fd,FBIOGET_VSCREENINFO,&info)==-1)
        return -errno;
    info.reserved[0]=0;
    info.reserved[1]=0;
    info.reserved[2]=0;
    info.xoffset=0;
    info.yoffset=0;
    info.activate=FB_ACTIVATE_NOW;
    info.bits_per_pixel=16;
    info.red.offset=11;
    info.red.length=5;
    info.green.offset=5;
    info.green.length=6;
    info.blue.offset=0;
    info.blue.length=5;
    info.transp.offset=0;
    info.transp.length=0;
    info.yres_virtual=info.yes*NUM_BUFFERS;
    uint32_t flags=PAGE_FLIP;
    if(ioctl(fd,FBIOPUT_VSCREENINFO,&info)==-1){
        info.yres_virtual=info.yres;
        flags&=~PAGE_FLIP;
        LOGW("FBIOPUT_VSCREENINFO failed ,page flipping not supported");
    }
    if(info.yres_virtual<info.yres*2){
        //需要至少2也翻转
        info.yres_virtual=info.yres;
        flags&=~PAGE_FLIP;
        LOGW("page flipping not supported (yres_virtaul=%d ,reuqested=%d)",info.yres_virtual,info.yres*2);
    }
    if(ioctl(fd,FBIOGET_VSCREENINFO,&info)==-1)
        return -errno;
    int refreshRate=1000000000000LLU/
    (
        uint64_t(info.upper_margin+info.lower_margin+info.yres)
        *(info.left_margin+info.right_margin+info.xres)
        *info.pixclock
    );
    if(refreshRate == 0){
        refreshRate = 60*1000;//60Hz
    }

    if(int(info.width)<=0 || int(info.height)<=0){
        //驱动没有返回信息，默认为160dpi
        info.width=((info.xres*25.4f)/160.0f+0.5f);
        info.height=((info.yres*25.4f)/160.0f+0.5f);
    }
    float xdpi=(info.xres*25.4f)/info.width;
    float ydpi=(info.yres*25.4f)/info.height;

    if(ioctl(fd,FBIOGET_FSCREENINFO,&finfo)==-1)
        return -errno;
    if(finfo.smem_len<=0)
        return -errno;

    module->flags=flags;
    module->info=info;
    module->finfo=finfo;
    moudle->xdpi=xdpi;
    module->ydpi=ydpi;
    module->fps=fps;
    int err;
    size_t fbSize=roundUpToPageSize(finfo.line_lenght*info.yres_virtual);
    module->framebuffer=new private_handle_t(dup(fd),fbSize,private_handle_t::PRIV_FLAGS_USES_PMEM);
    module->numBuffers=info.yres_virtual/info.yres;
    module->bufferMask=0;
    void*vaddr=mmap(0,fbSize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(vaddr==MAP_FAILED){
        return -errno;
    }
    module->framebuffer->base=intptr_t(vaddr);
    memset(vaddr,0,fbSize);
    return 0;
}
```
完成的流程是：

* 打开FrameBuffer设备
* 判断是否支持PAGES_FLIP
* 计算刷新率
* 打印Gralloc信息
* 填充private_module_t

定义函数fb_post()，功能将某个缓冲区显示在屏幕上.

```c
static int fb_post(struct frambuffer_device_t*dev,buffer_handle_t buffer){
    if(private_handle_t::validate(buffer)<0){
        return -EINVAL;
    }
    fb_context_t*ctx=(fb_context_t*)dev;
    private_handle_t const*hnd=reinterpret_cast<private_handle_const*>(buffer);
    private_module_t*m=reinterpret_cast<private_module_t*>(dev->common.module);
    if(m->currentBuffer){
        m->base.unlock(&m->base,m->currentBuffer);
        m->currentBuffer=0;
    }

    if(hnd->flags&private_handle_t::PRIV_FLAGS_FRAMEBUFFERS){
        m->base.lock(&m->base,buffer,private_module_t::PRIV_USAGE_LOCKED_FOR_POST,0,0,m->info.xres,m->info.yres,NULL);    
        const size_t offset = hnd->base-m->framebuffer->base;
        m->info.activate=FB_ACTIVATE_VBL;
        m->info.yoffset=offset/m->finfo.line_length;
        if(ioctl(m->framebuffer->fd,FBIOPUT_VSCREENINFO,&m->info)==-1){
            LOGE("FBIOPUT_VSCREENINFO failed");
            m->base.unlock(&m->base,buffer);
            return -errno;
        }
        m->currentBuffer=buffer;
    }else{
       //if we can't do the page_flip,just copy the buffer to the front
       //FIXME: use copybit HAL instread of memcpy
       
       void*fb_vaddr;
       void*buffer_vaddr;
       m->base.lock(&m->base,m->framebuffer,GRALLOC_USAGE_SW_WRITE_RARELY,0,0,m->info.xres,m->info.yres,&fb_vaddr);
       m->base.lock(&m->base,buffer,GRALLOC_USAGE_SW,READ_RERELY,0,0,m->indo.xres,m->info.yres,&buffer_vaddr);
       memcpy(fb_vaddr,buffer_vaddr,m->finfo.line_length*m->info.yres);
       m->base.ulock(&m->base,buffer);
       m->base.ulock(&m->base,m->framebuffer);
    }
    return 0;
}
```
上面代码检查buffer是否合法，然后进行类型转换处理。如果currentbuffer非空，则执行unlock解锁。
 **注意**：屏幕上显示的内容其实是通过硬件DMA读取显示缓冲区的数据，而在数据中写入显示缓冲区的数据。为了避免两个同时进行，Gralloc使用了科学合理的处理方式。

 * 锁定其中的一个后再写入内容，写完解锁。
 * 在解锁期间，此显示缓冲不能硬件DMA获取，在此期间另一个缓冲区被解锁。此时可以显示到屏幕上。
