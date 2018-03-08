/*
 * =====================================================================================
 *
 *       Filename:  ClientDemo.cpp
 *
 *    Description:  客户端
 *
 *        Version:  1.0
 *        Created:  03/08/2018 01:12:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "IMyService.h"
int main(int argc,char*argv[]){
    sp<IServiceManager> sm=defaultServiceManager();
    sp<IBinder> binder=sm->getService(String16("service.myservice"));
    //将binder对象转换为强引用类型IMyService
    sp<IMyService> cs=interface_cast<IMyService>(binder);
    //利用binder引用调用远程方法sayHello()
    cs->sayHello();
    return 0;
}

