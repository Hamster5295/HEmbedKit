#if !defined(__HKIT_H__)
#define __HKIT_H__

#include <stdint.h>
#include <stdarg.h>

// 禁用 C23 标准警告
#pragma clang diagnostic ignored "-Wc23-extensions"

// 基本数据类型
#define u8    uint8_t
#define u16   uint16_t
#define u32   uint32_t
#define i8    int8_t
#define i16   int16_t
#define i32   int32_t
#define f32   float
#define f64   double

#define bool  u8

#define true  1
#define false 0
#define null  0

// 这个 include 是为了找到当前的 hal 库
// 从而不需要手动设置 stm32 型号
#include "main.h"

// HAL 库的数据类型
#define STATUS HAL_StatusTypeDef

#ifdef HAL_UART_STATE_READY
#define UART UART_HandleTypeDef
#endif

#ifdef HAL_I2C_ERROR_NONE
#define I2C I2C_HandleTypeDef
#endif

#ifdef TIM_CLOCKSOURCE_INTERNAL
#define TIM TIM_HandleTypeDef
#endif

/**
 * HKit 所能触发的所有错误码
 */
typedef enum {
    // 字符串错误
    /**
     * 当一个字符串长度超过 HSTR_MAX_LENGTH, 或缺少终止符 "\0" 时触发
     */
    HERROR_STR_Overflow = 0x10,

    // DEBUG 错误
    /**
     * 当 HDEBUG 不可用，却使用了 HDEBUG 函数时触发，可能是因为没有定义 ENABLE_HDEBUG 或调用初始化 HDEBUG_Init(huart)
     */
    HERROR_DEBUG_Unavailable = 0x20,

    // WIFI 错误
    /**
     * 当 HWIFI 尚未初始化，却先调用了其他相关函数时触发。需要优先调用初始化 HWIFI_Init()
     */
    HERROR_WIFI_Unavailable = 0x30,

    /**
     * 当 HWIFI 初始化时，未能获得 AT 响应触发
     */
    HERROR_WIFI_InitFailed = 0x31,

    /**
     * 当 WIFI 外设当前正在执行某个操作(等待操作指令的回复)，却仍尝试执行新操作时触发
     */
    HERROR_WIFI_Busy = 0x32,

    /**
     * 当阻塞操作 WIFI 时，响应超时触发
     */
    HERROR_WIFI_BlockTimeout = 0x33,

    /**
     * 当接收缓冲区溢出时触发
     */
    HERROR_WIFI_RecvOverflow = 0x34,

    /**
     * 当发送缓冲区溢出时触发
     */
    HERROR_WIFI_SendOverflow = 0x35,

    // E2PROM错误
    /**
     * 当 HPROM 尚未初始化，却先调用了其他相关函数时触发。需要优先调用初始化 HPROM_Init()
     */
    HERROR_PROM_Unavailable = 0x40,

    /**
     * 当 HPROM 初始化失败时触发。注意检查接线与引脚上拉情况
     */
    HERROR_PROM_InitFailed = 0x41,

    /**
     * 当 HPROM 读写超时时触发
     */
    HERROR_PROM_TIMEOUT = 0x42
} HError;

// 函数
/**
 * 初始化 HKit
 * @param err_handler 错误处理函数。错误代码可以在 HError 查看。
 */
void HKIT_Init(void (*err_handler)(HError err));

/**
 * 触发一次错误处理
 * @param err 触发所传递的错误码
 */
void HKIT_TriggerError(HError err);

/**
 * 默认错误处理器，会将错误转为字符串后输出
 */
void HKIT_DefaultErrorHandler(HError err);

#endif // __HKIT_H__
