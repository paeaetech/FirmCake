#ifndef OWNTYPES_H_FESDE5BI
#define OWNTYPES_H_FESDE5BI

#include <inttypes.h>

struct Point {
	uint32_t x;
	uint32_t y;
	uint32_t z;
	
	Point() : x(0),y(0),z(0) {}
	Point(uint32_t _x,uint32_t _y,uint32_t _z) : x(_x), y(_y), z(_z) {}
};


#endif /* end of include guard: OWNTYPES_H_FESDE5BI */
