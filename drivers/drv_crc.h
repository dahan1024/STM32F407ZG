/**
  \file       drv_crc.h
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


#ifndef DRIVERS_DRV_CRC_H_
#define DRIVERS_DRV_CRC_H_

#include <types.h>
#include <stm32f4xx.h>


void crc32Feed(uint32_t *start, uint32_t *end);
uint32_t crc32B(uint32_t *start, uint32_t *end);

enum {CrcCheckVal = 0x2144DF1C};

#endif /* DRIVERS_DRV_CRC_H_ */
