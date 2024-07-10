#if !defined(__HSTR_H__)
#define __HSTR_H__

#include "hkit.h"
#include "hdebug.h"

// 禁用指针隐式变换警告，因为常量字符串必定爆这个
#pragma clang diagnostic ignored "-Wpointer-sign"

// 常量
/**
 * HString 允许的字符串最大长度，等于 16 位无符号整形能表达的最大值
 */
#define HSTR_MAX_LENGTH 65535

/**
 * HString 的终止标志
 */
#define HSTR_END_MARK '\0'

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
    (str + HSTR_Len(str))

// 函数
/**
 * 获取指定字符串长度，不包含终止标志
 * @param str 要查询的字符串
 * @return 字符串长度
 */
u16 HSTR_Len(u8 *str);

/**
 * 将 source 字符串复制到 dest 中
 * @param source 要复制的字符串
 * @param dest 要粘贴的字符串
 * @return 原封不动的 dest 指针.
 */
u8 *HSTR_Copy(u8 *dest, u8 *source);

/**
 * 将给定字符串连接到一起，放入 left 字符串的尾部
 * @param left 左侧字符串
 * @param right 右侧字符串
 * @return 连接后 left 指针
 */
u8 *HSTR_Conn(u8 *left, u8 *right);

/**
 * 连接多个字符串，并将结果存放在第一个字符串中
 * @param size 总共有多少字符串要拼起来？
 * @param base 第一个字符串，拼也是在他身上拼起来
 * @return 指向 base 的指针
 */
u8 *HSTR_Conns(u16 size, u8 *base, ...);

/**
 * 比较两个字符串
 * @param left 左侧
 * @param right 右侧
 * @return 比较结果，0 为相等，255(-1)为小于，1为大于，可以理解成左边减去右边的结果
 */
u8 HSTR_Compare(u8 *left, u8 *right);

/**
 * 将 8 位无符号整数转换为字符串
 * @param number 要转换的数字
 * @param str 存储字符串的数组
 * @return str指针本身
 */
u8 *HSTR_U8ToString(u8 number, u8 *str);

/**
 * 将 16 位无符号整数转换为字符串
 * @param number 要转换的数字
 * @param str 存储字符串的数组
 * @return str指针本身
 */
u8 *HSTR_U16ToString(u16 number, u8 *str);

/**
 * 将 32 位无符号整数转换为字符串
 * @param number 要转换的数字
 * @param str 存储字符串的数组
 * @return str指针本身
 */
u8 *HSTR_U32ToString(u32 number, u8 *str);

#endif // __HSTR_H__
