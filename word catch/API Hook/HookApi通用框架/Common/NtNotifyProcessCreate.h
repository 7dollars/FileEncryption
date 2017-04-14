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

#define		FILE_DEVICE_UNKNOWN		0x00000022

//������
#define		DRIVER_NAME			L"\\Device\\ProcessNotify"
//�豸��
#define		DEVICE_NAME			L"\\DosDevices\\ProcessNotify"
//�����¼���
#define		PROCESS_EVENT_NAME	L"\\BaseNamedObjects\\ProcessNotifyEvent"

#ifndef CTL_CODE	// IO ����������
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#endif // CTL_CODE

//---------------------------------------------------------------------------------------
// �ص���Ϣ�ṹ��
typedef struct _CallbackInfo
{
    HANDLE  hParentId;		//������PID
    HANDLE  hProcessId;		//�´������̵�PID
    BOOLEAN bCreate;		//�������ǳ��� -> TRUE ����; FALSE ����
}CALLBACK_INFO, *PCALLBACK_INFO;

//----------------------------------------------------------------------------------------
// IO������
#define	IOCTL_GET_PROCESS_INFO	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

