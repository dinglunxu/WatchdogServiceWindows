#ifndef CUTILS_H
#define CUTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

#include "common.h"
#include "../lib/cjson/cJSON.h"

//获取本程序完整路径
TCHAR *  GetFullPath();
//获取本程序工作目录
TCHAR *  GetFullDir();
//初始化日志
BOOL InitLog();

#endif // !CUTILS_h

#ifdef __cplusplus
}
#endif

#endif
