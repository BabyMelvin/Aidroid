/*
 * =====================================================================================
 *
 *       Filename:  IMyService.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/08/2018 10:03:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef I_MY_SERVICE_H
#define I_MY_SERVICE_H
#include <stdio.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/Binder.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
using namespace android;
namespace android{
    class IMyService:public IInterface{
        public:
            //使用宏，申明MyService
            DECLARE_META_INTERFACE(MyService);
            //定义方法
            virtual void sayHello()=0;
    };
    enum{
        //命令字段
        HELLO=1,
    };
    //申明客户端BpMyService
    class BpMyService:public BpInterface<IMyService>{
        public:
           BpMyService(const sp<IBinder>& impl); virtual void sayHello(); }
    //申明服务端BnMyService
    class BnMyService:public BnInterface<IMyService>{
        public:
            virtual status_t onTransact(uint32_t code,const Parcel&data,Parcel*reply);
            virtual void sayHello();
    }

}
#endif

