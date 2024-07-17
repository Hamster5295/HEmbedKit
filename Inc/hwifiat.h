/**
 * ESP8266 系 AT 指令集驱动
 */

#if !defined(__HWIFIAT_H__)
#define __HWIFIAT_H__

#include "hkit.h"

#pragma clang diagnostic ignored "-Wswitch"

// 常量
#if !defined(HWIFI_TIMEOUT_LEN)
/**
 * 与 WIFI 外设通信的超时时限
 */
#define HWIFI_TIMEOUT_LEN 2000
#endif

#if !defined(HWIFI_BLOCK_TIMEOUT_LEN)
/**
 * 与 WIFI 外设通信的超时时限
 */
#define HWIFI_BLOCK_TIMEOUT_LEN 8000
#endif

#if !defined(HWIFI_RECV_BUFFER_SIZE)
/**
 * 接收 WIFI 外设的缓冲区大小
 */
#define HWIFI_RECV_BUFFER_SIZE 2048
#endif

#if !defined(HWIFI_SEND_BUFFER_SIZE)
/**
 * 发送 WIFI 外设的缓冲区大小
 */
#define HWIFI_SEND_BUFFER_SIZE 2048
#endif

typedef enum HWIFI_State {
    HWIFI_State_Idle            = 0x00,
    HWIFI_State_Init            = 0x01,
    HWIFI_State_WaitForOK       = 0x02,
    HWIFI_State_WaitForSend     = 0x03,
    HWIFI_State_WaitForResponse = 0x04,
} HWIFI_State;

typedef enum HWIFI_Context {
    HWIFI_CTX_None               = 0x00,
    HWIFI_CTX_Init               = 0x01,
    HWIFI_CTX_SetCWMode          = 0x02,
    HWIFI_CTX_SetSoftAp          = 0x03,
    HWIFI_CTX_ConnectToWIFI      = 0x04,
    HWIFI_CTX_ScanWIFI           = 0x05,
    HWIFI_CTX_Send               = 0x06,
    HWIFI_CTX_StartTCPServer     = 0x09,
    HWIFI_CTX_StopTCPServer      = 0x0A,
    HWIFI_CTX_QueryStationIP     = 0x0B,
    HWIFI_CTX_QueryStationMAC    = 0x0C,
    HWIFI_CTX_StartTCPConnection = 0x0D,
    HWIFI_CTX_StopTCPConnection  = 0x0E,
    HWIFI_CTX_SetMQTTUserConfig  = 0x0F,
    HWIFI_CTX_ConnectMQTT        = 0x10,
    HWIFI_CTX_PublishMQTT        = 0x11,

    // 掩码，可以与上面的共存
    HWIFI_CTX_AP     = 0x8000,
    HWIFI_CTX_SERVER = 0x4000,
    HWIFI_CTX_CLIENT = 0x2000
} HWIFI_Context;

typedef enum HWIFI_Code {
    HWIFI_OK         = 0x00,
    HWIFI_ERROR      = 0x01,
    HWIFI_FAILED     = 0x02,
    HWIFI_TIMEOUT    = 0x03,
    HWIFI_CONNECT    = 0x04,
    HWIFI_DISCONNECT = 0x05,
    HWIFI_GETIP      = 0x06,
    HWIFI_CLOSED     = 0x07,
    HWIFI_RECV       = 0x08,
    HWIFI_BUSY       = 0x09
} HWIFI_Code;

typedef enum HWIFI_Mode {
    HWIFI_MODE_Station = 0x01,
    HWIFI_MODE_SoftAP  = 0x02,
    HWIFI_MODE_Both    = 0x03,
} HWIFI_Mode;

typedef enum HWIFI_Encryption {
    HWIFI_ENC_OPEN         = 0,
    HWIFI_ENC_WPA_PSK      = 2,
    HWIFI_ENC_WPA2_PSK     = 3,
    HWIFI_ENC_WPA_WPA2_PSK = 4,
} HWIFI_Encryption;

typedef enum HWIFI_MQTT_Scheme {
    HWIFI_MQTTSCHEME_TCP                    = 1,
    HWIFI_MQTTSCHEME_TLS_NO_CERTI           = 2,
    HWIFI_MQTTSCHEME_TLS_CERTI_SERVER       = 3,
    HWIFI_MQTTSCHEME_TLS_CERTI_CLIENT       = 4,
    HWIFI_MQTTSCHEME_TLS_CERTI_BOTH         = 5,
    HWIFI_MQTTSCHEME_WEBSOCKET_TCP          = 6,
    HWIFI_MQTTSCHEME_WEBSOCKET_NO_CERTI     = 7,
    HWIFI_MQTTSCHEME_WEBSOCKET_CERTI_SERVER = 8,
    HWIFI_MQTTSCHEME_WEBSOCKET_CERTI_CLIENT = 9,
    HWIFI_MQTTSCHEME_WEBSOCKET_CERTI_BOTH   = 10,
} HWIFI_MQTT_Scheme;

// 宏

