/*****************************************************************************
 * | File      	:   DEV_Config.c
 * | Author      :   Waveshare team
 * | Function    :   Hardware underlying interface
 * | Info        :
 *----------------
 * |	This version:   V2.0
 * | Date        :   2020-06-17
 * | Info        :   Basic version
 *
 ******************************************************************************/
#include "DEV_Config.h"
#include <fcntl.h>
#include <unistd.h>

#ifdef USE_BCM2835_LIB
#include <bcm2835.h>
#elif USE_WIRINGPI_LIB
#include "dev_hardware_i2c.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#elif USE_LGPIO_LIB
#include <lgpio.h>
#define LFLAGS 0
#define NUM_MAXBUF 4
#elif USE_GPIOD_LIB
#include "RPI_gpiod.h"
#include "dev_hardware_SPI.h"
#include "dev_hardware_i2c.h"
#endif

#if USE_LGPIO_LIB
// Kira
int GPIO_Handle_96_127;  // Chip3
int GPIO_Handle_128_158; // Chip4
int SPI_Handle;
int IIC_Handle;

// Kira
/**
 * Get handle of different gpio chip.
 */
int getGpioHandle(int Pin) {
  if (Pin >= 128 && Pin < 160) {
    return GPIO_Handle_128_158;
  } else if (Pin >= 96) {
    return GPIO_Handle_96_127;
  } else {
    Debug("Get err pin: %d\r\n", Pin);
    return -1;
  }
}
/**
 * Translate system gpio pin into gpio chip inner pin.
 */
int getGpioInternalPin(int Pin) {
  if (Pin >= 128 && Pin < 160) {
    return Pin - 128;
  } else if (Pin >= 96) {
    return Pin - 96;
  } else {
    Debug("Get err pin: %d\r\n", Pin);
    return -1;
  }
}
#endif

/**
 * GPIO // Move to header
 **/
int IIC_Address;

void DEV_SetIicAddress(int addr) { IIC_Address = addr; }

uint32_t fd;
/*****************************************
                GPIO
*****************************************/
void DEV_Digital_Write(UWORD Pin, UBYTE Value) {
#ifdef USE_BCM2835_LIB
  bcm2835_gpio_write(Pin, Value);
#elif USE_WIRINGPI_LIB
  digitalWrite(Pin, Value);
#elif USE_LGPIO_LIB
  lgGpioWrite(getGpioHandle(Pin), getGpioInternalPin(Pin), Value);
#elif USE_GPIOD_LIB
  GPIOD_Write(Pin, Value);
#endif
}

UBYTE DEV_Digital_Read(UWORD Pin) {
  UBYTE Read_value = 0;
#ifdef USE_BCM2835_LIB
  Read_value = bcm2835_gpio_lev(Pin);
#elif USE_WIRINGPI_LIB
  Read_value = digitalRead(Pin);
#elif USE_LGPIO_LIB
  Read_value = lgGpioRead(getGpioHandle(Pin), getGpioInternalPin(Pin));
#elif USE_GPIOD_LIB
  Read_value = GPIOD_Read(Pin);
#endif
  // Debug("Pin = %d,Read = %d\r\n", Pin, Read_value);
  return Read_value;
}

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode) {
#ifdef USE_BCM2835_LIB
  if (Mode == 0 || Mode == BCM2835_GPIO_FSEL_INPT) {
    bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_INPT);
  } else {
    bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_OUTP);
  }
#elif USE_WIRINGPI_LIB
  if (Mode == 0 || Mode == INPUT) {
    pinMode(Pin, INPUT);
    pullUpDnControl(Pin, PUD_UP);
  } else {
    pinMode(Pin, OUTPUT);
    // Debug (" %d OUT \r\n",Pin);
  }
#elif USE_LGPIO_LIB
  if (Mode == 0 || Mode == LG_SET_INPUT) {
    int innerPin = getGpioInternalPin(Pin);
    lgGpioClaimInput(getGpioHandle(Pin), LFLAGS, innerPin);
    // Debug("IN Pin = %d\r\n", Pin);
  } else {
    int innerPin = getGpioInternalPin(Pin);
    lgGpioClaimOutput(getGpioHandle(Pin), LFLAGS, innerPin, LG_LOW);
    // Debug("OUT Pin = %d\r\n", Pin);
  }
