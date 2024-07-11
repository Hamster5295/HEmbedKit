#include "hwifiat.h"
#include "hstr.h"
#include "hdebug.h"

UART *hwifi_port = NULL;

u8 hwifi_send_buffer[HWIFI_SEND_BUFFER_SIZE] = {0};
u8 hwifi_recv_buffer[HWIFI_RECV_BUFFER_SIZE] = {0};
HWIFI_State hwifi_state                      = HWIFI_State_Idle;
HWIFI_Context hwifi_ctx                      = HWIFI_Ctx_None;

bool block_flag = false;

void (*result_handler)(HWIFI_Context ctx, HWIFI_Code status, u8 *content) = NULL;

#define HWIFI_ASSERT_INIT()                             \
    do {                                                \
        if (hwifi_port == null) {                       \
            HKIT_TriggerError(HERROR_WIFI_Unavailable); \
            return HAL_ERROR;                           \
        }                                               \
    } while (0)

#define HWIFI_ASSERT_IDLE()               \
    do {                                  \
        if (hwifi_state) return HAL_BUSY; \
    } while (0)

#define HWIFI_CLEAR_STATE()             \
    do {                                \
        hwifi_state = HWIFI_State_Idle; \
        hwifi_ctx &= 0xFF00;            \
        block_flag = false;             \
    } while (0)

#define HWIFI_CALL_END(ctx, state) \
    hwifi_ctx |= ctx;              \
    hwifi_state = state;           \
    return ctx

#define HWIFI_CALL_END_FOR_OK(ctx) \
    HWIFI_CALL_END(ctx, HWIFI_State_WaitForOK)

#define HWIFI_RECV_WITH_OK(size) \
    (HSTR_Equal(hwifi_recv_buffer + size - 4, "OK\r\n", 4))

#define HWIFI_RECV_WITH_ERROR(size) \
    (HSTR_Equal(hwifi_recv_buffer + size - 7, "ERROR\r\n", 7))

/**
 * 发送指定长度的数据
 * @param pdata 数据首指针
 * @param len 数据长度
 */
STATUS send(u8 *pdata, u16 len)
{
    HWIFI_ASSERT_INIT();
    HDEBUG_PrintSize(pdata, len);
    return HAL_UART_Transmit(hwifi_port, pdata, len, HWIFI_TIMEOUT_LEN);
}

/**
 * 发送字符串，直到遇到终止符。不发送终止符。
 * @param str 要发送的字符串
 */
STATUS send_str(u8 *str)
{
    STATUS s;
    for (u8 i = 0; *(str + i) != HSTR_END_MARK; i++) {
        s = send(str + i, 1);
        if (s != HAL_OK) return s;
    }
    return HAL_OK;
}

/**
 * 发送字符串，直到遇到终止符，并将终止符发送出去。
 * @param str 要发送的字符串
 */
STATUS send_stre(u8 *str)
{
    STATUS s;
    u8 i = 0;
    for (; *(str + i) != HSTR_END_MARK; i++) {
        s = send(str + i, 1);
        if (s != HAL_OK) return s;
    }
    return send(str + i, 1);
}

/**
 * 发送换行符
 */
STATUS send_crlf()
{
    return send("\r\n", 2);
}

/**
 * 发送英文逗号
 */
STATUS send_comma()
{
    return send(",", 1);
}

/**
 * 发送英文逗号
 */
STATUS send_quot()
{
    return send("\"", 1);
}

HWIFI_Context HWIFI_Init(UART *huart, void (*res_handler)(HWIFI_Context ctx, HWIFI_Code status, u8 *content))
{
    hwifi_port     = huart;
    result_handler = res_handler;
    if (huart != NULL) {
        HAL_UART_RegisterRxEventCallback(huart, HWIFI_RxEventCallback);
        HAL_UARTEx_ReceiveToIdle_IT(hwifi_port, hwifi_recv_buffer, HWIFI_RECV_BUFFER_SIZE);
        send_str("ATE0\r\n");
    }

    HWIFI_CALL_END(HWIFI_Ctx_Init, HWIFI_State_Init);
}

