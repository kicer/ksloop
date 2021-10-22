#include <stdint.h>
#include "i2c_sw_master.h"


#define I2C_WRITE       ((uint8_t)0)
#define I2C_READ        ((uint8_t)1)
#define SHT3x_ADDR0     ((uint8_t)(0x44<<1))


/* SHT3x芯片初始化 */
void sht3x_init(void) {
    i2c_sw_Init();
}

void sht3x_deinit(void) {
    i2c_sw_deinit();
}

/* SHT3x开始测量，等待15ms后读取数据 */
void sht3x_meas(void) {
    /* sht3x with A=0 */
    i2c_sw_StartCondition();
    i2c_sw_WriteByte(SHT3x_ADDR0+I2C_WRITE);
    i2c_sw_WriteByte(0x24);
    i2c_sw_WriteByte(0x00);
    i2c_sw_StopCondition();
}

/* SHT3x读取测量数据
 *      0: 读取出错或未读出
 *      x: 4字节传感器数值，本地或云端计算得出
 */
int sht3x_read(uint16_t *t, uint16_t *rh) {
    uint8_t rxByte[6];
    int ack_err = 0;
    i2c_sw_StartCondition();
    ack_err = i2c_sw_WriteByte(SHT3x_ADDR0+I2C_READ);
    if(ack_err == 0) {
        for(int k=0; k<6; k++) {
            ack_err += i2c_sw_ReadByte(rxByte+k, (k==5)?1:0);
        }
        if((rxByte[3]==0)&&(rxByte[4]==0)) {
            ack_err += 1;
        }
    }
    i2c_sw_StopCondition();
    if(ack_err == 0) {
        /* 温湿度计算公式 */
        *t = (uint32_t)1750*(((uint16_t)rxByte[0]<<8)+rxByte[1])/65535+550;
        *rh = (uint32_t)1000*(((uint16_t)rxByte[3]<<8)+rxByte[4])/65535;
    }

    return ack_err;
}
