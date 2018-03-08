/*
 * =====================================================================================
 *
 *       Filename:  ServerDemo.cpp
 *
 *    Description:  服务端，服务
 *
 *        Version:  1.0
 *        Created:  03/08/2018 01:08:10 PM
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

int main(int argc,char* argv){
    //获取service manager引用
    sp<IServiceManager> sm=defaultServiceManager();
    //注册名为“service.myservice”服务
    sm->addService(String16("service.myservice"),new BnMyService());
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return 0;
}

