# Helloworld

需要自己手动写的文件：

```
IMelvin.hal
service.cpp
android.hardware.melvin@1.0-service.rc
```

其他文件都是通过脚本生成

## 1.先写`.hal`文件

然后执行：

```
$PACKAGE=android.hardware.melvin@1.0
$LOC=hardware/interfaces/melvin/1.0/default
$hidl-gen -o $LOC -Lc++-impl -randroid.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport $PACKAGE
$hidl-gen -o $LOC -Landroidbp-impl -randroid.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport $PACKAGE
```

然后执行脚本文件

```
$source hardware/interfaces/update-makefiles.sh 
```

会生成Android.bp和Android.mk文件

# 2.添加service和rc文件


注意：`.h`文件的具体引用参考Android.bp文件，out目录

# 3.编译生成的库文件

* `android.hardware.melvin@1.0`:这个是hal编译的接口文件库，`android.hardware.melvin@1.0.so`,**client端**
* `android.hardware.melvin@1.0-impl.so`:这个是default目录生成的，作为`service端`


# 4.manifest

```xml
<hal format="hidl">
	<name>android.hardware.melvin</name>
	<transport>hwbinder</transport>
	<version>1.0</version>
		<interface>
			<name>IMelvin</name>
			<instance>default</instance>
		</interface>
</hal>
```
