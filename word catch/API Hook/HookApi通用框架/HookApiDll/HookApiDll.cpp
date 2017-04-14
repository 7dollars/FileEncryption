/*
 *
 *                         Hook Api Dll �ļ�
 *
 *                      ʵ�ֽ��̵�Api Hook����
 *
 *                          Made By Adly
 *
 *                       Email: Adly369046978@163.com
 *
 *                           2008-7-22
 *
 */

#include <stdio.h>
#include <windows.h>
#include "HookApiDll.h"


HANDLE hHookThread = NULL;
DWORD dwThreadId = 0;

//---------------------------------------
//��������
//
//---------------------------------------
//ж����Hook��Api
void UnHookApi();
//�õ���Ҫ��̬��õ�Api��ַ
BOOL LookupApiAddress();
//---------------------------------------
//�����ⲿ��������
//---------------------------------------
//HOOK API ��ں���
extern void HookApi();


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//DLL������

BOOL WINAPI DllMain(
			 HINSTANCE hinstDLL,	//Dllģ����
			 DWORD fdwReason,		//����ԭ��
			 LPVOID lpvReserved		//����
			 )
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:		//���̼���Dll
		//------------------------------------------------------------------
		//���� CreateThread �������߳������� API HOOK
		//
		//  ע��:
		//      �����õ���API(����kernel32���е�API)��Ҫ�ö�̬��ȡ����
		//      ����GetModuleHandle,���ʧ��,����LoadLibraryA������Ӧ��dll�ļ�
		//      Ȼ������GetProcAddress�õ�API��ַ
		

		//�õ���Ҫ��̬��õ�Api��ַ
		if(!GetApiAddress())
		{
			//��ȡApi��ַʧ��
			return FALSE;
		}


		hHookThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookApi, NULL, 0, &dwThreadId);
		break;
	case DLL_THREAD_ATTACH:			//�̼߳���Dll

		break;
	case DLL_THREAD_DETACH:			//�߳�ж��Dll
		
		break;
	case DLL_PROCESS_DETACH:		//����ж��Dll
		//------------------------------------------------------------------
		//���� TerminateThread �����´������߳�
		//��ж����HOOK��API
		//
		//  ע��:
		//      �����õ���API(����kernel32���е�API)��Ҫ�ö�̬��ȡ����
		//      ����GetModuleHandle,���ʧ��,����LoadLibraryA������Ӧ��dll�ļ�
		//      Ȼ������GetProcAddress�õ�API��ַ

		::TerminateThread(hHookThread, 0);		//��ֹHOOK�߳�
		UnHookApi();	//ж����HOOK��API
		break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//////////  ע��:                                                        ////////////
//////////      �����õ���API(��Kernel32)��Ҫ�ö�̬��ȡ����              ////////////
//////////      ����GetModuleHandle,���ʧ��,����LoadLibraryA������Ӧ��dll�ļ� //////
//////////      Ȼ������GetProcAddress�õ�API��ַ                        ////////////
/////////////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------
//ͨ��HOOK API�ӿڵĹ���
//
//    ������HOOK�˵�API��Ϣ�����һ������
//


//--------------------------------------------------------------------
// ���������������������
//
// Add Here ...
pHookApiInfo pHookApiInfoHead = NULL;		//����ͷָ��

//�������ӵ�����
BOOL InsertHookApiInfoList(pHookApiInfo pHookApiInfoNode)
{

	if(!pHookApiInfoNode)
	{
		//Ҫ����Ľ��Ϊ��
		return FALSE;
	}

	if(pHookApiInfoHead)		//��Ϊ����
	{
		//���¼���Ľ����Ϊ����ͷ

		pHookApiInfoNode->Next = pHookApiInfoHead;	//����ͷ�ŵ��¼������Next��
		pHookApiInfoHead = pHookApiInfoNode;		//���½�����ͷָ��ռ���Ľ��
	}
	else		//����ͷΪ��
	{
		//��ͷָ��ָ���¼���Ľ��

		pHookApiInfoNode->Next = NULL;
		pHookApiInfoHead = pHookApiInfoNode;
	}

	return TRUE;
}

//�Ƴ�������Hook�ɹ���Api
BOOL RemoveAllHookApi()
{
	pHookApiInfo pHAIH;

	pHAIH = pHookApiInfoHead;		//����ͷ

	while(pHAIH)	//������л����ڽ��
	{

		//ж��Hook�˵�Api
		HookOnOrOff(pHAIH->pfApiAddress, pHAIH->OldApiMachineCode, 5);

		pHookApiInfo pTemp;

		//pTempָ��ͷ���
		pTemp = pHAIH;

		//ͷ������
		pHAIH = pHAIH->Next;

		//�ͷ���ж���˵Ľ��
		free(pTemp);
	}

	return TRUE;
}

//ͨ��Api���õ����ڽ��
pHookApiInfo GetNodeByApi(char *szApiName)
{
	pHookApiInfo pHAIH;

	pHAIH = pHookApiInfoHead;		//����ͷ

	while(pHAIH)	//������л����ڽ��
	{

		if(!strcmp(pHAIH->szApiName, szApiName))
		{
			//�ҵ��ý��

			return pHAIH;
		}

		pHAIH = pHAIH->Next;		//ָ����һ�����
	}

	return NULL;
}

