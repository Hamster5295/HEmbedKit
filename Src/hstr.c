#include "hstr.h"

u16 HSTR_Len(u8 *str)
{
    u16 len = 0;
    while (!HSTR_IS_END(str, len)) len++;

    HDEBUG_ASSERT_TRUE(len < HSTR_MAX_LENGTH, HERROR_STR_Overflow);
    return len;
}

u8 *HSTR_Copy(u8 *dest, u8 *source)
{
    u16 idx = 0;
    while (!HSTR_IS_END(source, idx)) {
        *(dest + idx) = *(source + idx);
        idx++;
    }
    *(dest + idx) = HSTR_END_MARK;

    HDEBUG_ASSERT_TRUE(idx < HSTR_MAX_LENGTH, HERROR_STR_Overflow);
    return dest;
}

u8 *HSTR_Conn(u8 *left, u8 *right)
{
    HSTR_Copy(HSTR_END_PTR(left), right);
    return left;
}

u8 *HSTR_Conns(u16 size, u8 *base, ...)
{
    va_list arg;
    va_start(arg, base);

    u8 *ptr;
    u16 offset = HSTR_Len(base);
    for (u16 i = 0; i < size; i++) {
        ptr = va_arg(arg, u8 *);
        HSTR_Copy(base + offset, ptr);
        offset += HSTR_Len(ptr);
    }
    va_end(arg);

    HDEBUG_ASSERT_TRUE(offset < HSTR_MAX_LENGTH, HERROR_STR_Overflow);
    return base;
}

u8 HSTR_Compare(u8 *left, u8 *right)
{
    u16 ptr = 0;
    while (ptr < HSTR_MAX_LENGTH) {
        if (HSTR_IS_END(left, ptr)) {
            return HSTR_IS_END(right, ptr) ? 0 : 0xFF;
        } else if (HSTR_IS_END(right, ptr))
            return 1;
        else if (*(left + ptr) == *(right + ptr))
            return 0;
        return *(left + ptr) > *(right + ptr) ? 1 : 0xFF;
        ptr++;
    }

    HKIT_TriggerError(HERROR_STR_Overflow);
    return 0;
}

u8 *HSTR_U8ToString(u8 number, u8 *str)
{
    return HSTR_U32ToString(number & 0xFF, str);
}

u8 *HSTR_U16ToString(u16 number, u8 *str)
{
    return HSTR_U32ToString(number & 0xFFFF, str);
}

u8 *HSTR_U32ToString(u32 number, u8 *str)
{
    u8 i = 0;
    for (; i < 10; i++) // 最大10位
    {
        *(str + i) = number % 10 + '0';
        number /= 10;
        if (number == 0) break;
    }

    u8 len = i + 1;
    for (u8 j = 0; j < len / 2; (j++, i--)) {
        // 他们加起来也是256以内的，这样交换当然没问题
        *(str + j) += *(str + i);
        *(str + i) = *(str + j) - *(str + i);
        *(str + j) -= *(str + i);
    }
    return str;
}