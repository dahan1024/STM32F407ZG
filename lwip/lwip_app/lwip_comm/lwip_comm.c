#include <stdio.h>

#include <common.h>
#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/mem.h"
#include "lwip/init.h"
#include "netif/ethernetif.h"
#include <malloc.h>
#include <drv_delay.h>
#include <fpga.h>

lwip_dev_t lwipdev;						//lwip控制结构体
struct netif lwip_netif;				//定义一个全局的网络接口

u32 lwip_localtime;			//lwip本地时间计数器,单位:ms

//lwip 默认IP设置
//lwipx:lwip控制结构体指针
void lwip_comm_default_ip_set(lwip_dev_t *lwipx)
{
//	u32 sn0;
//	sn0 = *(vu32 *)(0x1FFF7A10);	//获取STM32的唯一ID的前24位作为MAC地址后三字节
//
//	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
//	lwipx->mac[0] = 2;//高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
//	lwipx->mac[1] = 0;
//	lwipx->mac[2] = 0;
//	lwipx->mac[3] = (sn0 >> 16) & 0xFF;//低三字节用STM32的唯一ID
//	lwipx->mac[4] = (sn0 >> 8) & 0xFFF;
//	lwipx->mac[5] = sn0 & 0xFF;
	uint8_t fpga_mac[6];
	int32_t i;

	if (fpga_get_mac(fpga_mac) < 0)
	{
		error("Get fpga mac failed\n");
		return;
	}

	for (i = 0; i < 6; i++)
	{
		lwipx->fpga_mac[i] = fpga_mac[i];
		lwipx->mcu_mac[i] = fpga_mac[i];
	}

	lwipx->mcu_mac[1] += 1;
}

//LWIP初始化(LWIP启动的时候使用)
//返回值:0,成功
//      1,内存错误
//      2,LAN8720初始化失败
//      3,网卡添加失败.
u8 lwip_comm_init(void)
{
	struct netif *netif_init_flag;		//调用netif_add()函数时的返回值,用于判断网络初始化是否成功
	if (ETH_Mem_Malloc()) return 1;		//内存申请失败
	if (LAN8720_Init()) return 2;			//初始化LAN8720失败
	lwip_init();						//初始化LWIP内核
	lwip_comm_default_ip_set(&lwipdev);	//设置默认IP等信息

	printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",
			lwipdev.mcu_mac[0], lwipdev.mcu_mac[1], lwipdev.mcu_mac[2], lwipdev.mcu_mac[3], lwipdev.mcu_mac[4], lwipdev.mcu_mac[5]);

	netif_init_flag = netif_add(&lwip_netif, NULL, &ethernetif_init, &ethernet_input);//向网卡列表中添加一个网口
	
	if (netif_init_flag == NULL)
		return 3;//网卡添加失败
	else//网口添加成功后,设置netif为默认值,并且打开netif网口
	{
		netif_set_default(&lwip_netif); //设置netif为默认网口
		netif_set_up(&lwip_netif);		//打开netif网口
	}

	return 0;							//操作OK.
}

//当接收到数据后调用 
void lwip_pkt_handle(void)
{
	//从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理
	ethernetif_input(&lwip_netif);
}



























