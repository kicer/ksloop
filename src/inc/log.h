/* io-uart logger */
#ifndef _LOG_H_
#define _LOG_H_


#include <stdint.h>

enum { /* loglevel */
	LOG_NONE     = 0x00, /* for disable */
	LOG_ERROR    = 0x01,
	LOG_WARNING  = 0x02,
	LOG_INFO     = 0x03,
	LOG_DEBUG    = 0x04,
	LOG_HEX      = 0x80, /* for dump binary */
};

extern int log_init(uint8_t loglevel);

/* dmesg(LOG_DEBUG, STRING("hello");
 * dmesg_hex(LOG_DEBUG, "hello", pkg); */
#define STRING(s)  (s),(sizeof(s)-1)
#define dmesg_hex(l,h,p,s) do {     \
        dmesg(l,STRING(h));         \
        dmesg((LOG_HEX|(l)),p,s);   \
    } while(0)
extern int dmesg(uint8_t loglevel, const uint8_t *pbuf, uint8_t size);

#define LOG_TX_MAXSIZE    64
#define LOG_BAUDRATE      9600

#define LOG_DEVICE_UART   1


#endif /* _LOG_H_ */
