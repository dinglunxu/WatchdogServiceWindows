# WatchdogService-windows
> 以 Windows 服务运行的看门狗程序。

- 程序采用windows服务的方式在后台运行,服务启动方式为自动，在开机时自动启动看门狗服务。
- 周期性检测指定的服务状态，如果服务是停止的，就启动该服务，应用程序检测程序正在开发中。
- 可以通过配置文件的方式指定监控服务和程序


## 程序逻辑

看门狗服务定期获取指定的 Windows 服务状态，如果服务处于停止状态，就启动该服务。

```c
typedef struct Service {
    _TCHAR Name[255];          // 服务名称
    BOOL Active;               // 激活该服务对应的看门狗服务
    long Period;               // 检查周期
    long ElapsedTime;          // 从上次检查到现在的时间
    PROCESS_INFORMATION pi;    // 服务的进程信息
}Srv;

typedef struct Application {
    _TCHAR Path[1024];         // 带程序名称的路径
    _TCHAR Aguments[1024];     // 程序运行参数
    _TCHAR CurrentDir[1024];   // 指定程序运行时的工作目录
    BOOL Active;               // 激活该应用程序对应的看门狗功能
    long Period;               // 检查周期
    long ElapsedTime;          // 从上次检查到现在的时间
    PROCESS_INFORMATION pi;    // 程序的进程信息
}App;
```

- 程序运行是必须要确保运行目录下的logs目录存在,以及conf目录的config.json存在

## 如何配置

配置文件包含3部分，分别是依赖链 Chain ， Windows 服务 Service ， 应用程序 App 。
依赖链和应用程序 App 暂时未开发，只创建了配置文件结构。
Windows 服务 Service 包含一个共用的默认检测周期 Period ，在服务未指定检测周期时作为服务的检测周期。
Windows 服务 Service 还包含一个 Windows 服务列表 Services ，Services 数组可以配置多个需要检测的 Windows 服务。

应用程序 App 包含一个共用的默认检测周期 Period ，在应用程序未指定检测周期时作为服务的检测周期。
应用程序 App 还包含一个应用程序列表 Apps ， Apps 数组可以配置多个需要检测的程序，相同的程序以进程信息区分。

服务和应用程序的 Active 为 true 时激活对应的看门狗功能， Active 为 false 时不激活对应的看门狗功能。

配置文件会在程序启动过程中读取，如果在程序运行之后更改了配置文件，请重新启动看门狗服务使更改生效。

```json
{
	"Chain":	{
		"Chains":	[]
	},
	"Service":	{
		"Default":	{
			"Period":	10000
		},
		"Services":	[{
				"Name":	"InfluxDB",
				"Active":	true,
				"Period":	1000
			}, {
				"Name":	"telegraf",
				"Active":	true,
				"Period":	1000
			}, {
				"Name":	"Grafana",
				"Active":	true,
				"Period":	1000
			}]
	},
	"App":	{
		"Default":	{
			"Period":	10000
		},
		"Apps":	[{
				"Name":	"Notepad",
				"Active":	true,
				"Period":	1000,
				"Aguments":	"",
				"CurrentDir":	".\\\\"
			}, {
				"Name":	"Calc",
				"Active":	true,
				"Period":	1000,
				"Aguments":	"",
				"CurrentDir":	".\\\\"
			}]
	}
}
```

