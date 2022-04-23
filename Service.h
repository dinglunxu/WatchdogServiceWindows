
#ifndef Service__h
#define Service__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

#include "WatchdogService.h"
//初始化服务
void RunService(_TCHAR*SrvName);
//安装服务
BOOL InstallService(_TCHAR *path);
//判断是否已经安装过服务
BOOL IsInstalled(SC_HANDLE* phSCM, _TCHAR* SrvName);
//检查服务状态
int GetSrvSta(SC_HANDLE* phSCM, _TCHAR* SrvName);
//启动已经存在的服务
int StartSrv(SC_HANDLE* phSCM, _TCHAR* SrvName);
//打印错误日志
void Lerror(const char * format);

#ifdef __cplusplus
}
#endif

#endif

#endif
