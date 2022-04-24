#ifndef Common_H
#define Common_H


#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

//服务名称
#define ServiceName L"WatchdogService"
#define ServiceAguments L" -s"
#define CONFIG_PATH L".\\conf"
//看门狗配置文件
#define CONFIG_FILE_PATH  L"./conf/config.json"
//日志路径
#define LOG_PATH  L".\\logs\\"
//日志文件名称
#define LOG_FILE_PATH L".\\logs\\WatchdogService_log.log"
//错误日志路径
#define ERROR_LOG_PATH L".\\logs\\WatchdogService_error.log"
//日志文件对象
FILE * FPLOG;
//错误日志文件对象
FILE * ERROR_LOG;

//基本检测周期
#define BasicPeriod 1000 // 每隔1秒检测一次

#endif // !Common_H
