#include "drv_i2c.h"
#include "drv_delay.h"

// I2C1 SCL/SDA macro definition
#define I2C1_GPIO			GPIOB
#define I2C1_SCL_PIN		GPIO_Pin_6
#define I2C1_SDA_PIN		GPIO_Pin_7

// I2C SCL/SDA set low/high macro definition
#define I2C_SCL_L          GPIO_ResetBits(I2C1_GPIO, I2C1_SCL_PIN)
#define I2C_SCL_H          GPIO_SetBits(I2C1_GPIO, I2C1_SCL_PIN)
#define I2C_SDA_L          GPIO_ResetBits(I2C1_GPIO, I2C1_SDA_PIN)
#define I2C_SDA_H          GPIO_SetBits(I2C1_GPIO, I2C1_SDA_PIN)

// I2C SDA state macro definition
#define I2C_SDA_STATE      GPIO_ReadInputDataBit(I2C1_GPIO, I2C1_SDA_PIN)

// I2C nop macro definition
#define I2C_NOP            delay_us(1)


/*
----------------------------------------------------------------
---                    Aux FUNCTIONS                       ---
----------------------------------------------------------------
*/

// I2C initialization. Use analog I2C
void i2c_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    // Enable GPIOB clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // GPIO configuration, Open-drain output
    GPIO_InitStructure.GPIO_Pin = I2C1_SCL_PIN | I2C1_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(I2C1_GPIO, &GPIO_InitStructure);
}

// I2C start
u8 I2C_START(void)
{
    I2C_SDA_H;
    I2C_NOP;

    I2C_SCL_H;
    I2C_NOP;

    if(!I2C_SDA_STATE)
        return I2C_BUS_BUSY;

    I2C_SDA_L;
    I2C_NOP;

    I2C_SCL_L;
    I2C_NOP;

    if(I2C_SDA_STATE)
        return I2C_BUS_ERROR;

    return I2C_READY;
}

// I2C stop
void I2C_STOP(void)
{
    I2C_SDA_L;
    I2C_NOP;

    I2C_SCL_H;
    I2C_NOP;

    I2C_SDA_H;
    I2C_NOP;
}

uint8_t I2C_WaitACK(void)
{
	uint16_t timeOut = 5000;

    I2C_SDA_H;
    I2C_NOP;
    I2C_SCL_H;
    I2C_NOP;

	while (I2C_SDA_STATE)
	{
		timeOut--;
		if (timeOut == 0)
		{
			I2C_STOP();
			return I2C_NACK;
		}
	}

	I2C_SCL_L;

	return I2C_ACK;
}

// I2C send ACK
static void I2C_SendACK(void)
{
    I2C_SDA_L;
    I2C_NOP;
    I2C_SCL_H;
    I2C_NOP;
    I2C_SCL_L;
    I2C_NOP;
}

// I2C send NACK
static void I2C_SendNACK(void)
{
    I2C_SDA_H;
    I2C_NOP;
    I2C_SCL_H;
    I2C_NOP;
    I2C_SCL_L;
    I2C_NOP;
}

// I2C send one byte
u8 I2C_SendByte(u8 i2c_data)
{
    u8 i;

    I2C_SCL_L;
    for(i=0;i<8;i++)
    {
        if(i2c_data&0x80)
            I2C_SDA_H;
        else
            I2C_SDA_L;

        i2c_data<<=1;
        I2C_NOP;

        I2C_SCL_H;
        I2C_NOP;
        I2C_SCL_L;
        I2C_NOP;
    }

    return I2C_WaitACK();
}

// I2C receive one byte
u8 I2C_ReceiveByte(void)
{
    u8 i,i2c_data;

    I2C_SDA_H;
    I2C_SCL_L;
    i2c_data=0;

    for(i=0;i<8;i++)
    {
        I2C_SCL_H;
        I2C_NOP;
        i2c_data<<=1;

        if(I2C_SDA_STATE)
            i2c_data|=0x01;

        I2C_SCL_L;
        I2C_NOP;
    }
    I2C_SendNACK();
    return i2c_data;
}

// I2C receive one byte with ACK
u8 I2C_ReceiveByte_WithACK(void)
{
    u8 i,i2c_data;

    I2C_SDA_H;
    I2C_SCL_L;
    i2c_data=0;

    for(i=0;i<8;i++)
    {
        I2C_SCL_H;
        I2C_NOP;
        i2c_data<<=1;

        if(I2C_SDA_STATE)
            i2c_data|=0x01;

        I2C_SCL_L;
        I2C_NOP;
    }
    I2C_SendACK();
    return i2c_data;
}

/*
----------------------------------------------------------------
---                    I2C FUNCTIONS                       ---
----------------------------------------------------------------
*/
uint8_t I2C_Scan(uint8_t slave_address)
{
    if (I2C_START() != I2C_READY)
    {
    	I2C_STOP();
    	return I2C_ERR;
    }

	if (I2C_SendByte(slave_address) != I2C_ACK)	    //·¢ËÍÐ´ÃüÁî
		return I2C_ERR;

	I2C_STOP();

	return I2C_OK;
}

uint8_t I2C_Devices(void)
{
	uint8_t status = 0;
	uint8_t count = 0;
	uint8_t i;

	for (i = 0; i < 128; i++)
	{
		status = I2C_Scan(i << 1);

		if (status == I2C_OK)
		{
			count++;
		}
	}

	return count;
}

#if 0
// I2C write one byte
void I2C_WriteByte(uint8_t DeviceAddr, uint8_t address, uint8_t data)
{
    I2C_START();
    I2C_SendByte(DeviceAddr);
    I2C_SendByte(address);
    I2C_SendByte(data);
    I2C_STOP();
}

// I2C read one byte
uint8_t I2C_ReadByte(uint8_t DeviceAddr, uint8_t address)
{
    uint8_t i;
    I2C_START();
    I2C_SendByte(DeviceAddr);
    I2C_SendByte(address);
    I2C_START();
    I2C_SendByte(DeviceAddr + 1);
    i = I2C_ReceiveByte();
    I2C_STOP();
    return i;
}

// I2C write multiple bytes
u8 i2c1_write(u8 dev_addr, u8 reg_addr, u8 i2c_len, u8 *i2c_data_buf)
{
    u8 i;
    I2C_START();
    I2C_SendByte(dev_addr << 1 | I2C_Direction_Transmitter);
    I2C_SendByte(reg_addr);
    for (i = 0; i < i2c_len; i++)
        I2C_SendByte(i2c_data_buf[i]);

    I2C_STOP();
    return 0x00;
}

// I2C read multiple bytes
u8 i2c1_read(u8 dev_addr, u8 reg_addr, u8 i2c_len, u8 *i2c_data_buf)
{
    I2C_START();
    I2C_SendByte(dev_addr << 1 | I2C_Direction_Transmitter);
    I2C_SendByte(reg_addr);
    I2C_START();
    I2C_SendByte(dev_addr << 1 | I2C_Direction_Receiver);

    while (i2c_len)
    {
        if (i2c_len == 1)
            *i2c_data_buf = I2C_ReceiveByte();
        else
            *i2c_data_buf = I2C_ReceiveByte_WithACK();
        i2c_data_buf++;
        i2c_len--;
    }
    I2C_STOP();
    return 0x00;
}

#endif

