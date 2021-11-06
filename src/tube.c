#include "hw_bare.h"
#include "tube.h"
#include "i2c_sw_master.h"


/* ################# tube functions #################   */
uint8_t TUBE_CODE[] = { /* 0-9 */
    0x3F,0x06,0x5B,0x4F,0x66,
    0x6D,0x7D,0x07,0x7F,0x6F,
};

#define TUBE_SIZE   4
#define TUBE_NULL   0xFF
#define TUBE_MINUS  0x40

/* ######## TM1650 driver ########## */
#define TM1650_CMD_SHOW      0x48
#define TM1650_SHOW_OFF      0x00
#define TM1650_MODE_4x8      0x01
#define TM1650_MODE_4x7      0x09

#define NOP()       hw_delay_us(2)

static int _tube_level = 8;

static void tm1650_init(void) {
    i2c_sw_Init();
}
static int tm1650_write(uint8_t cmd, uint8_t dat) {
    int ack_err = 0;
    i2c_sw_StartCondition();
    ack_err += i2c_sw_WriteByte(cmd);
    ack_err += i2c_sw_WriteByte(dat);
    i2c_sw_StopCondition();
    return ack_err;
}

static int _tube_show(uint8_t pdat[TUBE_SIZE]) {
    int ack_err = 0;
    ack_err += tube_set_bright(_tube_level);
    for(int i=0; i<TUBE_SIZE; i++) {
        ack_err += tm1650_write(0x68+(i<<1), pdat[i]);
    }
    return ack_err;
}

/* level: 0~8; 0:off */
int tube_set_bright(uint8_t level) {
    int ack_err = 0;
    _tube_level = level;
    if(level == 0) {
        ack_err += tm1650_write(TM1650_CMD_SHOW, TM1650_SHOW_OFF);
    } else {
        if(level>8) level = 0;
        ack_err += tm1650_write(TM1650_CMD_SHOW, TM1650_MODE_4x8|(level<<4));
    }
    return ack_err;
}

int tube_show_digi(uint32_t val) {
    uint8_t _tube_data[TUBE_SIZE];
    if(val > 9999) val = 9999;
    _tube_data[3] = val%10; val = val/10;
    _tube_data[2] = val%10; val = val/10;
    _tube_data[1] = val%10; val = val/10;
    _tube_data[0] = val%10;

    /* fix value */
    if(_tube_data[0] == 0) {
        _tube_data[0] = TUBE_NULL;
        if(_tube_data[1] == 0) {
            _tube_data[1] = TUBE_NULL;
            if(_tube_data[2] == 0) {
                _tube_data[2] = TUBE_NULL;
            }
        }
    }

    for(int i=0; i<TUBE_SIZE; i++) {
        uint8_t ch = _tube_data[i];
        if(ch<10) {
            ch = TUBE_CODE[ch];
        } else {
            ch = 0x00;
        }
        _tube_data[i] = ch;
    }

    return _tube_show(_tube_data);
}

int tube_show_wait(void) {
    uint8_t _tube_data[TUBE_SIZE];
    for(int i=0; i<TUBE_SIZE; i++) {
        _tube_data[i] = TUBE_MINUS;
    }
    return _tube_show(_tube_data);
}

int tube_init(uint8_t bright_level) {
    tm1650_init();
    //return tube_set_bright(bright_level);
    return 0;
}
