#include <string.h>

#include <eeprom.h>

#include "netif/ethernetif.h" 
#include "lan8720.h"  
#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/mem.h"


//由ethernetif_init()调用用于初始化硬件
//netif:网卡结构体指针 
//返回值:ERR_OK,正常
//       其他,失败
static err_t low_level_init(struct netif *netif)
{
#ifdef CHECKSUM_BY_HARDWARE
	int i; 
#endif 
	netif->hwaddr_len = ETHARP_HWADDR_LEN; //设置MAC地址长度,为6个字节

	if (eeprom_get_pc_hwaddr(netif->pc_hwaddr))
	{
		printf("get_pc_hwaddr(): read eeprom failed\n");
		return ERR_VAL;
	}

	//初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
	netif->fpga_hwaddr[0]=lwipdev.fpga_mac[0];
	netif->fpga_hwaddr[1]=lwipdev.fpga_mac[1];
	netif->fpga_hwaddr[2]=lwipdev.fpga_mac[2];
	netif->fpga_hwaddr[3]=lwipdev.fpga_mac[3];
	netif->fpga_hwaddr[4]=lwipdev.fpga_mac[4];
	netif->fpga_hwaddr[5]=lwipdev.fpga_mac[5];

	netif->hwaddr[0]=lwipdev.mcu_mac[0];
	netif->hwaddr[1]=lwipdev.mcu_mac[1];
	netif->hwaddr[2]=lwipdev.mcu_mac[2];
	netif->hwaddr[3]=lwipdev.mcu_mac[3];
	netif->hwaddr[4]=lwipdev.mcu_mac[4];
	netif->hwaddr[5]=lwipdev.mcu_mac[5];
	netif->mtu=1500; //最大允许传输单元,允许该网卡广播和ARP功能

	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	
	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr); //向STM32F4的MAC地址寄存器中写入MAC地址
	ETH_DMATxDescChainInit(DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);
#ifdef CHECKSUM_BY_HARDWARE 	//使用硬件帧校验
	for (i = 0; i < ETH_TXBUFNB; i++)	//使能TCP,UDP和ICMP的发送帧校验,TCP,UDP和ICMP的接收帧校验在DMA中配置了
	{
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	}
#endif
	ETH_Start(); //开启MAC和DMA				
	return ERR_OK;
} 
//用于发送数据包的最底层函数(lwip通过netif->linkoutput指向该函数)
//netif:网卡结构体指针
//p:pbuf数据结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
static err_t low_level_output(struct netif *netif, const void *pkt, u16_t pkt_len)
{
	u8 res;
	int l = 0;

	u8 *buffer = (u8 *)ETH_GetCurrentTxBuffer();
	memcpy((void *)buffer, pkt, pkt_len);
	l = pkt_len;
	res = ETH_Tx_Packet(l);

	if (res == ETH_ERROR)
		return ERR_MEM;//返回错误状态

	return ERR_OK;
}

///用于接收数据包的最底层函数
//neitif:网卡结构体指针
//返回值:pbuf数据结构体指针
static void low_level_input(struct netif *netif, const void **pkt, u16_t *pkt_len)
{
	void *ptr;
	u16_t len;
	FrameTypeDef frame;
	u8 *buffer;

	frame = ETH_Rx_Packet();
	len = frame.length;//得到包大小
	buffer = (u8 *)frame.buffer;//得到包数据地址

	ptr = mem_malloc(len);
	memcpy(ptr, (const void *)buffer, len);
	*pkt = (const void *)ptr;

	*pkt_len = len;

	frame.descriptor->Status = ETH_DMARxDesc_OWN;//设置Rx描述符OWN位,buffer重归ETH DMA
	if((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)//当Rx Buffer不可用位(RBUS)被设置的时候,重置它.恢复传输
	{ 
		ETH->DMASR = ETH_DMASR_RBUS;//重置ETH DMA RBUS位
		ETH->DMARPDR = 0;//恢复DMA接收
	}

	return;
}

//网卡接收数据(lwip直接调用)
//netif:网卡结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
err_t ethernetif_input(struct netif *netif)
{
	err_t err;
	const void *pkt;
	u16_t pkt_len;

	low_level_input(netif, &pkt, &pkt_len);
	if (pkt_len == 0)
		return ERR_MEM;

	err=netif->input(pkt, pkt_len, netif);
	if (err != ERR_OK)
	{
		LWIP_DEBUGF(NETIF_DEBUG,("ethernetif_input: IP input error\n"));
		mem_free((void *)pkt);
		pkt = NULL;
	}

	return err;
} 
//使用low_level_init()函数来初始化网络
//netif:网卡结构体指针
//返回值:ERR_OK,正常
//       其他,失败
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL", (netif != NULL));

	netif->name[0] = IFNAME0; 	//初始化变量netif的name字段
	netif->name[1] = IFNAME1; 	//在文件外定义这里不用关心具体值
	netif->linkoutput = low_level_output;//ARP模块发送数据包函数
	low_level_init(netif); 		//底层硬件初始化函数
	return ERR_OK;
}