#elif USE_GPIOD_LIB
  if (Mode == 0 || Mode == GPIOD_IN) {
    GPIOD_Direction(Pin, GPIOD_IN);
    // Debug("IN Pin = %d\r\n",Pin);
  } else {
    GPIOD_Direction(Pin, GPIOD_OUT);
    // Debug("OUT Pin = %d\r\n",Pin);
  }
#endif
}

/**
 * delay x ms
 **/
void DEV_Delay_ms(UDOUBLE xms) {
#ifdef USE_BCM2835_LIB
  bcm2835_delay(xms);
#elif USE_WIRINGPI_LIB
  delay(xms);
#elif USE_LGPIO_LIB
  lguSleep(xms / 1000.0);
#elif USE_GPIOD_LIB
  UDOUBLE i;
  for (i = 0; i < xms; i++) {
    usleep(1000);
  }
#endif
}

static void DEV_GPIO_Init(void) {
  DEV_GPIO_Mode(EPD_RST_PIN, 1);
  DEV_GPIO_Mode(EPD_DC_PIN, 1);
  DEV_GPIO_Mode(EPD_CS_PIN, 1);
  DEV_GPIO_Mode(EPD_BUSY_PIN, 0);

  DEV_Digital_Write(EPD_CS_PIN, 1);

  DEV_GPIO_Mode(TRST, 1);
  DEV_GPIO_Mode(INT, 0);
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI
protocol parameter: Info:
******************************************************************************/
UBYTE DEV_ModuleInit(void) {
#ifdef USE_BCM2835_LIB
  if (!bcm2835_init()) {
    printf("bcm2835 init failed  !!! \r\n");
    return 1;
  } else {
    printf("bcm2835 init success !!! \r\n");
  }

  DEV_GPIO_Init();

  bcm2835_i2c_begin();
  bcm2835_i2c_setSlaveAddress(IIC_Address);
  // bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500);

  bcm2835_spi_begin(); // Start spi interface, set spi pin for the reuse
                       // function
  bcm2835_spi_setBitOrder(
      BCM2835_SPI_BIT_ORDER_MSBFIRST);        // High first transmission
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0); // spi mode 0
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32); // Frequency
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                   // set CE0
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);   // enable cs0
#elif USE_WIRINGPI_LIB
  // if(wiringPiSetup() < 0) {//use wiringpi Pin number table
  if (wiringPiSetupGpio() < 0) { // use BCM2835 Pin number table
    printf("set wiringPi lib failed	!!! \r\n");
    return 1;
  } else {
    printf("set wiringPi lib success !!! \r\n");
  }
  // GPIO Config
  DEV_GPIO_Init();

  // wiringPiSPISetup(0,10000000);
  wiringPiSPISetupMode(0, 10000000, 0);
  fd = wiringPiI2CSetup(IIC_Address);

  DEV_HARDWARE_I2C_begin("/dev/i2c-1");
  DEV_HARDWARE_I2C_setSlaveAddress(IIC_Address);
#elif USE_LGPIO_LIB

  // Kira
  GPIO_Handle_96_127 = lgGpiochipOpen(3);
  if (GPIO_Handle_96_127 < 0) {
    Debug("gpiochip96 (/dev/gpiochip3) Export Failed\n");
    return -1;
  }

  // Kira
  GPIO_Handle_128_158 = lgGpiochipOpen(4);
  if (GPIO_Handle_128_158 < 0) {
    Debug("gpiochip128 (/dev/gpiochip4) Export Failed\n");
    return -1;
  }

  // Kira
  SPI_Handle = lgSpiOpen(3, 0, 10000000, 0);
  IIC_Handle = lgI2cOpen(2, IIC_Address, 0);
  DEV_GPIO_Init();
#elif USE_GPIOD_LIB
  GPIOD_Export();
  DEV_GPIO_Init();

  DEV_HARDWARE_I2C_begin("/dev/i2c-1");
  DEV_HARDWARE_I2C_setSlaveAddress(IIC_Address);

  DEV_HARDWARE_SPI_begin("/dev/spidev0.0");
  DEV_HARDWARE_SPI_setSpeed(10000000);
#endif
  return 0;
}

/**
 * SPI
 **/
