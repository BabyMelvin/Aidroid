 默认ndk-build 主项目中执行。-C 参数用于指定命令中NDK项目位置。

* ndk-build -C /path/to/the/project
* ndk-build -B 强制全部重新构建
* ndk-build clean
* ndk-build -j4
* ndk-build NDK_LOG=1  开启大量log