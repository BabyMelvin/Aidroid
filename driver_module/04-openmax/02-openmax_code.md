# 实现OpenMax IL层接口
在Android 系统中，主要使用了OpenMax的编码/解码器功能，这些功能主要是通过Openmax IL层接口实现的。

## 1.OpenMax IL层的接口
### 1.1 头文件
在OpenMax IL层的接口中定义了若干个头文件，这些头文件中定义了实现OpenMax IL层接口的内容：

* `OMX_Type.h`:OpenMax IL的数据类型的定义。
* `OMX_Core.h`:OpenMax IL核心的API
* `OMX_Component.h`：OpenMax IL组件相关的API
* `OMX_Audio.h`:音频相关的常量和数据结构。
* `OMX_IVCCommon.h`:图形和视频公共过的常量和数据结构。
* `OMX_Image.h`：图像相关的常量和数据结构。
* `OMX_Video.h`:视频相关的常量和数据结构。
* `OMX_Other.h`:其他数据结构，包括A/V同步。
* `OMX_Index.h`:OpenMax IL定义的数据结构索引。
* `OMX_ContentPipe.h`:内容的管道定义。

### 1.2 实现过程
在具体实现OpenMax IL层接口时，程序员主要任务是实现包含函数指针的结构体。

* 1.在`OMX_Component.h`中定义的OMX_COMPONENETTYPE结构体，这是OpenMax IL层的核心内容，表示一个组件：

```c
typedef struct OMX_COMPONENTTYPE
{
	OMX_U32 nSize;    //定义此结构体大小
	OMX_VERSIONTYPE nVersion; //版本号
	OMX_PTR 	pComponentPrivate; //此组件的私有数据指针
	//调用者(IL Client)设置的指针，用于保存它的私有数据，
	//回传给所有的回调者
	OMX_PTR     pApplicationPrivate;	
	OMX_ERRORTYPE(//下面函数指针返回OMX_core.h中对应的内容
		OMX_IN  OMX_HANDLETYPE hComponent,
		OMX_OUT OMX_STRING PComponentName,
		OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
		OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
		OMX_OUT OMX_UUIDTYPE* pComponentUUID
	);
	OMX_ERRORTYPE (*SendCommand)(//发送命令
		OMX_IN	OMX_HANDLETYPE hComponent,
		OMX_IN  OMX_COMMANDTYPE Cmd,
		OMX_IN	OMX_U32	pParaml,
		OMX_IN	OMX_PTR	pCmdData
	);
	OMX_ERRORTYPE (*GetParameter)(//获得参数
		OMX_IN OMX_HANDLETYPE hComponent,
		OMX_IN OMX_INDEXTYPE nParamIndex,
		OMX_INOUT OMX_PTR pComponentParameterStruture
	);
	OMX_ERRORTYPE (*SetParameter)(//设置参数
		OMX_IN OMX_HANDLETYPE hComponent,
		OMX_IN OMX_INDEXTYPE  nIndex,
		OMX_IN OMX_PTR pComponentParameterStructure
	);
	//获得适配和设置适配
	//GetConfig和SetConfig
	//转换成OMX结构的索引
	//GetExtensionIndex
	//获得组件当前的状态
	//GetState
	//用于连接到另一个组件
	//ComponentTunnelRequest
	//为某个端口使用Buffer
	//UseBuffer
	//为某个端口分配Buffer
	//FreeBuffer
	//组件消耗此Buffer
	//EmptyThisBuffer
	//组件填充此Buffer
	//FillThisBuffer
	//设置回调函数
	//SetCallbacks
	//反初始化组件
	//ComponentDeInit
	
}OMX_COMPONENTTYPE;
```
上述OMX_COMPONENTTYPE结构后，其中各个函数指针就是调用者可以使用的内容。各个函数指针和文件OMX_core.h中定义内容相对应。例如文件OMX_core.h中定义OMX_FreeBuffer的代码如下。

