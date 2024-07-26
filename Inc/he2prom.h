#if !defined(__HE2PROM_H__)
#define __HE2PROM_H__

#include "hkit.h"

#if defined(I2C)

#if !defined(HPROM_TIMEOUT)
/**
 * E2PROM 读写的超时时间
 */
#define HPROM_TIMEOUT 500
#endif // HPROM_TIMEOUT

#if !defined(HPROM_TRAIL_TIME)
/**
 * E2PROM 初始化时测试通信的次数
 */
#define HPROM_TRAIL_TIME 3
#endif // HPROM_TRAIL_TIME

#if !defined(HPROM_DELAY)

#define HPROM_DELAY 5

#endif // HPROM_DELAY

// 函数
/**
 * 初始化 E2PROM 通信外设
 * @param hi2c I2C接入端口
 * @param addr_read E2PROM 的读取地址
 * @param addr_write E2PROM 的写入
 */
void HPROM_Init(I2C *hi2c, u16 addr_read, u16 addr_write, u16 addr_size);

/**
 * 读取一个字节
 * @param addr 字节地址
 * @return 读取到的字节值
 */
u8 HPROM_Read_Byte(u16 addr);

/**
 * 读取多个字节
 * @param addr 起始地址
 * @param len 读取长度
 * @return 读取得到的数组，以 '\0' 作为末尾（即可以当做字符串使用）
 */
u8 *HPROM_Read(u16 addr, u16 len);

/**
 * 写入一个字节
 * @param addr 地址
 * @param byte 要写入的字节
 */
void HPROM_Write_Byte(u16 addr, u8 byte);

/**
 * 写入多个字节
 * @param addr 地址
 * @param arr 字节首位置
 * @param len 写入的长度
 */
void HPROM_Write(u16 addr, u8 *arr, u16 len);

/**
 * 写入一个字符串，直到 \0 截止，且 \0 会写入
 * @param addr 地址
 * @param str 字符串首地址
 */
void HPROM_WriteStr(u16 addr, u8 *str);

#endif // I2C

#endif // __HE2PROM_H__