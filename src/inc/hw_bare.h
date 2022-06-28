/*
** $Id: hw_bare.h $
** hw_bare - bare of hardware api
** Kicer, Shandong (https://foresh.com)
*/

#ifndef _HW_BARE_H_
#define _HW_BARE_H_


#define HWB_VER_MAJOR   "2"
#define HWB_VER_MINOR   "1"
#define HWB_VER_RELEASE "1"

#define HWB_VERSION  "HwBare" HWB_VERSION_MAJOR "." HWB_VERSION_MINOR
#define HWB_RELEASE  HWB_VERSION "." HWB_VERSION_RELEASE

#define HWB_SIGNATURE   "HwBare"

#include "hwb_aux.h"
#include "hwb_gpio.h"
#include "hwb_ticks.h"
#include "hwb_log.h"
#include "hwb_os.h"


#endif /* _HW_BARE_H_ */
