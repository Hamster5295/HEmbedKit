# Hamster Embedded Kit
一个硬件库，重写了很多标准函数以实现明确的行为定义！  
需要与 STM32 HAL 库配合使用  

## Example
```C
#include "hdebug.h"

HDEBUG_Init(&huart1);

HDEBUG_Println("Hello World!");

```

## Roadmap
- [x] 数据类型定义
- [x] 错误处理
- [x] 字符串基础操作
  - [x] 字符转移与连接
  - [x] 基本类型 ToString
- [ ] ESP WIFI AT 指令集驱动
  - [x] WLAN
  - [x] TCP/IP
  - [ ] MQTT 
- [ ] ZX-D30 蓝牙 AT 指令集驱动
- [ ] OLED 屏幕驱动