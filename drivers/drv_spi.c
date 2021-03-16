#include "drv_spi.h"

///////////////////////////////////////////////////////////////////////////////

// SPI1
// SCK  PB3
// MISO PB4
// MOSI PB5

// SPI2
// SCK  PB13
// MISO PB14
// MOSI PB15

///////////////////////////////////////////////////////////////////////////////
// SPI Defines and Variables
///////////////////////////////////////////////////////////////////////////////

#define SPI1_GPIO_CLOCK       	RCC_AHB1Periph_GPIOB
#define SPI1_GPIO             	GPIOB
#define SPI1_SCK_PIN          	GPIO_Pin_3
#define SPI1_SCK_PIN_SOURCE   	GPIO_PinSource3
#define SPI1_MISO_PIN         	GPIO_Pin_4
#define SPI1_MISO_PIN_SOURCE  	GPIO_PinSource4
#define SPI1_MOSI_PIN			GPIO_Pin_5
#define SPI1_MOSI_PIN_SOURCE  	GPIO_PinSource5

//#define SPI2_GPIO_CLOCK       	RCC_AHB1Periph_GPIOB
//#define SPI2_GPIO             	GPIOB
//#define SPI2_SCK_PIN          	GPIO_Pin_13
//#define SPI2_SCK_PIN_SOURCE   	GPIO_PinSource13
//#define SPI2_MISO_PIN			GPIO_Pin_14
//#define SPI2_MISO_PIN_SOURCE	GPIO_PinSource14
//#define SPI2_MOSI_PIN         	GPIO_Pin_15
//#define SPI2_MOSI_PIN_SOURCE  	GPIO_PinSource15

#define SPI3_GPIO_CLOCK       	RCC_AHB1Periph_GPIOC
#define SPI3_GPIO             	GPIOC
#define SPI3_SCK_PIN          	GPIO_Pin_10
#define SPI3_SCK_PIN_SOURCE   	GPIO_PinSource10
#define SPI3_MISO_PIN			GPIO_Pin_11
#define SPI3_MISO_PIN_SOURCE	GPIO_PinSource11
#define SPI3_MOSI_PIN         	GPIO_Pin_12
#define SPI3_MOSI_PIN_SOURCE  	GPIO_PinSource12
///////////////////////////////////////////////////////////////////////////////
// SPI Initialize
///////////////////////////////////////////////////////////////////////////////

BOOL spi1_initialized = FALSE;
//BOOL spi2_initialized = FALSE;
BOOL spi3_initialized = FALSE;

void spi_init(SPI_TypeDef *SPIx)
{
    volatile uint8_t dummyread __attribute__((unused));

    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    ///////////////////////////////////
    if ((SPIx == SPI1) && !spi1_initialized)
    {
    	// Enable SPI1_CS_GPIO_CLOCK
    	RCC_AHB1PeriphClockCmd(SPI1_CS_GPIO_CLOCK, ENABLE);

        // Init cs pin
        GPIO_InitStructure.GPIO_Pin   = SPI1_CS_PIN;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

        GPIO_Init(SPI1_CS_GPIO, &GPIO_InitStructure);

        GPIO_SetBits(SPI1_CS_GPIO, SPI1_CS_PIN);
        ///////////////////////////////

        // Enable GPIOA clock
        RCC_AHB1PeriphClockCmd(SPI1_GPIO_CLOCK, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

        GPIO_PinAFConfig(SPI1_GPIO, SPI1_SCK_PIN_SOURCE,  GPIO_AF_SPI1);
        GPIO_PinAFConfig(SPI1_GPIO, SPI1_MISO_PIN_SOURCE, GPIO_AF_SPI1);
        GPIO_PinAFConfig(SPI1_GPIO, SPI1_MOSI_PIN_SOURCE, GPIO_AF_SPI1);

        // Init pins
        GPIO_InitStructure.GPIO_Pin   = SPI1_SCK_PIN | SPI1_MISO_PIN | SPI1_MOSI_PIN;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

        GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);

    	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
    	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);

        SPI_I2S_DeInit(SPI1);

        SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
        SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
//        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  // 42/256 MHz SPI Clock
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  	// 42/4 MHz SPI Clock
        SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial     = 7;

        SPI_Init(SPI1, &SPI_InitStructure);

        SPI_CalculateCRC(SPI1, DISABLE);

        SPI_Cmd(SPI1, ENABLE);


        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

        dummyread = SPI_I2S_ReceiveData(SPI1);

        spi1_initialized = TRUE;
    }

