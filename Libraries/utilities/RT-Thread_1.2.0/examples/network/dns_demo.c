#include <rtthread.h>
#include <lwip/netdb.h> /* Ϊ�˽�������������Ҫ����netdb.hͷ�ļ� */
#include <lwip/sockets.h> /* ʹ��BSD socket����Ҫ����sockets.hͷ�ļ� */


void dns_test(const char* url)
{
    struct hostent *h;

    /*  ȡ��������Ϣ */
    h= (struct hostent *) gethostbyname(url);
    if(h == NULL)
    {
        /*  gethostbynameʧ�� */
        rt_kprintf("Socket error\n");
        return;

    }
    /*  ��ӡ����ȡ�õ���Ϣ */
    rt_kprintf("Host name : %s\n", h->h_name);
    rt_kprintf("IP Address : %s\n", inet_ntoa (*((struct in_addr *)h->h_addr)));

    return ;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
/* ���dns_test������finsh shell�� */
FINSH_FUNCTION_EXPORT(dns_test, dns test);
#endif
