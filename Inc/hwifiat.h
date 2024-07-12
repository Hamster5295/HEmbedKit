/**
 * ESP8266 系 AT 指令集驱动
 */

#if !defined(__HWIFIAT_H__)
#define __HWIFIAT_H__

#include "hkit.h"

// 常量
#if !defined(HWIFI_TIMEOUT_LEN)
/**
 * 与 WiFi 外设通信的超时时限
 */
#define HWIFI_TIMEOUT_LEN 2000
#endif

#if !defined(HWIFI_BLOCK_TIMEOUT_LEN)
/**
 * 与 WiFi 外设通信的超时时限
 */
#define HWIFI_BLOCK_TIMEOUT_LEN 8000
#endif

#if !defined(HWIFI_RECV_BUFFER_SIZE)
/**
 * 接收 WiFi 外设的缓冲区大小
 */
#define HWIFI_RECV_BUFFER_SIZE 2048
#endif

#if !defined(HWIFI_SEND_BUFFER_SIZE)
/**
 * 发送 WiFi 外设的缓冲区大小
 */
#define HWIFI_SEND_BUFFER_SIZE 1024
#endif

typedef enum HWIFI_State {
    HWIFI_State_Idle            = 0x00,
    HWIFI_State_Init            = 0x01,
    HWIFI_State_WaitForOK       = 0x02,
    HWIFI_State_WaitForSend     = 0x03,
    HWIFI_State_WaitForResponse = 0x04,
} HWIFI_State;

typedef enum HWIFI_Context {
    HWIFI_Ctx_None          = 0x00,
    HWIFI_Ctx_Init          = 0x01,
    HWIFI_Ctx_SetCWMode     = 0x02,
    HWIFI_Ctx_SetSoftAp     = 0x03,
    HWIFI_Ctx_ConnectToWiFi = 0x04,

    // 掩码，可以与上面的共存
    HWIFI_Ctx_AP     = 0x8000,
    HWIFI_Ctx_SERVER = 0x4000
} HWIFI_Context;

typedef enum HWIFI_Code {
    HWIFI_OK,
    HWIFI_ERROR,
    HWIFI_FAILED,
    HWIFI_TIMEOUT,
    HWIFI_CONNECTED,
    HWIFI_DISCONNECTED,
    HWIFI_CLOSED
} HWIFI_Code;

typedef enum HWIFI_CWMode {
    HWIFI_CWMODE_Station = 0x01,
    HWIFI_CWMODE_SoftAP  = 0x02,
    HWIFI_CWMODE_Both    = 0x03,
} HWIFI_CWMode;

typedef enum HWIFI_Encryption {
    HWIFI_ENC_OPEN         = 0,
    HWIFI_ENC_WPA_PSK      = 2,
    HWIFI_ENC_WPA2_PSK     = 3,
    HWIFI_ENC_WPA_WPA2_PSK = 34,
} HWIFI_Encryption;

// 宏


// 函数
/**
 * 初始化 WIFI 串口。HWIFIAT 采用中断接收，故需要打开该串口的全局中断，并启用注册式回调（CubeMX -> Project Manager -> Advanced Settings -> Register Callback）
 * @param huart 要使用的串口
 * @param result_handler 结果处理回调
 */
HWIFI_Context HWIFI_Init(UART *huart, void (*result_handler)(HWIFI_Context ctx, HWIFI_Code status, u8 *content));

/**
 * 为注册的串口分配的事件回调
 */
void HWIFI_RxEventCallback(UART *uart, u16 size);

/**
 * WiFi 模块当前是否空闲？
 * @return 是否空闲
 */
bool HWIFI_IsIdle();

/**
 * 阻塞等待某个 WiFi 操作执行完毕
 * @param ctx 需要等待的操作，一旦该上下文退出则继续执行
 */
STATUS HWIFI_Block(HWIFI_Context ctx);

/**
 * 设置 WiFi 模块连接的工作模式
 */
HWIFI_Context HWIFI_SetCWMode(HWIFI_CWMode mode);

/**
 * 配置软路由模式，即热点模式
 * @param ssid WiFi 热点名称
 * @param pwd 密码
 * @param channel WiFi 信道
 * @param enc 加密方式，需要与密码配合
 */
HWIFI_Context HWIFI_SetSoftAp(char *ssid, char *pwd, u8 channel, HWIFI_Encryption enc);

/**
 * 连接到指定的 WiFi
 * @param ssid WiFi 名称
 * @param pwd WiFi 密码
 */
HWIFI_Context HWIFI_ConnectToWiFi(char *ssid, char *pwd);

/**
 * 使用一段拼好的字符串连接到指定 WiFi
 * @param token 拼好的字符串, 格式是 "WiFi名称","WiFi密码"
 */
HWIFI_Context HWIFI_ConnectToWiFiByToken(char *token);

#endif // __HWIFIAT_H__
