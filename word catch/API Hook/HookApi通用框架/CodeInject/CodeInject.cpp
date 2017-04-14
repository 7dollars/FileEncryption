/*
 *
 *                                   代码注入函数
 *
 *                     将Dll文件通过CreateRemoteThread函数注入到指定进程
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
#include "..\\Common\\NtSysOperate.h"		//驱动文件加载/卸载头等文件
#include "..\\Common\\NtNotifyProcessCreate.h"	//驱动文件头

#define		KERNEL_MODULE_NAME		"kernel32"		//Windows内核dll模块名
													//LoadLibraryA函数由此模块导出

#define		SYS_NAME		"NtNotifyProcessCreate.sys"		//驱动文件名


extern HANDLE g_hShutdownEvent;		//导出关闭事件句柄

HANDLE g_hShutdownOverEvent;

CRITICAL_SECTION cs;		//临界段,链表成原子操作
char g_szDllFileName[MAX_PATH+1];

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//--------------------------------------------------------------
//管理注入链表操作
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pPidModuleListHead g_pListHead = NULL;		//全局链表头指针
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//进程创建撤消监视线程
static void CreateProcessPrompt(PCALLBACK_INFO pProcInfo)
{
	//如果是创建进程,则传递PID,进行代码注入
	if(pProcInfo->bCreate)
	{
		//创建进程
		DWORD dwDllModuleBase;
		if(!CodeInject((DWORD)(pProcInfo->hProcessId), g_szDllFileName, &dwDllModuleBase))
		{
			//注入Dll失败
			//Do Nothing
			printf("注入进程Pid: %05d (父进程Pid: %05d)失败!\n", \
				(DWORD)(pProcInfo->hProcessId), \
				(DWORD)(pProcInfo->hParentId));
		}
		else
		{
			//注入Dll成功
			printf("注入进程Pid: %05d (父进程Pid: %05d)成功!!!\n", \
				(DWORD)(pProcInfo->hProcessId), \
				(DWORD)(pProcInfo->hParentId));
		}
	}
	else
	{
		//撤消进程
		DWORD dwDllModuleBase;
		if((dwDllModuleBase = LookupDllModuleBaseByPid((DWORD)(pProcInfo->hProcessId))) != ~0)
		{
			if(!UnCodeInject((DWORD)(pProcInfo->hProcessId), dwDllModuleBase))
			{
				//卸载Dll失败
				//Do Nothing
				//
				// Execute Here
				//
				//-----------------------------测试
				/*
				printf("卸载进程Pid: %05d (父进程Pid: %05d)失败!\n", \
					(DWORD)(pProcInfo->hProcessId), \
					(DWORD)(pProcInfo->hParentId));
					*/
			}
			else
			{
				//卸载Dll成功
				//
				// Never Execute
				//
				//-----------------------------测试
				/*
				printf("卸载进程Pid: %05d (父进程Pid: %05d)成功!!!\n", \
					(DWORD)(pProcInfo->hProcessId), \
					(DWORD)(pProcInfo->hParentId));
					*/
			}
		}
		else
		{
			//通过Pid查找Dll模块基址失败
			//
			//---------------------------------测试
			/*
			printf("通过Pid查找Dll模块基址( Pid: %05d )失败 (父进程Pid: %05d)!\n", \
				(DWORD)(pProcInfo->hProcessId), \
				(DWORD)(pProcInfo->hParentId));
				*/
		}
	}

	//释放内存
	free(pProcInfo);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//--------------------------------------------------------------
