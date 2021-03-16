#include <stdio.h>

#include <stm32f4xx.h>
#include <drv_spi.h>
#include <drv_delay.h>

#include "w25qxx.h"

#define	W25QXX_SPI					SPI1
#define W25QXX_CS_DISABLE			SPI1_CS_DISABLE
#define W25QXX_CS_ENABLE			SPI1_CS_ENABLE

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector

static uint8_t SPI1_ReadWriteByte(uint8_t data);
static uint8_t w25qxx_read_sr(void);        			//��ȡ״̬�Ĵ���
//static void w25qxx_write_sr(uint8_t sr);				//дW25QXX״̬�Ĵ���
static void w25qxx_write_enable(void);  		//дʹ��
//static void w25qxx_write_disable(void);			//W25QXXд��ֹ
static void w25qxx_wait_busy(void);				//�ȴ�����
static void w25qxx_erase_sector(uint32_t sector_address);	//��������
static void w25qxx_write_page(uint32_t start_address, uint8_t *buf, uint16_t len);
static void w25qxx_write_no_check(uint32_t start_address, uint8_t *buf, uint16_t len);

static uint8_t SPI1_ReadWriteByte(uint8_t data)
{
	return spi_transfer(W25QXX_SPI, data);
}

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static uint8_t w25qxx_read_sr(void)
{
	uint8_t byte=0;
	W25QXX_CS_ENABLE;                            	//ʹ������
	SPI1_ReadWriteByte(W25X_ReadStatusReg);    		//���Ͷ�ȡ״̬�Ĵ�������
	byte = SPI1_ReadWriteByte(0xFF);             	//��ȡһ���ֽ�
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
	return byte;
}

#if 0
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
static void w25qxx_write_sr(uint8_t sr)
{
	W25QXX_CS_ENABLE;                         		//ʹ������
	SPI1_ReadWriteByte(W25X_WriteStatusReg);		//����дȡ״̬�Ĵ�������
	SPI1_ReadWriteByte(sr);               			//д��һ���ֽ�
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
}
#endif

//W25QXXдʹ��
//��WEL��λ
static void w25qxx_write_enable(void)
{
	W25QXX_CS_ENABLE;                            	//ʹ������
    SPI1_ReadWriteByte(W25X_WriteEnable);      		//����дʹ��
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
}

#if 0
//W25QXXд��ֹ
//��WEL����
static void w25qxx_write_disable(void)
{
	W25QXX_CS_ENABLE;                            	//ʹ������
    SPI1_ReadWriteByte(W25X_WriteDisable);     		//����д��ָֹ��
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
}
#endif

//�ȴ�����
static void w25qxx_wait_busy(void)
{
	while ((w25qxx_read_sr() & 0x01) == 0x01);		// �ȴ�BUSYλ���
}

//����һ������
//sector_address:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
static void w25qxx_erase_sector(uint32_t sector_address)
{
	//����falsh�������,������
 	printf("fe:%x\r\n", sector_address);
 	sector_address *= 4096;
    w25qxx_write_enable();                  		//SET WEL
    w25qxx_wait_busy();

  	W25QXX_CS_ENABLE;                            	//ʹ������
    SPI1_ReadWriteByte(W25X_SectorErase);      		//������������ָ��
    SPI1_ReadWriteByte((uint8_t)((sector_address) >> 16));  	//����24bit��ַ
    SPI1_ReadWriteByte((uint8_t)((sector_address) >> 8));
    SPI1_ReadWriteByte((uint8_t)sector_address);
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
    w25qxx_wait_busy();   				   			//�ȴ��������
}

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//start_address:��ʼд��ĵ�ַ(24bit)
//buf:���ݴ洢��
//len:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
static void w25qxx_write_page(uint32_t start_address, uint8_t *buf, uint16_t len)
{
 	uint16_t i;
    w25qxx_write_enable();                  		//SET WEL
    w25qxx_wait_busy();

	W25QXX_CS_ENABLE;                            	//ʹ������
    SPI1_ReadWriteByte(W25X_PageProgram);      		//����дҳ����
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 16)); 	//����24bit��ַ
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 8));
    SPI1_ReadWriteByte((uint8_t)start_address);
    for (i = 0; i < len; i++)
    	SPI1_ReadWriteByte(buf[i]);					//ѭ��д��
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
	w25qxx_wait_busy();					   			//�ȴ�д�����
}

//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//start_address:��ʼд��ĵ�ַ(24bit)
//buf:���ݴ洢��
//len:Ҫд����ֽ���(���65535)
//CHECK OK
static void w25qxx_write_no_check(uint32_t start_address, uint8_t *buf, uint16_t len)
{
	uint16_t bytes_to_write;

	bytes_to_write = 256 - start_address % 256; //��ҳʣ����ֽ���
	if (len <= bytes_to_write)
		bytes_to_write = len;					//������256���ֽ�

	while(1)
	{
		w25qxx_write_page(start_address, buf, bytes_to_write);
		if (len == bytes_to_write)
			break;							//д�������
	 	else //len>bytes_to_write
		{
			buf += bytes_to_write;
			start_address += bytes_to_write;
			len -= bytes_to_write;			  		//��ȥ�Ѿ�д���˵��ֽ���

			if (len > 256) bytes_to_write = 256; 	//һ�ο���д��256���ֽ�
			else bytes_to_write = len; 	  			//����256���ֽ���
		}
	}
}

