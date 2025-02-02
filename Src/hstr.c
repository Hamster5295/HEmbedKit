#include "hstr.h"
#include "hdebug.h"

u8 hstr_buffer[HSTR_BUFFER_SIZE + 1]; // +1 是为了最后一个字符串的 \0 放得下
u32 hstr_buffer_ptr = 0;

#define HSTR_ASSERT_LEN(size)                                   \
    do {                                                        \
        if (size > HSTR_BUFFER_SIZE) {                          \
            HDEBUG_Print("Required: ");                         \
            HDEBUG_Println(HSTR_U16ToString(size));             \
            HDEBUG_Print("Buffer: ");                           \
            HDEBUG_Println(HSTR_U16ToString(HSTR_BUFFER_SIZE)); \
            HKIT_TriggerError(HERROR_STR_Overflow);             \
        }                                                       \
    } while (0)
/**
 * 从缓冲区分配 size 长度的空间并给出
 * @param size 需要分配的长度
 * @return 分配后空间的首地址
 */
u8 *hstr_alloc(u16 size)
{
    HSTR_ASSERT_LEN(size);
    if (HSTR_BUFFER_SIZE - hstr_buffer_ptr < size) {
        hstr_buffer_ptr = size;
        return hstr_buffer;
    } else {
        u8 *addr = hstr_buffer + hstr_buffer_ptr;
        hstr_buffer_ptr += size;
        return addr;
    }
}

u8 *HSTR_New(u8 *data)
{
    return HSTR_NewSize(data, HSTR_GetLen(data));
}

u8 *HSTR_NewSize(u8 *data, u16 len)
{
    u8 *str      = HSTR_CopySize(hstr_alloc(len + 1), data, len);
    *(str + len) = HSTR_END_MARK;
    return str;
}

u8 *HSTR_NewEmpty(u16 len)
{
    u8 *str = hstr_alloc(len);
    for (len--; len > 0; len--) *(str + len) = 0;
    *str = 0;
    return str;
}

u16 HSTR_GetLen(u8 *str)
{
    u16 len = 0;
    while (!HSTR_IS_END(str, len)) len++;
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
    return dest;
}

u8 *HSTR_CopySize(u8 *dest, u8 *source, u16 len)
{
    u16 idx = 0;
    for (; idx < len; idx++) {
        *(dest + idx) = *(source + idx);
    }
    return dest;
}

u8 *HSTR_Concat(u8 *left, u8 *right)
{
    u16 len_left = HSTR_GetLen(left);
    u8 *str      = hstr_alloc(len_left + HSTR_GetLen(right));
    HSTR_Copy(str, left);
    HSTR_Copy(str + len_left, right);
    return str;
}

u8 *HSTR_Concats(u16 count, ...)
{
    u16 len = 0;
    va_list argl, argc;
    va_start(argl, count);
    va_copy(argc, argl);
    for (u8 i = 0; i < count; i++) {
        len += HSTR_GetLen(va_arg(argl, u8 *));
    }
    va_end(argl);

    u8 *buf = hstr_alloc(len + 1);
    len     = 0;
    for (u8 i = 0; i < count; i++) {
        u8 *str    = va_arg(argc, u8 *);
        u16 offset = 0;
        while (!HSTR_IS_END(str, offset)) *(buf + len++) = *(str + offset++);
    }
    *(buf + len) = HSTR_END_MARK;
    va_end(argc);
    return buf;
}

u8 HSTR_Compare(u8 *left, u8 *right)
{
    u16 ptr = 0;
    while (ptr < HSTR_BUFFER_SIZE) {
        if (HSTR_IS_END(left, ptr)) {
            return HSTR_IS_END(right, ptr) ? 0 : 0xFF;
        } else if (HSTR_IS_END(right, ptr))
            return 1;
        else if (*(left + ptr) != *(right + ptr))
            return *(left + ptr) > *(right + ptr) ? 1 : 0xFF;
        ptr++;
    }

    HKIT_TriggerError(HERROR_STR_Overflow);
    return 0;
}

u8 HSTR_Equal(u8 *left, u8 *right, u16 len)
{
    for (u16 i = 0; i < len; i++)
        if (*(left + i) != *(right + i)) return false;
    return true;
}