//向链中插入结点
//
//    链表中的结点以Pid值,从大到小排列
BOOL InsertList(pPidModuleList pList)
{
	pPidModuleListHead pLH;
	pPidModuleList pL;

	//进入临界段
	EnterCriticalSection(&cs);

	//如果不存在头结点,则生成一个头结点
	if(!g_pListHead)
	{
		g_pListHead = (pPidModuleListHead)malloc(sizeof(PidModuleListHead));
		if(!g_pListHead)
		{
			//生成链表头失败
			MessageBox(NULL, "生成链表头失败,程序结束!", "严重错误", MB_OK);

			g_hShutdownOverEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	//创建关闭完毕提示事件

			::SetEvent(g_hShutdownEvent);	//设置退出事件,使创建进程提示结束,并自动卸载驱动
			
			//等待驱动卸载完成
			::WaitForSingleObject(g_hShutdownOverEvent, INFINITE);

			LeaveCriticalSection(&cs);		//离开临界段

			::TerminateProcess(NULL, -1);

			return FALSE;
		}

		//初始化链表头
		memset(g_pListHead, 0, sizeof(PidModuleListHead));

		g_pListHead->First = NULL;
		g_pListHead->Last = NULL;
		g_pListHead->ulCount = 0;
	}

	pLH = g_pListHead;

	if(pLH)
	{
		//存在链表头
		if(pLH->First)
		{
			//不是空链表
			if(pLH->First->Pid < pList->Pid)
			{
				//插入为第一个结点
				pList->Next = pLH->First;

				pLH->First = pList;

				pLH->ulCount++;

				LeaveCriticalSection(&cs);		//离开临界段

				return TRUE;
			}

			pL = pLH->First;	//pL指向第一个结点

			do
			{
				if(pL->Next)		//还存在下一个结点
				{
					if(pL->Next->Pid < pList->Pid)
					{
						//找到要插入的位置
						pList->Next = pL->Next;
						pL->Next = pList;

						pLH->ulCount++;
						
						LeaveCriticalSection(&cs);		//离开临界段

						return TRUE;
					}
				}
				else		//这是最后一个结点了
				{
					//插入到最后一个结点
					pList->Next = NULL;
					pL->Next = pList;

					pLH->ulCount++;

					pLH->Last = pList;

					LeaveCriticalSection(&cs);		//离开临界段

					return TRUE;
				}

			}while(pL = pL->Next);	//结点指针后移
		}
		else		//此链表还没有结点
		{
			//直接加入到pLH->First和pLH->Last
			pLH->First = pList;
			pLH->Last = pList;
			pLH->ulCount++;

			LeaveCriticalSection(&cs);		//离开临界段

			return TRUE;
		}
	}

	LeaveCriticalSection(&cs);		//离开临界段

	return FALSE;
}

//--------------------------------------------------------------
//移除链表中的指定结点
BOOL RemoveList(DWORD Pid)
{
	pPidModuleListHead pLH;
	pPidModuleList pL;

	//进入临界段
	EnterCriticalSection(&cs);

	pLH = g_pListHead;

	if(pLH)
	{
		//存在链表头
		if(pLH->First)
		{
			//不是空链表
			if(pLH->First->Pid == Pid)
			{
				//第一个结点为要删除的结点
				if(pLH->First->Next)		//不是删除最后一个结点
				{
					pPidModuleList pTemp;
					
					pTemp = pLH->First;		//指向第一个结点
					
					pLH->First = pLH->First->Next;		//头结点中的Next指针存放
					
					free(pTemp);			//释放删除的结点内存空间
				}
				else			//删除的是最后一个结点
				{
					free(pLH->First);
					pLH->First = NULL;		//头结点的First指针置为NULL

					pLH->Last = NULL;
				}

				pLH->ulCount--;

				LeaveCriticalSection(&cs);		//离开临界段

				return TRUE;
			}

			pL = pLH->First;	//pL指向第一个结点

			do
			{
				if(pL->Next->Pid == Pid)
				{
					//找到要删除的结点
					if(pL->Next->Next != NULL)
					{
						//要删除的结点不是最后一个结点
						pPidModuleList pTemp;

						pTemp = pL->Next;		//保存要删除的结点位置

						pL->Next = pTemp->Next;	//从链中去掉结点

						free(pTemp);
					}
					else
					{
						//要删除的结点是最后一个结点
						free(pL->Next);
						pL->Next = NULL;	//pL成为了最后一个结点,将它指向NULL

						pLH->Last = pL;
					}

					pLH->ulCount--;

					LeaveCriticalSection(&cs);		//离开临界段

					return TRUE;
				}

			}while((pL = pL->Next) && (pL->Next));	//结点指针后移,并存在下一个结点
		}
	}

	LeaveCriticalSection(&cs);		//离开临界段

	return FALSE;
}

//--------------------------------------------------------------
//输入链表中的信息
void PrintList()
{
	pPidModuleListHead pLH;
	pPidModuleList pL;
	ULONG i = 0;

	//进入临界段
	EnterCriticalSection(&cs);

	pLH = g_pListHead;

	if(pLH)
	{
		//存在链表头
		if(pLH->First)
		{
			//不是空链表
			pL = pLH->First;	//pL指向第一个结点

			//打印链表信息
			printf("--------------  链表信息  -------------\n");
			printf("链表中共有结点 %d 个\n\n", pLH->ulCount);

			do
			{
				printf("%04d PID: %05d ModuleBase: %08X\n", ++i, pL->Pid, pL->dwDllModuleBase);

			}while(pL = pL->Next);	//结点指针后移

			printf("------------  链表信息结束  -----------\n");
		}
	}

	LeaveCriticalSection(&cs);		//离开临界段
}

