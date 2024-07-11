#include "hdebug.h"
#include "hstr.h"

bool HDEBUG_IsAvailable()
{
#ifdef ENABLE_HDEBUG
    return hdebug_port != null;
#else
    return false;
#endif
}

#ifdef ENABLE_HDEBUG

UART_HandleTypeDef *hdebug_port = null;

#define HDEBUG_ASSERT_INIT() HDEBUG_ASSERT_TRUE(hdebug_port, HERROR_DEBUG_NotInited)

void __HDEBUG_Init(UART_HandleTypeDef *huart)
{
    hdebug_port = huart;
}

void __HDEBUG_Print(u8 *str)
{
    HDEBUG_ASSERT_INIT();
    HAL_UART_Transmit(hdebug_port, str, HSTR_GetLen(str), HDEBUG_TIMEOUT);
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
    __HDEBUG_Println(HKIT_ErrorToString(code));
}

#endif