#ifndef _TUBE_H_
#define _TUBE_H_


#include <stdint.h>

extern int tube_init(uint8_t bright_level);
extern int tube_show_wait(void);
extern int tube_show_digi(uint32_t val);
extern int tube_set_bright(uint8_t bright_level);


#endif /* _TUBE_H_ */