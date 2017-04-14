/*
 *
 *                         Hook Api Dll 头文件
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


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//导出  进行API HOOK
extern "C" __declspec(dllexport) void HookApi();
//UNHOOK API 入口函数
extern "C" __declspec(dllexport) void UnHookApi();
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//链表结点结构体
typedef struct _HookApiInfo
{
	char szLibModuleName[100];		//库模块名 - 通常为去掉".dll"的dll模块名 (如:kernel32)
	char szLibDllModuleName[100];	//Dll库名  - 通常为库模块名加上".dll" (如:kernel32.dll)
	char szApiName[100];			//Hook 的 Api 名字(如: MessageBoxA)
	FARPROC pfApiAddress;			//Api地址
	UCHAR OldApiMachineCode[5];		//原始Api的前5个字节机器码
	UCHAR NewJmpMachineCode[5];		//新构造的JMP机器码(5字节)
	_HookApiInfo *Next;
}HookApiInfo, *pHookApiInfo;


//HookOn或HookOff函数
BOOL HookOnOrOff(LPVOID lpfFunctionAddress, UCHAR *ucFunctionJmpCode, DWORD dwModifyLen);
//通过Api名得到所在结点
pHookApiInfo GetNodeByApi(char *szApiName);
//初始化Hook操作
BOOL InitHook(char *szLibModuleName/*库名*/, \
			  char *szLibDllModuleName/*库的Dll名*/, \
			  char *szApiName/*Api名*/, \
			  DWORD NewFunctionAddress/*替换函数地址*/ \
			  );

//动态获取Api地址
BOOL GetApiAddress();