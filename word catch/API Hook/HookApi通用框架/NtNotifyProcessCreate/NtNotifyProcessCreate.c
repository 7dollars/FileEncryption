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

#include <ntddk.h>
#include <ntstatus.h>		//状态码头定义
#include "..\\Common\\NtNotifyProcessCreate.h"	//头文件

//----------------------------------------------------------------------------------------
// 扩展设备结构体
typedef struct _DEVICE_EXTENSION 
{
    PDEVICE_OBJECT DeviceObject;
    HANDLE  hProcessHandle;
    PKEVENT ProcessEvent;

    HANDLE  hPParentId;
    HANDLE  hPProcessId;
    BOOLEAN bPCreate;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//----------------------------------------------------------------------------------
//全局设备对象指针
PDEVICE_OBJECT g_pDeviceObject = NULL;

//----------------------------------------------------------------------------------
// 监视进程回调函数
VOID ProcNotifyCallBack(
	IN HANDLE hParentId, 
	IN HANDLE hProcessId, 
	IN BOOLEAN bCreate
)
{
	PDEVICE_EXTENSION pDevEx;	//设备扩展指针
	
	pDevEx = g_pDeviceObject->DeviceExtension;	//得到全局设备对象中的设备扩展
	pDevEx->hPParentId = hParentId;		//得到父进程ID
	pDevEx->hPProcessId = hProcessId;	//得到当前进程ID
	pDevEx->bPCreate = bCreate;			//创建进程?
	
	//设置扩展中的事件,使Ring3程序得到通知
	//此时,Ring3程序应该使用DeviceIoControl发送IOCTL_GET_PROCESS_INFO控制码
	//得到进程创建信息
	KeSetEvent(pDevEx->ProcessEvent, 0, FALSE);
	KeClearEvent(pDevEx->ProcessEvent);			//清除通知事件
	
	DbgPrint("CreateProcess: hParentId: %08X hProcessId: %08X bCreate: %d", 
		hParentId, hProcessId, bCreate);
}

//----------------------------------------------------------------------------------
// 分发例程
NTSTATUS ProcNotifyDeviceIoCtl(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	//得到当前Irp栈位置
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
	//得到当前设备扩展
	PDEVICE_EXTENSION pDevEx = pDeviceObject->DeviceExtension;
	PCALLBACK_INFO pCallbackInfo;	//回调信息结构体
	
	//对IO控制码进行分发
	switch(IrpStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_GET_PROCESS_INFO:
		//获取进程创建信息
		if(IrpStack->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(CALLBACK_INFO))
		{
			//返回进程创建信息结构
			pCallbackInfo = Irp->AssociatedIrp.SystemBuffer;
			pCallbackInfo->hParentId = pDevEx->hPParentId;
			pCallbackInfo->hProcessId = pDevEx->hPProcessId;
			pCallbackInfo->bCreate = pDevEx->bPCreate;
			
			//状态信息成功
			ntStatus = STATUS_SUCCESS;
		}
		else
		{
			//返回缓冲区太小
			ntStatus = STATUS_BUFFER_TOO_SMALL;
		}
		break;
	
	default:
		break;
	}
	
	//Irp中存放状态信息
	Irp->IoStatus.Status = ntStatus;
	
	if(STATUS_SUCCESS == ntStatus)
	{
		//成功,返回
		Irp->IoStatus.Information = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
	}
	else
	{
		//失败,返回长度0
		Irp->IoStatus.Information = 0;
	}
	
	//完成请求
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	DbgPrint("CreateProcess: DeviceIoControl Success!");
	
	return ntStatus;
}

//----------------------------------------------------------------------------------
// 卸载例程
void ProcNotifyUnload(PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	UNICODE_STRING uszDeviceString;		//设备名字符串
	
	//进程创建提示从链表清除
	ntStatus = PsSetCreateProcessNotifyRoutine(ProcNotifyCallBack, TRUE);
	
	//初始化Unicode字符串
	RtlInitUnicodeString(&uszDeviceString, DEVICE_NAME);
	
	//删除符号链接
	IoDeleteSymbolicLink(&uszDeviceString);
	
	//删除设备对象
	IoDeleteDevice(pDriverObject->DeviceObject);
	
	DbgPrint("CreateProcess: Unload Success!");
}

//----------------------------------------------------------------------------------
// 创建例程
NTSTATUS ProcNotifyCreate(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	//返回状态信息
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	
	//完成请求
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	DbgPrint("CreateProcess: Create Success!");
	
	return STATUS_SUCCESS;
}

//----------------------------------------------------------------------------------
// 驱动入口
NTSTATUS DriverEntry(
	PDRIVER_OBJECT pDriverObject, 
	PUNICODE_STRING pRegistryPath
)
{
	UNICODE_STRING uszDriverString;		//驱动名字符串
	UNICODE_STRING uszDeviceString;		//设备名字符串
	UNICODE_STRING uszProcessEventString;	//事件名字符串
	
	PDEVICE_OBJECT pDeviceObject;		//设备对象
	PDEVICE_EXTENSION pDevEx;			//设备扩展
	
	NTSTATUS ntStatus = STATUS_SUCCESS;
	
	//------测试
	DbgPrint("CreateProcess: DriverEntry!");
	
	//初始化Unicode字符串
	RtlInitUnicodeString(&uszDriverString, DRIVER_NAME);
	RtlInitUnicodeString(&uszDeviceString, DEVICE_NAME);
	RtlInitUnicodeString(&uszProcessEventString, PROCESS_EVENT_NAME);
	
	//创建设备对象
	ntStatus = IoCreateDevice(
		pDriverObject, //驱动对象
		sizeof(DEVICE_EXTENSION), //扩展对象大小
		&uszDriverString, //驱动字符串
		FILE_DEVICE_UNKNOWN, //设备类型
		FILE_DEVICE_SECURE_OPEN, //设备特征
		FALSE, //唯一,系统保留,FALSE
		&pDeviceObject //设备对象
	);
	
	if(STATUS_SUCCESS != ntStatus)
	{
		//创建设备对象失败
	//------测试
	DbgPrint("CreateProcess: IoCreateDevice Failed!");
		return ntStatus;
	}
	
	//------测试
	DbgPrint("CreateProcess: IoCreateDevice Success!");
	
	//取得设备扩展
	pDevEx = pDeviceObject->DeviceExtension;
	
	//保存设备对象到全局变量
	g_pDeviceObject = pDeviceObject;
	
	///////////////////////////////////////////////////////////////////////
	//分配处理例程
	//
	//创建例程
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = ProcNotifyCreate;
	//分发例程
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcNotifyDeviceIoCtl;
	//卸载例程
	pDriverObject->DriverUnload = ProcNotifyUnload;
	
	//创建符号链接
	ntStatus = IoCreateSymbolicLink(&uszDeviceString, &uszDriverString);
	
	if(STATUS_SUCCESS != ntStatus)
	{
		//创建符号链接失败
		IoDeleteDevice(pDeviceObject);	//删除设备对象
		return ntStatus;
	}
	
	//创建一个句柄相关的事件
	pDevEx->ProcessEvent = IoCreateNotificationEvent(&uszProcessEventString, &pDevEx->hProcessHandle);
	
	//清除事件
	KeClearEvent(pDevEx->ProcessEvent);
	
	//创建进程提示回调例程加到链表中
	ntStatus = PsSetCreateProcessNotifyRoutine(ProcNotifyCallBack, FALSE);
	
	DbgPrint("CreateProcess: Load Success!");
	
	return ntStatus;
}

