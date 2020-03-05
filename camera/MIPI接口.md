# MIPI

MIPI（移动行业处理器接口）是Mobile Industry Processor Interface的缩写。MIPI（移动行业处理器接口）是MIPI联盟发起的为移动应用处理器制定的开放标准。

MIPI是差分串口传输，速度快，抗干扰。主流手机模组现在都是用MIPI传输，传输时使用4对差分信号传输图像数据和一对差分时钟信号；最初是为了减少LCD屏和主控芯片之间连线的数量而设计的，后来发展到高速了，支持高分辨率的显示屏，现在基本上都是MIPI接口了。

MIPI摄像头有三个电源：VDDIO（IO电源），AVDD（模拟电源），DVDD（内核数字电源），不同sensor模组的摄像头供电不同，AVDD有2.8V或3.3V的；DVDD一般使用1.5V或更高，不同厂家的设计不同，1.5V可能由sensor模组提供或外部供给，可以使用外部供电则建议使用外部供，电压需大于内部的DVDD；VDDIO电压应与MIPI信号线的电平一致，若信号线是2.8V电平，则VDDIO也应供2.8V，有些sensor模组也可以不供VDDIO，由内部提供。

补充说明：MIPI的camera接口叫`CSI`，MIPI的display接口叫`DSI`。

MIPI联盟的MIPI DSI规范
　　
## 1、名词解释

* DCS （DisplayCommandSet）：DCS是一个标准化的命令集，用于命令模式的显示模组。
* DSI， CSI （DisplaySerialInterface， CameraSerialInterface
	* DSI 定义了一个位于处理器和显示模组之间的高速串行接口。
	* CSI 定义了一个位于处理器和摄像模组之间的高速串行接口。
* D-PHY：提供DSI和CSI的物理层定义