/*
 *
 *                                   ����ע�뺯��
 *
 *                     ��Dll�ļ�ͨ��CreateRemoteThread����ע�뵽ָ������
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-20
 */

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include "..\\Common\\CodeInject.h"
#include "..\\Common\\NtSysOperate.h"		//�����ļ�����/ж��ͷ���ļ�
#include "..\\Common\\NtNotifyProcessCreate.h"	//�����ļ�ͷ

#define		KERNEL_MODULE_NAME		"kernel32"		//Windows�ں�dllģ����
													//LoadLibraryA�����ɴ�ģ�鵼��

#define		SYS_NAME		"NtNotifyProcessCreate.sys"		//�����ļ���


extern HANDLE g_hShutdownEvent;		//�����ر��¼����

HANDLE g_hShutdownOverEvent;

CRITICAL_SECTION cs;		//�ٽ��,�����ԭ�Ӳ���
char g_szDllFileName[MAX_PATH+1];

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//--------------------------------------------------------------
//����ע���������
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pPidModuleListHead g_pListHead = NULL;		//ȫ������ͷָ��
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//���̴������������߳�
static void CreateProcessPrompt(PCALLBACK_INFO pProcInfo)
{
	//����Ǵ�������,�򴫵�PID,���д���ע��
	if(pProcInfo->bCreate)
	{
		//��������
		DWORD dwDllModuleBase;
		if(!CodeInject((DWORD)(pProcInfo->hProcessId), g_szDllFileName, &dwDllModuleBase))
		{
			//ע��Dllʧ��
			//Do Nothing
			printf("ע�����Pid: %05d (������Pid: %05d)ʧ��!\n", \
				(DWORD)(pProcInfo->hProcessId), \
				(DWORD)(pProcInfo->hParentId));
		}
		else
		{
			//ע��Dll�ɹ�
			printf("ע�����Pid: %05d (������Pid: %05d)�ɹ�!!!\n", \
				(DWORD)(pProcInfo->hProcessId), \
				(DWORD)(pProcInfo->hParentId));
		}
	}
	else
	{
		//��������
		DWORD dwDllModuleBase;
		if((dwDllModuleBase = LookupDllModuleBaseByPid((DWORD)(pProcInfo->hProcessId))) != ~0)
		{
			if(!UnCodeInject((DWORD)(pProcInfo->hProcessId), dwDllModuleBase))
			{
				//ж��Dllʧ��
				//Do Nothing
				//
				// Execute Here
				//
				//-----------------------------����
				/*
				printf("ж�ؽ���Pid: %05d (������Pid: %05d)ʧ��!\n", \
					(DWORD)(pProcInfo->hProcessId), \
					(DWORD)(pProcInfo->hParentId));
					*/
			}
			else
			{
				//ж��Dll�ɹ�
				//
				// Never Execute
				//
				//-----------------------------����
				/*
				printf("ж�ؽ���Pid: %05d (������Pid: %05d)�ɹ�!!!\n", \
					(DWORD)(pProcInfo->hProcessId), \
					(DWORD)(pProcInfo->hParentId));
					*/
			}
		}
		else
		{
			//ͨ��Pid����Dllģ���ַʧ��
			//
			//---------------------------------����
			/*
			printf("ͨ��Pid����Dllģ���ַ( Pid: %05d )ʧ�� (������Pid: %05d)!\n", \
				(DWORD)(pProcInfo->hProcessId), \
				(DWORD)(pProcInfo->hParentId));
				*/
		}
	}

	//�ͷ��ڴ�
	free(pProcInfo);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//--------------------------------------------------------------
//�����в�����
//
//    �����еĽ����Pidֵ,�Ӵ�С����
BOOL InsertList(pPidModuleList pList)
{
	pPidModuleListHead pLH;
	pPidModuleList pL;

	//�����ٽ��
	EnterCriticalSection(&cs);

	//���������ͷ���,������һ��ͷ���
	if(!g_pListHead)
	{
		g_pListHead = (pPidModuleListHead)malloc(sizeof(PidModuleListHead));
		if(!g_pListHead)
		{
			//��������ͷʧ��
			MessageBox(NULL, "��������ͷʧ��,�������!", "���ش���", MB_OK);

			g_hShutdownOverEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	//�����ر������ʾ�¼�

			::SetEvent(g_hShutdownEvent);	//�����˳��¼�,ʹ����������ʾ����,���Զ�ж������
			
			//�ȴ�����ж�����
			::WaitForSingleObject(g_hShutdownOverEvent, INFINITE);

			LeaveCriticalSection(&cs);		//�뿪�ٽ��

			::TerminateProcess(NULL, -1);

			return FALSE;
		}

		//��ʼ������ͷ
		memset(g_pListHead, 0, sizeof(PidModuleListHead));

		g_pListHead->First = NULL;
		g_pListHead->Last = NULL;
		g_pListHead->ulCount = 0;
	}

	pLH = g_pListHead;

	if(pLH)
	{
		//��������ͷ
		if(pLH->First)
		{
			//���ǿ�����
			if(pLH->First->Pid < pList->Pid)
			{
				//����Ϊ��һ�����
				pList->Next = pLH->First;

				pLH->First = pList;

				pLH->ulCount++;

				LeaveCriticalSection(&cs);		//�뿪�ٽ��

				return TRUE;
			}

			pL = pLH->First;	//pLָ���һ�����

			do
			{
				if(pL->Next)		//��������һ�����
				{
					if(pL->Next->Pid < pList->Pid)
					{
						//�ҵ�Ҫ�����λ��
						pList->Next = pL->Next;
						pL->Next = pList;

						pLH->ulCount++;
						
						LeaveCriticalSection(&cs);		//�뿪�ٽ��

						return TRUE;
					}
				}
				else		//�������һ�������
				{
					//���뵽���һ�����
					pList->Next = NULL;
					pL->Next = pList;

					pLH->ulCount++;

					pLH->Last = pList;

					LeaveCriticalSection(&cs);		//�뿪�ٽ��

					return TRUE;
				}

			}while(pL = pL->Next);	//���ָ�����
		}
		else		//������û�н��
		{
			//ֱ�Ӽ��뵽pLH->First��pLH->Last
			pLH->First = pList;
			pLH->Last = pList;
			pLH->ulCount++;

			LeaveCriticalSection(&cs);		//�뿪�ٽ��

			return TRUE;
		}
	}

	LeaveCriticalSection(&cs);		//�뿪�ٽ��

	return FALSE;
}

//--------------------------------------------------------------
//�Ƴ������е�ָ�����
BOOL RemoveList(DWORD Pid)
{
	pPidModuleListHead pLH;
	pPidModuleList pL;

	//�����ٽ��
	EnterCriticalSection(&cs);

	pLH = g_pListHead;

	if(pLH)
	{
		//��������ͷ
		if(pLH->First)
		{
			//���ǿ�����
			if(pLH->First->Pid == Pid)
			{
				//��һ�����ΪҪɾ���Ľ��
				if(pLH->First->Next)		//����ɾ�����һ�����
				{
					pPidModuleList pTemp;
					
					pTemp = pLH->First;		//ָ���һ�����
					
					pLH->First = pLH->First->Next;		//ͷ����е�Nextָ����
					
					free(pTemp);			//�ͷ�ɾ���Ľ���ڴ�ռ�
				}
				else			//ɾ���������һ�����
				{
					free(pLH->First);
					pLH->First = NULL;		//ͷ����Firstָ����ΪNULL

					pLH->Last = NULL;
				}

				pLH->ulCount--;

				LeaveCriticalSection(&cs);		//�뿪�ٽ��

				return TRUE;
			}

			pL = pLH->First;	//pLָ���һ�����

			do
			{
				if(pL->Next->Pid == Pid)
				{
					//�ҵ�Ҫɾ���Ľ��
					if(pL->Next->Next != NULL)
					{
						//Ҫɾ���Ľ�㲻�����һ�����
						pPidModuleList pTemp;

						pTemp = pL->Next;		//����Ҫɾ���Ľ��λ��

						pL->Next = pTemp->Next;	//������ȥ�����

						free(pTemp);
					}
					else
					{
						//Ҫɾ���Ľ�������һ�����
						free(pL->Next);
						pL->Next = NULL;	//pL��Ϊ�����һ�����,����ָ��NULL

						pLH->Last = pL;
					}

					pLH->ulCount--;

					LeaveCriticalSection(&cs);		//�뿪�ٽ��

					return TRUE;
				}

			}while((pL = pL->Next) && (pL->Next));	//���ָ�����,��������һ�����
		}
	}

	LeaveCriticalSection(&cs);		//�뿪�ٽ��

	return FALSE;
}

//--------------------------------------------------------------
//���������е���Ϣ
void PrintList()
{
	pPidModuleListHead pLH;
	pPidModuleList pL;
	ULONG i = 0;

	//�����ٽ��
	EnterCriticalSection(&cs);

	pLH = g_pListHead;

	if(pLH)
	{
		//��������ͷ
		if(pLH->First)
		{
			//���ǿ�����
			pL = pLH->First;	//pLָ���һ�����

			//��ӡ������Ϣ
			printf("--------------  ������Ϣ  -------------\n");
			printf("�����й��н�� %d ��\n\n", pLH->ulCount);

			do
			{
				printf("%04d PID: %05d ModuleBase: %08X\n", ++i, pL->Pid, pL->dwDllModuleBase);

			}while(pL = pL->Next);	//���ָ�����

			printf("------------  ������Ϣ����  -----------\n");
		}
	}

	LeaveCriticalSection(&cs);		//�뿪�ٽ��
}

//--------------------------------------------------------------
//ͨ��Pid,����Dllģ���ַ
DWORD LookupDllModuleBaseByPid(DWORD Pid)
{
	pPidModuleListHead pLH;
	pPidModuleList pL;
	ULONG i = 0;

	//�����ٽ��
	EnterCriticalSection(&cs);

	pLH = g_pListHead;

	if(pLH)
	{
		//��������ͷ
		if(pLH->First)
		{
			//���ǿ�����
			pL = pLH->First;	//pLָ���һ�����

			do
			{
				if(pL->Pid == Pid)	//���ҵ�Ҫ�ҵ�Dllģ���ַ
				{
					LeaveCriticalSection(&cs);		//�뿪�ٽ��

					return pL->dwDllModuleBase;
				}

			}while(pL = pL->Next);	//���ָ�����

		}
	}

	LeaveCriticalSection(&cs);		//�뿪�ٽ��

	return ~0;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//--------------------------------------------------------------
//�޸Ľ���������Ȩ��
//����������Ȩ����DEBUGģʽ,��ʹ���ַ������� "SeDebugPrivilege"
BOOL EnableDebugPriv(LPCTSTR szPrivilege)
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, szPrivilege, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	
	return TRUE;
}

//--------------------------------------------------------------
//����ע��
BOOL CodeInject(IN DWORD Pid, IN char *szDllFileName, OUT LPDWORD hDllModuleBase)
{
	HANDLE hThread;						//Զ�߳�
	char szDllPathName[MAX_PATH + 1];	//Dllȫ·����
	char *pDllRemote;					//Զ�̵߳�ַ,ָ����Dllȫ·�����ĵ�ַ
	HANDLE hProcess;					//�򿪽��̾��,��Ŀ�����
	DWORD dwRet;


	memset(szDllPathName, 0, sizeof(szDllPathName));

	//�õ�Dll��ȫ·����
	if(!::GetFullPathName(szDllFileName, sizeof(szDllPathName) - 1, szDllPathName, NULL))
	{
		//����
		return FALSE;
	}
	if(::GetFileAttributes(szDllPathName) == -1)
	{
		//�ļ�Dll�ļ�������
		return FALSE;
	}

	//����������Ȩ��
	if(!EnableDebugPriv("SeDebugPrivilege"))
	{
		//������Ȩ��ʧ��
		//Do Nothing
	}


	//��Ŀ�����
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if(!hProcess)
	{
		//�򿪽���ʧ��
		return FALSE;
	}

	pDllRemote = (char *)::VirtualAllocEx(hProcess, NULL, sizeof(szDllPathName), \
		MEM_COMMIT, PAGE_READWRITE);
	if(!pDllRemote)
	{
		//����ʧ��

		//�رս��̾��
		::CloseHandle(hProcess);

		return FALSE;
	}

	if(!::WriteProcessMemory(hProcess, pDllRemote, (void*)szDllPathName, \
		sizeof(szDllPathName), NULL))
	{
		//д��Dll·������

		//�ͷŷ�����ڴ�
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//�رս��̾��
		::CloseHandle(hProcess);

		return FALSE;
	}

	hThread = ::CreateRemoteThread(hProcess, NULL, 0, \
		(LPTHREAD_START_ROUTINE)::GetProcAddress( \
		::GetModuleHandle(KERNEL_MODULE_NAME), \
		"LoadLibraryA"), pDllRemote, 0, NULL);

	//�ȴ�2��
	if(::WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
	{
		//��ʱû�з���

		//�ͷŷ�����ڴ�
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//�رս��̾��
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	if(!hThread)
	{
		//����Զ�߳�ʧ��

		//�ͷŷ�����ڴ�
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//�رս��̾��
		::CloseHandle(hProcess);

		return FALSE;
	}

	//�õ�����ģ��Ļ�ַ
	while((dwRet = ::GetExitCodeThread(hThread, hDllModuleBase)) == STILL_ACTIVE)
	{
		Sleep(1);
	}
	if(!dwRet || !(LPDWORD)(*hDllModuleBase))
	{
		//�õ����ػ�ַʧ��
		//Do Nothing
		printf("Pid: %05d LoadLibraryA Failed!\n", Pid);
		
		//�ͷŷ�����ڴ�
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//�ر�Զ�߳̾��
		::CloseHandle(hThread);
		//�رս��̾��
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	
	//�ͷŷ�����ڴ�
	::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
	//�ر�Զ�߳̾��
	::CloseHandle(hThread);
	//�رս��̾��
	::CloseHandle(hProcess);


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//�����������м�����Ӧ���
	//����һ���½��
	pPidModuleList pList = (pPidModuleList)malloc(sizeof(PidModuleList));
	
	memset(pList, 0, sizeof(PidModuleList));

	pList->Next = NULL;
	pList->Pid = Pid;
	pList->dwDllModuleBase = (DWORD)(*hDllModuleBase);

	if(!InsertList(pList))
	{
		//�����������
		free(pList);
	}
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	return TRUE;
}

//--------------------------------------------------------------
//ж��ע��Ĵ���ģ��
BOOL UnCodeInject(IN DWORD Pid, IN DWORD hDllModuleBase)
{
	//Զ�߳̾��
	HANDLE hThread;
	//ж��Ŀ����̾��
	HANDLE hProcess;
	DWORD dwRet;
	DWORD dwFreeLibRet;


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//�����ͷ������е���Ӧ���
	if(!RemoveList(Pid))
	{
		//���ɾ������
		//Do Nothing
	}
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	//��Ŀ�����
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if(!hProcess)
	{
		//�򿪽���ʧ��
		return FALSE;
	}

	hThread = ::CreateRemoteThread(hProcess, NULL, 0, \
		(LPTHREAD_START_ROUTINE)::GetProcAddress( \
		::GetModuleHandle(KERNEL_MODULE_NAME), \
		"FreeLibrary"), (void*)hDllModuleBase, 0, NULL);

	//�ȴ�2��
	if(::WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
	{
		//��ʱû�з���
		
		//�رս��̾��
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	if(!hThread)
	{
		//����Զ�߳�ʧ��

		//�رս��̾��
		::CloseHandle(hProcess);

		return FALSE;
	}

	//�õ��ͷſⷵ�ص�ֵ
	while((dwRet = ::GetExitCodeThread(hThread, &dwFreeLibRet)) == STILL_ACTIVE)
	{
		Sleep(1);
	}
	if(!dwRet || !dwFreeLibRet)
	{
		//�ͷſ�ʧ��
		//Do Nothing
		printf("FreeLibrary Failed!\n");

		//�ر�Զ�߳̾��
		::CloseHandle(hThread);
		//�رս��̾��
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	//�ر�Զ�߳̾��
	::CloseHandle(hThread);
	//�رս��̾��
	::CloseHandle(hProcess);

	return TRUE;
}

//--------------------------------------------------------------
//������ע�����н���
BOOL CodeInjectAll(IN char *szDllFileName)
{
	//�������н���,����ע��
	HANDLE hProcessSnap = NULL;
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe32 = {0};
	DWORD dwCurrentId;
	DWORD dwDllModuleBase;


	//��ʼ���ٽ��
	InitializeCriticalSection(&cs);

	//����Dll�ļ���Ϊȫ�ֱ���
	memset(g_szDllFileName, 0, sizeof(g_szDllFileName));
	strcpy(g_szDllFileName, szDllFileName);

	//�õ��������PID
	dwCurrentId = ::GetCurrentProcessId();

	//��ϵͳ�е��������н��̽��п���
	hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//��ȡ����ʧ��
		return FALSE;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);;

	//��������
	if(::Process32First(hProcessSnap, &pe32))
	{
		//�õ���һ���ɹ�
		do
		{
			if(dwCurrentId != pe32.th32ProcessID)	//ö�ٳ������������ȫ�����н���
			{
				//����Եõ��Ľ�����Ϣ���в���

				//����Dllע��
				if(!CodeInject(pe32.th32ProcessID, szDllFileName, &dwDllModuleBase))
				{
					//ע��ʧ��
				}
			}

		}while(::Process32Next(hProcessSnap, &pe32));

		//�رտ��վ��
		::CloseHandle(hProcessSnap);
	}

	//------------------------------------------------------------------
	//�����������̴����������ӽ���
	char SysPathName[MAX_PATH+1];

	memset(SysPathName, 0, sizeof(SysPathName));

	//�õ���ǰ��������ȫ·����
	if(::GetCurrentDirectory(sizeof(SysPathName) - 1, SysPathName))
	{
		//�õ���ǰĿ¼�ɹ�
		strcat(SysPathName, "\\");
		strcat(SysPathName, SYS_NAME);

		//----------����
		printf("����ȫ·����: %s\n", SysPathName);///////////////����

		//��ʼ����������ʾ
		if(!CreateEventNotifyRing3(SysPathName, (DWORD)CreateProcessPrompt))
		{
			MessageBox(NULL, "���̼����̰߳�װʧ��!", "����", MB_OK);
			return FALSE;
		}
	}

	return TRUE;
}

//--------------------------------------------------------------
//ж������ע��Ľ���ģ��
BOOL UnCodeInjectAll()
{
	//--------------------------------------------
	//ж���������¼�
	g_hShutdownOverEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	//�����ر������ʾ�¼�

	::SetEvent(g_hShutdownEvent);	//�����˳��¼�,ʹ����������ʾ����,���Զ�ж������

	//�ȴ�����ж�����
	::WaitForSingleObject(g_hShutdownOverEvent, INFINITE);

	//�ر��¼����
	::CloseHandle(g_hShutdownOverEvent);

	//---------------------------------------------------
	//ע��ģ��ж��
	//��������ȡ������ע���ģ��,����ж��
	pPidModuleListHead pLH;

	pLH = g_pListHead;		//ָ��ͷ���

	if(pLH)
	{
		
		while(pLH->First)	//�������һ�����
		{
			if(!UnCodeInject(pLH->First->Pid, pLH->First->dwDllModuleBase))
			{
				//ж�ش˽��ʧ��
				//Do Nothing
			}
		}
		
		free(g_pListHead);		//�ͷ�ͷ���
	}

	//ɾ���ٽ��
	DeleteCriticalSection(&cs);

	return TRUE;
}