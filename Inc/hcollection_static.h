#if !defined(__HCOLLECTION_STATIC_H__)
#define __HCOLLECTION_STATIC_H__

#define HCOLS_CONV_ARRAY(name, size, datatype)                                                                                                                                                                 \
    typedef struct __HCOLS_CONV_ARR_##name {                                                                                                                                                                   \
        datatype arr[size];                                                                                                                                                                                    \
        u16 ptr;                                                                                                                                                                                               \
        u32 __sum;                                                                                                                                                                                             \
        u32 (*mean)();                                                                                                                                                                                         \
        void (*push)(datatype data);                                                                                                                                                                           \
        datatype (*max)();                                                                                                                                                                                     \
        datatype (*min)();                                                                                                                                                                                     \
        datatype (*range)();                                                                                                                                                                                   \
    } __HCOLS_CONV_ARR_##name;                                                                                                                                                                                 \
    u32 __HCOLS_CONV_ARR_##name##_mean();                                                                                                                                                                      \
    void __HCOLS_CONV_ARR_##name##_push(datatype data);                                                                                                                                                        \
    datatype __HCOLS_CONV_ARR_##name##_max();                                                                                                                                                                  \
    datatype __HCOLS_CONV_ARR_##name##_min();                                                                                                                                                                  \
    datatype __HCOLS_CONV_ARR_##name##_range();                                                                                                                                                                \
    __HCOLS_CONV_ARR_##name name = {{0}, 0, 0, __HCOLS_CONV_ARR_##name##_mean, __HCOLS_CONV_ARR_##name##_push, __HCOLS_CONV_ARR_##name##_max, __HCOLS_CONV_ARR_##name##_min, __HCOLS_CONV_ARR_##name##_range}; \
    u32 __HCOLS_CONV_ARR_##name##_mean()                                                                                                                                                                       \
    {                                                                                                                                                                                                          \
        return name.__sum / size;                                                                                                                                                                              \
    }                                                                                                                                                                                                          \
    void __HCOLS_CONV_ARR_##name##_push(datatype data)                                                                                                                                                         \
    {                                                                                                                                                                                                          \
        name.__sum -= name.arr[name.ptr];                                                                                                                                                                      \
        name.__sum += data;                                                                                                                                                                                    \
        name.arr[name.ptr++] = data;                                                                                                                                                                           \
        if (name.ptr >= size) name.ptr = 0;                                                                                                                                                                    \
    }                                                                                                                                                                                                          \
    datatype __HCOLS_CONV_ARR_##name##_max()                                                                                                                                                                   \
    {                                                                                                                                                                                                          \
        datatype m = name.arr[0];                                                                                                                                                                              \
        for (u16 i = 1; i < size; i++)                                                                                                                                                                         \
            if (m < name.arr[i]) m = name.arr[i];                                                                                                                                                              \
        return m;                                                                                                                                                                                     \
    }                                                                                                                                                                                                          \
    datatype __HCOLS_CONV_ARR_##name##_min()                                                                                                                                                                   \
    {                                                                                                                                                                                                          \
        datatype m = name.arr[0];                                                                                                                                                                              \
        for (u16 i = 1; i < size; i++)                                                                                                                                                                         \
            if (m > name.arr[i]) m = name.arr[i];                                                                                                                                                              \
        return m;                                                                                                                                                                                     \
    }                                                                                                                                                                                                          \
    datatype __HCOLS_CONV_ARR_##name##_range()                                                                                                                                                                 \
    {                                                                                                                                                                                                          \
        datatype min = name.arr[0];                                                                                                                                                                            \
        datatype max = name.arr[0];                                                                                                                                                                            \
        for (u16 i = 1; i < size; i++) {                                                                                                                                                                       \
            if (max < name.arr[i]) max = name.arr[i];                                                                                                                                                              \
            if (min > name.arr[i]) min = name.arr[i];                                                                                                                                                              \
        }                                                                                                                                                                                                      \
        return max - min;                                                                                                                                                                                      \
    }

#endif // __HCOLLECTION_STATIC_H__
