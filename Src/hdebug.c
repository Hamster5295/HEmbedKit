#include "hdebug.h"
#include "hstr.h"

UART_HandleTypeDef *hdebug_port = null;

#define HDEBUG_ASSERT_INIT() HDEBUG_ASSERT_TRUE(hdebug_port, HERROR_DEBUG_NotInited)

void HDEBUG_Init(UART_HandleTypeDef *huart)
{
    hdebug_port = huart;
}

bool HDEBUG_IsInited()
{
    return hdebug_port != null;
}

#ifdef ENABLE_HDEBUG

void HDEBUG_Print(u8 *str)
{
    HDEBUG_ASSERT_INIT();
    HAL_UART_Transmit(hdebug_port, str, HSTR_GetLen(str), HDEBUG_TIMEOUT);
}

void HDEBUG_Println(u8 *str)
{
    HDEBUG_Print(str);
    HDEBUG_PrintCRLF();
}

void HDEBUG_PrintCRLF()
{
    HDEBUG_Print("\r\n");
}

void HDEBUG_PrintError(HError code)
{
    HDEBUG_Println(HKIT_ErrorToString(code));
}

#endif