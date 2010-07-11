#ifndef OWNTYPES_H_FESDE5BI
#define OWNTYPES_H_FESDE5BI

#include <inttypes.h>

struct Point {
	int32_t x;
	int32_t y;
	int32_t z;
	
	Point() : x(0),y(0),z(0) {}
	Point(int32_t _x,int32_t _y,int32_t _z) : x(_x), y(_y), z(_z) {}
	
	void set(Point &rB) 
	{
		x = rB.x;
		y = rB.y;
		z = rB.z;
	}
	void zero()
	{
		x = 0; 
		y = 0; 
		z = 0;
	}
};


#endif /* end of include guard: OWNTYPES_H_FESDE5BI */
