#ifndef __CH55X_H_
#define __CH55X_H_


#if defined(CH551)
    #include "ch552.h"
    #define FLASH_BLOCK_SIZE    ((uint8_t)128)
#elif defined(CH552)
    #include "ch552.h"
    #define FLASH_BLOCK_SIZE    ((uint8_t)128)
#elif defined(CH554)
    #include "ch554.h"
    #define FLASH_BLOCK_SIZE    ((uint8_t)128)
#else
    #error "Please select first the target CH55X device used in your application (in ch55x.h file)"
#endif


#define try_param(x) ((void)x)

#define BSP_wfi()                  ((void)0)
#define BSP_SWReset()              ((void)0)
#define BSP_enableInterrupt()      ((void)0)


#endif /* __CH55X_H_ */