void DEV_SPI_WriteByte(uint8_t Value) {
#ifdef USE_BCM2835_LIB
  bcm2835_spi_transfer(Value);
#elif USE_WIRINGPI_LIB
  wiringPiSPIDataRW(0, &Value, 1);
#elif USE_LGPIO_LIB
  lgSpiWrite(SPI_Handle, (char *)&Value, 1);
#elif USE_GPIOD_LIB
  DEV_HARDWARE_SPI_TransferByte(Value);
#endif
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len) {
#ifdef USE_BCM2835_LIB
  char rData[Len];
  bcm2835_spi_transfernb(pData, rData, Len);
#elif USE_WIRINGPI_LIB
  wiringPiSPIDataRW(0, pData, Len);
#elif USE_LGPIO_LIB
  lgSpiWrite(SPI_Handle, (char *)pData, Len);
#elif USE_GPIOD_LIB
  DEV_HARDWARE_SPI_Transfer(pData, Len);
#endif
}

UBYTE I2C_Write_Byte(UWORD Reg, char *Data, UBYTE len) {
  char wbuf[50] = {(Reg >> 8) & 0xff, Reg & 0xff};
  for (UBYTE i = 0; i < len; i++) {
    wbuf[i + 2] = Data[i];
  }
#ifdef USE_BCM2835_LIB
  if (bcm2835_i2c_write(wbuf, len + 2) != BCM2835_I2C_REASON_OK) {
    printf("WRITE ERROR \r\n");
    return -1;
  }
#elif USE_WIRINGPI_LIB
  if (len > 1)
    printf("wiringPi I2C WARING \r\n");
  wiringPiI2CWriteReg16(fd, wbuf[0], wbuf[1] | (wbuf[2] << 8));
#elif USE_LGPIO_LIB
  lgI2cWriteDevice(IIC_Handle, wbuf, len + 2);
#elif USE_GPIOD_LIB
  DEV_HARDWARE_I2C_write(wbuf, len + 2);
#endif

  return 0;
}

#ifdef USE_WIRINGPI_LIB
static I2C_Write_WiringPi(UWORD Reg) {
  wiringPiI2CWriteReg8(fd, (Reg >> 8) & 0xff, Reg & 0xff);
}
#endif

UBYTE I2C_Read_Byte(UWORD Reg, char *Data, UBYTE len) {
  char *rbuf = Data;

#ifdef USE_BCM2835_LIB
  I2C_Write_Byte(Reg, 0, 0);
  if (bcm2835_i2c_read(rbuf, len) != BCM2835_I2C_REASON_OK) {
    printf("READ ERROR \r\n");
    return -1;
  }
#elif USE_WIRINGPI_LIB
  I2C_Write_WiringPi(Reg);
  for (UBYTE i = 0; i < len; i++) {
    rbuf[i] = wiringPiI2CRead(fd);
  }
#elif USE_LGPIO_LIB
  I2C_Write_Byte(Reg, 0, 0);
  lgI2cReadDevice(IIC_Handle, rbuf, len);
#elif USE_GPIOD_LIB
  I2C_Write_Byte(Reg, 0, 0);
  DEV_HARDWARE_I2C_read(rbuf, len);
#endif

  return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void) {
  TRST_0;
  DEV_Digital_Write(EPD_CS_PIN, 0);
  DEV_Digital_Write(EPD_DC_PIN, 0);
  DEV_Digital_Write(EPD_RST_PIN, 0);
#ifdef USE_BCM2835_LIB
  bcm2835_i2c_end();
  bcm2835_spi_end();
  bcm2835_close();
#elif USE_WIRINGPI_LIB

#elif USE_LGPIO_LIB
  lgI2cClose(IIC_Handle);
  lgSpiClose(SPI_Handle);
  // Kira
  lgGpiochipClose(GPIO_Handle_96_127);
  lgGpiochipClose(GPIO_Handle_128_158);
#elif USE_GPIOD_LIB
  DEV_HARDWARE_I2C_end();
  DEV_HARDWARE_SPI_end();
  GPIOD_Unexport(EPD_CS_PIN);
  GPIOD_Unexport(EPD_DC_PIN);
  GPIOD_Unexport(EPD_RST_PIN);
  GPIOD_Unexport(EPD_BUSY_PIN);
  GPIOD_Unexport_GPIO();
#endif
}
