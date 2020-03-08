[深度好文](https://www.jianshu.com/p/5f538820e370)

1.Camera总体架构

* 1.1 Android系统架构
* 1.2 MTK Android Camera架构分层及代码结构

2.Camera HAL层分析

* 2.1 Camera HAL层的主要类介绍
* 2.2 Camera硬件抽象层的三种业务
* 2.2.1取景器preview(使用YUV原始数据格式，发送到视频输出设备)
* 2.2.2拍摄照片(可以使用原始数据或者压缩图像数据)
* 2.2.3 视频录制（将数据传送给视频编码器程序）

3.Camera Framework层分析

* 3.1 Camera Framwork整体架构和类关系简介
* 3.2 Camera Framwork层代码调用流程分析
	* 3.2.1应用层onCreate函数入口
	* 3.2.2 Framework层的open方法
	* 3.2.3 JNI层的调用
	* 3.2.4 Camera connect到服务
	* 3.2.5 Camera服务之Binder代理端
	* 3.2.6 Camera服务之CameraService分析
	* 3.2.7 Camera服务之Client分析
	* 3.2.8 Camera服务之Callback分析

4.Camera 应用层对Framework的封装分析

* 4.1 应用层对Framework层封装的层次和类介绍
* 4.2 camera的调用流程

5.Camera 应用初始化分析

6.Camera UI介绍

* 6.1 Camera UI总体预览
* 6.2 ModePicker类
* 6.3 IndicatorManager类
* 6.4 RemainingManager类
* 6.5 PickerManager类
* 6.6 ThumbnailManager类
* 6.7 ShutterManager类
* 6.8 SettingManager类
* 6.9 FocusManager类

7.Camera 预览流程

* 7.1 Camera预览命令流程
* 7.1.1 App层流程
* 7.1.2 Framework层流程
* 7.1.3 Binder调用
* 7.1.4 HAL层流程
* 7.2 数据流程
* 7.3 函数调用流程

8.Camera拍照流程

* 8.1 CameraService初始化过程
* 8.2应用程序链接相机服务过程
	* 8.2.1 Camera连接整个过程
	* 8.2.2 Camera takepicture过程
	* 8.2.3 Camera takepicture过程总结

9.Camera Feature 介绍和API使用

* 9.1 Face detection人脸检测
	* 9.1.1人脸检测主要介绍代码实现要素
	* 9.1.2调用流程，这里以普通模式为例
* 9.2人脸美化Face beauty
	* 9.2.1功能介绍
	* 9.2.2三个参数的客制化
	* 9.2.3常见问题
	* 9.2.4调用流程
* 9.3 ASD:自动场景检测
	* 9.3.1限制条件
	* 9.3.2常见问题
	* 9.3.3代码实现要素
* 9.4 Continue Shot
	* 9.4.1简单介绍与使用方法
	* 9.4.2 Continue Shot Spec and Limitation
	* 9.4.3 Continue Shot效果调试/客制化参数
	* 9.4.4 Continue Shot常见问题
* 9.5零延时拍照ZSD
	* 9.5.1简介
	* 9.5.2 ZSD Spec and Limitation
	* 9.5.3 ZSD客制化参数
	* 9.5.4 ZSD常见问题
* 9.6笑脸模式
	* 9.6.1简介
	* 9.6.2限制条件
	* 9.6.3 FD常见问题
* 9.7物体动态追踪Object Tracking
	* 9.7.1简介
	* 9.7.2说明与限制条件
	* 9.7.3 OT效果调试/客制化参数
* 9.8全景拍照
	* 9.8.1简介
	* 9.8.2规则和限制条件
	* 9.8.3全景拍照客制化参数
	* 9.8.4全景拍照常见问题
	* 9.8.5全景拍照相关问题debug流程
* 9.9多视角拍照（MAV）
	* 9.9.1简介
	* 9.9.2限制与规则
	* 9.9.3常见问题
* 9.10情景照片Live Photo
	* 9.10.1简介
	* 9.10.2适用场景及使用方法
	* 9.10.3限制条件
* 9.11 HDR
	* 9.11.1简介
	* 9.11.2 HDR 规则和限制条件
	* 9.11.3 HDR效果调试/客制化参数
	* 9.11.4 HDR常见问题

10.Camera中第三方算法添加

* 10.1 Camera HAL层预览
* 10.2 Camera图像缓冲Queue
* 10.3第三方算法预览部分
	* 10.3.1 ExtImgProc介绍
	* 10.3.2 相关类结构图
	* 10.3.3 ExtImpProc处理过程
	* 10.3.4三方算法对性能影响的简单分析
* 10.4拍照的第三方算法添加
	* 10.4.1移植capture的第三方算法
	* 10.4.2对ZSD 做第三方算法处理：(CapBufShot.cpp)
* 10.5 新增一个Capture Mode

11.拍照、录像接口调用及开发

* 11.1 调用系统Camera APP实现功能
	* 11.1.1 实现拍照
	* 11.1.2 实现摄像
* 11.2调用Camera API实现相应功能
	* 11.2.1添加权限
	* 11.2.2 实现拍照功能
	* 11.2.3实现摄像功能

12.语音拍照详解

* 12.1设置界面应用是否增加开启、关闭语言识别的开关
* 12.2增加第三方应用支持设置
* 12.3增加第三方应用支持的关键词
* 12.4更改默认设置
* 12.5更改默认语言
* 12.6增加新的语言支持
* 12.7增加语言拍照和语言设置的关联

13.Camera Performance问题分析初步

* 13.1 Camera startup time
	* 13.1.1 Camera startup time 的不同模式差异分析
	* 13.1.2 Camera startup time 分析LOG
	* 13.1.3 Camera startup time 的参考数据
* 13.2 Shot to Shot/shutter delay
	* 13.2.1 Shot to Shot/shutter delay分析LOG
	* 13.2.2参考数据
	* 13.2.3各阶段受客制化影响的主要因素
* 13.3 main/sub sensor switch time
	* 13.3.1 Shot to Shot/shutter delay分析LOG
	* 13.3.2参考数据
* 13.4目前平台可优化的地方
	* 13.4.1 Stop preview节省帧率
	* 13.4.1拍照回显定格时间较长

14.HQ Camera整体优化方案介绍

* 14.1改进思路
* 14.2用户需求（竞品机优势）
* 14.3 Camera基础效果优化（持续进行中）
* 14.4软件特效类 132
* 14.5其他提升体验的细节优化（进展）
* 14.6前摄选型要求
* 14.7夜拍效果提升选型要求
* 14.8提高拍照速度-修改原理及细节
* 14.9优化暗光下MFLL和MBF功能阀值
* 14.10 零延时拍照
* 14.11下一步计划