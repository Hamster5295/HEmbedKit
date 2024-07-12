#include "hstr.h"

u8 hstr_buffer[HSTR_BUFFER_SIZE + 1]; // +1 是为了最后一个字符串的 \0 放得下
u32 hstr_buffer_ptr = 0;

/**
 * 从缓冲区分配 size 长度的空间并给出
 * @param size 需要分配的长度
 * @return 分配后空间的首地址
 */
u8 *hstr_alloc(u16 size)
{
    HDEBUG_ASSERT_TRUE(size < HSTR_BUFFER_SIZE, HERROR_STR_Overflow);
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
    return HSTR_Copy(hstr_alloc(HSTR_GetLen(data)), data);
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

u8 *HSTR_Concat(u8 *left, u8 *right)
{
    u16 len_left = HSTR_GetLen(left);
    u8 *str      = hstr_alloc(len_left + HSTR_GetLen(right));
    HSTR_Copy(str, left);
    HSTR_Copy(str + len_left, right);
    return str;
}

u8 HSTR_Compare(u8 *left, u8 *right)
{
    u16 ptr = 0;
    while (ptr < HSTR_BUFFER_SIZE) {
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
    u8 *str = hstr_alloc(len + 1);

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

#ifdef ENABLE_HDEBUG
u8 *HSTR_ErrorToString(HError err)
{
    switch (err) {
        case HERROR_STR_Overflow:
            return HSTR_New("String overflows. Define HSTR_BUFFER_SIZE as a larger value!");

        case HERROR_DEBUG_Unavailable:
            return HSTR_New("HDEBUG is not available.\r\nENABLE_HDEBUG is not defined, or HDEBUG_Init(huart) is not called in advance.");

        case HERROR_WIFI_Unavailable:
            return HSTR_New("HWiFi is not available.\r\nPlease call HWIFI_Init() in advance!");

        case HERROR_WIFI_InitFailed:
            return HSTR_New("HWiFi init failed. Response from module is not correct.");

        case HERROR_WIFI_BlockTimeout:
            return HSTR_New("HWiFi blocked operation timeout.");

        default:
            return HSTR_Concat("Unknown Error! Code: ", HSTR_U8ToString(err));
    }
}
#endif