void HWIFI_RxEventCallback(UART *uart, u16 size)
{
    if (uart != hwifi_port) return;
    HAL_UARTEx_ReceiveToIdle_IT(hwifi_port, hwifi_recv_buffer, HWIFI_RECV_BUFFER_SIZE);

    hwifi_recv_buffer[size] = HSTR_END_MARK;

    HDEBUG_LogSize("HWiFi", HSTR_Concat("Recv: ", hwifi_recv_buffer), size + 6);

    switch (hwifi_state) {

        case HWIFI_State_Init:
            if (HSTR_Equal(hwifi_recv_buffer + size - 6, "ATE0\r\n", 6) || HSTR_Equal(hwifi_recv_buffer + size - 4, "OK\r\n", 4)) {
                HDEBUG_Log("HWiFi", "Initialized");
                result_handler(hwifi_ctx, HWIFI_OK, hwifi_recv_buffer);
                HWIFI_CLEAR_STATE();

            } else {
                HKIT_TriggerError(HERROR_WIFI_InitFailed);
                result_handler(hwifi_ctx, HWIFI_ERROR, hwifi_recv_buffer);
                HWIFI_CLEAR_STATE();
            }
            break;

        case HWIFI_State_WaitForOK:
            if (HWIFI_RECV_WITH_OK(size)) {
                result_handler(hwifi_ctx, HWIFI_OK, NULL);
                HWIFI_CLEAR_STATE();

            } else if (HWIFI_RECV_WITH_ERROR(size)) {
                result_handler(hwifi_ctx, HWIFI_ERROR, NULL);
                HWIFI_CLEAR_STATE();
            }
            break;

        case HWIFI_State_WaitForSend:
            if (*(hwifi_recv_buffer + size - 2) == '>') {
            }
            break;

        case HWIFI_State_WaitForResponse:
            HWIFI_Context ctx = hwifi_ctx & 0xFF;
            switch (ctx) {

                case HWIFI_Ctx_ConnectToWiFi:
                    if (HSTR_Equal(hwifi_recv_buffer + size - 11, "CONNECTED\r\n", 11))
                        result_handler(ctx, HWIFI_CONNECTED, NULL);

                    else if (HSTR_Equal(hwifi_recv_buffer + size - 8, "GOT IP\r\n", 8))
                        result_handler(ctx, HWIFI_GETIP, NULL);

                    else if (HWIFI_RECV_WITH_OK(size)) {
                        result_handler(ctx, HWIFI_OK, NULL);
                        HWIFI_CLEAR_STATE();

                    }

                    else if (HSTR_Equal(hwifi_recv_buffer, "+CWJAP:", 7))
                        result_handler(ctx, HWIFI_FAILED, hwifi_recv_buffer + 7);

                    else if (HWIFI_RECV_WITH_ERROR(size)) {
                        result_handler(ctx, HWIFI_ERROR, NULL);
                        HWIFI_CLEAR_STATE();
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

STATUS HWIFI_Block(HWIFI_Context ctx)
{
    ctx &= 0xFF;
    u32 tick   = HAL_GetTick();
    block_flag = true;
    while (block_flag && ctx == (hwifi_ctx & 0xFF)) {
        __nop();
        if (HAL_GetTick() - tick > HWIFI_TIMEOUT_LEN) {
            HDEBUG_Log("HWiFi", "Operation Timeout");
            block_flag = false;
            return HAL_TIMEOUT;
        }
    }
    block_flag = false;
    return HAL_OK;
}

HWIFI_Context HWIFI_SetCWMode(HWIFI_CWMode mode)
{
    send_str("AT+CWMODE=");
    send_str(HSTR_U8ToString(mode));
    send_crlf();
    HWIFI_CALL_END_FOR_OK(HWIFI_Ctx_SetCWMode);
}

HWIFI_Context HWIFI_SetSoftAp(char *ssid, char *pwd, u8 channel, HWIFI_Encryption enc)
{
    send_str("AT+CWSAP=\"");
    send_str(ssid);
    send("\",\"", 3);
    send_str(pwd);
    send("\",", 2);
    send_str(HSTR_U8ToString(channel));
    send_comma();
    send_str(HSTR_U8ToString(enc));
    send_crlf();
    HWIFI_CALL_END_FOR_OK(HWIFI_Ctx_SetSoftAp);
}

HWIFI_Context HWIFI_ConnectToWiFi(char *ssid, char *pwd)
{
    send_str("AT+CWJAP=\"");
    send_str(ssid);
    send("\",\"", 3);
    send_str(pwd);
    send("\"\r\n", 3);
    HWIFI_CALL_END(HWIFI_Ctx_ConnectToWiFi, HWIFI_State_WaitForResponse);
}

HWIFI_Context HWIFI_ConnectToWiFiByToken(char *token)
{
    send_str("AT+CWJAP=\"");
    send_str(token);
    send_crlf();
    HWIFI_CALL_END(HWIFI_Ctx_ConnectToWiFi, HWIFI_State_WaitForResponse);
}