```cpp
#define OMX_FreeBuffer{
hComponet,
mPortIndex,
pBuffer) 
((OMX_COMPONENTTYPE*)hComponent)->FreeBuffer(
hComponent,
mPortIndex,
pBuffer)
```
在文件中OMX_core.h中定义OMX_FillThisBuffer代码如下:

```c
#define OMX_FillThisBuffer(
hComponet,
pBuffer)
((OMX_COMPONENTTYPE*)hComponent)->FillThisBuffer(
hComponent,
pBuffer)
```

在文件OMX_Component.h中定义端口类型为OMX_PORTDOMAINTYPE枚举类型：

```c
typedef enum OMX_PORTDOMAINTYPE{
	OMX_PortDomainAudio,	//音频类型端口
	OMX_PortDomainVideo,	//视频类型端口
	OMX_PortDomainImage,	//图像类型端口
	OMX_PortDomainOther,	//其他类型端口
	OMX_PortDomainKhtonsExtensions=0x6F00000,
	OMX_PortDomainVendorStartUnused=0x7F00000,
	OMX_PortDomainMax=0x7fffffff
}OMX_PORTDOMAINTYPE;
```

使用OMX_PARAM_PORTDEFINITIONTYPE类来定义端口具体内容（OMX_Component.h)

```c
typdef struct OMX_PARAM_PORTDEFININITONTYPE{
	OMX_U32 nSize;	//结构体大小
	OMX_VERSIONTYPE nVersion; //版本
	OMX_U32	nPortIndex;		//端口号
	OMX_DIRTYPE eDir;		//端口的方向
	OMX_U32	nBufferCountActual;	//为此端口实际分配的Buffer的数目
	OMX_U32	nBufferCountMin;	//此端口最小Buffer的数目
	OMX_U32 nBufferSize;		//缓冲区的字节数
	OMX_BOOL bEnabled;		//是否能用
	OMX_BOOL bPopulated;	//是否在填充
	OMX_PORTDOMAINTYPE eDomain;	//端口实际的内容，由类型确定具体结构
	union{
		OMX_AUDIO_PORTDEFINITIONTYPE auido;
		OMX_AUDIO_PORTDEFINITIONTYPE video;
		OMX_AUDIO_PORTDEFINITIONTYPE image;
		OMX_AUDIO_PORTDEFINITIONTYPE other;
	}format;
	OMX_BOOL bBuffersContiguous;
	OMX_U32  nBufferAlignment;
}OMX_PARAM_PORTDEFINITIONTYPE;
```

在文件OMX_Core.h中定义的枚举类型OMX_STATETYPE用于表示OpenMax的状态机：

```cpp
typedef enum OMX_STATETYPE{
	OMX_StateInvalid,	//如果组件检测到内部数据结构被破坏
	OMX_StateLoaded,	//组件被加载但是没有完全初始化
	OMX_StateIdel,		//组件初始化完成，准备开始
	OMX_StateExecuting,	//组件接收开始命令，正在建立数据
	OMX_StateWaitForResource,	//组件正在等待资源
	OMX_StateKhronsoExtension=0x6F000000，//保留
	OMX_StateVendorStartUnused=0x7F00000,//保留
}OMX_STATETYPE;
```

在OMX_Core.h中定义枚举类型OMX_COMMANDTYPE,此枚举表示组件命令类型;

```cpp
typedef enum OMX_COMMANDTYPE{
	OMX_CommandStateSet, //改变状态机器
	OMX_CommandFlush,	//刷新数据队列
	OMX_CommandPortDisable,//使能端口
	OMX_CommandMarkBuffer,//标记组件成Buffer用于观察
	OMX_CommandKhronosExtensions=0x6F0000,
	OMX_CommandVendorStartUnused=0x7F00000,
	OMX_CommandMax=0x7FFFFFF
}OMX_COMMANDTYPE;
```
