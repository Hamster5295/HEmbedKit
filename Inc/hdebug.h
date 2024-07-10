#if !defined(__HDEBUG_H__)
#define __HDEBUG_H__

#include "hkit.h"

// 常量
/**
 * DEBUG 串口输出的超时时间
 */
#define HDEBUG_TIMEOUT 500

// 宏
/**
 * 判定一个表达式是否为 true, 如果不是则触发错误代码为 code 的错误处理
 * @param expr 表达式，要求为真
 * @param code 错误代码，如果表达式为假则触发
 */
#define HDEBUG_ASSERT_TRUE(expr, err)        \
    do {                                     \
        if (!(expr)) HKIT_TriggerError(err); \
    } while (0)

/**
 * 判定一个表达式是否为 false, 如果不是则触发错误代码 code
 * @param expr 表达式，要求为假
 * @param code 错误代码，如果表达式为真则触发
 */
#define HDEBUG_ASSERT_FALSE(expr, err)    \
    do {                                  \
        if (expr) HKIT_TriggerError(err); \
    } while (0)

#ifdef ENABLE_HDEBUG

/**
 * 初始化，配置输出串口
 * @param huart 调试信息输出串口
 */
#define HDInit(huart) HDEBUG_Init(huart)

/**
 * 不换行地输出字符串
 * @param str 要打印的字符串
 */
#define HDPrint(str) HDEBUG_Print(str)

/**
 * 输出一行字符串
 * @param str 要打印的字符串
 */
#define HDPrintln(str) HDEBUG_Println(str)

/**
 * 输出换行符 \\r\\n
 */
#define HDPrintCRLF() HDEBUG_PrintCRLF()

/**
 * 输出错误信息
 * @param code 错误码
 */
#define HDPrintError(code) HDEBUG_PrintError(code)

#else
#define HDInit(huart)      //
#define HDPrint(str)       //
#define HDPrintln(str)     //
#define HDPrintCRLF(str)   //
#define HDPrintError(code) //
#endif

// 函数
/**
 * 初始化，配置输出串口
 * @param huart 调试信息输出串口
 */
void HDEBUG_Init(UART_HandleTypeDef *huart);

/**
 * 不换行地输出字符串
 * @param str 要打印的字符串
 */
void HDEBUG_Print(u8 *str);

/**
 * 输出一行字符串
 * @param str 要打印的字符串
 */
void HDEBUG_Println(u8 *str);

/**
 * 输出换行符 \\r\\n
 */
void HDEBUG_PrintCRLF();

/**
 * 输出错误信息
 * @param code 错误码
 */
void HDEBUG_PrintError(HError code);

#endif // __HDEBUG_H__
