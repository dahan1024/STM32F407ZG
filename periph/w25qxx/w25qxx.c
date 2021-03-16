#include <stdio.h>

#include <stm32f4xx.h>
#include <drv_spi.h>
#include <drv_delay.h>

#include "w25qxx.h"

#define	W25QXX_SPI					SPI1
#define W25QXX_CS_DISABLE			SPI1_CS_DISABLE
#define W25QXX_CS_ENABLE			SPI1_CS_ENABLE

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector

static uint8_t SPI1_ReadWriteByte(uint8_t data);
static uint8_t w25qxx_read_sr(void);        			//读取状态寄存器
//static void w25qxx_write_sr(uint8_t sr);				//写W25QXX状态寄存器
static void w25qxx_write_enable(void);  		//写使能
//static void w25qxx_write_disable(void);			//W25QXX写禁止
static void w25qxx_wait_busy(void);				//等待空闲
static void w25qxx_erase_sector(uint32_t sector_address);	//扇区擦除
static void w25qxx_write_page(uint32_t start_address, uint8_t *buf, uint16_t len);
static void w25qxx_write_no_check(uint32_t start_address, uint8_t *buf, uint16_t len);

static uint8_t SPI1_ReadWriteByte(uint8_t data)
{
	return spi_transfer(W25QXX_SPI, data);
}

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static uint8_t w25qxx_read_sr(void)
{
	uint8_t byte=0;
	W25QXX_CS_ENABLE;                            	//使能器件
	SPI1_ReadWriteByte(W25X_ReadStatusReg);    		//发送读取状态寄存器命令
	byte = SPI1_ReadWriteByte(0xFF);             	//读取一个字节
	W25QXX_CS_DISABLE;                            	//取消片选
	return byte;
}

#if 0
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
static void w25qxx_write_sr(uint8_t sr)
{
	W25QXX_CS_ENABLE;                         		//使能器件
	SPI1_ReadWriteByte(W25X_WriteStatusReg);		//发送写取状态寄存器命令
	SPI1_ReadWriteByte(sr);               			//写入一个字节
	W25QXX_CS_DISABLE;                            	//取消片选
}
#endif

//W25QXX写使能
//将WEL置位
static void w25qxx_write_enable(void)
{
	W25QXX_CS_ENABLE;                            	//使能器件
    SPI1_ReadWriteByte(W25X_WriteEnable);      		//发送写使能
	W25QXX_CS_DISABLE;                            	//取消片选
}

#if 0
//W25QXX写禁止
//将WEL清零
static void w25qxx_write_disable(void)
{
	W25QXX_CS_ENABLE;                            	//使能器件
    SPI1_ReadWriteByte(W25X_WriteDisable);     		//发送写禁止指令
	W25QXX_CS_DISABLE;                            	//取消片选
}
#endif

//等待空闲
static void w25qxx_wait_busy(void)
{
	while ((w25qxx_read_sr() & 0x01) == 0x01);		// 等待BUSY位清空
}

//擦除一个扇区
//sector_address:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
static void w25qxx_erase_sector(uint32_t sector_address)
{
	//监视falsh擦除情况,测试用
 	printf("fe:%x\r\n", sector_address);
 	sector_address *= 4096;
    w25qxx_write_enable();                  		//SET WEL
    w25qxx_wait_busy();

  	W25QXX_CS_ENABLE;                            	//使能器件
    SPI1_ReadWriteByte(W25X_SectorErase);      		//发送扇区擦除指令
    SPI1_ReadWriteByte((uint8_t)((sector_address) >> 16));  	//发送24bit地址
    SPI1_ReadWriteByte((uint8_t)((sector_address) >> 8));
    SPI1_ReadWriteByte((uint8_t)sector_address);
	W25QXX_CS_DISABLE;                            	//取消片选
    w25qxx_wait_busy();   				   			//等待擦除完成
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//start_address:开始写入的地址(24bit)
//buf:数据存储区
//len:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
static void w25qxx_write_page(uint32_t start_address, uint8_t *buf, uint16_t len)
{
 	uint16_t i;
    w25qxx_write_enable();                  		//SET WEL
    w25qxx_wait_busy();

	W25QXX_CS_ENABLE;                            	//使能器件
    SPI1_ReadWriteByte(W25X_PageProgram);      		//发送写页命令
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 16)); 	//发送24bit地址
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 8));
    SPI1_ReadWriteByte((uint8_t)start_address);
    for (i = 0; i < len; i++)
    	SPI1_ReadWriteByte(buf[i]);					//循环写数
	W25QXX_CS_DISABLE;                            	//取消片选
	w25qxx_wait_busy();					   			//等待写入结束
}