//*********************************************************************
//HOOK/UNHOOK API����Ҫ�ĺ���
//
//  IN
//      lpfFunctionAddress          - ҪHOOK��API��ַ
//      ucFunctionJmpCode           - Ҫ�޸�Ϊ����ת������
//      dwModifyLen                 - Ҫ�޸ĵ��ֽڳ���
//
//  OUT
//      BOOL                        - �ɹ�����TRUE,���򷵻�FALSE
//
BOOL HookOnOrOff(LPVOID lpfFunctionAddress, UCHAR *ucFunctionJmpCode, DWORD dwModifyLen)
{
	HANDLE hProc;
	DWORD dwCurrentPid;
	DWORD dwOldProtect;

	//�õ���ǰ����PID
	dwCurrentPid = ::GetCurrentProcessId();
	
	//�򿪵�ǰ����
	hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwCurrentPid);
	if(!hProc)
	{
		//OpenProcessʧ��
		return FALSE;
	}

	//�޸Ľ�ҪHOOK��API��ǰdwModifyLen���ֽڵ�����Ϊ�ɶ�д
	::VirtualProtectEx(hProc, lpfFunctionAddress, dwModifyLen, \
		PAGE_READWRITE, &dwOldProtect);

	//��ҪHOOK��API��ǰdwModifyLen���ֽ��滻Ϊ��תָ�������
	::WriteProcessMemory(hProc, lpfFunctionAddress, \
		ucFunctionJmpCode, dwModifyLen, NULL);

	//��ԭԭ����
	::VirtualProtectEx(hProc, lpfFunctionAddress, dwModifyLen, \
		dwOldProtect, &dwOldProtect);

	//�رմ򿪵Ľ��̾��
	::CloseHandle(hProc);

	return TRUE;
}

//*********************************************************************

//��ʼ��Hook����
BOOL InitHook(char *szLibModuleName/*����*/, \
			  char *szLibDllModuleName/*���Dll��*/, \
			  char *szApiName/*Api��*/, \
			  DWORD NewFunctionAddress/*�滻������ַ*/ \
			  )
{
	HMODULE hMod = NULL;
	FARPROC fpApiAddress = NULL;
	UCHAR OldApiMachineCode[5];		//ԭʼApi��ǰ5���ֽڻ�����
	UCHAR NewJmpMachineCode[5];		//�¹����JMP������(5�ֽ�)


	//�жϸý�������ǰ��û�м��ظ�API��
	hMod = ::GetModuleHandle(szLibModuleName);
	if(!hMod)
	{
		//��ǰû�м��ع�
		hMod = ::LoadLibrary(szLibDllModuleName);
		if(!hMod)
		{
			//���ؿ�ʧ��
			return FALSE;
		}
	}

	fpApiAddress = ::GetProcAddress(hMod, szApiName);
	if(!fpApiAddress)
	{
		//�õ� MessageBoxA ��ַʧ��
		return FALSE;
	}

	//�滻ԭAPI��ǰ5���ֽ�Ϊ��תָ��
	//ʹ����ִ���º���
	__asm
	{
		//; ����ԭAPIǰ5�ֽ�
		lea edi, OldApiMachineCode ;
		mov esi, fpApiAddress ;
		cld ;
		movsd ;
		movsb ;

		//; ������ת������
		//; NewMessageBoxACode[0] = 0xE9;	// JMP ָ�������
		mov byte ptr [NewJmpMachineCode], 0E9h ;

		//������תָ���е���Ե�ַ
		mov eax, NewFunctionAddress ;
		mov ebx, fpApiAddress ;
		sub eax, ebx ;
		sub eax, 5 ;
		mov dword ptr [NewJmpMachineCode+1], eax ;
	}

	//----------------------------------------
	//���ﲻ���ͷŸÿ�
	//  1.�����API���ڴ�ǰ�Ѽ���, GetModuleHandle �ͻ�ɹ�
	//  2.�����API���ڴ�ǰû����, ����ʱ���� LoadLibrary ������
	//      ��ʱ�����ͷ�,����,�������ʹ�ö�̬���ظ�API��
	//      ��˶���Ҳ����Hook
	//
	//	  ::FreeLibrary(hMod);

	//��ʼHook
	if(!HookOnOrOff(fpApiAddress, NewJmpMachineCode, 5))
	{
		//Hook ʧ��
		// Do Something here
		//

		return FALSE;
	}

	//------------------------------------
	//���ｫHook���Api��Ϣ��ӵ�������
	//
	// Add Here ...

	//�����½��ռ�
	pHookApiInfo pNewNode = (pHookApiInfo)malloc(sizeof(HookApiInfo));
	if(!pNewNode)
	{
		//�����ڴ�ʧ��
		return FALSE;
	}

	memset(pNewNode, 0, sizeof(HookApiInfo));

	//�������Ϣ
	strcpy(pNewNode->szLibModuleName, szLibModuleName);
	pNewNode->szLibModuleName[sizeof(pNewNode->szLibModuleName)-1] = '\0';
	strcpy(pNewNode->szLibDllModuleName, szLibDllModuleName);
	pNewNode->szLibDllModuleName[sizeof(pNewNode->szLibDllModuleName)-1] = '\0';
	strcpy(pNewNode->szApiName, szApiName);
	pNewNode->szApiName[sizeof(pNewNode->szApiName)-1] = '\0';
	pNewNode->Next = NULL;
	memcpy(pNewNode->NewJmpMachineCode, NewJmpMachineCode, 5);
	memcpy(pNewNode->OldApiMachineCode, OldApiMachineCode, 5);
	pNewNode->pfApiAddress = fpApiAddress;

	//������������
	if(!InsertHookApiInfoList(pNewNode))
	{
		//��������ʧ��
		return FALSE;
	}

	return TRUE;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//UNHOOK API ��ں���
void UnHookApi()
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//����ж����HOOK��API

	//ж��������Hook��Api
	RemoveAllHookApi();

}