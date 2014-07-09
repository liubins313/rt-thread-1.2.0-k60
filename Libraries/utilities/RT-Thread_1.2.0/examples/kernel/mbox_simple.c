/*
 * 程序清单：邮箱例程
 *
 * 这个程序会创建2个动态线程，一个静态的邮箱对象，其中一个线程往邮箱中发送邮件，
 * 一个线程往邮箱中收取邮件。
 */
#include <rtthread.h>
#include "tc_comm.h"

/* 指向线程控制块的指针 */
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

/* 邮箱控制块 */
static struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[128];

static char mb_str1[] = "I'm a mail!";
static char mb_str2[] = "this is another mail!";

/* 线程1入口 */
static void thread1_entry(void* parameter)
{
    unsigned char* str;

    while (1)
    {
        rt_kprintf("thread1: try to recv a mail\n");

        /* 从邮箱中收取邮件 */
        if (rt_mb_recv(&mb, (rt_uint32_t*)&str, RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: get a mail from mailbox, the content:%s\n", str);

            /* 延时10个OS Tick */
            rt_thread_delay(10);
        }
    }
}

/* 线程2入口 */
static void thread2_entry(void* parameter)
{
    rt_uint8_t count;
    rt_err_t  result;
    count = 0;
    while (1)
    {
        count ++;
        if (count & 0x1)
        {
            /* 发送mb_str1地址到邮箱中 */
            if(rt_mb_send(&mb, (rt_uint32_t)&mb_str1[0]) != RT_EOK)
            {
                rt_kprintf("mb send failed\r\n");
            }
        }
        else
        {
            /* 发送mb_str2地址到邮箱中 */
            if(rt_mb_send(&mb, (rt_uint32_t)&mb_str2[0]) != RT_EOK)
            {
                rt_kprintf("mb send failed\r\n");
            }
        }

        /* 延时20个OS Tick */
        rt_thread_delay(5);
    }
}

int mbox_simple_init()
{
    /* 初始化一个mailbox */
    rt_mb_init(&mb,
               "mbt",             /* 名称是mbt */
               &mb_pool[0],       /* 邮箱用到的内存池是mb_pool */
               sizeof(mb_pool)/4, /* 大小是mb_pool大小除以4，因为一封邮件的大小是4字节 */
               RT_IPC_FLAG_FIFO); /* 采用FIFO方式进行线程等待 */

    /* 创建线程1 */
    tid1 = rt_thread_create("t1",
                            thread1_entry, RT_NULL, /* 线程入口是thread1_entry, 入口参数是RT_NULL */
                            THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
    else
        tc_stat(TC_STAT_END | TC_STAT_FAILED);

    /* 创建线程2 */
    tid2 = rt_thread_create("t2",
                            thread2_entry, RT_NULL, /* 线程入口是thread2_entry, 入口参数是RT_NULL */
                            THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
    else
        tc_stat(TC_STAT_END | TC_STAT_FAILED);

    return 0;
}

int _tc_mbox_simple()
{
    /* 设置TestCase清理回调函数 */
//    tc_cleanup(_tc_cleanup);
    mbox_simple_init();

    /* 返回TestCase运行的最长时间 */
    return 100;
}
/* 输出函数命令到finsh shell中 */
FINSH_FUNCTION_EXPORT_ALIAS(_tc_mbox_simple, __cmd_mbox_simple,  a simple mailbox example);

