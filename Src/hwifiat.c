#include "hwifiat.h"
#include "hstr.h"
#include "hdebug.h"

UART *hwifi_port = NULL;

u8 hwifi_send_buffer[HWIFI_SEND_BUFFER_SIZE] = {0};
u16 hwifi_send_len                           = 0;

// 双缓冲
u8 hwifi_recv_buffer1[HWIFI_RECV_BUFFER_SIZE] = {0};
u8 hwifi_recv_buffer2[HWIFI_RECV_BUFFER_SIZE] = {0};
u16 hwifi_recv_len1                           = 0;
u16 hwifi_recv_len2                           = 0;

u8 *hwifi_recv_buffer = hwifi_recv_buffer1;
u16 *hwifi_recv_len   = &hwifi_recv_len1;

typedef struct HWIFI_State_Flags {
    u8 reserved : 7;
    u8 buffer : 1;
} HWIFI_State_Flags;

HWIFI_State_Flags flags;

HWIFI_State hwifi_state = HWIFI_State_Idle;
HWIFI_Context hwifi_ctx = HWIFI_CTX_None;

u8 hwifi_internal_state = 0;
u32 hwifi_block_tick    = 0;

void (*result_handler)(HWIFI_Context ctx, HWIFI_Code status, u8 *content) = NULL;

void (*hwifi_handle_queue[3])();
u8 hwifi_handle_queue_head = 0, hwifi_handle_queue_tail = 0;

#define HWIFI_ASSERT_INIT()                                                 \
    do {                                                                    \
        if (hwifi_port == NULL) HKIT_TriggerError(HERROR_WIFI_Unavailable); \
    } while (0)

#define HWIFI_ASSERT_IDLE()                                   \
    do {                                                      \
        if (hwifi_state) HKIT_TriggerError(HERROR_WIFI_Busy); \
    } while (0)

#define HWIFI_ASSERT()   \
    HWIFI_ASSERT_INIT(); \
    HWIFI_ASSERT_IDLE();

#define HWIFI_ASSERT_RECV_LEN(size)                                                     \
    do {                                                                                \
        if (size > HWIFI_RECV_BUFFER_SIZE) HKIT_TriggerError(HERROR_WIFI_RecvOverflow); \
    } while (0)

#define HWIFI_ASSERT_SEND_LEN(size)                                                     \
    do {                                                                                \
        if (size > HWIFI_SEND_BUFFER_SIZE) HKIT_TriggerError(HERROR_WIFI_RecvOverflow); \
    } while (0)

#define HWIFI_HandleResult(ctx, status, content) \
    do {                                         \
        hwifi_state = HWIFI_State_Idle;          \
        hwifi_ctx &= 0xFF00;                     \
        result_handler(ctx, status, content);    \
    } while (0)

#define HWIFI_CALL_END(ctx, state) \
    hwifi_ctx |= ctx;              \
    hwifi_state          = state;  \
    hwifi_internal_state = 0;      \
    return ctx

#define HWIFI_CALL_END_FOR_OK(ctx) \
    HWIFI_CALL_END(ctx, HWIFI_State_WaitForOK)

#define HWIFI_CALL_END_FOR_SEND(ctx) \
    HWIFI_CALL_END(ctx, HWIFI_State_WaitForSend)

#define HWIFI_CALL_END_FOR_RESPONSE(ctx) \
    HWIFI_CALL_END(ctx, HWIFI_State_WaitForResponse)

#define HWIFI_RECV_WITH_OK(size) \
    (HSTR_Equal(buf + size - 4, "OK\r\n", 4))

#define HWIFI_RECV_WITH_ERROR(size) \
    (HSTR_Equal(buf + size - 7, "ERROR\r\n", 7))

/**
 * 发送指定长度的数据
 * @param pdata 数据首指针
 * @param len 数据长度
 */
