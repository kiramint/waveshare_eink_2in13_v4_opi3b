#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_
/***********************************************************************************************************************
            ------------------------------------------------------------------------
            |\\\																///|
            |\\\					Hardware interface							///|
            ------------------------------------------------------------------------
***********************************************************************************************************************/

// EXTERN
#ifdef __cplusplus
extern "C"
{
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "Debug.h"

// #define IIC_Address			0x14
// #define IIC_Address_Read	0x29
// #define IIC_Address_Write	0x28

/**
 * data
 **/
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

// TP Define
#define TRST 130
#define INT 128

#define TRST_0 DEV_Digital_Write(TRST, 0)
#define TRST_1 DEV_Digital_Write(TRST, 1)

#define INT_0 DEV_Digital_Write(INT, 0)
#define INT_1 DEV_Digital_Write(INT, 1)

/**
 * GPIOI config
 **/
#define EPD_RST_PIN 118
#define EPD_DC_PIN 132
#define EPD_CS_PIN 134
#define EPD_BUSY_PIN 129

    /*------------------------------------------------------------------------------------------------------*/
    void DEV_SetIicAddress(int addr);

    UBYTE DEV_ModuleInit(void);
    void DEV_ModuleExit(void);

    void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
    void DEV_Delay_ms(UDOUBLE xms);

    void DEV_Digital_Write(UWORD Pin, UBYTE Value);
    UBYTE DEV_Digital_Read(UWORD Pin);

    void DEV_SPI_WriteByte(UBYTE Value);
    void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);

    UBYTE I2C_Write_Byte(UWORD Reg, char *Data, UBYTE len);
    UBYTE I2C_Read_Byte(UWORD Reg, char *Data, UBYTE len);

// EXTERN END
#ifdef __cplusplus
}
#endif
#endif
