/**
  \file       drv_crc.c
  \brief      Stdlib for STM32s CRC engine.
  \copyright  Copyright (C) 2013 Ashima Research. All rights reserved. This
              file distributed under the MIT Expat License. See LICENSE file.
              https://github.com/ashima/AQ32Plus
  \remark     Ported for Focused Flight 32.
  \remark     The STM32Fs CRC engine uses the same polynomial as CRC32B (i.e.
              ethernet) but shifts in the oposite direction. __REVs are added
              to fix this, which shouldn't effect executino much as the CRC
              engine takes 4 clocks to execute.
*/


#include "drv_crc.h"

typedef volatile uint32_t vuint32_t;

/*
  \brief    Reset the STM32s CRC Engine.
  \remark   4 NOPs are added to garrentee the reset is complete BEFORE data
            is pushed, because the reset does not seem to be treated as
            a dataregister write with the correct write stalling. In inlined
            code without the NOPs the first word was corrupted.
 */
static void crc32Reset(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

	CRC->CR = CRC_CR_RESET;
	__NOP(); // 4 Clocks to finish reset.
	__NOP();
	__NOP();
	__NOP();
}

/*
  \brief    Write a 32bit word to the STMs CRC engine.
  \param x  The 32bit word to write.
 */
static void crc32Write(vuint32_t x)
{
	CRC->DR = __RBIT(x);
}

/*
  \brief   Read the STM32s CRC engine data register.
  \return  A 32 bit word read form the data register.
 */
static vuint32_t crc32Read (void)
{
	return __RBIT(CRC->DR);
}

/*
  \brief  Feed a uint32_t buffer to the STMs CRC engine. The buffer
          must have a size that is a multiple of 4 bytes.
  \param  start  start of buffer pointer.
  \param  end    past-end of buffer pointer.
 */
void crc32Feed(uint32_t *start, uint32_t *end)
{
	while ( start < end )
		crc32Write(*start++);
	//assert(start==end);
}

/*
  \brief  CRC32B a uint32_t buffer using the STMs CRC engine. The buffer
          must have a size that is a multiple of 4 bytes. The CRC engine
          is reset at the start, and the result is NOTed as expected for
          ethernet or pk CRCs.
  \param start  start of buffer pointer.
  \param end    past-end of buffer pointer.
 */
uint32_t crc32B(uint32_t *start, uint32_t *end)
{
	crc32Reset();
	crc32Feed(start,end);
	return ~crc32Read();
}



