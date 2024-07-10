#include "hkit.h"
#include "hstr.h"

void (*error_handler)(HError err) = HKIT_DefaultErrorHandler;

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
            HSTR_Copy(str, "String overflows over 65535 bytes");
            break;

        case HERROR_DEBUG_NotInited:
            HSTR_Copy(str, "HDEBUG is not initialized.\r\n HDEBUG_Init(UART_HandleType_Def) is required.");
            break;

        default:
            HSTR_Copy(str, "Unknown Error! Code: ");
            HSTR_Conn(str, HSTR_U8ToString(err, str));
            break;
    }
    return str;
}

void HKIT_DefaultErrorHandler(HError err)
{
#ifdef ENABLE_HDEBUG
    if (HDEBUG_IsInited()) {
        HDPrintError(err);
    }
#endif
}