/*
 *
 *                         Hook Api Dll 文件
 *
 *                      实现进程的Api Hook操作
 *                    新的Api Hook在此文件中添加
 *
 *                          Made By Adly
 *
 *                       Email: Adly369046978@163.com
 *
 *                           2008-7-22
 *
 */
#define	_WIN32_WINNT	0x0500		//使得 CopyFileExW 有定义
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "HookApiDll.h"


/////////////////////////////////////////////////////////////////////////////////////
//////////  注意:                                                        ////////////
//////////      所有用到的API(除Kernel32)都要用动态获取技术              ////////////
//////////      先用GetModuleHandle,如果失败,再用LoadLibraryA加载相应的dll文件 //////
//////////      然后利用GetProcAddress得到API地址                        ////////////
/////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//动态获取Api地址
//
//  返回TRUE,使得该Dll加载成功
//  返回FALSE,使得该Dll加载失败
BOOL GetApiAddress()
{

	return TRUE;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// 这里进行API替换函数的添加


//--------------------------------------------------------------------
// MessageBoxA
//
// 说明:
//     MessageBoxA Hook 测试
//
// 注意:
//     替换函数必须加上 WINAPI 以指明它的调用方式为 "标准调用" 否则函数返回时
//     因栈不平衡,会引起异常!!!!!!!!!!!
//
int WINAPI NewMessageBoxA(
						  HWND hWnd,          // handle to owner window
						  LPCTSTR lpText,     // text in message box
						  LPCTSTR lpCaption,  // message box title
						  UINT uType          // message box style
						  )
{
	//通过Api名,得到信息结点
	pHookApiInfo pNode;
	pNode = GetNodeByApi("MessageBoxA");
	if(!pNode)
	{
		return FALSE;
	}

	//还原原API
	HookOnOrOff(pNode->pfApiAddress, pNode->OldApiMachineCode, 5);

	////////////////////////////////////////////
	// Add Invoke Old Function
	int iRet = MessageBoxA(hWnd, "哈哈,你被HOOK啦!!!!!!! ^_^", "哈哈", MB_OK);

	//调用原 API 真实调用
	iRet = MessageBoxA(hWnd, lpText, lpCaption, uType);

	//继续Hook
	HookOnOrOff(pNode->pfApiAddress, pNode->NewJmpMachineCode, 5);

	return iRet;
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//HOOK API 入口函数
void HookApi()
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//这里进行API的HOOK操作

	if( \
		!InitHook("User32", "User32.dll", "MessageBoxA", (DWORD)NewMessageBoxA) \
		)
	{
		//Hook 失败
		//////////////////////////////
		//Do Something here
		
	}
	else
	{
		//Hook 成功
		//////////////////////////////
		//Do Something here

	}
}
