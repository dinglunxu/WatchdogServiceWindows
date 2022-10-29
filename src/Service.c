#include "../include/Service.h"
#define LOG_TAG L"ServiceInstall"
#include "../include/Logger.h"

//运行服务
void RunService(_TCHAR* SrvName) {
    SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = SrvName;
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    StartServiceCtrlDispatcher(ServiceTable);
}

//判断是否已经安装过服务
BOOL IsInstalled(SC_HANDLE* phSCM, _TCHAR* SrvName) {
    BOOL bResult = FALSE;
    SC_HANDLE hSCM = phSCM;
    if (NULL == phSCM)
        hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

    if (!hSCM) {
        log_e(_T("OpenSCManager failed, Error code: %d.\n"), GetLastError());
        return FALSE;
    }

    SC_HANDLE hService = OpenService(hSCM, SrvName, SERVICE_QUERY_CONFIG);
    if (!hService) {
        if (NULL == phSCM)
            CloseServiceHandle(hSCM);
        log_i(_T("Service not found \n"));
        return FALSE;
    }

    bResult = TRUE;
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return bResult;
}

//获取服务状态
//BOOL GetSrvSta(SC_HANDLE* phSCM, _TCHAR* SrvName, SERVICE_STATUS_PROCESS* pssStatus) {
BOOL GetSrvSta(SC_HANDLE* phSCM, _TCHAR* SrvName) {
    BOOL bResult = FALSE;
    //SERVICE_STATUS_PROCESS* ssStatus = (SERVICE_STATUS_PROCESS*)calloc(1, sizeof(SERVICE_STATUS_PROCESS));
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    SC_HANDLE *hSCM = phSCM;
    if (NULL == phSCM)
        hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!hSCM) {
        log_e(_T("OpenSCManager failed, Error code: %d.\n"), GetLastError());
        return FALSE;
    }
    SC_HANDLE hService = OpenService(hSCM, SrvName, SERVICE_QUERY_STATUS);


    if (!hService) {
        if (NULL == phSCM)
            CloseServiceHandle(hSCM);
        log_e(_T("OpenService failed, Service name: %s, Error code: %d.\n")
            , SrvName, GetLastError());
        return -1;
    }

    if (!QueryServiceStatusEx(
        hService,                       // handle to service 
        SC_STATUS_PROCESS_INFO,         // information level
        (LPBYTE)&ssStatus,              // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded)) {              // size needed if buffer is too small

        CloseServiceHandle(hService);
        if (NULL == phSCM)
            CloseServiceHandle(hSCM);
        log_e(_T("Query Service Status failed, Service name: %s, Error code: %d.\n")
            , SrvName, GetLastError());
        return -1;
    }

    if (ssStatus.dwCurrentState == SERVICE_STOPPED) {
        log_i(_T("Service is stopped, Service name: %s.\n"), SrvName);
        bResult = TRUE;
    }
    
    CloseServiceHandle(hService);
    if (NULL == phSCM)
        CloseServiceHandle(hSCM);
    
    return bResult;
}


//启动服务
int StartSrv(SC_HANDLE* phSCM, _TCHAR* SrvName) {
    int iResult = 0;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;
    SC_HANDLE* hSCM = phSCM;
    if (NULL == phSCM)
        hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!hSCM) {
        log_e(_T("OpenSCManager failed, Service name: %s, Error code: %d.\n"), GetLastError());
        return -1;
    }
    SC_HANDLE hService = OpenService(hSCM, SrvName, SERVICE_ALL_ACCESS);
    if (!hService) {
        if (NULL == phSCM)
            CloseServiceHandle(hSCM);
        log_e(_T("OpenService failed, Service name: %s, Error code: %d.\n")
            , SrvName, GetLastError());
        return -1;
    }

    if (!QueryServiceStatusEx(
        hService,                       // handle to service 
        SC_STATUS_PROCESS_INFO,         // information level
        (LPBYTE)&ssStatus,              // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded)) {              // size needed if buffer is too small
            
        CloseServiceHandle(hService);
        if (NULL == phSCM)
            CloseServiceHandle(hSCM);
        log_e(_T("QueryServiceStatusEx failed, Service name: %s, Error code: %d.\n"), GetLastError());
        return -1;
    }

    if (ssStatus.dwCurrentState != SERVICE_STOPPED) {
        CloseServiceHandle(hService);
        if (NULL == phSCM)
            CloseServiceHandle(hSCM);
        log_e(_T("Service is already running, Service name: %s.\n"), SrvName);
        return -1;
    }

    if (!StartService(
        hService,  // handle to service 
        0,           // number of arguments 
        NULL))      // no arguments 
    {
        printf(_T("Start service failed, Service name: %s, Error code: %d.\n")
            , SrvName, GetLastError());
        CloseServiceHandle(hService);
        if (NULL == phSCM)
            CloseServiceHandle(hSCM);
        return 0;
    }
    else {
        log_i(_T("Service started successfully, %s.\n"), SrvName);
    }

    return iResult;
}


//安装服务
BOOL InstallService(_TCHAR *path) {
    //打开服务控制器
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (hSCM == NULL) {
        log_e(_T("打开服务控制器失败..., Error code: %d.\n"), GetLastError());
        return FALSE;
    }

    log_i(_T("安装服务中...\n"));
    if (IsInstalled(&hSCM, ServiceName)) {
        log_i(_T("服务已安装...\n"));
        return FALSE;
    }


    _TCHAR* arg = ServiceAguments;
    //int PathSize = sizeof(path) + sizeof(ServiceAguments) + 1;
    //TCHAR* fullPath = (TCHAR*)malloc(sizeof(TCHAR*) * PathSize);
    //int j = _snwprintf(fullPath, PathSize, "%s%s", path, (TCHAR*)(ServiceAguments));
    //fullPath = _tcscat(fullPath, path);
    //fullPath = _tcscat(fullPath, ServiceAguments);
    _TCHAR* fullPath = _tcscat(GetFullPath(), ServiceAguments);

    SC_HANDLE hService = CreateService(hSCM, 
        ServiceName, 
        ServiceName,
        SERVICE_QUERY_STATUS,
        SERVICE_WIN32_OWN_PROCESS, 
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL, 
        fullPath,
        NULL, 
        NULL, 
        NULL, 
        NULL,
        NULL);
    if (hService == NULL){
        log_e(_T("服务创建失败..., Error code: %d.\n"), GetLastError());
        return FALSE;
    }
    //释放句柄
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);

    return TRUE;
}

void Lerror(const char * format) {
    int rst = GetLastError();
    if (rst) {
        log_e(format,rst);
    }
}