//--------------------------------------------------------------
//通过Pid,查找Dll模块基址
DWORD LookupDllModuleBaseByPid(DWORD Pid)
{
	pPidModuleListHead pLH;
	pPidModuleList pL;
	ULONG i = 0;

	//进入临界段
	EnterCriticalSection(&cs);

	pLH = g_pListHead;

	if(pLH)
	{
		//存在链表头
		if(pLH->First)
		{
			//不是空链表
			pL = pLH->First;	//pL指向第一个结点

			do
			{
				if(pL->Pid == Pid)	//查找到要找的Dll模块基址
				{
					LeaveCriticalSection(&cs);		//离开临界段

					return pL->dwDllModuleBase;
				}

			}while(pL = pL->Next);	//结点指针后移

		}
	}

	LeaveCriticalSection(&cs);		//离开临界段

	return ~0;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//--------------------------------------------------------------
//修改进程运行特权级
//若需提升特权级到DEBUG模式,请使用字符串参数 "SeDebugPrivilege"
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
//代码注入
BOOL CodeInject(IN DWORD Pid, IN char *szDllFileName, OUT LPDWORD hDllModuleBase)
{
	HANDLE hThread;						//远线程
	char szDllPathName[MAX_PATH + 1];	//Dll全路径名
	char *pDllRemote;					//远线程地址,指向存放Dll全路径名的地址
	HANDLE hProcess;					//打开进程句柄,即目标进程
	DWORD dwRet;


	memset(szDllPathName, 0, sizeof(szDllPathName));

	//得到Dll的全路径名
	if(!::GetFullPathName(szDllFileName, sizeof(szDllPathName) - 1, szDllPathName, NULL))
	{
		//出错
		return FALSE;
	}
	if(::GetFileAttributes(szDllPathName) == -1)
	{
		//文件Dll文件不存在
		return FALSE;
	}

	//提升进程特权级
	if(!EnableDebugPriv("SeDebugPrivilege"))
	{
		//提升特权级失败
		//Do Nothing
	}


	//打开目标进程
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if(!hProcess)
	{
		//打开进程失败
		return FALSE;
	}

	pDllRemote = (char *)::VirtualAllocEx(hProcess, NULL, sizeof(szDllPathName), \
		MEM_COMMIT, PAGE_READWRITE);
	if(!pDllRemote)
	{
		//分配失败

		//关闭进程句柄
		::CloseHandle(hProcess);

		return FALSE;
	}

	if(!::WriteProcessMemory(hProcess, pDllRemote, (void*)szDllPathName, \
		sizeof(szDllPathName), NULL))
	{
		//写入Dll路径出错

		//释放分配的内存
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//关闭进程句柄
		::CloseHandle(hProcess);

		return FALSE;
	}

	hThread = ::CreateRemoteThread(hProcess, NULL, 0, \
		(LPTHREAD_START_ROUTINE)::GetProcAddress( \
		::GetModuleHandle(KERNEL_MODULE_NAME), \
		"LoadLibraryA"), pDllRemote, 0, NULL);

	//等待2秒
	if(::WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
	{
		//超时没有返回

		//释放分配的内存
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//关闭进程句柄
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	if(!hThread)
	{
		//创建远线程失败

		//释放分配的内存
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//关闭进程句柄
		::CloseHandle(hProcess);

		return FALSE;
	}

	//得到加载模块的基址
	while((dwRet = ::GetExitCodeThread(hThread, hDllModuleBase)) == STILL_ACTIVE)
	{
		Sleep(1);
	}
	if(!dwRet || !(LPDWORD)(*hDllModuleBase))
	{
		//得到加载基址失败
		//Do Nothing
		printf("Pid: %05d LoadLibraryA Failed!\n", Pid);
		
		//释放分配的内存
		::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
		//关闭远线程句柄
		::CloseHandle(hThread);
		//关闭进程句柄
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	
	//释放分配的内存
	::VirtualFreeEx(hProcess, pDllRemote, sizeof(szDllPathName), MEM_RELEASE);
	//关闭远线程句柄
	::CloseHandle(hThread);
	//关闭进程句柄
	::CloseHandle(hProcess);


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//这里在链表中加入相应结点
	//生成一个新结点
	pPidModuleList pList = (pPidModuleList)malloc(sizeof(PidModuleList));
	
	memset(pList, 0, sizeof(PidModuleList));

	pList->Next = NULL;
	pList->Pid = Pid;
	pList->dwDllModuleBase = (DWORD)(*hDllModuleBase);

	if(!InsertList(pList))
	{
		//加入链表出错
		free(pList);
	}
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	return TRUE;
}

//--------------------------------------------------------------
//卸载注入的代码模块
BOOL UnCodeInject(IN DWORD Pid, IN DWORD hDllModuleBase)
{
	//远线程句柄
	HANDLE hThread;
	//卸载目标进程句柄
	HANDLE hProcess;
	DWORD dwRet;
	DWORD dwFreeLibRet;


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//这里释放链表中的相应结点
	if(!RemoveList(Pid))
	{
		//结点删除出错
		//Do Nothing
	}
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	//打开目标进程
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if(!hProcess)
	{
		//打开进程失败
		return FALSE;
	}

	hThread = ::CreateRemoteThread(hProcess, NULL, 0, \
		(LPTHREAD_START_ROUTINE)::GetProcAddress( \
		::GetModuleHandle(KERNEL_MODULE_NAME), \
		"FreeLibrary"), (void*)hDllModuleBase, 0, NULL);

	//等待2秒
	if(::WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
	{
		//超时没有返回
		
		//关闭进程句柄
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	if(!hThread)
	{
		//创建远线程失败

		//关闭进程句柄
		::CloseHandle(hProcess);

		return FALSE;
	}

	//得到释放库返回的值
	while((dwRet = ::GetExitCodeThread(hThread, &dwFreeLibRet)) == STILL_ACTIVE)
	{
		Sleep(1);
	}
	if(!dwRet || !dwFreeLibRet)
	{
		//释放库失败
		//Do Nothing
		printf("FreeLibrary Failed!\n");

		//关闭远线程句柄
		::CloseHandle(hThread);
		//关闭进程句柄
		::CloseHandle(hProcess);
		
		return FALSE;
	}

	//关闭远线程句柄
	::CloseHandle(hThread);
	//关闭进程句柄
	::CloseHandle(hProcess);

	return TRUE;
}

//--------------------------------------------------------------
//将代码注入所有进程
BOOL CodeInjectAll(IN char *szDllFileName)
{
	//遍历所有进程,进行注入
	HANDLE hProcessSnap = NULL;
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe32 = {0};
	DWORD dwCurrentId;
	DWORD dwDllModuleBase;


	//初始化临界段
	InitializeCriticalSection(&cs);

	//保存Dll文件名为全局变量
	memset(g_szDllFileName, 0, sizeof(g_szDllFileName));
	strcpy(g_szDllFileName, szDllFileName);

	//得到本程序的PID
	dwCurrentId = ::GetCurrentProcessId();

	//对系统中的所有运行进程进行快照
	hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//获取快照失败
		return FALSE;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);;

	//遍历进程
	if(::Process32First(hProcessSnap, &pe32))
	{
		//得到第一个成功
		do
		{
			if(dwCurrentId != pe32.th32ProcessID)	//枚举除本进程以外的全部运行进程
			{
				//这里对得到的进程信息进行操作

				//进行Dll注入
				if(!CodeInject(pe32.th32ProcessID, szDllFileName, &dwDllModuleBase))
				{
					//注入失败
				}
			}

		}while(::Process32Next(hProcessSnap, &pe32));

		//关闭快照句柄
		::CloseHandle(hProcessSnap);
	}

	//------------------------------------------------------------------
	//这里启动进程创建撤消监视进程
	char SysPathName[MAX_PATH+1];

	memset(SysPathName, 0, sizeof(SysPathName));

	//得到当前驱动程序全路径名
	if(::GetCurrentDirectory(sizeof(SysPathName) - 1, SysPathName))
	{
		//得到当前目录成功
		strcat(SysPathName, "\\");
		strcat(SysPathName, SYS_NAME);

		//----------测试
		printf("驱动全路径名: %s\n", SysPathName);///////////////测试

		//开始创建进程提示
		if(!CreateEventNotifyRing3(SysPathName, (DWORD)CreateProcessPrompt))
		{
			MessageBox(NULL, "进程监视线程安装失败!", "错误", MB_OK);
			return FALSE;
		}
	}

	return TRUE;
}

//--------------------------------------------------------------
//卸载所有注入的进程模块
BOOL UnCodeInjectAll()
{
	//--------------------------------------------
	//卸载驱动的事件
	g_hShutdownOverEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);	//创建关闭完毕提示事件

	::SetEvent(g_hShutdownEvent);	//设置退出事件,使创建进程提示结束,并自动卸载驱动

	//等待驱动卸载完成
	::WaitForSingleObject(g_hShutdownOverEvent, INFINITE);

	//关闭事件句柄
	::CloseHandle(g_hShutdownOverEvent);

	//---------------------------------------------------
	//注入模块卸载
	//从链表中取出所有注入的模块,进行卸载
	pPidModuleListHead pLH;

	pLH = g_pListHead;		//指向头结点

	if(pLH)
	{
		
		while(pLH->First)	//不是最后一个结点
		{
			if(!UnCodeInject(pLH->First->Pid, pLH->First->dwDllModuleBase))
			{
				//卸载此结点失败
				//Do Nothing
			}
		}
		
		free(g_pListHead);		//释放头结点
	}

	//删除临界段
	DeleteCriticalSection(&cs);

	return TRUE;
}