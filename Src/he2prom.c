#include "he2prom.h"
#include "hdebug.h"
#include "hstr.h"

I2C *hprom_port = NULL;
u16 addr_rd, addr_wr, mem_addr_size;

void HPROM_Init(I2C *hi2c, u16 addr_read, u16 addr_write, u16 addr_size)
{
    hprom_port    = hi2c;
    addr_rd       = addr_read;
    addr_wr       = addr_write;
    mem_addr_size = addr_size;

    if (HAL_I2C_IsDeviceReady(hprom_port, addr_rd, HPROM_TRAIL_TIME, HPROM_TIMEOUT) != HAL_OK) {
        HDEBUG_Print("Failed to receive response from E2PROM with address ");
        HDEBUG_Println(HSTR_U16ToString(addr_rd));
        HDEBUG_Print("Error Code: ");
        HDEBUG_Println(HSTR_U32ToString(HAL_I2C_GetError(hprom_port)));
        HKIT_TriggerError(HERROR_PROM_InitFailed);
    }
    if (HAL_I2C_IsDeviceReady(hprom_port, addr_wr, HPROM_TRAIL_TIME, HPROM_TIMEOUT) != HAL_OK) {
        HDEBUG_Print("Failed to receive response from E2PROM with address ");
        HDEBUG_Println(HSTR_U16ToString(addr_wr));
        HDEBUG_Print("Error Code: ");
        HDEBUG_Println(HSTR_U32ToString(HAL_I2C_GetError(hprom_port)));
        HKIT_TriggerError(HERROR_PROM_InitFailed);
    }

    HDEBUG_Log("HPROM", "Initialized");
}

u8 HPROM_Read_Byte(u16 addr)
{
    u8 byte = 0;
    HAL_I2C_Mem_Read(hprom_port, addr_rd, addr, mem_addr_size, &byte, 1, HPROM_TIMEOUT);
    return byte;
}

u8 *HPROM_Read(u16 addr, u16 len)
{
    u8 *buf = HSTR_NewEmpty(len + 1);
    HAL_I2C_Mem_Read(hprom_port, addr_rd, addr, mem_addr_size, buf, len, HPROM_TIMEOUT);
    *(buf + len) = HSTR_END_MARK;
    return buf;
}

void HPROM_Write_Byte(u16 addr, u8 byte)
{
    HAL_I2C_Mem_Write(hprom_port, addr_wr, addr, mem_addr_size, &byte, 1, HPROM_TIMEOUT);
    HAL_Delay(5);
}

void HPROM_Write(u16 addr, u8 *arr, u16 len)
{
    u8 offset = 8 - addr % 8;
    if (len <= offset)
        HAL_I2C_Mem_Write(hprom_port, addr_wr, addr, mem_addr_size, arr, len, HPROM_TIMEOUT);
    else {
        len -= offset;
        u16 cnt    = len / 8;
        u8 remains = len % 8, i = 0;
        HAL_I2C_Mem_Write(hprom_port, addr_wr, addr, mem_addr_size, arr, offset, HPROM_TIMEOUT);
        HAL_Delay(5);
        for (i = 0; i < cnt; i++) {
            HAL_I2C_Mem_Write(hprom_port, addr_wr, addr + offset + i * 8, mem_addr_size, arr + offset + i * 8, 8, HPROM_TIMEOUT);
            HAL_Delay(5);
        }
        if (remains)
            HAL_I2C_Mem_Write(hprom_port, addr_wr, addr + offset + i * 8, mem_addr_size, arr + offset + i * 8, remains, HPROM_TIMEOUT);
        HAL_Delay(5);
    }
}

void HPROM_WriteStr(u16 addr, u8 *str)
{
    HPROM_Write(addr, str, HSTR_GetLen(str) + 1);
}
