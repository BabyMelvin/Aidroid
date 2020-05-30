# camera申请buf流程

以linux4.9版本代码arm架构进行介绍

## 1.VIDIOC_REQBUFS

在应用层通过ioctl传输VIDIOC_REQBUFS命令，将会传递count、type和memory等参数，通过ioctl将会执行到camera的驱动函数`vidioc_reqbufs()`。通过驱动的该函数，最终将会调用到videobuf2-v4l2.c的`vb2_core_reqbufs()`函数。`vb2_core_reqbufs()`函数的操作：

* 检查传递下来的count是否为0，如果是0，则为释放资源；
* 确认应用层传递下来的count是否超过内核限制；
* 通过vb2_queue的queue_setup()得到每个buf的大小以及planes等信息；
* 将通过`__vb2_queue_alloc(q, memory, num_buffers, num_planes, plane_sizes)`申请buf；

而__vb2_queue_alloc()函数又是怎样申请内存的呢？

* 1.首先的，将会为每个buf申请一个vb2_buffer结构体描述buf的结构信息；
* 2.如果申请的buf是MMAP的memory type，将会通过`__vb2_buf_mem_alloc(vb)`申请buf；

```c
static int __vb2_buf_mem_alloc(struct vb2_buffer *vb)
{
	/* camera是主控芯片接收数据，VIDEO_CAPTURE，所以dma_dir值为DMA_FROM_DEVICE */
	enum dma_data_direction dma_dir = q->is_output ? DMA_TO_DEVICE : DMA_FROM_DEVICE;
	
	void *mem_priv;
	int plane;
	int ret = -ENOMEM;

	/*
	 * Allocate memory for all planes in this buffer
	 * NOTE: mmapped areas should be page aligned
	 */
	for (plane = 0; plane < vb->num_planes; ++plane) {
		/* 申请的大小需要是页对齐 */
		unsigned long size = PAGE_ALIGN(vb->planes[plane].length);

		/* 在这里，将会调用queue的mem_ops的alloc成员函数申请buf，
		 * 这个mem_ops是在创建video节点的时候赋值，申请cma内存，
		 * 一般会赋值为vb2_dma_contig_memops，所以将会调用vb2_dc_alloc()函数 */
		mem_priv = call_ptr_memop(vb, alloc, q->alloc_devs[plane] ? : q->dev,
					q->dma_attrs, size, dma_dir, q->gfp_flags);
		if (IS_ERR(mem_priv)) {
			if (mem_priv)
				ret = PTR_ERR(mem_priv);
			goto free;
		}

		/* Associate allocator private data with this plane */
		vb->planes[plane].mem_priv = mem_priv;
	}
return 0;
free:
	/* Free already allocated memory if one of the allocations failed */
	for (; plane > 0; --plane) {
		call_void_memop(vb, put, vb->planes[plane - 1].mem_priv);
		vb->planes[plane - 1].mem_priv = NULL;
	}

	return ret;
}
```

`vb2_dc_alloc()`函数将会调用`dma_alloc_attrs()`函数进行申请cma内存，申请到的内存信息将保存在vb2_dc_buf结构体，而vb2_buffer类型的`vb->planes[plane].mem_priv`将保存这个结构体的信息。

回到`__vb2_queue_alloc()`函数，通过`__vb2_buf_mem_alloc()`申请到cma内存之后，通过`__setup_offsets()`完成offset参数填充，简单理解就是每个plane的偏移，第0个的从0开始，然后逐个加上相应plane的偏移。
最后，如果有设置，则是调用buf queue的buf_init函数进行相应的设置，填充相应信息之后，返回应用层。

# 2.VIDIOC_QUERYBUF
执行上面接收的ioctl之后，由于用户层传递的是V4L2_MEMORY_MMAP，所以，应用层将会通过`VIDIOC_QUERYBUF`查询这些buf的信息并通过mmap映射到用户空间。
在调用这个ioctl的时候，用户层将会传递和VIDIOC_REQBUFS时一致的type、memory信息，这些在内核将会查询是否一致，同时将会传递一个index，表示将要查询哪个buf的信息。

整个查询过程，将会通过`vb2_core_querybuf()`函数调用到v4l2_buf_ops的fill_user_buffer成员函数`__fill_v4l2_buffer()`。在该函数中，就是通过`to_vb2_v4l2_buffer()`函数从vb_buffer信息填充到v4l2_buffer并返回。

## 2.1 疑问1：`__vb2_buf_mem_alloc()`函数是如何申请到物理地址连续的内存块？

在该函数中，将会通过以下代码块，逐个申请plane的大小内存。

```c
mem_priv = call_ptr_memop(vb, alloc,
				q->alloc_devs[plane] ? : q->dev,
				q->dma_attrs, size, dma_dir, q->gfp_flags);
```
通过call_ptr_memop宏的封装，将会调用到注册video节点时绑定的vb2_queue mem_ops结构体的alloc成员函数。这个mem_ops一般的，是赋值为vb2_dma_contig_memops，而vb2_dma_contig_memops的alloc成员函数赋值为`vb2_dc_alloc()`函数。内存申请函数调用流程`vb2_dc_alloc()`—>`dma_alloc_attrs()`。


