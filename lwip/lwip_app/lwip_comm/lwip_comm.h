#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
#include "lan8720.h" 

#define LWIP_MAX_DHCP_TRIES		4   //DHCP服务器最大重试次数
   
//lwip控制结构体
typedef struct {
	u8 fpga_mac[6];			// fpga MAC地址
	u8 mcu_mac[6];			// mcu MAC地址
} lwip_dev_t;

extern lwip_dev_t lwipdev;	//lwip控制结构体

void lwip_pkt_handle(void);
	
void lwip_comm_default_ip_set(lwip_dev_t *lwipx);
u8 lwip_comm_init(void);

#endif













