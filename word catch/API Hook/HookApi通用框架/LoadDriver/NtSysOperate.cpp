/*
 *
 *                               ���ӽ���������������
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */


#include <stdio.h>
#include <windows.h>
#include "..\\Common\\NtNotifyProcessCreate.h"		//����ͷ�ļ�
#include "..\\Common\\NtSysOperate.h"				//��������

#define		EXE_DRIVER_NAME		"ProcessNotify"			//������������
#define		DISPLAY_NAME		"ProcessNotify Driver"	//��ʾ�ķ�����
#define		PROCESS_NOTIFY_NAME	"ProcessNotifyEvent"			//���̴�����ʾ������

//-----------------------------------------------------
//Ntddk.h�ж��������
#define		METHOD_BUFFERED			0
#define		METHOD_IN_DIRECT		1
#define		METHOD_OUT_DIRECT		2
#define		METHOD_NEITHER			3

#define		FILE_ANY_ACCESS			0
#define		FILE_SPECIAL_ACCESS		(FILE_ANY_ACCESS)
#define		FILE_READ_ACCESS		( 0x0001 )    // file & pipe
#define		FILE_WRITE_ACCESS		( 0x0002 )    // file & pipe
//-----------------------------------------------------

//�رս��̴�����ʾ�¼�
HANDLE g_hShutdownEvent = NULL;
//���ط��ص��������
HANDLE g_hDriver = NULL;
//ȫ�ֽ��̴�����ʾ�¼�
HANDLE g_hProcessEvent = NULL;
//ȫ��
DWORD g_lpfHandleCreateProcess = 0;

//���ùر������ʾ�¼�
extern HANDLE g_hShutdownOverEvent;


///////////////////////////////////////////////////
//��������
HANDLE LoadDriver(IN LPCTSTR lpFileName)
{
    HANDLE hDriver = INVALID_HANDLE_VALUE;
	char OpenName[MAX_PATH+1];

	memset(OpenName, 0, sizeof(OpenName));

	sprintf(OpenName, "\\\\.\\%s", EXE_DRIVER_NAME);

	//�򿪷��������
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL,
        SC_MANAGER_CREATE_SERVICE);

    if (NULL != hSCManager)
    {
		//��������
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
			if(GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)	//�Ѿ�����
			{
				//����ʧ��
				// Do Nothing
			}
			else
			{
				//�����Ѿ�����
				// Do Nothing
			}
		}
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        hDriver = CreateFileA(OpenName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hDriver == INVALID_HANDLE_VALUE)
		{
			//��ȡ�ļ����ʧ��
			// Do Nothing
		}
    }
    return hDriver;
}

///////////////////////////////////////////////////
//ж������
void UnloadDriver(IN HANDLE hDriver)
{
	//�ر��������
    CloseHandle(hDriver);

	//�򿪷��������
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL,
        SC_MANAGER_CREATE_SERVICE);

    if (NULL != hSCManager)
    {
		//�򿪷���
        SC_HANDLE hService = OpenService(hSCManager, EXE_DRIVER_NAME, DELETE | SERVICE_STOP);

        if (NULL != hService)
        {
            SERVICE_STATUS ss;
			//ֹͣ����
            ControlService(hService, SERVICE_CONTROL_STOP, &ss);
			//ɾ������
            DeleteService(hService);
			//�رշ�����
            CloseServiceHandle(hService);
        }
		//�رշ�����������
        CloseServiceHandle(hSCManager);
    }
}

///////////////////////////////////////////////////
//��ȡ�����Ľ�����Ϣ
BOOL GetProcInfo(IN HANDLE hDriver, OUT PCALLBACK_INFO pProcInfo)
{
	if(NULL == pProcInfo)
	{
		//���ָ��Ϊ��
		return FALSE;
	}

	//׼������DeviceIoControl��
	DWORD dwRet;
	BOOL bRet = DeviceIoControl(hDriver, IOCTL_GET_PROCESS_INFO, NULL, 0, \
		(LPVOID)pProcInfo, sizeof(CALLBACK_INFO), &dwRet, NULL);

	if(bRet)
	{
		//�ɹ�
		return TRUE;
	}
	
	return FALSE;
}

void MonitorThread()
{
	DWORD dwRet;
	DWORD threadID;
	
	//�����ر��¼�
	g_hShutdownEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//�����ȴ��¼�����
	HANDLE hHandleArray[2] = 
	{
		{g_hShutdownEvent}, 
		{g_hProcessEvent}
	};


	while(TRUE)
	{
		dwRet = ::WaitForMultipleObjects(
			sizeof(hHandleArray) / sizeof(hHandleArray[0]), //�ȴ�����
			&hHandleArray[0], //�¼�����
			FALSE, //ֻҪ���¼�����,�ͷ���
			INFINITE //һֱ��
			);
		
		if(hHandleArray[dwRet - WAIT_OBJECT_0] == g_hShutdownEvent)
		{
			//�ر��¼�����
			break;
		}
		else
		{
			//���̴�����ʾ�¼�
			
			//��ȡ�����Ľ�����Ϣ
			PCALLBACK_INFO pProcInfo = (PCALLBACK_INFO)malloc(sizeof(CALLBACK_INFO));
			if(GetProcInfo(g_hDriver, pProcInfo))
			{
				//�õ��´����Ľ�����Ϣ�ɹ�
				
				//�������߳�,���� lpfHandleCreateProcess ����
				::CreateThread(NULL, 0, \
					(LPTHREAD_START_ROUTINE)g_lpfHandleCreateProcess, \
					(PVOID)pProcInfo, \
					0, &threadID);
			}
		}
	}	// End whild
	
	//ж������
	UnloadDriver(g_hDriver);
	
	//�ر��¼�
	::CloseHandle(g_hShutdownEvent);
	::CloseHandle(g_hProcessEvent);
	
	//����
//	MessageBox(NULL, "ж�سɹ�!", "��ʾ", MB_OK);	//-----����

	//���ùر������ʾ�¼�
	::SetEvent(g_hShutdownOverEvent);
}

///////////////////////////////////////////////////
//�����¼���ʾ
BOOL CreateEventNotifyRing3(IN LPCTSTR lpFileName, IN DWORD lpfHandleCreateProcess)
{
	HANDLE m_hProcessEvent;
	DWORD threadID;

	//��������
	g_hDriver = LoadDriver(lpFileName);
	if(INVALID_HANDLE_VALUE == g_hDriver)
	{
		//����ʧ��
		MessageBox(NULL, "��������ʧ��!", "����", MB_OK);
		return FALSE;
	}

	//�������¼�
	m_hProcessEvent = ::OpenEvent(SYNCHRONIZE, FALSE, PROCESS_NOTIFY_NAME);

	if(m_hProcessEvent)
	{
		//������ʾ�¼��ɹ�

		g_hProcessEvent = m_hProcessEvent;
		g_lpfHandleCreateProcess = lpfHandleCreateProcess;

		//�������߳�
		::CreateThread(NULL, 0, \
			(LPTHREAD_START_ROUTINE)MonitorThread, \
			NULL, \
			0, &threadID);

	}	// End if
	else
	{
		MessageBox(NULL, "���̴����¼�����ʧ��!", "����", MB_OK);
		return FALSE;
	}	// End else

	return TRUE;
}