u8 *HSTR_U8ToString(u8 number)
{
    return HSTR_U32ToString(number & 0xFF);
}

u8 *HSTR_U16ToString(u16 number)
{
    return HSTR_U32ToString(number & 0xFFFF);
}

u8 *HSTR_U32ToString(u32 number)
{
    u8 i          = 0;
    u8 buffer[10] = {0};

    for (; i < 10; i++) // 最大10位
    {
        *(buffer + i) = number % 10 + '0';
        number /= 10;
        if (number == 0) break;
    }

    u8 len  = i + 1;
    u8 *str = hstr_alloc(len);

    for (u8 j = 0; j < len; (j++, i--)) {
        *(str + j) = *(buffer + i);
    }
    *(str + len) = HSTR_END_MARK;
    return str;
}

u8 *HSTR_I8ToString(i8 number)
{
    return HSTR_I32ToString(number & 0xFF);
}

u8 *HSTR_I16ToString(i16 number)
{
    return HSTR_I32ToString(number & 0xFFFF);
}

u8 *HSTR_I32ToString(i32 number)
{
    u8 i          = 0;
    u8 buffer[11] = {0};
    bool neg      = number < 0;

    for (; i < 10; i++) // 最大10位
    {
        *(buffer + i) = number % 10 + '0';
        number /= 10;
        if (number == 0) break;
    }

    u8 len = i + 1;

    if (neg) {
        buffer[len] = '-';
        len++;
        i++;
    }

    u8 *str = hstr_alloc(len + 1);

    for (u8 j = 0; j < len; (j++, i--)) {
        *(str + j) = *(buffer + i);
    }
    *(str + len) = HSTR_END_MARK;
    return str;
}

u8 *HSTR_FloatToString(float number, int digit)
{
    u8 i = 0;
    u8 buffer[64];

    i32 integer = (i32)number;
    u8 *intStr  = HSTR_I32ToString(integer);

    if (digit == 0) return intStr;
    u16 len     = HSTR_GetLen(intStr);
    buffer[len] = '.';

    number -= integer;
    for (i = 0; i < digit && i + len < 63; i++) {
        number *= 10;
        u8 val              = (u8)number;
        buffer[i + len + 1] = val + '0';
        number -= val;
    }
    buffer[i + len + 1] = HSTR_END_MARK;

    HSTR_CopySize(buffer, intStr, len);
    return HSTR_New(buffer);
}

u8 *HSTR_FloatToString_D4(float number)
{
    return HSTR_FloatToString(number, 3);
}

#ifdef ENABLE_HDEBUG
u8 *HSTR_ErrorToString(HError err)
{
    switch (err) {
        case HERROR_STR_Overflow:
            return HSTR_New("String overflows. Define HSTR_BUFFER_SIZE as a larger value!");

        case HERROR_DEBUG_Unavailable:
            return HSTR_New("HDEBUG is not available.\r\nENABLE_HDEBUG is not defined, or HDEBUG_Init(huart) is not called in advance.");

        case HERROR_WIFI_Unavailable:
            return HSTR_New("HWIFI is not available.\r\nPlease call HWIFI_Init() in advance!");

        case HERROR_WIFI_InitFailed:
            return HSTR_New("HWIFI init failed. Response from module is not correct.");

        case HERROR_WIFI_Busy:
            return HSTR_New("HWIFI is currently busy under another task!");

        case HERROR_WIFI_BlockTimeout:
            return HSTR_New("HWIFI blocked operation timeout.");

        case HERROR_WIFI_RecvOverflow:
            return HSTR_New("HWIFI recv buffer overflows. Define HWIFI_RECV_BUFFER_SIZE as a larger value!");

        case HERROR_WIFI_SendOverflow:
            return HSTR_New("HWIFI send buffer overflows. Define HWIFI_SEND_BUFFER_SIZE as a larger value!");

        case HERROR_PROM_Unavailable:
            return HSTR_New("HPROM is not available.\r\nPlease call HPROM_Init() in advance!");

        case HERROR_PROM_InitFailed:
            return HSTR_New("HPROM init failed. No response from module.");

        default:
            return HSTR_Concat("Unknown Error! Code: ", HSTR_U8ToString(err));
    }
}
#endif