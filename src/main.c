#pragma comment( linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"" )

#include "../include/stdafx.h"
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG          L"main"
#endif

extern BOOL(*createProcess)(const wchar_t *);

int wmain(int argc, _TCHAR *argv[]) {
    setlocale(LC_ALL, ".utf-8");

    //切换工作目录
    TCHAR* path = argv[0];
    //path = argv[0].find_first_of('\\');
    SetCurrentDirectory(GetFullDir());

    // check logs folder
    if (-1 == _taccess(CONFIG_PATH, 0)) {
        if (0 != _tmkdir(CONFIG_PATH))
            MessageBox(NULL, _T("Could not create folder"), _T("Error"), MB_ICONSTOP);
    }
    
    if (-1 == _taccess(LOG_PATH, 0)) {
        if (0 != _tmkdir(LOG_PATH))
            MessageBox(NULL, _T("Could not create folder"), _T("Error"), MB_ICONSTOP);
    }

    ERROR_LOG = _tfopen(ERROR_LOG_PATH,_T("w+"));
    if (ERROR_LOG == NULL) {
        MessageBox(NULL,_T("日志文件打开失败,程序退出.."),_T("错误"), MB_ICONSTOP);
        exit(EXIT_FAILURE);
    }
    setbuf(ERROR_LOG, NULL); //关闭缓冲
    
    // check log file
    FPLOG = _tfopen(LOG_FILE_PATH, _T("w+"));
    if (FPLOG == NULL) {
        _ftprintf(ERROR_LOG, _T("日志文件无权限访问,程序退出..\n"));
        exit(EXIT_FAILURE);
    }    
    setbuf(FPLOG, NULL); //关闭缓冲

    // check config file
    if (-1 == _taccess(CONFIG_FILE_PATH, 0)) {
            InitConfig();    
    }
    
    // Install watchdog as service 
    //-s 代表服务已经安装只需要启动服务即可
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/')))) {
        if (_wcsicmp(L"s", argv[1] + 1) == 0) {
            createProcess = CreateProcessForService;
            //启动服务
            RunService(ServiceName);
        }
    }
    else {
        //如果没有参数或者不是-s,表示服务未安装,先安装服务.
        BOOL result = InstallService(argv[0]);
        if (result) {
            //服务安装成功
            log_i(_T("服务安装成功..\n"));
        }
        else {
            log_e(_T("服务安装失败.., Error code: %d.\n"), GetLastError());
        }
        //createProcess = CreateProcessForService;
        //RunService(ServiceName);
        createProcess = CreateProcessNoService;
        Run();
    }

    //free(logFilePath);
    return 0;

}
