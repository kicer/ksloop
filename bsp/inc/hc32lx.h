#ifndef __HC32Lx_H
#define __HC32Lx_H


#if !defined  (HC32Lx)
#define HC32Lx
#endif /* HC32Lx */


/** @brief CMSIS Device version number V2.3.3 */
#define __HC32Lx_DEVICE_VERSION_MAIN   (0x02) /*!< [31:24] main version */
#define __HC32Lx_DEVICE_VERSION_SUB1   (0x03) /*!< [23:16] sub1 version */
#define __HC32Lx_DEVICE_VERSION_SUB2   (0x03) /*!< [15:8]  sub2 version */
#define __HC32Lx_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __HC32Lx_DEVICE_VERSION        ((__HC32Lx_DEVICE_VERSION_MAIN << 24)\
                                        |(__HC32Lx_DEVICE_VERSION_SUB1 << 16)\
                                        |(__HC32Lx_DEVICE_VERSION_SUB2 << 8 )\
                                        |(__HC32Lx_DEVICE_VERSION_RC))


#if defined(HC32L130E8) || defined(hc32l130e8)
  #include "hc32l130e8.h"
#elif defined(HC32L130F8) || defined(hc32l130f8)
  #include "hc32l130f8.h"
#elif defined(HC32L130J8) || defined(hc32l130j8)
  #include "hc32l130j8.h"
#elif defined(HC32L110B4) || defined(hc32l110b4)
  #include "hc32l110b4.h"
#elif defined(HC32L110B6) || defined(hc32l110b6)
  #include "hc32l110b6.h"
#elif defined(HC32L110C4) || defined(hc32l110c4)
  #include "hc32l110c4.h"
#elif defined(HC32L110C6) || defined(hc32l110c6)
  #include "hc32l110c6.h"
#else
 #error "Please select the target HC32Lx device used in your application"
#endif


#endif /* __HC32Lx_H */
