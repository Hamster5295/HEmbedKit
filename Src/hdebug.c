#include "hdebug.h"
#include "hstr.h"

#if defined(UART)

#ifdef ENABLE_HDEBUG
UART *hdebug_port = null;
#endif

bool HDEBUG_IsAvailable()
{
#ifdef ENABLE_HDEBUG
    return hdebug_port != null;
#else
    return false;
#endif
}

#ifdef ENABLE_HDEBUG

#define HDEBUG_ASSERT_INIT() HDEBUG_ASSERT_TRUE(hdebug_port, HERROR_DEBUG_Unavailable)

void __HDEBUG_Init(UART *huart)
{
    hdebug_port = huart;
    HDEBUG_Log("HDebug", "Initialized");
}

void __HDEBUG_PrintSize(u8 *str, u16 size)
{
    HDEBUG_ASSERT_INIT();
    HAL_UART_Transmit(hdebug_port, str, size, HDEBUG_TIMEOUT);
}

void __HDEBUG_Print(u8 *str)
{
    __HDEBUG_PrintSize(str, HSTR_GetLen(str));
}

void __HDEBUG_Println(u8 *str)
{
    __HDEBUG_Print(str);
    __HDEBUG_PrintCRLF();
}

void __HDEBUG_PrintCRLF()
{
    __HDEBUG_Print("\r\n");
}

void __HDEBUG_PrintError(HError code)
{
    __HDEBUG_Println(HSTR_ErrorToString(code));
}

#endif // ENABLE_HDEBUG

#endif // UART