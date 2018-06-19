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