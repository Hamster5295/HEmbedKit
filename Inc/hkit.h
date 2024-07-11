#if !defined(__HKIT_H__)
#define __HKIT_H__

#include <stdint.h>
#include <stdarg.h>

// Data Types
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

// HAL 库的数据类型
#define STATUS HAL_StatusTypeDef

#define UART   UART_HandleTypeDef
#define I2C    I2C_HandleTypeDef

// 这个 include 是为了找到当前的 hal 库
// 从而不需要手动设置 stm32 型号
#include "main.h"

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

    // WiFi 错误
    /**
     * 当 HWIFI 尚未初始化，却先调用了其他相关函数时触发。需要优先调用初始化 HWIFI_Init()
     */
    HERROR_WIFI_Unavailable = 0x30,

    /**
     * 当 HWIFI 初始化时，未能获得 AT 响应触发
     */
    HERROR_WIFI_InitFailed  = 0x31,
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
