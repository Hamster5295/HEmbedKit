#if !defined(__HSTR_H__)
#define __HSTR_H__

#include "hkit.h"
#include "hdebug.h"

// 禁用指针隐式变换警告，因为常量字符串必定爆这个
#pragma clang diagnostic ignored "-Wpointer-sign"

// 常量

/**
 * 字符串终止标志
 */
#define HSTR_END_MARK '\0'

#if !defined(HSTR_BUFFER_SIZE)
/**
 * 字符串缓冲区大小, 同时也是字符串的最大长度
 */
#define HSTR_BUFFER_SIZE 1024
#endif

// 宏
/**
 * 判断 str 在 offset 处是否终止
 * @param str 要判定的字符串
 * @param offset 字符串自首项的偏移量
 * @return 字符串是否尚未终止
 */
#define HSTR_IS_END(str, offset) \
    (*(str + offset) == HSTR_END_MARK)

/**
 * 获取 str 的末尾指针（指向其终止符）
 * @param str 要获取指针的字符串
 */
#define HSTR_END_PTR(str) \
    (str + HSTR_GetLen(str))

// 函数
/**
 * 从字符串常量新建一个可变字符串
 * @param data 字符串常量
 * @return 新建的字符串首位
 */
u8 *HSTR_New(u8 *data);

/**
 * 从给定长度的数据新建字符串
 * @param data 原字符串数据
 * @param len 字符串长度
 * @return 新字符串首位
 */
u8 *HSTR_NewSize(u8 *data, u16 len);

/**
 * 新建一个指定长度的空字符串
 * @param len 长度
 */
u8 *HSTR_NewEmpty(u16 len);

/**
 * 获取指定字符串长度，不包含终止标志
 * @param str 要查询的字符串
 * @return 字符串长度
 */
u16 HSTR_GetLen(u8 *str);

/**
 * 将 source 字符串复制到 dest 中
 * @param source 要复制的字符串
 * @param dest 要粘贴的字符串
 * @return dest 指针本身
 */
u8 *HSTR_Copy(u8 *dest, u8 *source);

/**
 * 将 source 字符串的指定长度复制到 dest 中
 * @param source 要复制的字符串
 * @param dest 要粘贴的字符串
 * @param u16 长度
 * @return dest 指针本身
 */
u8 *HSTR_CopySize(u8 *dest, u8 *source, u16 len);

/**
 * 将给定字符串连接到一起
 * @param left 左侧字符串
 * @param right 右侧字符串
 * @return 连接后字符串首位置指针
 */
u8 *HSTR_Concat(u8 *left, u8 *right);

/**
 * 将多个给定字符串连接到一起
 * @param count 字符串数量
 * @param ... 字符串, u8* 类型
 */
u8 *HSTR_Concats(u16 count, ...);

/**
 * 比较两个字符串
 * @param left 左侧
 * @param right 右侧
 * @return 比较结果，0 为相等，255(-1)为小于，1为大于，可以理解成左边减去右边的结果
 */
u8 HSTR_Compare(u8 *left, u8 *right);

/**
 * 比较两个字符串的某个部分, 只判断是否相等
 * @param left 左侧
 * @param right 右侧
 * @param len 要比较的长度
 * @return 比较结果
 */
bool HSTR_Equal(u8 *left, u8 *right, u16 len);

/**
 * 将 8 位无符号整数转换为字符串
 * @param number 要转换的数字
 * @return 字符串首位
 */
u8 *HSTR_U8ToString(u8 number);

/**
 * 将 16 位无符号整数转换为字符串
 * @param number 要转换的数字
 * @return 字符串首位
 */
u8 *HSTR_U16ToString(u16 number);

/**
 * 将 32 位无符号整数转换为字符串
 * @param number 要转换的数字
 * @return 字符串首位
 */
u8 *HSTR_U32ToString(u32 number);

/**
 * 将 8 位有符号整数转换为字符串
 * @param number 要转换的数字
 * @return 字符串首位
 */
u8 *HSTR_I8ToString(i8 number);

/**
 * 将 16 位有符号整数转换为字符串
 * @param number 要转换的数字
 * @return 字符串首位
 */
u8 *HSTR_I16ToString(i16 number);

/**
 * 将 32 位有符号整数转换为字符串
 * @param number 要转换的数字
 * @return 字符串首位
 */
u8 *HSTR_I32ToString(i32 number);

/**
 * 将单精度浮点数转换为字符串
 * @param number 要转换的数字
 * @param digit 小数位数
 * @return 字符串首位
 */
u8 *HSTR_FloatToString(float number, int digit);

/**
 * 将单精度浮点数转换为字符串，保留4位小数
 * @param number 要转换的数字
 * @return 字符串首位
 */
u8 *HSTR_FloatToString_D4(float number);

#ifdef ENABLE_HDEBUG
/**
 * 将错误代码转化为解释
 * @param err 错误代码
 * @return 存放错误信息的字符串
 */
u8 *HSTR_ErrorToString(HError err);
#endif

#endif // __HSTR_H__
