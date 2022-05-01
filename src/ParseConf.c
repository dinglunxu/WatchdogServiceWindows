#include "../include/stdafx.h"

//解析配置文件 
int ParseConf(Srv* SrvDef, int* TotalSrvs, Srv** ppSrvs,
    App* AppDef, int* TotalApps, App** ppApps) {
    /*
    *TotalSrvs = 2;
    *TotalApps = 2;
    *ppSrvs = (Srv*)calloc(*TotalApps, sizeof(Srv));
    *ppApps = (App*)calloc(*TotalApps, sizeof(App));
    //return 0;
    */
    //文件结构体
    FILE* config_file;
    //打开文件
    if ((config_file = _tfopen(CONFIG_FILE_PATH, _T("r"))) == NULL) {
        log_e(_T("配置文件打开失败!\n"));
        exit(EXIT_FAILURE);
    }

    //读取文件长度
    //定位文件偏移到末尾
    fseek(config_file, 0L, SEEK_END);
    //获取文件长度
    long total_size = ftell(config_file);
    if (total_size < 0) {
        log_e(_T("读取配置文件失败!配置文件长度为0\n"));
        exit(EXIT_FAILURE);
    }
    //分配内存
    char* json_data = malloc(sizeof(char) * total_size + 1);

    if (json_data == NULL) {
        log_e(_T("读取配置文件失败!分配内存失败\n"));
        exit(EXIT_FAILURE);
    }

    //设置文件指针到开头

    fseek(config_file, 0L, SEEK_SET);

    fread(json_data, sizeof(char), total_size, config_file);

    //关闭配置文件
    fclose(config_file);

    //解析json
    cJSON* root = NULL;
    cJSON* jsonSrv = NULL;
    cJSON* jsonSrvDef = NULL;
    cJSON* jsonSrvArr = NULL;
    cJSON* jsonSrvItem = NULL;
    cJSON* jsonSrvKV = NULL;
    cJSON* jsonSrvName = NULL;
    cJSON* jsonSrvActive = NULL;
    cJSON* jsonSrvPeriod = NULL;

    cJSON* jsonApp = NULL;
    cJSON* jsonAppDef = NULL;
    cJSON* jsonAppArr = NULL;
    cJSON* jsonAppItem = NULL;
    cJSON* jsonAppKV = NULL;
    cJSON* jsonAppName = NULL;
    cJSON* jsonAppAguments = NULL;
    cJSON* jsonAppCurrentDir = NULL;
    cJSON* jsonAppActive = NULL;
    cJSON* jsonAppPeriod = NULL;

    errno_t err = 0;
    size_t converted = 0;

    root = cJSON_Parse(json_data);
    //释放JSON字符串内存
    //free(json_data);

    /*======== Service ========*/

    jsonSrv = cJSON_GetObjectItem(root, "Service");
    if (jsonSrv) {
        // get default
        cJSON* jsonSrvDef = cJSON_GetObjectItem(jsonSrv, "Default");
        if (NULL != jsonSrvDef && cJSON_Object == jsonSrvDef->type) {
            jsonSrvKV = cJSON_GetObjectItem(jsonSrvDef, "Period");
            if (NULL != jsonSrvKV && cJSON_Number == jsonSrvKV->type)
                SrvDef->Period = jsonSrvKV->valueint;
            //cJSON_free(jsonKV);
        }
        //cJSON_free(jsonDef);
        // services array
        jsonSrvArr = cJSON_GetObjectItem(jsonSrv, "Services");
        if (NULL != jsonSrvArr && cJSON_Array == jsonSrvArr->type) {
            *TotalSrvs = cJSON_GetArraySize(jsonSrvArr);
            // molloc memory
            Srv* ppSrvstmp = (Srv*)calloc(*TotalSrvs, sizeof(Srv));
            // get services
            for (int i = 0; i < *TotalSrvs; i++) {
                jsonSrvItem = cJSON_GetArrayItem(jsonSrvArr, i);
                if (NULL != jsonSrvItem) {

                    // server name
                    cJSON* jsonSrvName = cJSON_GetObjectItem(jsonSrvItem, "Name");
                    if (NULL != jsonSrvName && cJSON_String == jsonSrvName->type)
                        err = mbstowcs_s(&converted,
                            ppSrvstmp[i].Name,
                            (size_t*)sizeof(jsonSrvName->valuestring) + 1,
                            jsonSrvName->valuestring,
                            _TRUNCATE);

                    // server active
                    cJSON* jsonSrvActive = cJSON_GetObjectItem(jsonSrvItem, "Active");
                    if (NULL != jsonSrvActive &&
                        (cJSON_True == jsonSrvActive->type ||
                            cJSON_False == jsonSrvActive->type))
                        ppSrvstmp[i].Active = jsonSrvActive->valueint;

                    // server period
                    cJSON* jsonSrvPeriod = cJSON_GetObjectItem(jsonSrvItem, "Period");
                    if (NULL != jsonSrvPeriod && cJSON_Number == jsonSrvPeriod->type)
                        ppSrvstmp[i].Period = jsonSrvPeriod->valueint;
                }
            }
            *ppSrvs = ppSrvstmp;
            cJSON_free(jsonSrvName);
            cJSON_free(jsonSrvPeriod);

        }
    }


    /*======== App ========*/

    jsonApp = cJSON_GetObjectItem(root, "App");
    if (jsonApp) {
        // get default
        cJSON* jsonAppDef = cJSON_GetObjectItem(jsonApp, "Default");
        if (NULL != jsonAppDef && cJSON_Object == jsonAppDef->type) {
            jsonAppKV = cJSON_GetObjectItem(jsonAppDef, "Period");
            if (NULL != jsonAppKV && cJSON_Number == jsonAppKV->type)
                AppDef->Period = jsonAppKV->valueint;
        }
        // ppAppstmp array
        cJSON* jsonAppArr = cJSON_GetObjectItem(jsonApp, "Apps");
        if (NULL != jsonAppArr && cJSON_Array == jsonAppArr->type) {
            *TotalApps = cJSON_GetArraySize(jsonAppArr);
            // molloc memory
            App* ppAppstmp = (App*)calloc(*TotalApps, sizeof(App));
            // get services
            for (int i = 0; i < *TotalApps; i++) {

                jsonAppItem = cJSON_GetArrayItem(jsonAppArr, i);
                if (NULL != jsonAppItem) {

                    // app name
                    cJSON* jsonAppName = cJSON_GetObjectItem(jsonAppItem, "Path");
                    if (NULL != jsonAppName && cJSON_String == jsonAppName->type)
                        err = mbstowcs_s(&converted,
                            ppAppstmp[i].Path,
                            (size_t*)sizeof(jsonAppName->valuestring) + 1,
                            jsonAppName->valuestring,
                            _TRUNCATE);

                    // app aguments
                    cJSON* jsonAppAguments = cJSON_GetObjectItem(jsonAppItem, "Aguments");
                    if (NULL != jsonAppAguments && cJSON_String == jsonAppAguments->type)
                        err = mbstowcs_s(&converted,
                            ppAppstmp[i].Aguments,
                            (size_t*)sizeof(jsonAppAguments->valuestring) + 1,
                            jsonAppAguments->valuestring,
                            _TRUNCATE);

                    // app current directory
                    cJSON* jsonAppCurrentDir = cJSON_GetObjectItem(jsonAppItem, "CurrentDir");
                    if (NULL != jsonAppCurrentDir && cJSON_String == jsonAppCurrentDir->type)
                        err = mbstowcs_s(&converted,
                            ppAppstmp[i].CurrentDir,
                            (size_t*)sizeof(jsonAppCurrentDir->valuestring) + 1,
                            jsonAppCurrentDir->valuestring,
                            _TRUNCATE);

                    // app active
                    cJSON* jsonAppActive = cJSON_GetObjectItem(jsonAppItem, "Active");
                    if (NULL != jsonAppActive &&
                        (cJSON_Number == jsonAppActive->type ||
                            cJSON_False == jsonAppActive->type))
                        ppAppstmp[i].Active = jsonAppActive->valueint;

                    // app period
                    cJSON* jsonAppPeriod = cJSON_GetObjectItem(jsonAppItem, "Period");
                    if (NULL != jsonAppPeriod && cJSON_Number == jsonAppPeriod->type)
                        ppAppstmp[i].Period = jsonAppPeriod->valueint;
                }
                cJSON_Delete(jsonAppPeriod);
            }
            *ppApps = ppAppstmp;
        }
    }

    //cJSON_Delete(root);

    return 0;
}



/* 初始化配置文件 */
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
