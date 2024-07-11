#include "hwifiat.h"
#include "hstr.h"
#include "hdebug.h"

UART *hwifi_port = NULL;

u8 hwifi_buffer[HWIFIAT_BUFFER_SIZE] = {0};
HWIFI_State hwifi_state              = HWIFI_Idle;
HWIFI_Context hwifi_ctx              = HWIFI_Ctx_None;

void (*result_handler)(HWIFI_Context ctx, STATUS status, u8 *content) = NULL;

#define HWIFI_ASSERT_INIT()                                                 \
    do {                                                                    \
        if (hwifi_port == null) HKIT_TriggerError(HERROR_WIFI_Unavailable); \
    } while (0)

#define HWIFI_ASSERT_IDLE()                      \
    do {                                         \
        if (hwifi_state & 0xFF) return HAL_BUSY; \
    } while (0)

/**
 * 发送指定长度的数据
 * @param pdata 数据首指针
 * @param len 数据长度
 */
STATUS send(u8 *pdata, u16 len)
{
    HWIFI_ASSERT_INIT();
    HDEBUG_PrintSize(pdata, len);
    return HAL_UART_Transmit(hwifi_port, pdata, len, HWIFIAT_TIMEOUT);
}

/**
 * 发送字符串，直到遇到终止符。不发送终止符。
 * @param str 要发送的字符串
 */
STATUS send_str(u8 *str)
{
    return send(str, HSTR_GetLen(str));
}

/**
 * 发送字符串，直到遇到终止符，并将终止符发送出去。
 * @param str 要发送的字符串
 */
STATUS send_stre(u8 *str)
{
    return send(str, HSTR_GetLen(str) + 1);
}

void HWIFI_Init(UART *huart, void (*res_handler)(HWIFI_Context ctx, STATUS status, u8 *content))
{
    hwifi_port     = huart;
    result_handler = res_handler;
    if (huart != NULL) {
        HAL_UART_RegisterRxEventCallback(huart, HWIFI_RxEventCallback);
        HAL_UARTEx_ReceiveToIdle_IT(hwifi_port, hwifi_buffer, HWIFIAT_BUFFER_SIZE);

        hwifi_ctx   = HWIFI_Ctx_Init;
        hwifi_state = HWIFI_Initing;
        send_str("ATE0\r\n");
    }
}

void HWIFI_RxEventCallback(UART *uart, u16 size)
{
    if (uart != hwifi_port) return;
    HAL_UARTEx_ReceiveToIdle_IT(hwifi_port, hwifi_buffer, HWIFIAT_BUFFER_SIZE);

    hwifi_buffer[size] = HSTR_END_MARK;

    HDEBUG_LogSize("HWiFi", HSTR_Concat("Recv: ", hwifi_buffer), size + 6);

    switch (hwifi_state & 0x00FF) {

        case HWIFI_Initing:
            if (HSTR_Equal(hwifi_buffer + size - 5, "ATE\r\n", 5) || HSTR_Equal(hwifi_buffer + size - 4, "OK\r\n", 4)) {
                HDEBUG_Log("HWiFi", "Initialized");
                result_handler(hwifi_ctx, HAL_OK, hwifi_buffer);
            } else {
                HKIT_TriggerError(HERROR_WIFI_InitFailed);
                result_handler(hwifi_ctx, HAL_ERROR, hwifi_buffer);
            }
            break;

        case HWIFI_WaitingForOK:
            if (HSTR_Equal(hwifi_buffer + size - 4, "OK\r\n", 4))
                result_handler(hwifi_ctx, HAL_OK, NULL);
            else if (HSTR_Equal(hwifi_buffer + size - 7, "ERROR\r\n", 7))
                result_handler(hwifi_ctx, HAL_ERROR, NULL);

            break;

        case HWIFI_WaitingForSend:
            if (*(hwifi_buffer + size - 2) == '>') {
            }
            break;
    }

    hwifi_ctx = HWIFI_Ctx_None;
    hwifi_state &= 0xFF00;
}