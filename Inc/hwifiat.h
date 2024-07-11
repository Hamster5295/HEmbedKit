/**
 * ESP8266 系 AT 指令集驱动
 */

#if !defined(__HWIFIAT_H__)
#define __HWIFIAT_H__

#include "hkit.h"

// 常量
#if !defined(HWIFIAT_TIMEOUT)
/**
 * 与 WiFi 外设通信的超时时限
 */
#define HWIFIAT_TIMEOUT 2000
#endif

#if !defined(HWIFIAT_BUFFER_SIZE)
/**
 * 接收 WiFi 外设数据的缓冲区大小
 */
#define HWIFIAT_BUFFER_SIZE 2048
#endif

typedef enum HWIFI_State {
    HWIFI_Idle           = 0x00,
    HWIFI_Initing        = 0x01,
    HWIFI_WaitingForOK   = 0x02,
    HWIFI_WaitingForSend = 0x03,

    // 掩码，可以与上面的共存
    HWIFI_AP     = 0x8000,
    HWIFI_SERVER = 0x4000
} HWIFI_State;

typedef enum HWIFI_Context {
    HWIFI_Ctx_None = 0x00,
    HWIFI_Ctx_Init = 0x01
} HWIFI_Context;

// 宏

// 函数
/**
 * 初始化 WIFI 串口。HWIFIAT 采用中断接收，故需要打开该串口的全局中断，并启用注册式回调（CubeMX -> Project Manager -> Advanced Settings -> Register Callback）
 * @param huart 要使用的串口
 * @param result_handler 结果处理回调
 */
void HWIFI_Init(UART *huart, void (*result_handler)(HWIFI_Context ctx, STATUS status, u8 *content));

/**
 * 为注册的串口分配的事件回调
 */
void HWIFI_RxEventCallback(UART *uart, u16 size);

#endif // __HWIFIAT_H__
