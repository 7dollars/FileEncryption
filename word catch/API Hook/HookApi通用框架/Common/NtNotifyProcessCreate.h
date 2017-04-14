/*
 *
 *                                   监视进程驱动
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */

#define		FILE_DEVICE_UNKNOWN		0x00000022

//驱动名
#define		DRIVER_NAME			L"\\Device\\ProcessNotify"
//设备名
#define		DEVICE_NAME			L"\\DosDevices\\ProcessNotify"
//进程事件名
#define		PROCESS_EVENT_NAME	L"\\BaseNamedObjects\\ProcessNotifyEvent"

#ifndef CTL_CODE	// IO 控制码生成
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#endif // CTL_CODE

//---------------------------------------------------------------------------------------
// 回调信息结构体
typedef struct _CallbackInfo
{
    HANDLE  hParentId;		//父进程PID
    HANDLE  hProcessId;		//新创建进程的PID
    BOOLEAN bCreate;		//创建还是撤消 -> TRUE 创建; FALSE 撤消
}CALLBACK_INFO, *PCALLBACK_INFO;

//----------------------------------------------------------------------------------------
// IO控制码
#define	IOCTL_GET_PROCESS_INFO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

