/*
 * =====================================================================================
 *
 *       Filename:  hello.c
 *
 *    Description:  hello driver
 *
 *        Version:  1.0
 *        Created:  06/05/2018 09:33:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Melvin");
MODULE_DESCRIPTION("hello world module\n");
static int __init  hello_init(void){
    printk(KERN_ERR "hello world\n");
    return 0;
}

static void  __exit hello_exit(void){
   printk(KERN_EMERG "hello exit\n"); 
}

module_init(hello_init);
module_exit(hello_exit);

