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

lwip_dev_t lwipdev;						//lwip���ƽṹ��
struct netif lwip_netif;				//����һ��ȫ�ֵ�����ӿ�

u32 lwip_localtime;			//lwip����ʱ�������,��λ:ms

//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(lwip_dev_t *lwipx)
{
//	u32 sn0;
//	sn0 = *(vu32 *)(0x1FFF7A10);	//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
//
//	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
//	lwipx->mac[0] = 2;//�����ֽ�(IEEE��֮Ϊ��֯ΨһID,OUI)��ַ�̶�Ϊ:2.0.0
//	lwipx->mac[1] = 0;
//	lwipx->mac[2] = 0;
//	lwipx->mac[3] = (sn0 >> 16) & 0xFF;//�����ֽ���STM32��ΨһID
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

//LWIP��ʼ��(LWIP������ʱ��ʹ��)
//����ֵ:0,�ɹ�
//      1,�ڴ����
//      2,LAN8720��ʼ��ʧ��
//      3,�������ʧ��.
u8 lwip_comm_init(void)
{
	struct netif *netif_init_flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	if (ETH_Mem_Malloc()) return 1;		//�ڴ�����ʧ��
	if (LAN8720_Init()) return 2;			//��ʼ��LAN8720ʧ��
	lwip_init();						//��ʼ��LWIP�ں�
	lwip_comm_default_ip_set(&lwipdev);	//����Ĭ��IP����Ϣ

	printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n",
			lwipdev.mcu_mac[0], lwipdev.mcu_mac[1], lwipdev.mcu_mac[2], lwipdev.mcu_mac[3], lwipdev.mcu_mac[4], lwipdev.mcu_mac[5]);

	netif_init_flag = netif_add(&lwip_netif, NULL, &ethernetif_init, &ethernet_input);//�������б������һ������
	
	if (netif_init_flag == NULL)
		return 3;//�������ʧ��
	else//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	{
		netif_set_default(&lwip_netif); //����netifΪĬ������
		netif_set_up(&lwip_netif);		//��netif����
	}

	return 0;							//����OK.
}

//�����յ����ݺ���� 
void lwip_pkt_handle(void)
{
	//�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP����
	ethernetif_input(&lwip_netif);
}



