//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//start_address:开始写入的地址(24bit)
//buf:数据存储区
//len:要写入的字节数(最大65535)
//CHECK OK
static void w25qxx_write_no_check(uint32_t start_address, uint8_t *buf, uint16_t len)
{
	uint16_t bytes_to_write;

	bytes_to_write = 256 - start_address % 256; //单页剩余的字节数
	if (len <= bytes_to_write)
		bytes_to_write = len;					//不大于256个字节

	while(1)
	{
		w25qxx_write_page(start_address, buf, bytes_to_write);
		if (len == bytes_to_write)
			break;							//写入结束了
	 	else //len>bytes_to_write
		{
			buf += bytes_to_write;
			start_address += bytes_to_write;
			len -= bytes_to_write;			  		//减去已经写入了的字节数

			if (len > 256) bytes_to_write = 256; 	//一次可以写入256个字节
			else bytes_to_write = len; 	  			//不够256个字节了
		}
	}
}

//初始化SPI FLASH的IO口
void w25qxx_init(void)
{
	spi_init(W25QXX_SPI);
}

//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
uint16_t w25qxx_read_id(void)
{
	uint16_t temp = 0;

	W25QXX_CS_ENABLE;
	SPI1_ReadWriteByte(0x90);			//发送读取ID命令
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	temp |= SPI1_ReadWriteByte(0xFF) << 8;
	temp |= SPI1_ReadWriteByte(0xFF);
	W25QXX_CS_DISABLE;

	return temp;
}
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//start_address:开始读取的地址(24bit)
//buf:数据存储区
//len:要读取的字节数(最大65535)
void w25qxx_read(uint32_t start_address, uint8_t *buf, uint16_t len)
{
 	uint16_t i;

	W25QXX_CS_ENABLE;                        	//使能器件
    SPI1_ReadWriteByte(W25X_ReadData);         	//发送读取命令
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 16));  	//发送24bit地址
    SPI1_ReadWriteByte((uint8_t)((start_address) >> 8));
    SPI1_ReadWriteByte((uint8_t)start_address);
    for (i = 0; i < len; i++)
	{
        buf[i] = SPI1_ReadWriteByte(0xFF);		//循环读数
    }
	W25QXX_CS_DISABLE;
}


//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//start_address:开始写入的地址(24bit)
//buf:数据存储区
//len:要写入的字节数(最大65535)
void w25qxx_write(uint32_t start_address, uint8_t *buf, uint16_t len)
{
	uint32_t sector_address;
	uint16_t offset;
	uint16_t bytes_to_write;
 	uint16_t i;
	uint8_t W25QXX_BUF[4096];

 	sector_address = start_address / 4096;//扇区地址
	offset = start_address % 4096;//在扇区内的偏移
	bytes_to_write = 4096 - offset;//扇区剩余空间大小
 	//printf("ad:%X,nb:%X\r\n",start_address,len);//测试用
 	if (len <= bytes_to_write)
 		bytes_to_write = len;//不大于4096个字节

	while(1)
	{
		w25qxx_read(sector_address * 4096, W25QXX_BUF, 4096);//读出整个扇区的内容

		for(i = 0; i < bytes_to_write; i++)//校验数据
		{
			if (W25QXX_BUF[offset + i] != 0xFF)
				break;//需要擦除
		}

		if (i < bytes_to_write)//需要擦除
		{
			w25qxx_erase_sector(sector_address);	//擦除这个扇区
			for (i = 0; i < bytes_to_write; i++)		//复制
			{
				W25QXX_BUF[i + offset] = buf[i];
			}
			w25qxx_write_no_check(sector_address * 4096, W25QXX_BUF, 4096);//写入整个扇区
		}
		else
			w25qxx_write_no_check(start_address, buf, bytes_to_write);//写已经擦除了的,直接写入扇区剩余区间.

		if (len == bytes_to_write) break;//写入结束了
		else//写入未结束
		{
			sector_address++;				//扇区地址增1
			offset = 0;						//偏移位置为0

		   	buf += bytes_to_write;  				//指针偏移
			start_address += bytes_to_write;		//写地址偏移
		   	len -= bytes_to_write;				//字节数递减
			if (len > 4096) bytes_to_write = 4096;	//下一个扇区还是写不完
			else bytes_to_write = len;			//下一个扇区可以写完了
		}
	}
}

//擦除整个芯片
//等待时间超长...
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable();                  		//SET WEL
    w25qxx_wait_busy();

  	W25QXX_CS_ENABLE;                            	//使能器件
    SPI1_ReadWriteByte(W25X_ChipErase);        		//发送片擦除命令
	W25QXX_CS_DISABLE;                            	//取消片选
	w25qxx_wait_busy();   				   			//等待芯片擦除结束
}

//进入掉电模式
void w25qxx_power_down(void)
{
  	W25QXX_CS_ENABLE;                            	//使能器件
    SPI1_ReadWriteByte(W25X_PowerDown);        		//发送掉电命令
	W25QXX_CS_DISABLE;                            	//取消片选
    delay_us(3);                               		//等待TPD
}

//唤醒
void w25qxx_wake_up(void)
{
  	W25QXX_CS_ENABLE;                            	//使能器件
    SPI1_ReadWriteByte(W25X_ReleasePowerDown);   	//  send W25X_PowerDown command 0xAB
	W25QXX_CS_DISABLE;                            	//取消片选
    delay_us(3);                               		//等待TRES1
}


























