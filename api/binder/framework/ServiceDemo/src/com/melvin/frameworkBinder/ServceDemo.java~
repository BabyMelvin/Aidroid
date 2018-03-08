package com.melvin.frameworkBinder;

import android.os.Looper;
import android.os.SerivceManager;

public class ServerDemo{
    public static void main(String[] args){
        System.out.println("MyService start");
        //开启循环执行
        Looper.prepareMainLooper();
        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_FOREGROUND);
        //注册服务
        ServiceManager.addService("MySerivce",new MyService());
        Looper.loop();
    }
}
