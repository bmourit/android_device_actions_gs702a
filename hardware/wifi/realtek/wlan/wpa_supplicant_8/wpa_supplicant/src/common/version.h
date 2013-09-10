#ifndef VERSION_H
#define VERSION_H

#ifndef VERSION_STR_POSTFIX
#define VERSION_STR_POSTFIX ""
#endif /* VERSION_STR_POSTFIX */

#define VERSION_STR "2.0-devel" VERSION_STR_POSTFIX

#ifdef REALTEK_WIFI_VENDOR
	#include "rtw_version.h"
	#ifdef RTW_VERSION
		#undef VERSION_STR
		#define VERSION_STR "2.0-devel" VERSION_STR_POSTFIX "_" RTW_VERSION
	#endif
#endif

#endif /* VERSION_H */