STATUS send(u8 *pdata, u16 len)
{
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
STATUS send_str_with_end(u8 *str)
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

void handle_recv(u8 *buf, u16 size)
{
    HDEBUG_LogSize("HWIFI", HSTR_Concat("Handle: ", buf), size + 6);
    HWIFI_Context ctx      = hwifi_ctx & 0x00FF;
    HWIFI_Context ctx_long = hwifi_ctx & 0xFF00;

    switch (hwifi_state) {

        case HWIFI_State_Init:

            switch (hwifi_internal_state) {
                case 0:
                    if (HSTR_Equal(buf + size - 7, "ready\r\n", 7)) {
                        HDEBUG_Log("HWIFI", "Reseted");
                        send_str("ATE0\r\n");
                        hwifi_internal_state = 1;
                    }
                    break;

                case 1:
                    if (HWIFI_RECV_WITH_OK(size) || HSTR_Equal(buf + size - 6, "ATE0\r\n", 6)) {
                        HDEBUG_Log("HWIFI", "Initialized");
                        HWIFI_HandleResult(ctx, HWIFI_OK, buf);
                    } else {
                        HKIT_TriggerError(HERROR_WIFI_InitFailed);
                        HWIFI_HandleResult(ctx, HWIFI_ERROR, buf);
                    }
                    break;
            }
            break;

        case HWIFI_State_WaitForOK:
            if (HWIFI_RECV_WITH_OK(size)) {

                switch (ctx) {
                    case HWIFI_CTX_StartTCPServer:
                        switch (hwifi_internal_state) {
                            case 0:
                                send_str("AT+CIPSERVER=1,");
                                send_str(hwifi_send_buffer);
                                send_crlf();
                                hwifi_internal_state = 1;
                                break;

                            case 1:
                                HWIFI_HandleResult(ctx, HWIFI_OK, NULL);
                                break;
                        }
                        break;

                    case HWIFI_CTX_StopTCPServer:
                        switch (hwifi_internal_state) {
                            case 0:
                                send_str("AT+CIPMUX=0\r\n");
                                hwifi_internal_state = 1;
                                break;

                            case 1:
                                HWIFI_HandleResult(ctx, HWIFI_OK, NULL);
                                break;
                        }
                        break;

                    case HWIFI_CTX_QueryStationIP:
                        u8 lptr = 10;
                        while (*(buf + lptr) != '"') lptr++;
                        u8 rptr = (++lptr) + 1;
                        while (*(buf + rptr) != '"') rptr++;
                        HWIFI_HandleResult(ctx, HWIFI_OK, HSTR_NewSize(buf + lptr, rptr - lptr));
                        break;

                    case HWIFI_CTX_QueryStationMAC:
                        lptr = 11;
                        while (*(buf + lptr) != '"') lptr++;
                        rptr = (++lptr) + 1;
                        while (*(buf + rptr) != '"') rptr++;
                        HWIFI_HandleResult(ctx, HWIFI_OK, HSTR_NewSize(buf + lptr, rptr - lptr));
                        break;

                    default:
                        HWIFI_HandleResult(ctx, HWIFI_OK, NULL);
                        break;
                }

            } else if (HWIFI_RECV_WITH_ERROR(size)) {
                HWIFI_HandleResult(ctx, HWIFI_ERROR, NULL);
            }
            break;

        case HWIFI_State_WaitForSend:
            // 检查是否准备好发送了
            if (*(buf + size - 1) == '>' || *(buf + size - 2) == '>') {
                // 发送内容
                switch (ctx) {
                    case HWIFI_CTX_Send:
                        send(hwifi_send_buffer, hwifi_send_len);
                        break;
                }
            } else if (HWIFI_RECV_WITH_OK(size)) {
                // 发送成功 SEND OK
                HWIFI_HandleResult(ctx, HWIFI_OK, NULL);
            } else if (HWIFI_RECV_WITH_ERROR(size)) {
                // 发送失败
                HWIFI_HandleResult(ctx, HWIFI_ERROR, NULL);
            }
            break;

        case HWIFI_State_WaitForResponse:
            switch (ctx) {

                case HWIFI_CTX_ConnectToWIFI:
                    if (HSTR_Equal(buf + size - 11, "CONNECTED\r\n", 11))
                        result_handler(ctx, HWIFI_CONNECT, NULL);

                    else if (HSTR_Equal(buf + size - 8, "GOT IP\r\n", 8))
                        result_handler(ctx, HWIFI_GETIP, NULL);

                    else if (HWIFI_RECV_WITH_OK(size)) {
                        HWIFI_HandleResult(ctx, HWIFI_OK, NULL);

                    }

                    else if (HSTR_Equal(buf, "+CWJAP:", 7))
                        result_handler(ctx, HWIFI_FAILED, buf + 7);

                    if (HWIFI_RECV_WITH_ERROR(size)) {
                        HWIFI_HandleResult(ctx, HWIFI_ERROR, NULL);
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    if (ctx_long & HWIFI_CTX_SERVER) {
        if (HSTR_Equal(buf + size - 9, "CONNECT", 7)) {
            u8 ptr = 1;
            while (*(buf + ptr) != ',') ptr++;
            result_handler(HWIFI_CTX_SERVER, HWIFI_CONNECT, HSTR_NewSize(buf, ptr));

        } else if (HSTR_Equal(buf + size - 8, "CLOSED", 6)) {
            u8 ptr = 1;
            while (*(buf + ptr) != ',') ptr++;
            result_handler(HWIFI_CTX_SERVER, HWIFI_CLOSED, HSTR_NewSize(buf, ptr));

        } else if (HSTR_Equal(buf + 2, "+IPD", 4)) {
            u8 lptr = 8;
            while (*(buf + lptr) != ':') lptr++;
            lptr++;

            u8 temp[2];
            temp[0] = buf[7];
            temp[1] = '\0';
            result_handler(HWIFI_CTX_SERVER, HWIFI_RECV, HSTR_Concat(temp, HSTR_NewSize(buf + lptr, size - lptr)));
        }
    }

    if (ctx_long & HWIFI_CTX_CLIENT) {
        if (HSTR_Equal(buf + size - 8, "CLOSED", 6)) {
            hwifi_ctx &= ~HWIFI_CTX_CLIENT;
            result_handler(HWIFI_CTX_CLIENT, HWIFI_CLOSED, NULL);

        } else if (HSTR_Equal(buf + 2, "+IPD", 4)) {
            u8 lptr = 5;
            while (*(buf + lptr) != ':') lptr++;
            lptr++;
            result_handler(HWIFI_CTX_CLIENT, HWIFI_RECV, HSTR_NewSize(buf + lptr, size - lptr));
        }
    }

    if (ctx_long & HWIFI_CTX_AP) {
        if (HSTR_Equal(buf, "+STA_CONNECTED:\"", 16)) {
            result_handler(HWIFI_CTX_AP, HWIFI_CONNECT, HSTR_NewSize(buf + 16, size - 19)); // 19 是 16 + 末尾的 "\"\r\n"
        } else if (HSTR_Equal(buf, "+DIST_STA_IP:\"", 14)) {
            u8 ptr = 34; // 34 = 14(前缀长度) + 17(MAC地址长度) + 3(引号与逗号), 理论上正好是ip首位
            result_handler(HWIFI_CTX_AP, HWIFI_GETIP, HSTR_NewSize(buf + ptr, size - ptr - 3));
        } else if (HSTR_Equal(buf, "+STA_DISCONNECTED:\"", 19)) {
            result_handler(HWIFI_CTX_AP, HWIFI_DISCONNECT, HSTR_NewSize(buf + 19, size - 22)); // 19 是 16 + 末尾的 "\"\r\n"
        }
    }
}

void handle_recv1()
{
    handle_recv(hwifi_recv_buffer1, hwifi_recv_len1);
}

void handle_recv2()
{
    handle_recv(hwifi_recv_buffer2, hwifi_recv_len2);
}

HWIFI_Context HWIFI_Init(UART *huart, void (*res_handler)(HWIFI_Context ctx, HWIFI_Code status, u8 *content))
{
    hwifi_port     = huart;
    result_handler = res_handler;
    if (huart != NULL) {
        HAL_UART_RegisterRxEventCallback(huart, HWIFI_RxEventCallback);
        HAL_UARTEx_ReceiveToIdle_IT(hwifi_port, hwifi_recv_buffer, HWIFI_RECV_BUFFER_SIZE);
        send_str("AT+RST\r\n");
    }

    HWIFI_CALL_END(HWIFI_CTX_Init, HWIFI_State_Init);
}

void HWIFI_Update()
{
    HAL_UARTEx_ReceiveToIdle_IT(hwifi_port, hwifi_recv_buffer, HWIFI_RECV_BUFFER_SIZE);
    while (hwifi_handle_queue_tail != hwifi_handle_queue_head) {
        __nop();
        if (hwifi_handle_queue[hwifi_handle_queue_tail] != NULL) {
            hwifi_handle_queue[hwifi_handle_queue_tail]();
        }
        hwifi_handle_queue_tail++;
        if (hwifi_handle_queue_tail >= 3) hwifi_handle_queue_tail = 0;
    }
}

bool HWIFI_IsIdle()
{
    return !hwifi_state;
}

void HWIFI_RxEventCallback(UART *uart, u16 size)
{
    if (uart != hwifi_port) return;

    HWIFI_ASSERT_RECV_LEN(size);

    // HDEBUG_Log("HWIFI", "Recv!");

    *hwifi_recv_len         = size;
    hwifi_recv_buffer[size] = HSTR_END_MARK;
    hwifi_block_tick        = 0;

    // 切换缓冲区
    if (flags.buffer) {
        hwifi_handle_queue[hwifi_handle_queue_head++] = handle_recv2;
        if (hwifi_handle_queue_head >= 3) hwifi_handle_queue_head = 0;

        hwifi_recv_buffer = hwifi_recv_buffer1;
        hwifi_recv_len    = &hwifi_recv_len1;
        flags.buffer      = false;
    } else {
        hwifi_handle_queue[hwifi_handle_queue_head++] = handle_recv1;
        if (hwifi_handle_queue_head >= 3) hwifi_handle_queue_head = 0;

        hwifi_recv_buffer = hwifi_recv_buffer2;
        hwifi_recv_len    = &hwifi_recv_len2;
        flags.buffer      = true;
    }
    HAL_UARTEx_ReceiveToIdle_IT(hwifi_port, hwifi_recv_buffer, HWIFI_RECV_BUFFER_SIZE);
}

STATUS HWIFI_Block(HWIFI_Context ctx)
{
    ctx &= 0xFF;
    hwifi_block_tick = HAL_GetTick();
    while (ctx == (hwifi_ctx & 0xFF)) {
        HWIFI_Update();
        if (HAL_GetTick() - hwifi_block_tick > HWIFI_BLOCK_TIMEOUT_LEN) {
            HKIT_TriggerError(HERROR_WIFI_BlockTimeout);
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

HWIFI_Context HWIFI_SetMode(HWIFI_Mode mode)
{
    HWIFI_ASSERT();
    send_str("AT+CWMODE=");
    send_str(HSTR_U8ToString(mode));
    send_crlf();

    if (mode == HWIFI_MODE_SoftAP || mode == HWIFI_MODE_Both)
        hwifi_ctx |= HWIFI_CTX_AP;
    else
        hwifi_ctx &= ~HWIFI_CTX_AP;

    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_SetCWMode);
}

HWIFI_Context HWIFI_SetSoftAPConfig(char *ssid, char *pwd, u8 channel, HWIFI_Encryption enc)
{
    HWIFI_ASSERT();
    send_str("AT+CWSAP=\"");
    send_str(ssid);
    send("\",\"", 3);
    send_str(pwd);
    send("\",", 2);
    send_str(HSTR_U8ToString(channel));
    send_comma();
    send_str(HSTR_U8ToString(enc));
    send_crlf();
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_SetSoftAp);
}

HWIFI_Context HWIFI_ConnectToWIFI(char *ssid, char *pwd)
{
    HWIFI_ASSERT();
    send_str("AT+CWJAP=\"");
    send_str(ssid);
    send("\",\"", 3);
    send_str(pwd);
    send("\"\r\n", 3);
    HWIFI_CALL_END_FOR_RESPONSE(HWIFI_CTX_ConnectToWIFI);
}

HWIFI_Context HWIFI_ConnectToWIFIByToken(char *token)
{
    HWIFI_ASSERT();
    send_str("AT+CWJAP=");
    send_str(token);
    send_crlf();
    HWIFI_CALL_END_FOR_RESPONSE(HWIFI_CTX_ConnectToWIFI);
}

HWIFI_Context HWIFI_ScanWIFI()
{
    HWIFI_ASSERT();
    send_str("AT+CWLAP\r\n");
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_ScanWIFI);
}

HWIFI_Context HWIFI_StartTCPServer(u16 port)
{
    HWIFI_ASSERT();
    HSTR_Copy(hwifi_send_buffer, HSTR_U16ToString(port));

    send_str("AT+CIPMUX=1\r\n");
    hwifi_ctx |= HWIFI_CTX_SERVER;
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_StartTCPServer);
}

HWIFI_Context HWIFI_StopTCPServer()
{
    HWIFI_ASSERT();

    send_str("AT+CIPSERVER=0\r\n");
    hwifi_ctx &= ~HWIFI_CTX_SERVER;
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_StopTCPServer);
}

HWIFI_Context HWIFI_StartTCPConnection(u8 *ip, u16 port)
{
    HWIFI_ASSERT();

    send_str("AT+CIPSTART=\"TCP\",\"");
    send_str(ip);
    send("\",", 2);
    send_str(HSTR_U16ToString(port));
    send_crlf();

    hwifi_ctx |= HWIFI_CTX_CLIENT;
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_StartTCPConnection);
}

HWIFI_Context HWIFI_StopTCPConnection()
{
    HWIFI_ASSERT();

    send_str("AT+CIPCLOSE\r\n");

    hwifi_ctx &= ~HWIFI_CTX_CLIENT;
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_StopTCPConnection);
}

