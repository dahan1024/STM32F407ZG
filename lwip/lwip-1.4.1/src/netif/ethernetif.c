#include <string.h>

#include <eeprom.h>

#include "netif/ethernetif.h" 
#include "lan8720.h"  
#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/mem.h"


//��ethernetif_init()�������ڳ�ʼ��Ӳ��
//netif:�����ṹ��ָ�� 
//����ֵ:ERR_OK,����
//       ����,ʧ��
static err_t low_level_init(struct netif *netif)
{
#ifdef CHECKSUM_BY_HARDWARE
	int i; 
#endif 
	netif->hwaddr_len = ETHARP_HWADDR_LEN; //����MAC��ַ����,Ϊ6���ֽ�

	if (eeprom_get_pc_hwaddr(netif->pc_hwaddr))
	{
		printf("get_pc_hwaddr(): read eeprom failed\n");
		return ERR_VAL;
	}

	//��ʼ��MAC��ַ,����ʲô��ַ���û��Լ�����,���ǲ����������������豸MAC��ַ�ظ�
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
	netif->mtu=1500; //��������䵥Ԫ,����������㲥��ARP����

	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
	
	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr); //��STM32F4��MAC��ַ�Ĵ�����д��MAC��ַ
	ETH_DMATxDescChainInit(DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);
#ifdef CHECKSUM_BY_HARDWARE 	//ʹ��Ӳ��֡У��
	for (i = 0; i < ETH_TXBUFNB; i++)	//ʹ��TCP,UDP��ICMP�ķ���֡У��,TCP,UDP��ICMP�Ľ���֡У����DMA��������
	{
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	}
#endif
	ETH_Start(); //����MAC��DMA				
	return ERR_OK;
} 
//���ڷ������ݰ�����ײ㺯��(lwipͨ��netif->linkoutputָ��ú���)
//netif:�����ṹ��ָ��
//p:pbuf���ݽṹ��ָ��
//����ֵ:ERR_OK,��������
//       ERR_MEM,����ʧ��
static err_t low_level_output(struct netif *netif, const void *pkt, u16_t pkt_len)
{
	u8 res;
	int l = 0;

	u8 *buffer = (u8 *)ETH_GetCurrentTxBuffer();
	memcpy((void *)buffer, pkt, pkt_len);
	l = pkt_len;
	res = ETH_Tx_Packet(l);

	if (res == ETH_ERROR)
		return ERR_MEM;//���ش���״̬

	return ERR_OK;
}

///���ڽ������ݰ�����ײ㺯��
//neitif:�����ṹ��ָ��
//����ֵ:pbuf���ݽṹ��ָ��
static void low_level_input(struct netif *netif, const void **pkt, u16_t *pkt_len)
{
	void *ptr;
	u16_t len;
	FrameTypeDef frame;
	u8 *buffer;

	frame = ETH_Rx_Packet();
	len = frame.length;//�õ�����С
	buffer = (u8 *)frame.buffer;//�õ������ݵ�ַ

	ptr = mem_malloc(len);
	memcpy(ptr, (const void *)buffer, len);
	*pkt = (const void *)ptr;

	*pkt_len = len;

	frame.descriptor->Status = ETH_DMARxDesc_OWN;//����Rx������OWNλ,buffer�ع�ETH DMA
	if((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)//��Rx Buffer������λ(RBUS)�����õ�ʱ��,������.�ָ�����
	{ 
		ETH->DMASR = ETH_DMASR_RBUS;//����ETH DMA RBUSλ
		ETH->DMARPDR = 0;//�ָ�DMA����
	}

	return;
}

//������������(lwipֱ�ӵ���)
//netif:�����ṹ��ָ��
//����ֵ:ERR_OK,��������
//       ERR_MEM,����ʧ��
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
//ʹ��low_level_init()��������ʼ������
//netif:�����ṹ��ָ��
//����ֵ:ERR_OK,����
//       ����,ʧ��
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL", (netif != NULL));

	netif->name[0] = IFNAME0; 	//��ʼ������netif��name�ֶ�
	netif->name[1] = IFNAME1; 	//���ļ��ⶨ�����ﲻ�ù��ľ���ֵ
	netif->linkoutput = low_level_output;//ARPģ�鷢�����ݰ�����
	low_level_init(netif); 		//�ײ�Ӳ����ʼ������
	return ERR_OK;
}














