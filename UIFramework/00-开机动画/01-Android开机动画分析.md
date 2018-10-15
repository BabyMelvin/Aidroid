Android系统在启动的过程中，最多可以出现三个画面，每一个画面都用来描述一个不同的启动阶段。本文将详细分析这三个开机画面的显示过程，以便可以开启我们对Android系统UI实现的分析之路。

* 第一个开机画面是在内核启动的过程中出现的，它是一个静态的画面。
* 第二个开机画面是在init进程启动的过程中出现的，它也是一个静态的画面。
* 第三个开机画面是在系统服务启动的过程中出现的，它是一个动态的画面。

无论是哪一个画面，它们都是在一个称为帧缓冲区（frame buffer，简称fb）的硬件设备上进行渲染的。接下来，我们就分别分析这三个画面是如何在fb上显示的。

# 1.第一个开机画面的显示过程
Android系统的第一个开机画面其实是Linux内核的启动画面。在默认情况下，这个画面是不会出现的，除非我们在编译内核的时候，启用以下两个编译选项：

```
CONFIG_FRAMEBUFFER_CONSOLE
CONFIG_LOGO
```

* 第一个编译选项表示内核支持帧缓冲区控制台，它对应的配置菜单项为：`Device Drivers ---> Graphics support ---> Console display driver support ---> Framebuffer Console support`
* 第二个编译选项表示内核在启动的过程中，需要显示LOGO，它对应的配置菜单项为：`Device Drivers ---> Graphics support ---> Bootup logo`

帧缓冲区硬件设备在内核中有一个对应的驱动程序模块fbmem，它实现在文件`kernel/goldfish/drivers/video/fbmem.c`中，它的初始化函数如下所示：

```c
/** 
 *      fbmem_init - init frame buffer subsystem 
 * 
 *      Initialize the frame buffer subsystem. 
 * 
 *      NOTE: This function is _only_ to be called by drivers/char/mem.c. 
 * 
 */  
  
static int __init  
fbmem_init(void)  
{  
        proc_create("fb", 0, NULL, &fb_proc_fops);  
        if (register_chrdev(FB_MAJOR,"fb",&fb_fops))  
                printk("unable to get major %d for fb devs\n", FB_MAJOR);  
        fb_class = class_create(THIS_MODULE, "graphics");  
        if (IS_ERR(fb_class)) {  
                printk(KERN_WARNING "Unable to create fb class; errno = %ld\n", PTR_ERR(fb_class));  
                fb_class = NULL;  
        }  
        return 0;  
}  
```
这个函数首先调用函数proc_create在`/proc`目录下创建了一个fb文件，接着又调用函数register_chrdev来注册了一个名称为fb的字符设备，最后调用函数class_create在`/sys/class`目录下创建了一个graphics目录，用来描述内核的图形系统。
模块fbmem除了会执行上述初始化工作之外，还会导出一个函数register_framebuffer：

```
EXPORT_SYMBOL(register_framebuffer);  
```
这个函数在内核的启动过程会被调用，以便用来执行注册帧缓冲区硬件设备的操作，它的实现如下所示：

```c
/** 
 *      register_framebuffer - registers a frame buffer device 
 *      @fb_info: frame buffer info structure 
 * 
 *      Registers a frame buffer device @fb_info. 
 * 
 *      Returns negative errno on error, or zero for success. 
 * 
 */  
  
int  
register_framebuffer(struct fb_info *fb_info)  
{  
        int i;  
        struct fb_event event;  
        ......  
  
        if (num_registered_fb == FB_MAX)  
                return -ENXIO;  
  
        ......  
  
        num_registered_fb++;  
        for (i = 0 ; i < FB_MAX; i++)  
                if (!registered_fb[i])  
                        break;  
        fb_info->node = i;  
        mutex_init(&fb_info->lock);  
        fb_info->dev = device_create(fb_class, fb_info->device,  
                                     MKDEV(FB_MAJOR, i), NULL, "fb%d", i);  
        if (IS_ERR(fb_info->dev)) {  
                /* Not fatal */  
                printk(KERN_WARNING "Unable to create device for framebuffer %d; errno = %ld\n", i, PTR_ERR(fb_info->dev));  
                fb_info->dev = NULL;  
        } else  
                fb_init_device(fb_info);  
  
        ......  
  
        registered_fb[i] = fb_info;  
  
        event.info = fb_info;  
        fb_notifier_call_chain(FB_EVENT_FB_REGISTERED, &event);  
        return 0;  
}  
```

由于系统中可能会存在多个帧缓冲区硬件设备，因此，fbmem模块使用一个数组registered_fb保存所有已经注册了的帧缓冲区硬件设备，其中，每一个帧缓冲区硬件都是使用一个结构体fb_info来描述的。

