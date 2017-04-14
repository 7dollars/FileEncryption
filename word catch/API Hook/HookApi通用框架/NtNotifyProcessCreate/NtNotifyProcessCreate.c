/*
 *
 *                                   ���ӽ�������
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */

#include <ntddk.h>
#include <ntstatus.h>		//״̬��ͷ����
#include "..\\Common\\NtNotifyProcessCreate.h"	//ͷ�ļ�

//----------------------------------------------------------------------------------------
// ��չ�豸�ṹ��
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
//ȫ���豸����ָ��
PDEVICE_OBJECT g_pDeviceObject = NULL;

//----------------------------------------------------------------------------------
// ���ӽ��̻ص�����
VOID ProcNotifyCallBack(
	IN HANDLE hParentId, 
	IN HANDLE hProcessId, 
	IN BOOLEAN bCreate
)
{
	PDEVICE_EXTENSION pDevEx;	//�豸��չָ��
	
	pDevEx = g_pDeviceObject->DeviceExtension;	//�õ�ȫ���豸�����е��豸��չ
	pDevEx->hPParentId = hParentId;		//�õ�������ID
	pDevEx->hPProcessId = hProcessId;	//�õ���ǰ����ID
	pDevEx->bPCreate = bCreate;			//��������?
	
	//������չ�е��¼�,ʹRing3����õ�֪ͨ
	//��ʱ,Ring3����Ӧ��ʹ��DeviceIoControl����IOCTL_GET_PROCESS_INFO������
	//�õ����̴�����Ϣ
	KeSetEvent(pDevEx->ProcessEvent, 0, FALSE);
	KeClearEvent(pDevEx->ProcessEvent);			//���֪ͨ�¼�
	
	DbgPrint("CreateProcess: hParentId: %08X hProcessId: %08X bCreate: %d", 
		hParentId, hProcessId, bCreate);
}

//----------------------------------------------------------------------------------
// �ַ�����
NTSTATUS ProcNotifyDeviceIoCtl(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	//�õ���ǰIrpջλ��
	PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
	//�õ���ǰ�豸��չ
	PDEVICE_EXTENSION pDevEx = pDeviceObject->DeviceExtension;
	PCALLBACK_INFO pCallbackInfo;	//�ص���Ϣ�ṹ��
	
	//��IO��������зַ�
	switch(IrpStack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_GET_PROCESS_INFO:
		//��ȡ���̴�����Ϣ
		if(IrpStack->Parameters.DeviceIoControl.OutputBufferLength >= sizeof(CALLBACK_INFO))
		{
			//���ؽ��̴�����Ϣ�ṹ
			pCallbackInfo = Irp->AssociatedIrp.SystemBuffer;
			pCallbackInfo->hParentId = pDevEx->hPParentId;
			pCallbackInfo->hProcessId = pDevEx->hPProcessId;
			pCallbackInfo->bCreate = pDevEx->bPCreate;
			
			//״̬��Ϣ�ɹ�
			ntStatus = STATUS_SUCCESS;
		}
		else
		{
			//���ػ�����̫С
			ntStatus = STATUS_BUFFER_TOO_SMALL;
		}
		break;
	
	default:
		break;
	}
	
	//Irp�д��״̬��Ϣ
	Irp->IoStatus.Status = ntStatus;
	
	if(STATUS_SUCCESS == ntStatus)
	{
		//�ɹ�,����
		Irp->IoStatus.Information = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
	}
	else
	{
		//ʧ��,���س���0
		Irp->IoStatus.Information = 0;
	}
	
	//�������
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	DbgPrint("CreateProcess: DeviceIoControl Success!");
	
	return ntStatus;
}

