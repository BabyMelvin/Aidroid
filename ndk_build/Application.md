用来描述应用程序需要哪些模块；

也定义所有模块的通用变量

* APP_MODULES:默认Android.mk文件声明的所有模块。 也可以覆盖，空格分开、构建模块列表
* APP_OPTIM：可设置为release或debug 改变生成二进制文件优化等级
* APP_CFLAGS： 任何C 和C++都传入到编译器中
* APP_CPPFLAGS: 任何C++编译器中