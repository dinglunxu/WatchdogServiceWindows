#include "stdafx.h"
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG L"WatchdogService"
#endif

SERVICE_STATUS                        ServiceStatus;                           //服务状态
SERVICE_STATUS_HANDLE                 hStatus;                                 //服务状态句柄

//----------------


PROCESS_INFORMATION                   pi;                                      //子进程句柄
DWORD                                 returnCode;                              //子进程返回码
STARTUPINFO                           si = { sizeof(STARTUPINFO) };

//---------------
HANDLE                                hToken;                                  //用户token
HANDLE                                hTokenDup;                               //用户token
LPVOID                                pEnv;                                    //环境信息


//看门狗函数主体
void WINAPI ServiceMain(DWORD argc, PWSTR* argv) {

    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
    ServiceStatus.dwWin32ExitCode = NO_ERROR;
    ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandler(ServiceName, ServiceCtrlHandler);
    if (!hStatus)
    {
        DWORD dwError = GetLastError();
        log_e(_T("启动服务失败!%d\n"), dwError);
        return;
    }

    //设置服务状态
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);
    
    Run();

    //停止服务
    ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
    SetServiceStatus(hStatus, &ServiceStatus);
}


//服务回调
void WINAPI ServiceCtrlHandler(DWORD fdwControl)
{
    switch (fdwControl) {
    case SERVICE_CONTROL_STOP:
        log_i(_T("WatchDog 服务停止...\n"));
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = 0;
        SetServiceStatus(hStatus, &ServiceStatus);
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        log_i(_T("WatchDog 服务终止...\n"));
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = 0;
        SetServiceStatus(hStatus, &ServiceStatus);
        break;
    default:
        break;
    }
}

//运行指定程序
void Run() {
    int TotalpSrvs = 1, TotalpApps = 1;
    Srv SrvDef = { .Name = NULL,
                            .Period = NULL,
                            .ElapsedTime = 0,
                            .pi.hProcess = NULL,
                            .pi.hThread = NULL,
                            .pi.dwProcessId = 0,
                            .pi.dwThreadId = NULL };
    App AppDef = { .Path = NULL,
                            .Aguments = NULL,
                            .CurrentDir = NULL,
                            .Period = 0,
                            .ElapsedTime = 0,
                            .pi.hProcess = NULL,
                            .pi.hThread = NULL,
                            .pi.dwProcessId = 0,
                            .pi.dwThreadId = NULL, };
    Srv* pSrvs = { NULL };
    App* pApps = { NULL };

    //*pSrvs = (Srv*)malloc(sizeof(Srv));
    //*pApps = (App*)malloc(sizeof(App));
    //memset(pSrvs, 0, sizeof(Srv));

    log_i(_T("服务调用成功!\n"));
    int res = ParseConf(&SrvDef, &TotalpSrvs, &pSrvs, &AppDef, &TotalpApps, &pApps);

    while (TRUE) {
        //服务检测
        if (0 < TotalpSrvs) {
            for (int i = 0; i < TotalpSrvs; i++) {
                if (!pSrvs[i].Active)
                    continue;
                if (GetSrvSta(NULL, pSrvs[i].Name)) {
                    StartSrv(NULL, pSrvs[i].Name);
                    //pSrvs[i].ElapsedTime += BasicPeriod;
                    log_i(pSrvs[i].Name);
                }
                else {
                    pSrvs[i].ElapsedTime = 0;
                }
            }
        }
        //应用程序检测
        if (0 < TotalpApps) {
            for (int i = 0; i < TotalpApps; i++) {
                if (!pApps[i].Active)
                    continue;

                if (FALSE) {
                    //pApps[i].ElapsedTime += BasicPeriod;
                }
                else {
                    pApps[i].ElapsedTime = 0;
                }
                _TCHAR* log = pApps[i].Path;
                log_i(log);
            }
        }
        //基本检测周期
        Sleep(BasicPeriod);
    }

    free(pSrvs);
    free(pApps);

    return;
}


BOOL CreateProcessNoService(const wchar_t * commandLine) {
    return  CreateProcess(NULL, commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
}

//服务环境下创建进程
BOOL CreateProcessForService(const wchar_t * commandLine) {

    DWORD dwSessionID = WTSGetActiveConsoleSessionId();

    //获取当前处于活动状态用户的Token
    if (!WTSQueryUserToken(dwSessionID, &hToken)) {
        int nCode = GetLastError();
        log_e(_T("获取用户token失败,错误码:%d\n"), nCode);
        CloseHandle(hToken);
        return FALSE;
    }

    //复制新的Token
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup)) {
        int nCode = GetLastError();
        log_e(_T("复制用户token失败,错误码:%d\n"), nCode);

        CloseHandle(hToken);
        return FALSE;
    }

    //创建环境信息
    if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE)) {
        DWORD nCode = GetLastError();
        log_e(_T("创建环境信息失败,错误码:%d\n"), nCode);
        CloseHandle(hTokenDup);
        CloseHandle(hToken);
        return FALSE;
    }

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = _T("winsta0\\default");

    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    //开始创建进程
    DWORD dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;


    if (!CreateProcessAsUser(hTokenDup, NULL, commandLine, NULL, NULL, FALSE, dwCreateFlag, pEnv, NULL, &si, &pi))
    {
        DWORD nCode = GetLastError();
        log_e(_T("创建进程失败,错误码:%d\n"), nCode);
        DestroyEnvironmentBlock(pEnv);
        CloseHandle(hTokenDup);
        CloseHandle(hToken);
        return FALSE;
    }
    //SetPrivilege(hToken, SE_CREATE_GLOBAL_NAME, TRUE);

    //创建一个进程
    return TRUE;
}
