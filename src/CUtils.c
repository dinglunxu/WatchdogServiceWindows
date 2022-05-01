#include "../include/CUtils.h"

//获取程序的完整路径
TCHAR *  GetFullPath() {
    TCHAR* szModuleFileName = NULL;
        szModuleFileName = (TCHAR*)malloc(sizeof(TCHAR) * MAX_PATH + 1);
    //if (!szModuleFileName)
        GetModuleFileName(NULL, szModuleFileName, MAX_PATH);
    return szModuleFileName;
}
//获取程序当前路径
TCHAR *  GetFullDir() {
    TCHAR * path = GetFullPath();
    char *ch = _tcsrchr(path, _T('\\'));
    *ch = 0;
    return path;
}
