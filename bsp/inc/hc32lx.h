#ifndef __HC32Lx_H
#define __HC32Lx_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


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


#if defined(HC32L130E8PA) || defined(hc32l130e8pa)
  #include "hc32l130e8pa.h"
#elif defined(HC32L130F8UA) || defined(hc32l130f8ua)
  #include "hc32l130f8ua.h"
#elif defined(HC32L130J8TA) || defined(hc32l130j8ta)
  #include "hc32l130j8ta.h"
#elif defined(HC32L110B4) || defined(hc32l110b4)
  #include "hc32l110b4.h"
#elif defined(HC32L110C6) || defined(hc32l110c6)
  #include "hc32l110c6.h"
#else
 #error "Please select the target HC32Lx device used in your application"
#endif


typedef enum 
{
  RESET = 0, 
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum 
{
  DISABLE = 0, 
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

#ifndef ERROR
typedef enum 
{
  ERROR = 0, 
  SUCCESS = !ERROR
} ErrorStatus;
#endif /* ERROR defined in hc32fx.h */


#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HC32Lx_H */
