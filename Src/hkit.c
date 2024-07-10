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

u8 *HKIT_ErrorToString(HError err)
{
    switch (err) {
        case HERROR_STR_Overflow:
            return HSTR_New("String overflows over 65535 bytes");

        case HERROR_DEBUG_NotInited:
            return HSTR_New("HDEBUG is not initialized.\r\n HDEBUG_Init(UART_HandleType_Def) is required.");

        default:
            return HSTP_Concat("Unknown Error! Code: ", HSTR_U8ToString(err));
    }
}

void HKIT_DefaultErrorHandler(HError err)
{
#ifdef ENABLE_HDEBUG
    if (HDEBUG_IsInited()) {
        HDPrintError(err);
    }
#endif
}