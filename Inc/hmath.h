#if !defined(__HMATH_H__)
#define __HMATH_H__

#define HCLAMP(x, max, min) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))

#endif // __HMATH_H__
