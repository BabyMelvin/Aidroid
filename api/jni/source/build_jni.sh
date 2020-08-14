#!/bin/bash

# 编译java 文件
javac HelloJni.java  User.java

# 自动生成 HelloJni.h文件
javah -jni HelloJni

#编译 HelloJni.cpp 成so文件

# 执行命令
# java HelloJni
if [ -z $JAVA_HOME ];then
    JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64
fi

# 需执行so库所在目录
export LD_LIBRARY_PATH="."
echo "JAVA_HOME:$JAVA_HOME"
echo "LD_LIBRARY_PATH:$LD_LIBRARY_PATH"

g++ -I$JAVA_HOME/include -I$JAVA_HOME/include/linux HelloJni.cpp -shared -fPIC -o libhello.so