```c
static inline void *dma_alloc_attrs(struct device *dev, size_t size,
				       dma_addr_t *dma_handle, gfp_t flag,
				       unsigned long attrs)
{
	/* 通过get_dma_ops()获取申请dma buf的句柄，
	 * 如果定义了CONFIG_HAS_DMA，则到<asm/dma-mapping.h>
	 * 查找该函数，一般的都会有定义，所以将会调用get_dma_ops()
	 * --->__generic_dma_ops()。最后在__generic_dma_ops()中
	 * 将会根据dev的dma_ops是否定义，将优先使用该ops进行分配内存 */
	struct dma_map_ops *ops = get_dma_ops(dev);
	void *cpu_addr;

	BUG_ON(!ops);

	if (dma_alloc_from_coherent(dev, size, dma_handle, &cpu_addr))
		return cpu_addr;

	if (!arch_dma_alloc_attrs(&dev, &flag))
		return NULL;
	if (!ops->alloc)
		return NULL;

	/* 一般的，最后是通过这里分配DMA内存，那么，这里的ops是哪里的呢？ */
	cpu_addr = ops->alloc(dev, size, dma_handle, flag, attrs);
	debug_dma_alloc_coherent(dev, size, *dma_handle, cpu_addr);
	return cpu_addr;
}
```
上面说到的ops，是通过device获取的，而在camera驱动中，将会是把一个platform型的设备传递下来，所以，这里的device是platform_device中的device。而一般的，在创建platform devices的时候，会在`of_platform_device_create_pdata()`函数，在这个函数中，通过`of_dma_configure()`—>`arch_setup_dma_ops()`，在`arch_setup_dma_ops()`函数中将会设置`dev->archdata.dma_ops`的值，最后的，在`get_dma_ops(dev)`中返回的，就是`dev->archdata.dma_ops`的值了，该值被赋值为`arm_coherent_dma_ops`。所以申请DMA buf的时候，将会调用它的alloc函数`arm_coherent_dma_alloc()`，这个就已经是DMA buf申请的接口函数了。

通过`arm_coherent_dma_alloc()`函数申请的物理地址连续的内存块，`arm_coherent_dma_alloc()`返回的是该内存块在内核中的虚拟地址，而函数参数handle将会保存内存块的物理地址（通过`*handle`可获得物理地址的值）。
回到`vb2_dc_alloc()`函数，物理地址的值，将保存在struct vb2_dc_buf结构体的dma_addr变量中，而**cookie**将保存这里内存在内核中的虚拟地址，最后，`vb->planes[plane].mem_priv`指针指向这个struct vb2_dc_buf结构体。

### 疑问：为什么在VIDIOC_QUERYBUF返回的buf中mem_offset为偏移的大小，但是最后又可以通过该变量的值使用到buf对应的物理地址呢？

这个，都是因为我们应用在VIDIOC_QUERYBUF得到buf信息之后调用`mmap()`函数进行映射，这个映射的过程会调用到camera驱动中的mmap()函数，在这个函数将会完成mem_offset值的填充，这个时候就可以通过mem_offset使用buf的物理地址。

一般的，在camera驱动中，mmap函数都将会调用到`vb2_mmap()`函数。
在`vb2_mmap()`函数中，传递进来的vma参数，其中vma->vm_pgoff变量的值，是在应用层调用`mmap()`函数传递进来的offset值表示的偏移量，不过该偏移量的单位为页面的大小（PAGE_SIZE，也就是应用层的offset左移12位），所以在该函数将会通过以下代码段找到对应的struct vb2_buffer结构体并进行映射。

```c
/* 找到结构体对应的索引 */
ret = __find_plane_by_offset(q, off, &buffer, &plane);

/* 进行对应的映射，将调用到vb2_dc_mmap()函数，最后通过dma_mmap_attrs()函数进行映射 */
ret = call_memop(vb, mmap, vb->planes[plane].mem_priv, vma);
```

通过上面的函数调用之后，就可以在应用层拿到对应着这块内存的用户进程虚拟地址，但是，我们在应用层可以通过buf的mem_offset变量拿到物理地址的，这个变量，又是什么时候将会进行对应的物理地址赋值操作呢？

这个赋值的操作，是在应用层通过VIDIOC_QBUF将buf添加到驱动队列的时候，最终将会调用到`vb2_core_qbuf()`函数，在该函数中，有以下代码段：

```c
if (q->start_streaming_called)
		__enqueue_in_driver(vb);
```

通过`__enqueue_in_driver()`函数，将会调用vb2_queue的ops结构体中的`buf_prepare()`成员函数，该函数是由camera驱动实现的，一般的，在该函数中将会进行以下操作：

```c
vb->planes[i].m.offset = vb2_dma_contig_plane_dma_addr(vb, i);
```

所以，就是在qbuf的时候，调用buf_prepare()成员函数对offset进行赋值，接着应用层就可以通过该变量直接使用物理地址操作camera输出的buf数据。