/*
 * =====================================================================================
 *
 *       Filename:  led.h
 *
 *    Description:  led 头文件
 *
 *        Version:  1.0
 *        Created:  03/09/2018 01:07:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <hardware/hardware.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <cutils/atomic.h>

struct led_module_t{
    struct hw_module_t common;
    int (*init_led)(struct led_control_device_t *dev);
};

struct led_control_device_t{
    struct hw_device_t common;
    //LED 设备文件描述符
    int fd;

    //支持的API,添加在这里
    int (*set_on)(struct led_control_device_t *dev,int32_t led);
    int (*set_off)(struct led_control_device_t*dev,int32_t led);
};
//#####################################################################

#define LED_HARDWARE_MODULE_ID "led"
//帮助的api
// 可以不用，调用的时候使用
static inline int led_control_open(const struct hw_module*module,struct led_control_device_t**device){
    return module->methods->open(module,LED_HARDWARE_MODULE_ID,(struct hw_device_t**)device);
}
