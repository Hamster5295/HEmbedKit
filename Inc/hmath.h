#if !defined(__HMATH_H__)
#define __HMATH_H__

#include "hkit.h"

#define HMAX(x, y)          (((x) > (y)) ? (x) : (y))
#define HMIN(x, y)          (((x) > (y)) ? (y) : (x))
#define HCLAMP(x, max, min) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))

#define HFLOOR(x)           ((i32)(x))
#define HCEIL(x)            ((i32)(x + 0.999f))
#define HROUND(x)           ((i32)(x + 0.5f))

#define HDIFF_ABS(x, y)     (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))

#endif // __HMATH_H__
