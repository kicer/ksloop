#ifndef _SHT3X_H_
#define _SHT3X_H_


#include <stdint.h>

/* SHT3x芯片初始化 */
extern void sht3x_init(void);
extern void sht3x_deinit(void);

/* SHT3x开始测量，等待15ms后读取数据 */
extern void sht3x_meas(void);

/* SHT3x读取测量数据 */
extern int sht3x_read(uint16_t *t, uint16_t *rh);


#endif /* _SHT3X_H_ */
