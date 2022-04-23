#pragma once

#ifndef ParseConf__h
#define ParseConf__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

//解析服务默认配置
int ParseConfForServices();


typedef struct Service {
    _TCHAR Name[255];          // 服务名称
    BOOL Active;               // 激活该服务对应的看门狗服务
    long Period;               // 检查周期
    long ElapsedTime;          // 从上次检查到现在的时间
    PROCESS_INFORMATION pi;    // 服务的进程信息
}Srv;

typedef struct Application {
    _TCHAR Path[1024];         // 带程序名称的路径
    _TCHAR Aguments[1024];     // 程序运行参数
    _TCHAR CurrentDir[1024];   // 指定程序运行时的工作目录
    BOOL Active;               // 激活该应用程序对应的看门狗功能
    long Period;               // 检查周期
    long ElapsedTime;          // 从上次检查到现在的时间
    PROCESS_INFORMATION pi;    // 程序的进程信息
}App;


//解析配置文件
int ParseConf(Srv* SrvDef, int* TotalSrvs, Srv** ppSrvs,
    App* AppDef, int* TotalApps, App** ppApps);



#ifdef __cplusplus
}
#endif

#endif

#endif