HWIFI_Context HWIFI_QueryStationIP()
{
    HWIFI_ASSERT();

    send_str("AT+CIPSTA?\r\n");
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_QueryStationIP);
}

HWIFI_Context HWIFI_QueryStationMAC()
{
    HWIFI_ASSERT();

    send_str("AT+CIPSTAMAC?\r\n");
    HWIFI_CALL_END_FOR_OK(HWIFI_CTX_QueryStationMAC);
}

HWIFI_Context HWIFI_Send(u8 *str, u16 len)
{
    HWIFI_ASSERT();

    HWIFI_ASSERT_SEND_LEN(len);
    HSTR_CopySize(hwifi_send_buffer, str, len);
    hwifi_send_len = len;

    send_str("AT+CIPSEND=");
    send_str(HSTR_U16ToString(len));
    send_crlf();
    HWIFI_CALL_END_FOR_SEND(HWIFI_CTX_Send);
}

HWIFI_Context HWIFI_SendStr(u8 *str)
{
    return HWIFI_Send(str, HSTR_GetLen(str));
}

HWIFI_Context HWIFI_Xfer()
{
    // 由于已经切换过了，所以需要与 flag 反着来
    return flags.buffer ? HWIFI_Send(hwifi_recv_buffer1, hwifi_recv_len1) : HWIFI_Send(hwifi_recv_buffer2, hwifi_recv_len2);
}

HWIFI_Context HWIFI_SendClient(u8 client, u8 *str, u16 len)
{
    HWIFI_ASSERT();

    HWIFI_ASSERT_SEND_LEN(len);
    HSTR_CopySize(hwifi_send_buffer, str, len);
    hwifi_send_len = len;

    send_str("AT+CIPSEND=");
    send(&client, 1);
    send_comma();
    send_str(HSTR_U16ToString(len));
    send_crlf();
    HWIFI_CALL_END_FOR_SEND(HWIFI_CTX_Send);
}

HWIFI_Context HWIFI_SendClientStr(u8 client, u8 *str)
{
    return HWIFI_SendClient(client, str, HSTR_GetLen(str));
}

HWIFI_Context HWIFI_XferClient(u8 client)
{
    // 由于已经切换过了，所以需要与 flag 反着来
    return flags.buffer ? HWIFI_SendClient(client, hwifi_recv_buffer1, hwifi_recv_len1) : HWIFI_SendClient(client, hwifi_recv_buffer2, hwifi_recv_len2);
}
