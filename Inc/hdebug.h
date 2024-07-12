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
#define HDEBUG_Init(huart) __HDEBUG_Init(huart)

/**
 * 不换行地输出字符串
 * @param str 要打印的字符串
 */
#define HDEBUG_Print(str) __HDEBUG_Print(str)

/**
 * 输出一行字符串
 * @param str 要打印的字符串
 */
#define HDEBUG_Println(str) __HDEBUG_Println(str)

/**
 * 输出换行符 \\r\\n
 */
#define HDEBUG_PrintCRLF() __HDEBUG_PrintCRLF()

/**
 * 打印指定长度的字符串
 * @param str 字符串首地址
 * @param size 要打印的长度
 */
#define HDEBUG_PrintSize(str, size) __HDEBUG_PrintSize(str, size);

/**
 * 输出错误信息
 * @param code 错误码
 */
#define HDEBUG_PrintError(code) __HDEBUG_PrintError(code)

/**
 * 以一个比较工整的格式输出
 * @param module 模块名称，会放在输出的前缀
 * @param msg 输出信息
 */
#define HDEBUG_Log(module, msg) \
    do {                        \
        HDEBUG_Print("[");      \
        HDEBUG_Print(module);   \
        HDEBUG_Print("] ");     \
        HDEBUG_Println(msg);    \
        HDEBUG_PrintCRLF();     \
    } while (0)

/**
 * 以一个比较工整的格式输出指定长度的内容
 * @param module 模块名称，会放在输出的前缀
 * @param msg 输出信息首位置
 * @param len 输出长度
 */
#define HDEBUG_LogSize(module, msg, len) \
    do {                                 \
        HDEBUG_Print("[");               \
        HDEBUG_Print(module);            \
        HDEBUG_Print("] ");              \
        HDEBUG_PrintSize(msg, len);      \
        HDEBUG_PrintCRLF();              \
    } while (0)

#else
#define HDEBUG_Init(huart)               //
#define HDEBUG_Print(str)                //
#define HDEBUG_Println(str)              //
#define HDEBUG_PrintCRLF(str)            //
#define HDEBUG_PrintSize(str, size)      //
#define HDEBUG_PrintError(code)          //
#define HDEBUG_Log(module, msg)          //
#define HDEBUG_LogSize(module, msg, len) //
#endif

// 函数
/**
 * 检查 HDEBUG 模块是否可以使用
 * @return 是否可以使用
 */
bool HDEBUG_IsAvailable();

#ifdef ENABLE_HDEBUG
/**
 * 这个函数不应当被直接调用！应当使用其宏形式，以便取消 Debug 时能完全从编译中去除。
 * 初始化，配置输出串口
 * @param huart 调试信息输出串口
 */
void __HDEBUG_Init(UART *huart);

/**
 * 这个函数不应当被直接调用！应当使用其宏形式，以便取消 Debug 时能完全从编译中去除。
 * 不换行地输出字符串
 * @param str 要打印的字符串
 */
void __HDEBUG_Print(u8 *str);

/**
 * 这个函数不应当被直接调用！应当使用其宏形式，以便取消 Debug 时能完全从编译中去除。
 * 输出一行字符串
 * @param str 要打印的字符串
 */
void __HDEBUG_Println(u8 *str);

/**
 * 这个函数不应当被直接调用！应当使用其宏形式，以便取消 Debug 时能完全从编译中去除。
 * 输出换行符 \\r\\n
 */
void __HDEBUG_PrintCRLF();

/**
 * 这个函数不应当被直接调用！应当使用其宏形式，以便取消 Debug 时能完全从编译中去除。
 * 打印指定长度的字符串
 * @param str 字符串首地址
 * @param size 要打印的长度
 */
void __HDEBUG_PrintSize(u8 *str, u16 size);

/**
 * 这个函数不应当被直接调用！应当使用其宏形式，以便取消 Debug 时能完全从编译中去除。
 * 输出错误信息
 * @param code 错误码
 */
void __HDEBUG_PrintError(HError code);
#endif // ENABLE_HDEBUG

#endif // __HDEBUG_H__