//    else if (SPIx == SPI2)
//    {
//    	// Enable SPI2_CS_GPIO_CLOCK
//    	RCC_AHB1PeriphClockCmd(SPI2_CS_GPIO_CLOCK, ENABLE);
//
//        // Init cs pin
//        GPIO_InitStructure.GPIO_Pin   = SPI2_CS_PIN;
//        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
//        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
//        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//
//        GPIO_Init(SPI2_CS_GPIO, &GPIO_InitStructure);
//
//        GPIO_SetBits(SPI2_CS_GPIO, SPI2_CS_PIN);
//        ///////////////////////////////
//
//        // Enable GPIOB clock
//        RCC_AHB1PeriphClockCmd(SPI2_GPIO_CLOCK, ENABLE);
//        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//
//        GPIO_PinAFConfig(SPI2_GPIO, SPI2_SCK_PIN_SOURCE,  GPIO_AF_SPI2);
//        GPIO_PinAFConfig(SPI2_GPIO, SPI2_MISO_PIN_SOURCE, GPIO_AF_SPI2);
//        GPIO_PinAFConfig(SPI2_GPIO, SPI2_MOSI_PIN_SOURCE, GPIO_AF_SPI2);
//
//        // Init pins
//        GPIO_InitStructure.GPIO_Pin   = SPI2_SCK_PIN | SPI2_MISO_PIN | SPI2_MOSI_PIN;
//        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
//        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//
//        GPIO_Init(SPI2_GPIO, &GPIO_InitStructure);
//
//    	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);
//    	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE);
//
//        SPI_I2S_DeInit(SPI2);
//
//        SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
//        SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
//        SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
//        SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
//        SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
//        SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
//        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  // 42/256 MHz SPI Clock
//        SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
//        SPI_InitStructure.SPI_CRCPolynomial     = 7;
//
//        SPI_Init(SPI2, &SPI_InitStructure);
//
//        SPI_CalculateCRC(SPI2, DISABLE);
//
//        SPI_Cmd(SPI2, ENABLE);
//
//
//        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
//
//        dummyread = SPI_I2S_ReceiveData(SPI2);
//    }

    else if ((SPIx == SPI3) && !spi3_initialized)
    {
    	// Enable SPI3_CS0_GPIO_CLOCK
    	RCC_AHB1PeriphClockCmd(SPI3_CS0_GPIO_CLOCK, ENABLE);

    	// Enable SPI3_CS1_GPIO_CLOCK
    	RCC_AHB1PeriphClockCmd(SPI3_CS1_GPIO_CLOCK, ENABLE);

        // Init cs0 pin
        GPIO_InitStructure.GPIO_Pin   = SPI3_CS0_PIN;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

        GPIO_Init(SPI3_CS0_GPIO, &GPIO_InitStructure);

        // Init cs1 pin
        GPIO_InitStructure.GPIO_Pin   = SPI3_CS1_PIN;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

        GPIO_Init(SPI3_CS1_GPIO, &GPIO_InitStructure);

        GPIO_SetBits(SPI3_CS0_GPIO, SPI3_CS0_PIN);
        GPIO_SetBits(SPI3_CS1_GPIO, SPI3_CS1_PIN);
        ///////////////////////////////

        // Enable GPIOC clock
        RCC_AHB1PeriphClockCmd(SPI3_GPIO_CLOCK, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

        GPIO_PinAFConfig(SPI3_GPIO, SPI3_SCK_PIN_SOURCE,  GPIO_AF_SPI3);
        GPIO_PinAFConfig(SPI3_GPIO, SPI3_MISO_PIN_SOURCE, GPIO_AF_SPI3);
        GPIO_PinAFConfig(SPI3_GPIO, SPI3_MOSI_PIN_SOURCE, GPIO_AF_SPI3);

        // Init pins
        GPIO_InitStructure.GPIO_Pin   = SPI3_SCK_PIN | SPI3_MISO_PIN | SPI3_MOSI_PIN;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

        GPIO_Init(SPI3_GPIO, &GPIO_InitStructure);

    	RCC_APB2PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);
    	RCC_APB2PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);

        SPI_I2S_DeInit(SPI3);

        SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
        SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;

//        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  // 42/256 MHz SPI Clock
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;  // 42/256 MHz SPI Clock

        SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
        SPI_InitStructure.SPI_CRCPolynomial     = 7;

        SPI_Init(SPI3, &SPI_InitStructure);

        SPI_CalculateCRC(SPI3, DISABLE);

        SPI_Cmd(SPI3, ENABLE);


        while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

        dummyread = SPI_I2S_ReceiveData(SPI3);

        spi3_initialized = TRUE;
    }
}

///////////////////////////////////////////////////////////////////////////////
// SPI Transfer
///////////////////////////////////////////////////////////////////////////////

uint8_t spi_transfer(SPI_TypeDef *SPIx, uint8_t data)
{
    uint16_t spi_timeout;

    spi_timeout = 0x1000;

    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)
        if ((spi_timeout--) == 0)
            return(0);

    SPI_I2S_SendData(SPIx, data);

    spi_timeout = 0x1000;

    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)
        if ((spi_timeout--) == 0)
    	    return(0);

    return((uint8_t)SPI_I2S_ReceiveData(SPIx));
}

///////////////////////////////////////////////////////////////////////////////
// Set SPI Divisor
///////////////////////////////////////////////////////////////////////////////

void spi_set_divisor(SPI_TypeDef *SPIx, uint16_t data)
{
#define BR_CLEAR_MASK 0xFFC7

    uint16_t temp_register;

    SPI_Cmd(SPIx, DISABLE);

    temp_register = SPIx->CR1;

    switch (data)
    {
        case 2:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_2;
            break;

        case 4:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_4;
            break;

        case 8:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_8;
            break;

        case 16:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_16;
            break;

        case 32:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_32;
            break;

        case 64:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_64;
            break;

        case 128:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_128;
            break;

        case 256:
            temp_register &= BR_CLEAR_MASK;
            temp_register |= SPI_BaudRatePrescaler_256;
            break;
    }

    SPIx->CR1 = temp_register;

    SPI_Cmd(SPIx, ENABLE);
}


///////////////////////////////////////////////////////////////////////////////

