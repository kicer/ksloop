#include "hw_bare.h"
#include "tube.h"
#include "i2c_sw_master.h"


/* ################# tube functions #################   */
uint8_t TUBE_CODE[] = {
    /* 0-9 */
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,
    /* A-Z */
    0x77,0x7c,0x39,0x5e,0x79,0x71,0x3d,0x76,0x0f,0x0e,
    0x75,0x38,0x37,0x54,0x5c,0x73,0x67,0x31,0x49,0x78,
    0x3e,0x1c,0x7e,0x64,0x6e,0x59
};

#define TUBE_SIZE   4
#define TUBE_NULL   0x00
#define TUBE_MINUS  0x40
#define TUBE_POINT  0x80

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
    /* fix with seg setting */
    int ack_err = 0;
    ack_err += tube_set_bright(_tube_level);
    ack_err += tm1650_write(0x68, pdat[3]);
    ack_err += tm1650_write(0x6A, pdat[0]);
    ack_err += tm1650_write(0x6C, pdat[1]);
    ack_err += tm1650_write(0x6E, pdat[2]);
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
        _tube_data[0] = 0xFF;
        if(_tube_data[1] == 0) {
            _tube_data[1] = 0xFF;
            if(_tube_data[2] == 0) {
                _tube_data[2] = 0xFF;
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

int tube_show_str(uint8_t *pstr, int size) {
    uint8_t _tube_data[TUBE_SIZE];
    for(int i=0; i<TUBE_SIZE; i++) {
        uint8_t code = TUBE_NULL;
        if(size >= TUBE_SIZE-i) {
            uint8_t ch = pstr[i];
            if((ch>='a')&&(ch<='z')) {
                code = TUBE_CODE[ch-'a'+10];
            } else if((ch>='A')&&(ch<='Z')) {
                code = TUBE_CODE[ch-'A'+10]|TUBE_POINT;
            } else if((ch>='0')&&(ch<='9')) {
                code = TUBE_CODE[ch-'0'];
            } else if(ch == '-') {
                code = TUBE_MINUS;
            } else if(ch == '.') {
                code = TUBE_POINT;
            }
        }
        _tube_data[i] = code;
    }
    return _tube_show(_tube_data);
}

int tube_show_label(uint8_t *pstr, int size, uint32_t val) {
    uint8_t dat[4];
    for(int i=0; i<size; i++) {
        dat[i] = pstr[i];
    }
    for(int i=size; i<4; i++) {
        dat[4+size-i-1] = '0'+val%10;
        val = val/10;
    }
    return tube_show_str(dat, sizeof(dat));
}

int tube_init(void) {
    tm1650_init();
    return 0;
}
