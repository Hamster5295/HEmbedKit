#if !defined(__HMATH_H__)
#define __HMATH_H__

#define HMAX(x, y)          (((x) > (y)) ? (x) : (y))
#define HMIN(x, y)          (((x) > (y)) ? (y) : (x))
#define HCLAMP(x, max, min) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))

#define HDIFF_ABS(x, y)     (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))

#endif // __HMATH_H__
