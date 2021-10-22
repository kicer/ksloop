#include "hw_bare.h"
#include "i2c_sw_master.h"


#define SOFT_I2C_ENABLE

/********* -- adapt the defines for your hw -- ********/
// SDA on port B, bit 5
#define SDA_IN()   do { \
    M0P_GPIO->PBDIR_f.PB05 = DIR_OUT; \
} while(0)

#define SDA_OUT()  do { \
    M0P_GPIO->PBDIR_f.PB05 = DIR_IN; \
} while(0)

#define SDA_LOW()  do { \
    M0P_GPIO->PBOUT_f.PB05 = 0; \
} while(0)

#define SDA_HIGH() do { \
    M0P_GPIO->PBOUT_f.PB05 = 1; \
} while(0)

#define SDA_READ   (M0P_GPIO->PBIN_f.PB05 != 0)


/********* -- adapt the defines for your hw -- ********/
// SCL on port B, bit 7
#define SCL_IN()   do { \
    M0P_GPIO->PBDIR_f.PB07 = DIR_OUT; \
} while(0)

#define SCL_OUT()  do { \
    M0P_GPIO->PBDIR_f.PB07 = DIR_IN; \
} while(0)

#define SCL_LOW()  do { \
    M0P_GPIO->PBOUT_f.PB07 = 0; \
} while(0)

#define SCL_HIGH() do { \
    M0P_GPIO->PBOUT_f.PB07 = 1; \
} while(0)

#define SCL_READ   (M0P_GPIO->PBIN_f.PB07 != 0)


#define I2C_NOP()  do {              \
    for(uint32_t i=0; i<20; i++) {   \
        __NOP();                     \
    }} while(0)

#define I2C_LONG_NOP()  do {         \
    for(uint32_t i=0; i<40; i++) {   \
        __NOP();                     \
    }} while(0)

/* clock streching timeout(0 for disabled) */
#define I2C_CS_TICKS        0


#ifdef SOFT_I2C_ENABLE

void i2c_sw_Init(void) {
    gpio_init_out(PB,05,1);
    gpio_init_out(PB,07,1);
}

void i2c_sw_deinit(void) {
    gpio_init_in(PB,05);
    gpio_init_in(PB,07);
    gpio_floating(PB,05);
    gpio_floating(PB,07);
}

void i2c_sw_StartCondition(void) {
    /* init to high */
    SDA_HIGH(); SCL_HIGH(); I2C_NOP();
    /* sda high->low when scl high */
    SDA_LOW(); I2C_LONG_NOP(); SCL_LOW();
    I2C_NOP();
}

void i2c_sw_StopCondition(void) {
    /* init to low */
    SCL_LOW(); SDA_LOW(); I2C_NOP();
    /* sda low->high when scl high */
    SCL_HIGH(); I2C_LONG_NOP(); SDA_HIGH();
    I2C_NOP();
}

int i2c_sw_WriteByte(uint8_t txByte) {
    int ackBit = 0;
    uint8_t mask;

    /* write byte */
    for(mask = 0x80; mask > 0; mask >>= 1) {
        I2C_NOP();
        if((mask & txByte) == 0) {
            SDA_LOW();
        } else {
            SDA_HIGH();
        }
        I2C_NOP();
        SCL_HIGH();
        I2C_LONG_NOP();
        SCL_LOW();
    }
    I2C_LONG_NOP();

    /* read ack */
    SDA_IN(); SCL_HIGH(); I2C_LONG_NOP();
    if(SDA_READ) ackBit = 1;

    /* to idle */
    SCL_LOW(); I2C_NOP(); SDA_OUT();
    return ackBit;
}

#ifdef SOFT_I2C_WRITEREVBYTE_ENABLE
int i2c_sw_WriteRevByte(uint8_t txByte) {
    int ackBit = 0;
    uint8_t mask;

    /* write byte */
    for(mask = 0x01; mask > 0; mask <<= 1) {
        I2C_NOP();
        if((mask & txByte) == 0) {
            SDA_LOW();
        } else {
            SDA_HIGH();
        }
        I2C_NOP();
        SCL_HIGH();
        I2C_LONG_NOP();
        SCL_LOW();
    }
    I2C_LONG_NOP();

    /* read ack */
    SDA_IN(); SCL_HIGH(); I2C_LONG_NOP();
    if(SDA_READ) ackBit = 1;

    /* to idle */
    SCL_LOW(); I2C_NOP(); SDA_OUT();
    return ackBit;
}
#endif /* SOFT_I2C_WRITEREVBYTE_ENABLE */

/* for low-speed device */
#if I2C_CS_TICKS > 0
static int i2c_sw_WaitWhileClockStreching(int timeout) {
    int busy = 0;
    if(timeout > 0) {
        SCL_IN();
        while(timeout-- > 0) {
            I2C_NOP();
            if(SCL_READ) break;
            if(timeout == 0) busy = 1;
        }
        SCL_OUT();
        SCL_HIGH();
    }
    return busy;
}
#endif /* I2C_CS_TICKS */

int i2c_sw_ReadByte(uint8_t *rxByte, uint8_t ackBit) {
    int error = 0;
    uint8_t mask;
    *rxByte = 0x00;

    /* read byte */
    SDA_IN(); 
    for(mask = 0x80; mask > 0; mask >>= 1) {
        I2C_NOP();
        SCL_HIGH();I2C_NOP();
#if I2C_CS_TICKS > 0
        error += i2c_sw_WaitWhileClockStreching(I2C_CS_TICKS);
#endif /* I2C_CS_TICKS */
        if(SDA_READ) *rxByte |= mask;
        SCL_LOW();
    }
    I2C_LONG_NOP();

    /* send ack */
    if(ackBit) { /* NACK */
        SDA_OUT();
        SDA_HIGH();
    } else { /* ACK */
        SDA_OUT();
        SDA_LOW();
    }
    I2C_NOP(); SCL_HIGH(); I2C_LONG_NOP();

    /* to idle */
    SCL_LOW(); SDA_HIGH(); I2C_LONG_NOP();
    return error;
}
#endif /* SOFT_I2C_ENABLE */