//��ʼ��SPI FLASH��IO��
void w25qxx_init(void)
{
	spi_init(W25QXX_SPI);
}

//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
uint16_t w25qxx_read_id(void)
{
	uint16_t temp = 0;

	W25QXX_CS_ENABLE;
	SPI1_ReadWriteByte(0x90);			//���Ͷ�ȡID����
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	temp |= SPI1_ReadWriteByte(0xFF) << 8;
	temp |= SPI1_ReadWriteByte(0xFF);
	W25QXX_CS_DISABLE;

	return temp;
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//start_address:��ʼ��ȡ�ĵ�ַ(24bit)
//buf:���ݴ洢��
//len:Ҫ��ȡ���ֽ���(���65535)
void w25qxx_read(uint32_t start_address, uint8_t *buf, uint16_t len)
{
 	uint16_t i;

	W25QXX_CS_ENABLE;                        	//ʹ������
    SPI1_ReadWriteByte(W25X_ReadData);         	//���Ͷ�ȡ����
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 16));  	//����24bit��ַ
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 8));
    SPI1_ReadWriteByte((uint8_t)start_address);
    for (i = 0; i < len; i++)
	{
        buf[i] = SPI1_ReadWriteByte(0xFF);		//ѭ������
    }
	W25QXX_CS_DISABLE;
}


//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//start_address:��ʼд��ĵ�ַ(24bit)
//buf:���ݴ洢��
//len:Ҫд����ֽ���(���65535)
void w25qxx_write(uint32_t start_address, uint8_t *buf, uint16_t len)
{
	uint32_t sector_address;
	uint16_t offset;
	uint16_t bytes_to_write;
 	uint16_t i;
	uint8_t W25QXX_BUF[4096];

 	sector_address = start_address / 4096;//������ַ
	offset = start_address % 4096;//�������ڵ�ƫ��
	bytes_to_write = 4096 - offset;//����ʣ��ռ��С
 	//printf("ad:%X,nb:%X\r\n",start_address,len);//������
 	if (len <= bytes_to_write)
 		bytes_to_write = len;//������4096���ֽ�

	while(1)
	{
		w25qxx_read(sector_address * 4096, W25QXX_BUF, 4096);//������������������

		for(i = 0; i < bytes_to_write; i++)//У������
		{
			if (W25QXX_BUF[offset + i] != 0xFF)
				break;//��Ҫ����
		}

		if (i < bytes_to_write)//��Ҫ����
		{
			w25qxx_erase_sector(sector_address);	//�����������
			for (i = 0; i < bytes_to_write; i++)		//����
			{
				W25QXX_BUF[i + offset] = buf[i];
			}
			w25qxx_write_no_check(sector_address * 4096, W25QXX_BUF, 4096);//д����������
		}
		else
			w25qxx_write_no_check(start_address, buf, bytes_to_write);//д�Ѿ������˵�,ֱ��д������ʣ������.

		if (len == bytes_to_write) break;//д�������
		else//д��δ����
		{
			sector_address++;				//������ַ��1
			offset = 0;						//ƫ��λ��Ϊ0

		   	buf += bytes_to_write;  				//ָ��ƫ��
			start_address += bytes_to_write;		//д��ַƫ��
		   	len -= bytes_to_write;				//�ֽ����ݼ�
			if (len > 4096) bytes_to_write = 4096;	//��һ����������д����
			else bytes_to_write = len;			//��һ����������д����
		}
	}
}

//��������оƬ
//�ȴ�ʱ�䳬��...
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable();                  		//SET WEL
    w25qxx_wait_busy();

  	W25QXX_CS_ENABLE;                            	//ʹ������
    SPI1_ReadWriteByte(W25X_ChipErase);        		//����Ƭ��������
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
	w25qxx_wait_busy();   				   			//�ȴ�оƬ��������
}

//�������ģʽ
void w25qxx_power_down(void)
{
  	W25QXX_CS_ENABLE;                            	//ʹ������
    SPI1_ReadWriteByte(W25X_PowerDown);        		//���͵�������
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
    delay_us(3);                               		//�ȴ�TPD
}

//����
void w25qxx_wake_up(void)
{
  	W25QXX_CS_ENABLE;                            	//ʹ������
    SPI1_ReadWriteByte(W25X_ReleasePowerDown);   	//  send W25X_PowerDown command 0xAB
	W25QXX_CS_DISABLE;                            	//ȡ��Ƭѡ
    delay_us(3);                               		//�ȴ�TRES1
}


