//----------------------------------------------------------------------------------
// ж������
void ProcNotifyUnload(PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	UNICODE_STRING uszDeviceString;		//�豸���ַ���
	
	//���̴�����ʾ���������
	ntStatus = PsSetCreateProcessNotifyRoutine(ProcNotifyCallBack, TRUE);
	
	//��ʼ��Unicode�ַ���
	RtlInitUnicodeString(&uszDeviceString, DEVICE_NAME);
	
	//ɾ����������
	IoDeleteSymbolicLink(&uszDeviceString);
	
	//ɾ���豸����
	IoDeleteDevice(pDriverObject->DeviceObject);
	
	DbgPrint("CreateProcess: Unload Success!");
}

//----------------------------------------------------------------------------------
// ��������
NTSTATUS ProcNotifyCreate(IN PDEVICE_OBJECT pDeviceObject, IN PIRP Irp)
{
	//����״̬��Ϣ
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	
	//�������
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	DbgPrint("CreateProcess: Create Success!");
	
	return STATUS_SUCCESS;
}

//----------------------------------------------------------------------------------
// �������
NTSTATUS DriverEntry(
	PDRIVER_OBJECT pDriverObject, 
	PUNICODE_STRING pRegistryPath
)
{
	UNICODE_STRING uszDriverString;		//�������ַ���
	UNICODE_STRING uszDeviceString;		//�豸���ַ���
	UNICODE_STRING uszProcessEventString;	//�¼����ַ���
	
	PDEVICE_OBJECT pDeviceObject;		//�豸����
	PDEVICE_EXTENSION pDevEx;			//�豸��չ
	
	NTSTATUS ntStatus = STATUS_SUCCESS;
	
	//------����
	DbgPrint("CreateProcess: DriverEntry!");
	
	//��ʼ��Unicode�ַ���
	RtlInitUnicodeString(&uszDriverString, DRIVER_NAME);
	RtlInitUnicodeString(&uszDeviceString, DEVICE_NAME);
	RtlInitUnicodeString(&uszProcessEventString, PROCESS_EVENT_NAME);
	
	//�����豸����
	ntStatus = IoCreateDevice(
		pDriverObject, //��������
		sizeof(DEVICE_EXTENSION), //��չ�����С
		&uszDriverString, //�����ַ���
		FILE_DEVICE_UNKNOWN, //�豸����
		FILE_DEVICE_SECURE_OPEN, //�豸����
		FALSE, //Ψһ,ϵͳ����,FALSE
		&pDeviceObject //�豸����
	);
	
	if(STATUS_SUCCESS != ntStatus)
	{
		//�����豸����ʧ��
	//------����
	DbgPrint("CreateProcess: IoCreateDevice Failed!");
		return ntStatus;
	}
	
	//------����
	DbgPrint("CreateProcess: IoCreateDevice Success!");
	
	//ȡ���豸��չ
	pDevEx = pDeviceObject->DeviceExtension;
	
	//�����豸����ȫ�ֱ���
	g_pDeviceObject = pDeviceObject;
	
	///////////////////////////////////////////////////////////////////////
	//���䴦������
	//
	//��������
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = ProcNotifyCreate;
	//�ַ�����
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcNotifyDeviceIoCtl;
	//ж������
	pDriverObject->DriverUnload = ProcNotifyUnload;
	
	//������������
	ntStatus = IoCreateSymbolicLink(&uszDeviceString, &uszDriverString);
	
	if(STATUS_SUCCESS != ntStatus)
	{
		//������������ʧ��
		IoDeleteDevice(pDeviceObject);	//ɾ���豸����
		return ntStatus;
	}
	
	//����һ�������ص��¼�
	pDevEx->ProcessEvent = IoCreateNotificationEvent(&uszProcessEventString, &pDevEx->hProcessHandle);
	
	//����¼�
	KeClearEvent(pDevEx->ProcessEvent);
	
	//����������ʾ�ص����̼ӵ�������
	ntStatus = PsSetCreateProcessNotifyRoutine(ProcNotifyCallBack, FALSE);
	
	DbgPrint("CreateProcess: Load Success!");
	
	return ntStatus;
}

