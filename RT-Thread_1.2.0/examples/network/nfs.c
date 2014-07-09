#include <rtthread.h>
#include <lwip/netdb.h> /* Ϊ�˽�������������Ҫ����netdb.hͷ�ļ� */
#include <lwip/sockets.h> /* ʹ��BSD socket����Ҫ����sockets.hͷ�ļ� */


void net_demo_nfs(void)
{
    if (dfs_mount(RT_NULL, "/", "nfs", 0, RT_NFS_HOST_EXPORT) == 0)
    rt_kprintf("NFSv3 File System initialized!\n");
    else
    rt_kprintf("NFSv3 File System initialzation failed!\n");

    return ;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
/* ���dns_test������finsh shell�� */
FINSH_FUNCTION_EXPORT(net_demo_nfs, dns test eg:nfs());
#endif