我们知道，在Linux内核中，每一个硬件设备都有一个主设备号和一个从设备号，它们用来唯一地标识一个硬件设备。对于帧缓冲区硬件设备来说，它们的主设备号定义为FB_MAJOR（29），而从设备号则与注册的顺序有关，它们的值依次等于0，1，2等。

每一个被注册的帧缓冲区硬件设备在`/dev/graphics`目录下都有一个对应的设备文件fb，其中，表示一个从设备号。例如，第一个被注册的帧缓冲区硬件设备在`/dev/graphics`目录下都有一个对应的设备文件fb0。用户空间的应用程序通过这个设备文件就可以操作帧缓冲区硬件设备了，即将要显示的画面渲染到帧缓冲区硬件设备上去。

这个函数最后会通过调用函数fb_notifier_call_chain来通知帧缓冲区控制台，有一个新的帧缓冲区设备被注册到内核中来了。

帧缓冲区控制台在内核中对应的驱动程序模块为fbcon，它实现在文件`kernel/goldfish/drivers/video/console/fbcon.c`中，它的初始化函数如下所示：

```c
static struct notifier_block fbcon_event_notifier = {  
        .notifier_call  = fbcon_event_notify,  
};  
  
......  
  
static int __init fb_console_init(void)  
{  
        int i;  
  
        acquire_console_sem();  
        fb_register_client(&fbcon_event_notifier);  
        fbcon_device = device_create(fb_class, NULL, MKDEV(0, 0), NULL,  
                                     "fbcon");  
  
        if (IS_ERR(fbcon_device)) {  
                printk(KERN_WARNING "Unable to create device "  
                       "for fbcon; errno = %ld\n",  
                       PTR_ERR(fbcon_device));  
                fbcon_device = NULL;  
        } else  
                fbcon_init_device();  
  
        for (i = 0; i < MAX_NR_CONSOLES; i++)  
                con2fb_map[i] = -1;  
  
        release_console_sem();  
        fbcon_start();  
        return 0;  
}
```
这个函数除了会调用函数device_create来创建一个类别为graphics的设备fbcon之外，还会调用函数fb_register_client来监听帧缓冲区硬件设备的注册事件，这是由函数fbcon_event_notify来实现的，如下所示：

```c
static int fbcon_event_notify(struct notifier_block *self,  
                              unsigned long action, void *data)  
{  
        struct fb_event *event = data;  
        struct fb_info *info = event->info;  
        ......  
        int ret = 0;  
  
        ......  
  
        switch(action) {  
        ......  
        case FB_EVENT_FB_REGISTERED:  
                ret = fbcon_fb_registered(info);  
                break;  
        ......  
  
        }  
  
done:  
        return ret;  
}  
```
帧缓冲区硬件设备的注册事件最终是由函数fbcon_fb_registered来处理的，它的实现如下所示：

```c
static int fbcon_fb_registered(struct fb_info *info)  
{  
        int ret = 0, i, idx = info->node;  
  
        fbcon_select_primary(info);  
  
        if (info_idx == -1) {  
                for (i = first_fb_vc; i <= last_fb_vc; i++) {  
                        if (con2fb_map_boot[i] == idx) {  
                                info_idx = idx;  
                                break;  
                        }  
                }  
  
                if (info_idx != -1)  
                        ret = fbcon_takeover(1);  
        } else {  
                for (i = first_fb_vc; i <= last_fb_vc; i++) {  
                        if (con2fb_map_boot[i] == idx)  
                                set_con2fb_map(i, idx, 0);  
                }  
        }  
  
        return ret;  
}  
```
函数fbcon_select_primary用来检查当前注册的帧缓冲区硬件设备是否是一个主帧缓冲区硬件设备。如果是的话，那么就将它的信息记录下来。这个函数只有当指定了CONFIG_FRAMEBUFFER_CONSOLE_DETECT_PRIMARY编译选项时才有效，否则的话，它是一个空函数。

在Linux内核中，每一个控制台和每一个帧缓冲区硬件设备都有一个从0开始的编号，它们的初始对应关系保存在全局数组con2fb_map_boot中。控制台和帧缓冲区硬件设备的初始对应关系是可以通过设置内核启动参数来初始化的。在模块fbcon中，还有另外一个全局数组con2fb_map，也是用来映射控制台和帧缓冲区硬件设备的对应关系，不过它映射的是控制台和帧缓冲区硬件设备的实际对应关系。

全局变量first_fb_vc和last_fb_vc是全局数组con2fb_map_boot和con2fb_map的索引值，用来指定系统当前可用的控制台编号范围，它们也是可以通过设置内核启动参数来初始化的。全局变量first_fb_vc的默认值等于0，而全局变量last_fb_vc的默认值等于`MAX_NR_CONSOLES - 1`。


