接上完成

* windowManagerService
* SurfaceFlinger
* 多屏

# 3.WindowManagerService
## 3.1职责

*  计算窗口大小
*  计算窗口Z轴位置
*  管理输入法窗口
*  管理壁纸窗口
*  执行窗口切换

### 屏幕的基本结构

![屏幕的基本结构](image/03-01.jpg)

## 3.2 计算窗口大小 – Content Region

![计算窗口大小](image/03-02.jpg)

### 计算窗口大小 – Visible Region

![计算窗口大小](image/03-03.jpg)

## 3.3 计算窗口Z轴位置 – Window Stack

![Window Stack](image/03-04.jpg)

### 计算窗口Z轴位置 – 计算时机  

![](image/03-05.jpg)

### 计算窗口Z轴位置 – 计算公式

```
Z = Base Layer + WINDOW_LAYER_MULTIPLIER(5)
Base Layer = T * TYPE_LAYER_MULTIPLIER(10000) + TYPE_LAYER_OFFSET(1000)
```


### 计算窗口Z轴位置 – 窗口主类型

![窗口主类型](image/03-06.png)

### 计算窗口Z轴位置 – 窗口子类型

![窗口子类型](image/03-07.jpg)

## 3.4 管理输入法窗口

![](image/03-08.jpg)

###　输入法窗口在Window Stack的位置

![](image/03-09.jpg)

## 3.5 管理壁纸窗口

![管理壁纸窗口](image/03-10.jpg)

### 壁纸窗口在Window Stack的位置

![Window Stack的位置](image/03-11.jpg)

## 3.6 执行窗口切换

![](image/03-12.jpg)

### 执行窗口切换 – Starting Window

![](image/03-13.jpg)

### 执行窗口切换 – 动画

![](image/03-14.jpg)

# 4.SurfaceFlinger

## 职责 

* 分配图形缓冲区
* 合成图形缓冲区
* 管理VSync事件

## 渲染过程

![](image/04-01.jpg)

## 4.1 分配图形缓冲区

![](image/04-02.png)

## 4.2 合成图形缓冲区

![](image/04-03.png)

### HWComposer实例：高通MDP4.0

![](image/04-04.jpg)

### 合成图形缓冲区 – 可见性计算

![](image/04-05.jpg)

## 4.3 管理VSync事件

![](image/04-06.png)

# 5.Android多屏支持
从4.2开始支持多屏幕

![](image/05-01.png)

## 5.1 屏幕类型

* Primary Display
	* 设备自带的屏幕,由SurfaceFlinger管理

* External Display
	* 通过HDMI连接,由SurfaceFlinger监控和管理

* Virtual Display
	* 通过`Miracast连接`(基于Wifi Direct技术),由DisplayManagerService监控和管理

App通过`android.app.Presentation`接口在指定的屏幕上创建窗口.[Presentation](https://developer.android.com/reference/android/app/Presentation)

```java
DisplayManager displayManager=(DisplayManager)context.getSystemService(Context.DISPLAY_SERVICE);
Display[] presentationDisplay=displayManager.getDisplays(DisplayManager.DISPLAY_CATEGORY_PRESENTATION);
if(presentationDisplay.length>0){
	//如果有多个可选显示，弹框由用户选择。这里默认第一个，系统推荐的
	Display display=presentationDisplay[0];
	Presentation presentation=new MyPresentation(context,presentationDisplay);
	presentation.show();
}
```




