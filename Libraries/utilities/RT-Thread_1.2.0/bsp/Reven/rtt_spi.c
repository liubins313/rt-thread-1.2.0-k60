#include <rthw.h>
#include <rtthread.h>
#include "spi.h"
#include "gpio.h"

#include <rtdevice.h>
#include <drivers/spi.h>

#include "rtt_spi.h"
static struct rt_spi_bus kinetis_spi;

#define  RTT_SPI_DRIVER_DEBUG  1

#if (RTT_SPI_DRIVER_DEBUG == 1)
#define RTT_SPI_DRIVER_TRACE	rt_kprintf
#else
#define RTT_SPI_DRIVER_TRACE(...)
#endif


static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration)
{
    SPI_InitTypeDef SPI_InitStruct1;
    /* data_width */
    if(configuration->data_width <= 8)
    {
        SPI_InitStruct1.dataSize = 8;
    }
    else if(configuration->data_width <= 16)
    {
        SPI_InitStruct1.dataSize = 16;
    }
    else
    {
        return RT_EIO;
    }
    /* baud */
    if(configuration->max_hz < 100)
    {
        SPI_InitStruct1.baudrate = 1000;
    }
    else if (configuration->max_hz > 50*1000*1000)
    {
        SPI_InitStruct1.baudrate = 50*1000*1000;
    }
    else
    {
        SPI_InitStruct1.baudrate = configuration->max_hz;
    }
    RTT_SPI_DRIVER_TRACE("spi bus baudrate:%d\r\n", SPI_InitStruct1.baudrate);
    /* frame foramt */
    SPI_InitStruct1.frameFormat = (SPI_FrameFormat_Type)(configuration->mode & 0x03);
    /* MSB or LSB */
    if(configuration->mode & RT_SPI_MSB)
    {
        SPI_InitStruct1.bitOrder = kSPI_MSBFirst;
    }
    else
    {
        SPI_InitStruct1.bitOrder = kSPI_LSBFirst;
    }
    SPI_InitStruct1.mode = kSPI_Master;
    SPI_InitStruct1.ctar = HW_CTAR0;
    SPI_InitStruct1.instance = HW_SPI2;
    SPI_Init(&SPI_InitStruct1);
    

    return RT_EOK;

}

static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
    struct rt_spi_configuration * config = &device->config;
    rt_uint32_t size = message->length;
    const rt_uint8_t * send_ptr = message->send_buf;
    rt_uint8_t * recv_ptr = message->recv_buf;
    struct kinetis_spi_cs * kinetis_spi_cs = device->parent.user_data;
    //rt_kprintf("spi size:%d\r\n", size);
    while(size--)
    {
        rt_uint16_t data = 0xFF;
        if(send_ptr != RT_NULL)
        {
            data = *send_ptr++;
        }
        /* ���һ�� ��������Ҫ�ͷ�CS */
        if((size == 0) && (message->cs_release))
        {  
            data = SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, data, kinetis_spi_cs->ch, kSPI_PCS_ReturnInactive);
        }
        else
        {
            data = SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, data, kinetis_spi_cs->ch, kSPI_PCS_KeepAsserted);
        }
        if(recv_ptr != RT_NULL)
        {
            *recv_ptr++ = data;
        }
    }
    return message->length;
}



static struct rt_spi_ops kinetis_spi_ops =
{
    configure,
    xfer
};

int rt_hw_spi_bus_init(uint32_t instance, const char *name)
{
    kinetis_spi.ops = &kinetis_spi_ops;
    
    return rt_spi_bus_register(&kinetis_spi, name, &kinetis_spi_ops); 
}



