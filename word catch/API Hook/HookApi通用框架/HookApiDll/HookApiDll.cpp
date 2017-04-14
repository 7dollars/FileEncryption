/*
 *
 *                         Hook Api Dll 文件
 *
 *                      实现进程的Api Hook操作
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
//函数声明
//
//---------------------------------------
//卸载已Hook的Api
void UnHookApi();
//得到需要动态获得的Api地址
BOOL LookupApiAddress();
//---------------------------------------
//引用外部函数声明
//---------------------------------------
//HOOK API 入口函数
extern void HookApi();


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//DLL主函数

BOOL WINAPI DllMain(
			 HINSTANCE hinstDLL,	//Dll模块句柄
			 DWORD fdwReason,		//调用原因
			 LPVOID lpvReserved		//保留
			 )
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:		//进程加载Dll
		//------------------------------------------------------------------
		//利用 CreateThread 创建新线程来进行 API HOOK
		//
		//  注意:
		//      所有用到的API(除了kernel32库中的API)都要用动态获取技术
		//      先用GetModuleHandle,如果失败,再用LoadLibraryA加载相应的dll文件
		//      然后利用GetProcAddress得到API地址
		

		//得到需要动态获得的Api地址
		if(!GetApiAddress())
		{
			//获取Api地址失败
			return FALSE;
		}


		hHookThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HookApi, NULL, 0, &dwThreadId);
		break;
	case DLL_THREAD_ATTACH:			//线程加载Dll

		break;
	case DLL_THREAD_DETACH:			//线程卸载Dll
		
		break;
	case DLL_PROCESS_DETACH:		//进程卸载Dll
		//------------------------------------------------------------------
		//利用 TerminateThread 结束新创建的线程
		//再卸载已HOOK的API
		//
		//  注意:
		//      所有用到的API(除了kernel32库中的API)都要用动态获取技术
		//      先用GetModuleHandle,如果失败,再用LoadLibraryA加载相应的dll文件
		//      然后利用GetProcAddress得到API地址

		::TerminateThread(hHookThread, 0);		//终止HOOK线程
		UnHookApi();	//卸载已HOOK的API
		break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//////////  注意:                                                        ////////////
//////////      所有用到的API(除Kernel32)都要用动态获取技术              ////////////
//////////      先用GetModuleHandle,如果失败,再用LoadLibraryA加载相应的dll文件 //////
//////////      然后利用GetProcAddress得到API地址                        ////////////
/////////////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------
//通用HOOK API接口的构造
//
//    将所有HOOK了的API信息构造成一个链表
//


//--------------------------------------------------------------------
// 这里添加链表管理操作函数
//
// Add Here ...
pHookApiInfo pHookApiInfoHead = NULL;		//链表头指针

//将结点添加到链表
BOOL InsertHookApiInfoList(pHookApiInfo pHookApiInfoNode)
{

	if(!pHookApiInfoNode)
	{
		//要插入的结点为空
		return FALSE;
	}

	if(pHookApiInfoHead)		//不为空链
	{
		//将新加入的结点作为链表头

		pHookApiInfoNode->Next = pHookApiInfoHead;	//链表头放到新加入结点的Next处
		pHookApiInfoHead = pHookApiInfoNode;		//重新将链表头指向刚加入的结点
	}
	else		//链表头为空
	{
		//将头指针指向新加入的结点

		pHookApiInfoNode->Next = NULL;
		pHookApiInfoHead = pHookApiInfoNode;
	}

	return TRUE;
}

//移除所有已Hook成功的Api
BOOL RemoveAllHookApi()
{
	pHookApiInfo pHAIH;

	pHAIH = pHookApiInfoHead;		//链表头

	while(pHAIH)	//如果链中还存在结点
	{

		//卸载Hook了的Api
		HookOnOrOff(pHAIH->pfApiAddress, pHAIH->OldApiMachineCode, 5);

		pHookApiInfo pTemp;

		//pTemp指向头结点
		pTemp = pHAIH;

		//头结点后移
		pHAIH = pHAIH->Next;

		//释放已卸载了的结点
		free(pTemp);
	}

	return TRUE;
}

//通过Api名得到所在结点
pHookApiInfo GetNodeByApi(char *szApiName)
{
	pHookApiInfo pHAIH;

	pHAIH = pHookApiInfoHead;		//链表头

	while(pHAIH)	//如果链中还存在结点
	{

		if(!strcmp(pHAIH->szApiName, szApiName))
		{
			//找到该结点

			return pHAIH;
		}

		pHAIH = pHAIH->Next;		//指向下一个结点
	}

	return NULL;
}

//*********************************************************************
//HOOK/UNHOOK API所需要的函数
//
//  IN
//      lpfFunctionAddress          - 要HOOK的API地址
//      ucFunctionJmpCode           - 要修改为的跳转机器码
//      dwModifyLen                 - 要修改的字节长度
//
//  OUT
//      BOOL                        - 成功返回TRUE,否则返回FALSE
//
BOOL HookOnOrOff(LPVOID lpfFunctionAddress, UCHAR *ucFunctionJmpCode, DWORD dwModifyLen)
{
	HANDLE hProc;
	DWORD dwCurrentPid;
	DWORD dwOldProtect;

	//得到当前进程PID
	dwCurrentPid = ::GetCurrentProcessId();
	
	//打开当前进程
	hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwCurrentPid);
	if(!hProc)
	{
		//OpenProcess失败
		return FALSE;
	}

	//修改将要HOOK的API的前dwModifyLen个字节的属性为可读写
	::VirtualProtectEx(hProc, lpfFunctionAddress, dwModifyLen, \
		PAGE_READWRITE, &dwOldProtect);

	//将要HOOK的API的前dwModifyLen个字节替换为跳转指令机器码
	::WriteProcessMemory(hProc, lpfFunctionAddress, \
		ucFunctionJmpCode, dwModifyLen, NULL);

	//还原原属性
	::VirtualProtectEx(hProc, lpfFunctionAddress, dwModifyLen, \
		dwOldProtect, &dwOldProtect);

	//关闭打开的进程句柄
	::CloseHandle(hProc);

	return TRUE;
}

//*********************************************************************

//初始化Hook操作
BOOL InitHook(char *szLibModuleName/*库名*/, \
			  char *szLibDllModuleName/*库的Dll名*/, \
			  char *szApiName/*Api名*/, \
			  DWORD NewFunctionAddress/*替换函数地址*/ \
			  )
{
	HMODULE hMod = NULL;
	FARPROC fpApiAddress = NULL;
	UCHAR OldApiMachineCode[5];		//原始Api的前5个字节机器码
	UCHAR NewJmpMachineCode[5];		//新构造的JMP机器码(5字节)


	//判断该进程内以前有没有加载该API库
	hMod = ::GetModuleHandle(szLibModuleName);
	if(!hMod)
	{
		//以前没有加载过
		hMod = ::LoadLibrary(szLibDllModuleName);
		if(!hMod)
		{
			//加载库失败
			return FALSE;
		}
	}

	fpApiAddress = ::GetProcAddress(hMod, szApiName);
	if(!fpApiAddress)
	{
		//得到 MessageBoxA 地址失败
		return FALSE;
	}

	//替换原API的前5个字节为跳转指令
	//使其先执行新函数
	__asm
	{
		//; 保存原API前5字节
		lea edi, OldApiMachineCode ;
		mov esi, fpApiAddress ;
		cld ;
		movsd ;
		movsb ;

		//; 构造跳转机器码
		//; NewMessageBoxACode[0] = 0xE9;	// JMP 指令机器码
		mov byte ptr [NewJmpMachineCode], 0E9h ;

		//构造跳转指令中的相对地址
		mov eax, NewFunctionAddress ;
		mov ebx, fpApiAddress ;
		sub eax, ebx ;
		sub eax, 5 ;
		mov dword ptr [NewJmpMachineCode+1], eax ;
	}

	//----------------------------------------
	//这里不用释放该库
	//  1.如果此API库在此前已加载, GetModuleHandle 就会成功
	//  2.如果此API库在此前没加载, 而此时调用 LoadLibrary 加载它
	//      此时不用释放,这样,如果后面使用动态加载该API库
	//      则此动作也将被Hook
	//
	//	  ::FreeLibrary(hMod);

	//开始Hook
	if(!HookOnOrOff(fpApiAddress, NewJmpMachineCode, 5))
	{
		//Hook 失败
		// Do Something here
		//

		return FALSE;
	}

	//------------------------------------
	//这里将Hook后的Api信息添加到链表中
	//
	// Add Here ...

	//分配新结点空间
	pHookApiInfo pNewNode = (pHookApiInfo)malloc(sizeof(HookApiInfo));
	if(!pNewNode)
	{
		//分配内存失败
		return FALSE;
	}

	memset(pNewNode, 0, sizeof(HookApiInfo));

	//填充结点信息
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

	//将结点加入链表
	if(!InsertHookApiInfoList(pNewNode))
	{
		//加入链表失败
		return FALSE;
	}

	return TRUE;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//UNHOOK API 入口函数
void UnHookApi()
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//这里卸载已HOOK的API

	//卸载所有已Hook的Api
	RemoveAllHookApi();

}