/*
 *
 *                                代码注入函数头文件
 *
 *                     将Dll文件通过CreateRemoteThread函数注入到指定进程
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-20
 */

///////////////////////////////////////////////////////////////////
//链表数据结构
typedef struct _PidModuleList
{
	_PidModuleList *Next;		//指向下一个结构
	DWORD Pid;					//Pid
	DWORD dwDllModuleBase;		//注入后的Dll模块基址
}PidModuleList, *pPidModuleList;

typedef struct _PidModuleListHead
{
	_PidModuleList *First;		//指向链中第一个结构
	_PidModuleList *Last;		//指向链中最后一个结构
	ULONG ulCount;			//链表中结构个数
}PidModuleListHead, *pPidModuleListHead;

/////////////////////////////////////////////////////
//通过Pid,在链表中查询相应的Dll模块基址
//
//  IN
//    Pid                     - 要查询的Pid
//
//  OUT
//    DWORD                   - 返回的Dll模块基址,如果失败,返回 ~0 (即 -1)
//
//
DWORD LookupDllModuleBaseByPid(DWORD Pid);

/////////////////////////////////////////////////////
//将Dll文件通过CreateRemoteThread函数注入到指定进程
//
//  IN
//    Pid                     - 要注入到的目标进程Pid
//
//    szDllFileName           - 要注入的Dll文件名(不必是全路径名)
//
//  OUT
//    BOOL                    - TRUE 成功, FALSE 失败
//
//    hDllModuleBase          - 返回Dll加载到远线程进程的基地址
//
//
BOOL CodeInject(IN DWORD Pid, IN char *szDllFileName, OUT LPDWORD hDllModuleBase);

/////////////////////////////////////////////////////
//卸载目标进程中注入的Dll模块
//
//  IN
//    Pid                     - 要卸载注入模块的目标进程Pid
//
//    hDllModuleBase          - 要卸载的远程模块基址
//
//  OUT
//    BOOL                    - TRUE 成功, FALSE 失败
//
//
BOOL UnCodeInject(IN DWORD Pid, IN DWORD hDllModuleBase);

/////////////////////////////////////////////////////
//打印链表中的信息
//
//  IN
//    void                    - 无
//
//  OUT
//    void                    - 无
//
//
void PrintList();

/////////////////////////////////////////////////////
//将目标Dll注入所有进程
//
//  IN
//    szDllFileName           - 要注入的Dll文件名(不必是全路径名)
//
//  OUT
//    BOOL                    - TRUE 成功, FALSE 失败
//
//
BOOL CodeInjectAll(IN char *szDllFileName);

/////////////////////////////////////////////////////
//卸载所有已注入的Dll模块
//
//  IN
//    void                    - 无
//
//  OUT
//    BOOL                    - TRUE 成功, FALSE 失败
//
//
BOOL UnCodeInjectAll();

