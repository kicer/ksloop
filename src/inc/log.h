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
extern int log_deinit(void);

extern int dmesg(uint8_t loglevel, const uint8_t *pbuf, uint8_t size);
/* dmesg_str(LOG_DEBUG, "hello");
 * dmesg_hex(LOG_DEBUG, "hello", pkg, size); */
#define dmesg_hex(l,h,p,s) do {     \
        dmesg(l,(uint8_t*)(h),(sizeof(h)-1)); \
        dmesg((LOG_HEX|(l)),p,s);   \
    } while(0)
#define dmesg_str(l,s) do {      \
        dmesg(l,(uint8_t*)(s),(sizeof(s)-1)); \
    } while(0)

#define LOG_BAUDRATE      9600
#define LOG_TX_MAXSIZE    64

#define LOG_DEVICE_UART   1
#if defined(LOG_DEVICE_UART)
    #include "uart1.h"    
    #define hal_log_init()     uart1_init(LOG_BAUDRATE)
    #define hal_log_deinit     uart1_deinit
    #define hal_log_send       uart1_send
#else
    #error "Please defined the log.device function"
#endif /* LOG_DEVICE_UART */


#endif /* _LOG_H_ */
