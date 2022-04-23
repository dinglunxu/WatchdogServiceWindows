#ifndef WatchdogService__h
#define WatchdogService__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__


#include <UserEnv.h>
#include <WtsApi32.h>
#include <sddl.h>
#include "lib/cjson/cJSON.h"
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG    "WatchdogService"
#endif
#include "CUtils.h"
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")


#ifndef Watch_Dog_Service_H
#define Watch_Dog_Service_H
//看门狗结构体
typedef struct DogFood {
    //程序运行状态
    char status;
    //时间戳
    long long timestamp;
}DogFood;
//用于创建进程的函数指针
BOOL(*createProcess)(const wchar_t *);
#endif // !WatchdogService.h


//服务主体
void WINAPI ServiceMain(DWORD argc, PWSTR* argv);
//回调函数
void WINAPI ServiceCtrlHandler(DWORD);
//唤起指定的GUI程序
void Run();

//创建进程服务运行的环境下
BOOL CreateProcessForService(const wchar_t *);
//创建进程在非服务运行的环境下
BOOL CreateProcessNoService(const wchar_t *);


#ifdef __cplusplus
}
#endif

#endif

#endif
