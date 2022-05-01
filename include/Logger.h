#ifndef logger__h
#define logger__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__


#include "common.h"

#if !defined(LOG_TAG)
#define LOG_TAG          L"NO_TAG"
#endif

#include <stdarg.h>
#include <time.h>

#define __STR2WSTR(str) L##str
#define _STR2WSTR(str) __STR2WSTR(str)
#ifdef __FUNCTIONW__
#undef __FUNCTIONW__
#define __FUNCTIONW__ _STR2WSTR(__FUNCTION__)
#endif

/* 日志级别枚举类型 */
enum Level { L_DEBUG, L_INFO, L_ERROR };
/* 打印debug级别的日志 */
#define log_d(...) PrintLog(L_DEBUG,LOG_TAG,__FUNCTIONW__,__LINE__,__VA_ARGS__) 
/* 打印info级别的日志 */
#define log_i(...) PrintLog(L_INFO,LOG_TAG,__FUNCTIONW__,__LINE__,__VA_ARGS__)
/* 打印error级别的日志 */
#define log_e(...) PrintLog(L_ERROR,LOG_TAG,__FUNCTIONW__,__LINE__,__VA_ARGS__)

//#endif // LOGGER_H


// 打印日志
void PrintLog(int level,TCHAR * tag,TCHAR * func,long line,TCHAR * format,...);



#ifdef __cplusplus
}
#endif

#endif

#endif
