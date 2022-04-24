#include "CUtils.h"

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


int InitConfig() {

    cJSON* root = NULL;
    cJSON* jsonObj = NULL;
    cJSON* jsonArr = NULL;
    cJSON* jsonItem = NULL;

    //创建根对象
    root = cJSON_CreateObject();
    
    //cJSON_AddStringToObject(root, "cmd", "notepad");

    //添加依赖链对象
    jsonObj = cJSON_CreateObject();
    cJSON_AddArrayToObject(jsonObj, "Chains");
    cJSON_AddItemToObject(root, "Chain", jsonObj);

    // 添加服务对象

    jsonItem = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonItem, "Period", cJSON_CreateNumber(10000));

    jsonObj = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonObj, "Default", jsonItem);

    jsonArr = cJSON_CreateArray();

    jsonItem = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonItem, "Name", "InfluxDB");
    cJSON_AddBoolToObject(jsonItem, "Active", TRUE);
    cJSON_AddNumberToObject(jsonItem, "Period", 1000);
    cJSON_AddItemToArray(jsonArr, jsonItem);
    
    jsonItem = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonItem, "Name", "telegraf");
    cJSON_AddBoolToObject(jsonItem, "Active", TRUE);
    cJSON_AddNumberToObject(jsonItem, "Period", 1000);
    cJSON_AddItemToArray(jsonArr, jsonItem);

    jsonItem = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonItem, "Name", "Grafana");
    cJSON_AddBoolToObject(jsonItem, "Active", TRUE);
    cJSON_AddNumberToObject(jsonItem, "Period", 1000);
    cJSON_AddItemToArray(jsonArr, jsonItem);

    cJSON_AddItemToObject(jsonObj, "Services", jsonArr);
    cJSON_AddItemToObject(root, "Service", jsonObj);

    //添加应用程序对象
    jsonItem = cJSON_CreateObject();
    cJSON_AddNumberToObject(jsonItem, "Period", 10000);

    jsonObj = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonObj, "Default", jsonItem);

    jsonArr = cJSON_CreateArray();
    jsonItem = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonItem, "Path", "Notepad");
    cJSON_AddStringToObject(jsonItem, "Aguments", "");
    cJSON_AddStringToObject(jsonItem, "CurrentDir", ".\\");
    cJSON_AddBoolToObject(jsonItem, "Active", TRUE);
    cJSON_AddNumberToObject(jsonItem, "Period", 1000);
    cJSON_AddItemToArray(jsonArr, jsonItem);

    //cJSON_free(jsonItem);
    jsonItem = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonItem, "Path", "Calc");
    cJSON_AddStringToObject(jsonItem, "Aguments", "");
    cJSON_AddStringToObject(jsonItem, "CurrentDir", ".\\");
    cJSON_AddBoolToObject(jsonItem, "Active", TRUE);
    cJSON_AddNumberToObject(jsonItem, "Period", 1000);
    cJSON_AddItemToArray(jsonArr, jsonItem);


    cJSON_AddItemToObject(jsonObj, "Apps", jsonArr);
    cJSON_AddItemToObject(root, "App", jsonObj);

    //将配置写入文件
    FILE* file = NULL;
    file = _tfopen(CONFIG_FILE_PATH, _T("a"));

    if (NULL == file) {
        MessageBox(NULL, _T("配置写入失败"), _T("错误"), MB_ICONSTOP);
        return -1;
    }
    _TCHAR* conf = cJSON_Print(root);

    int res = fputs(conf, file);
    if (EOF == res) {
        MessageBox(NULL, _T("配置写入失败"), _T("错误"), MB_ICONSTOP);
        return -1;
    }
    fclose(file);

    cJSON_Delete(root);
    cJSON_free(conf);


    return 0;
}
