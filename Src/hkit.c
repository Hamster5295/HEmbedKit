#include "hkit.h"
#include "hstr.h"

void (*error_handler)(HError err) = null;

void HKIT_Init(void (*err_handler)(HError err))
{
    error_handler = err_handler;
}

void HKIT_TriggerError(HError err)
{
    if (error_handler != null) error_handler(err);
}

u8 *HKIT_ErrorToString(HError err, u8 *str)
{
    switch (err) {
        case HERROR_STR_Overflow:
            HSTR_Copy(str, "字符串溢出上限 65535 字节");
            break;

        case HERROR_DEBUG_NotInited:
            HSTR_Copy(str, "HDEBUG 没有初始化, 需要调用 HDEBUG_Init()");
            break;

        default:
            HSTR_Copy(str, "未知错误!");
            // TODO: 等以后有整数转字符串了，在这里补错误代码
            break;
    }
    return str;
}