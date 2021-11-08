#ifndef _TUBE_H_
#define _TUBE_H_


#include <stdint.h>

extern int tube_init(void);
extern int tube_show_digi(uint32_t val, uint8_t point);
extern int tube_show_str(uint8_t *pstr, int size);
extern int tube_show_label(uint8_t *pstr, int size, uint32_t val);
extern int tube_set_bright(uint8_t bright_level);


#endif /* _TUBE_H_ */
