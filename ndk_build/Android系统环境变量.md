# 1.构建系统定义的变量

* TARGET_ARCH:目标CPU体系结构的名称，例如arm
* TARGET_PLATFORM： 目标Android平台，android-3
* TARGET_ARCH_ABI: 目标体系和ABI名称：armeabi-v7a
* TARGET_ABI:目标平台和ABI的串联，android-3-armeabi-v7a

# 2.模块变量

* LOCAL_MODULE_FILENAME：可选变量，用来重新定义生成输出文件名称。可覆盖LOCAL_MODULE的值
* LOCAL_CPP_EXTENSION：C++可以指定多个扩展名，默认为cpp.  (cpp, cxx等）
* LOCAL_CPP_FEATURES：可选变量，C++特性
	* LOCAL_CPP_FEATURES := rtti
* LOCAL_C_INCLUDES: 可选目录列表，NDK安装路径相对路径
	* LOCAL_C_INCLUDES:=$(LOCAL_PATH)/include

* LOCAL_CFLAGS: 编译源文件被传入编译器
	* LOCAL_CFLAGS ：= -DNDEBUG -DPORT=1234
* LOCAL_CPP_FLAGS
* LOCAL_WHOLE_STATIC_LIBRARIES：LOCAL_STATIC_LIBRARIES变体，指明应该被包含在生成共享库中所有静态库内容，当有循环依赖关系，这个时候很有用。
* LOCAL_LDLIBS：链接标志可选列表。
	* LOCAL_LDFLAGS :=-llog
* LOCAL_ARM_MODE:默认情况使用16位指令生成
	* 用来设置为32位指令，LOCAL_ARM_MODE := arm
* LOCAL_ARM_NEON:ARM体系特有变量。制定源文件用高级单指令多数据流(Single Instruction Multiple Data SIMD， a.k.a NEON) 内联函数
	* LOCAL_ARM_NEON :=true  改变整个构建系统行为
	* LOCAL_SRC_FILES ：= file1.c file2.c.neon 只构建低啊有NEON内联函数特定文件
* LOCAL_DISABLE_NO_EXECUTE：可选变量，用来禁用 NX Bit安全特性（Never Execute永远不执行，隔离代码区和存储区）
	* LOCAL_DISABLE_NO_EXECUTE := true
* LOCAL_EXPORT_CFLAGS： 编译标志会添加到通用LOCAL_STATIC_LIBRARIES 或 LOCAL_SHARED_LIBRARIES 使用，谁去shared则添加到谁的LOCAL_CFLAGS
	* LOCAL_EXPORT_CPPFLAGS 类似
	* LOCAL_EXPROT_LDFLAGS 类似
	* LOCAL_EXPROT_C_INCLUDES 类似
* LOCAL_SHARED_COMMANDS：大量资源推荐设置为true
* LOCAL_FILTER_ASM:用来过滤LOCAL_SRC_FILES变量装配文件的应用程序

# 2.构建系统函数宏

* all-subdir-makefiles: 返回当前目录所有子目录下Android.mk构建文件列表
	* include $(call all-subdir-makefiles)
* this-makefile: 返回当前Android.mk构建文件的路径
* parent-makefile:返回包含当前构建文件的父Android.mk 构建文件的路径
* grand-parent-makefile