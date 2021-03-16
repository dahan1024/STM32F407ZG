/*
 * ad9543.h
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: cjl
 */

#ifndef PERIPH_AD9543_AD9543_H_
#define PERIPH_AD9543_AD9543_H_

#include <types.h>

/******************************************************************************/
/*********************************** AD9543 ***********************************/
/******************************************************************************/
#define AD9543_READ                         (1 << 15)
#define AD9543_WRITE                        (0 << 15)
#define AD9543_CNT(x)                       (((x) - 1) << 13)
#define AD9543_ADDR(x)                      ((x) & 0X1FFF)

/* Chip configuration registers */
#define AD9543_REG_CHIP_PORT_CFG            0x00
#define AD9543_REG_CHIP_ID                  0x01
#define AD9543_REG_CHIP_GRADE               0x02

/* Channel index and transfer registers */
#define AD9543_REG_DEVICE_UPDATE            0xFF

/* ADC functions registers */
#define AD9543_REG_MODES                    0x08
#define AD9543_REG_TEST_IO                  0x0D
#define AD9543_REG_ADC_INPUT                0x0F
#define AD9543_REG_OFFSET                   0x10
#define AD9543_REG_OUT_MODE                 0x14
#define AD9543_REG_OUT_ADJ                  0x15
#define AD9543_REG_OUT_PHASE                0x16
#define AD9543_REG_OUT_DELAY                0x17
#define AD9543_REG_V_REF                    0x18
#define AD9543_REG_ANALOG_INPUT             0x2C
#define AD9543_REG_BUFF_CURRENT_1           0x36
#define AD9543_REG_BUFF_CURRENT_2           0x107

/* AD9543_REG_CHIP_PORT_CFG */
#define AD9543_CHIP_PORT_CGF_LSB_FIRST      (1 << 6)
#define AD9543_CHIP_PORT_CGF_SOFT_RST       (1 << 5)

/* AD9543_REG_CHIP_GRADE */
#define AD9543_CHIP_GRADE_BITS(x)           (((x) & 0x3) << 4)

/* AD9543_REG_DEVICE_UPDATE */
#define AD9543_DEVICE_UPDATE_SW             (1 << 0)

/* AD9543_REG_MODES */
#define AD9543_MODES_INT_PD_MODE(x)         (((x) & 0x3) << 0)

/* AD9543_REG_TEST_IO */
#define AD9543_TEST_IO_RST_PN_LONG          (1 << 5)
#define AD9543_TEST_IO_RST_PN_SHORT         (1 << 4)
#define AD9543_TEST_IO_OUT_TEST(x)          (((x) & 0xF) << 0)

/* AD9543_REG_ADC_INPUT */
#define AD9543_ADC_INPUT_XVREF              (1 << 7)
#define AD9543_ADC_INPUT_ANALOG_DSCN        (1 << 2)

/* AD9543_REG_OUT_MODE */
#define AD9543_OUT_MODE_DOUT_DISBL          (1 << 4)
#define AD9543_OUT_MODE_OUT_INV             (1 << 2)
#define AD9543_OUT_MODE_DATA_FORMAT(x)      (((x) & 0x3) << 0)

/* AD9543_REG_OUT_ADJ */
#define AD9543_OUT_ADJ_LVDS                 (1 << 3)
#define AD9543_OUT_ADJ_OUT_CURRENT(x)       (((x) & 0x7) << 0)

/* AD9543_REG_OUT_PHASE */
#define AD9543_OUT_PHASE_DCO_INV            (1 << 7)

/* AD9543_REG_OUT_DELAY */
#define AD9543_OUT_DELAY_DCO_DLY_EN         (1 << 7)
#define AD9543_OUT_DELAY_OUT_DLY(x)         (((x) & 0x1F) << 0)

/* AD9543_REG_V_REF */
#define AD9543_V_REF_IN_FS_RANGE(x)         (((x) & 0xF) << 0)

/* AD9543_REG_ANALOG_INPUT */
#define AD9543_ANALOG_INPUT_COUPLING        (1 << 2)

/* AD9543_REG_BUFF_CURRENT_1 */
#define AD9543_BUFF_CURRENT_1(x)            (((x) & 0x3F) << 2)

/* AD9543_REG_BUFF_CURRENT_2 */
#define AD9543_BUFF_CURRENT_2(x)            (((x) & 0x3F) << 2)

void ad9543_init(void);

int32_t ad9543_write(uint16_t reg_addr, uint8_t reg_val);

int32_t ad9543_read(uint16_t reg_addr, uint8_t *reg_val);

#endif /* PERIPH_AD9543_AD9543_H_ */

