#ifndef _I2C_SW_MASTER_H_
#define _I2C_SW_MASTER_H_


#include <stdint.h>

/* Initializes the ports for I2C interface. */
void i2c_sw_Init(void);
void i2c_sw_deinit(void);

/* Writes a start condition on I2C-Bus. */
//       _____
// SDA:       |_____
//       _______
// SCL:         |___
void i2c_sw_StartCondition(void);

/* Writes a stop condition on I2C-Bus. */
//              _____
// SDA:   _____|
//            _______
// SCL:   ___|
void i2c_sw_StopCondition(void);

/* @return: ackBit (0:ACK, 1:NACK) */
int i2c_sw_WriteByte(uint8_t txByte);

/* @param: ackBit (0:ACK, 1:NACK) */
/* @return: error count */
int i2c_sw_ReadByte(uint8_t *rxByte, uint8_t ackBit);

//#define SOFT_I2C_WRITEREVBYTE_ENABLE
int i2c_sw_WriteRevByte(uint8_t txByte);


#endif /* _I2C_SW_MASTER_H_ */
