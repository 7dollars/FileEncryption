/*
 *
 *                               监视进程驱动操作函数
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */


#include <stdio.h>
#include <windows.h>
#include "..\\Common\\NtNotifyProcessCreate.h"		//驱动头文件
#include "..\\Common\\NtSysOperate.h"				//函数声明

#define		EXE_DRIVER_NAME		"ProcessNotify"			//创建的驱动名
#define		DISPLAY_NAME		"ProcessNotify Driver"	//显示的服务名
#define		PROCESS_NOTIFY_NAME	"ProcessNotifyEvent"			//进程创建提示驱动名

//-----------------------------------------------------
//Ntddk.h中定义的数据
#define		METHOD_BUFFERED			0
#define		METHOD_IN_DIRECT		1
#define		METHOD_OUT_DIRECT		2
#define		METHOD_NEITHER			3

#define		FILE_ANY_ACCESS			0
#define		FILE_SPECIAL_ACCESS		(FILE_ANY_ACCESS)
#define		FILE_READ_ACCESS		( 0x0001 )    // file & pipe
#define		FILE_WRITE_ACCESS		( 0x0002 )    // file & pipe
//-----------------------------------------------------

//关闭进程创建提示事件
HANDLE g_hShutdownEvent = NULL;
//加载返回的驱动句柄
HANDLE g_hDriver = NULL;
//全局进程创建提示事件
HANDLE g_hProcessEvent = NULL;
//全局
DWORD g_lpfHandleCreateProcess = 0;

//引用关闭完成提示事件
extern HANDLE g_hShutdownOverEvent;


///////////////////////////////////////////////////
//加载驱动
HANDLE LoadDriver(IN LPCTSTR lpFileName)
{
    HANDLE hDriver = INVALID_HANDLE_VALUE;
	char OpenName[MAX_PATH+1];

	memset(OpenName, 0, sizeof(OpenName));

	sprintf(OpenName, "\\\\.\\%s", EXE_DRIVER_NAME);

	//打开服务管理器
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL,
        SC_MANAGER_CREATE_SERVICE);

    if (NULL != hSCManager)
    {
		//创建服务
        SC_HANDLE hService = CreateService(hSCManager, EXE_DRIVER_NAME,
            DISPLAY_NAME, SERVICE_START,
            SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
            SERVICE_ERROR_IGNORE, lpFileName, NULL, NULL, NULL, NULL, NULL);

        if (ERROR_SERVICE_EXISTS == GetLastError())
        {
            hService = OpenService(hSCManager, EXE_DRIVER_NAME, SERVICE_START);
        }
        if(!StartService(hService, 0, NULL))
		{
			if(GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)	//已经启动
			{
				//启动失败
				// Do Nothing
			}
			else
			{
				//服务已经启动
				// Do Nothing
			}
		}
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        hDriver = CreateFileA(OpenName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hDriver == INVALID_HANDLE_VALUE)
		{
			//获取文件句柄失败
			// Do Nothing
		}
    }
    return hDriver;
}

///////////////////////////////////////////////////
//卸载驱动
void UnloadDriver(IN HANDLE hDriver)
{
	//关闭驱动句柄
    CloseHandle(hDriver);

	//打开服务管理器
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL,
        SC_MANAGER_CREATE_SERVICE);

    if (NULL != hSCManager)
    {
		//打开服务
        SC_HANDLE hService = OpenService(hSCManager, EXE_DRIVER_NAME, DELETE | SERVICE_STOP);

        if (NULL != hService)
        {
            SERVICE_STATUS ss;
			//停止服务
            ControlService(hService, SERVICE_CONTROL_STOP, &ss);
			//删除服务
            DeleteService(hService);
			//关闭服务句柄
            CloseServiceHandle(hService);
        }
		//关闭服务管理器句柄
        CloseServiceHandle(hSCManager);
    }
}

///////////////////////////////////////////////////
//获取创建的进程信息
BOOL GetProcInfo(IN HANDLE hDriver, OUT PCALLBACK_INFO pProcInfo)
{
	if(NULL == pProcInfo)
	{
		//输出指针为空
		return FALSE;
	}

	//准备发送DeviceIoControl码
	DWORD dwRet;
	BOOL bRet = DeviceIoControl(hDriver, IOCTL_GET_PROCESS_INFO, NULL, 0, \
		(LPVOID)pProcInfo, sizeof(CALLBACK_INFO), &dwRet, NULL);

	if(bRet)
	{
		//成功
		return TRUE;
	}
	
	return FALSE;
}

void MonitorThread()
{
	DWORD dwRet;
	DWORD threadID;
	
	//创建关闭事件
	g_hShutdownEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//创建等待事件数组
	HANDLE hHandleArray[2] = 
	{
		{g_hShutdownEvent}, 
		{g_hProcessEvent}
	};


	while(TRUE)
	{
		dwRet = ::WaitForMultipleObjects(
			sizeof(hHandleArray) / sizeof(hHandleArray[0]), //等待个数
			&hHandleArray[0], //事件数组
			FALSE, //只要有事件发生,就返回
			INFINITE //一直等
			);
		
		if(hHandleArray[dwRet - WAIT_OBJECT_0] == g_hShutdownEvent)
		{
			//关闭事件产生
			break;
		}
		else
		{
			//进程创建提示事件
			
			//获取创建的进程信息
			PCALLBACK_INFO pProcInfo = (PCALLBACK_INFO)malloc(sizeof(CALLBACK_INFO));
			if(GetProcInfo(g_hDriver, pProcInfo))
			{
				//得到新创建的进程信息成功
				
				//创建新线程,运行 lpfHandleCreateProcess 函数
				::CreateThread(NULL, 0, \
					(LPTHREAD_START_ROUTINE)g_lpfHandleCreateProcess, \
					(PVOID)pProcInfo, \
					0, &threadID);
			}
		}
	}	// End whild
	
	//卸载驱动
	UnloadDriver(g_hDriver);
	
	//关闭事件
	::CloseHandle(g_hShutdownEvent);
	::CloseHandle(g_hProcessEvent);
	
	//测试
//	MessageBox(NULL, "卸载成功!", "提示", MB_OK);	//-----测试

	//设置关闭完成提示事件
	::SetEvent(g_hShutdownOverEvent);
}

///////////////////////////////////////////////////
//创建事件提示
BOOL CreateEventNotifyRing3(IN LPCTSTR lpFileName, IN DWORD lpfHandleCreateProcess)
{
	HANDLE m_hProcessEvent;
	DWORD threadID;

	//加载驱动
	g_hDriver = LoadDriver(lpFileName);
	if(INVALID_HANDLE_VALUE == g_hDriver)
	{
		//加载失败
		MessageBox(NULL, "加载驱动失败!", "错误", MB_OK);
		return FALSE;
	}

	//打开驱动事件
	m_hProcessEvent = ::OpenEvent(SYNCHRONIZE, FALSE, PROCESS_NOTIFY_NAME);

	if(m_hProcessEvent)
	{
		//创建提示事件成功

		g_hProcessEvent = m_hProcessEvent;
		g_lpfHandleCreateProcess = lpfHandleCreateProcess;

		//创建新线程
		::CreateThread(NULL, 0, \
			(LPTHREAD_START_ROUTINE)MonitorThread, \
			NULL, \
			0, &threadID);

	}	// End if
	else
	{
		MessageBox(NULL, "进程创建事件创建失败!", "错误", MB_OK);
		return FALSE;
	}	// End else

	return TRUE;
}

