/*
 *
 *                                 进程创建监视测试
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */

#include <stdio.h>
#include <windows.h>
#include "..\\Common\\CodeInject.h"		//代码注入头文件


int main()
{
	//////////////////////////////////////////////////////////
	////---------------测试代码
	char szDllName[] = "HookApiDll.dll";

	//测试代码注入
	if(!CodeInjectAll(szDllName))
	{
		printf("CodeInjectAll Failed!\n");
		//这里结束程序
		::TerminateProcess(NULL, -1);
	}
	else
	{
		printf("CodeInjectAll Success!\n");
	}
	
	//打印链表
	PrintList();


	getchar();	//输出换行符后开始卸载
	//测试注入代码卸载
	if(!UnCodeInjectAll())
	{
		printf("UnCodeInjectAll Failed!\n");
	}
	else
	{
		printf("UnCodeInjectAll Success!\n");
	}

	return 0;
}
