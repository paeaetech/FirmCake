
#ifndef VERSION_H_UBUGHFE1
#define VERSION_H_UBUGHFE1


#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION (VERSION_MAJOR*100+VERSION_MINOR)
#define NAME "FirmCake"
#define _VERSIONSTRING(x,y) "v" #x "." #y
#define VERSIONSTRING _VERSIONSTRING(1,0)

//build_id.h contains makefile generated BUILD_ID string define. you can also define it manually

//#define BUILD_ID "none"
#ifndef BUILD_ID
#include "build_id.h"
#endif

#endif /* end of include guard: VERSION_H_UBUGHFE1 */