// 函数
/**
 * 初始化 WIFI 串口。HWIFIAT 采用中断接收，故需要打开该串口的全局中断，并启用注册式回调（CubeMX -> Project Manager -> Advanced Settings -> Register Callback）
 * @param huart 要使用的串口
 * @param result_handler 结果处理回调
 */
HWIFI_Context
HWIFI_Init(UART *huart, void (*result_handler)(HWIFI_Context ctx, HWIFI_Code status, u8 *content));

/**
 * 更新函数，解析刚刚收到的数据
 */
void HWIFI_Update();

/**
 * 为注册的串口分配的事件回调
 */
void HWIFI_RxEventCallback(UART *uart, u16 size);

/**
 * WIFI 模块当前是否空闲？
 * @return 是否空闲
 */
bool HWIFI_IsIdle();

/**
 * 阻塞等待某个 WIFI 操作执行完毕
 * @param ctx 需要等待的操作，一旦该上下文退出则继续执行
 */
STATUS HWIFI_Block(HWIFI_Context ctx);

/**
 * 设置 WIFI 模块连接的工作模式
 */
HWIFI_Context HWIFI_SetMode(HWIFI_Mode mode);

/**
 * 配置软路由模式，即热点模式
 * @param ssid WIFI 热点名称
 * @param pwd 密码
 * @param channel WIFI 信道
 * @param enc 加密方式，需要与密码配合
 */
HWIFI_Context HWIFI_SetSoftAPConfig(char *ssid, char *pwd, u8 channel, HWIFI_Encryption enc);

/**
 * 连接到指定的 WIFI
 * @param ssid WIFI 名称
 * @param pwd WIFI 密码
 */
HWIFI_Context HWIFI_ConnectToWIFI(char *ssid, char *pwd);

/**
 * 使用一段拼好的字符串连接到指定 WIFI
 * @param token 拼好的字符串, 格式是 "WIFI名称","WIFI密码"
 */
HWIFI_Context HWIFI_ConnectToWIFIByToken(char *token);

/**
 * 扫描周围 WIFI 并返回列表
 */
HWIFI_Context HWIFI_ScanWIFI();

/**
 * 启动 TCP 服务器
 * @param port 端口, 0~65535 均可
 */
HWIFI_Context HWIFI_StartTCPServer(u16 port);

/**
 * 关闭 TCP 服务器
 */
HWIFI_Context HWIFI_StopTCPServer();

/**
 * 通过指定地址和端口建立TCP连接
 * @param ip IP地址首位
 * @param port 端口号
 */
HWIFI_Context HWIFI_StartTCPConnection(u8 *ip, u16 port);

/**
 * 关闭 TCP 连接
 */
HWIFI_Context HWIFI_StopTCPConnection();

/**
 * 查询当前网络下的 IP 地址
 */
HWIFI_Context HWIFI_QueryStationIP();

/**
 * 查询当前网络下的 MAC 地址
 */
HWIFI_Context HWIFI_QueryStationMAC();

/**
 * 发送指定长度数据
 * @param str 数据首位置
 * @param len 数据长度
 */
HWIFI_Context HWIFI_Send(u8 *str, u16 len);

/**
 * 发送一个字符串，直到遇到终止符。终止符也会被发送出去。
 * @param str 字符串首位置
 */
HWIFI_Context HWIFI_SendStr(u8 *str);

/**
 * 将接收缓冲区的数据原封不动地发出去
 */
HWIFI_Context HWIFI_Xfer();

/**
 * 发送指定长度数据数据给指定连接
 * @param client 连接的名称，通常是从 0 开始计的数字
 * @param str 发送的数据
 * @param len 数据长度
 */
HWIFI_Context HWIFI_SendClient(u8 client, u8 *str, u16 len);

/**
 * 发送字符串给指定连接，直到遇到终止符。终止符也会被发送出去。
 * @param client 连接的名称，通常是从 0 开始计的数字
 * @param str 发送的数据
 */
HWIFI_Context HWIFI_SendClientStr(u8 client, u8 *str);

/**
 * 将接收缓冲区的数据原封不动地发给指定连接
 */
HWIFI_Context HWIFI_XferClient(u8 client);

/**
 * 设置 MQTT 连接的配置
 * @param scheme 连接方式
 * @param client_id 客户端 id
 * @param username 认证用户名
 * @param pwd 认证密码
 */
HWIFI_Context HWIFI_SetMQTTUserConfig(HWIFI_MQTT_Scheme scheme, u8 *client_id, u8 *username, u8 *pwd);

/**
 * 连接到 MQTT 服务器
 * @param url 服务器 url
 * @param port 服务器端口
 * @param retry 断开时自动重连
 */
HWIFI_Context HWIFI_ConnectMQTT(u8 *url, u16 port, bool retry);

/**
 * 向 MQTT 话题发布定长数据
 * @param topic 要发布的话题
 * @param data 数据
 * @param len 数据长度
 * @param qos 服务质量，0~2，影响数据传输的可靠性
 * @param retain 是否在服务器端保存该数据，直到下一个 retain 数据到达
 */
HWIFI_Context HWIFI_PublishMQTTSize(u8 *topic, u8 *data, u16 len, u8 qos, bool retain);

#endif // __HWIFIAT_H__
