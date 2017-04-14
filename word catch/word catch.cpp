// word catch.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <string.h>
#include <STDIO.H>
#include <IPHlpApi.h>
#include <winsock.h> 
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "WS2_32.lib")
int testnet();
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	
	// TODO: Place code here.
	int info=0;
    char path[MAX_PATH]={0};
	DWORD Finfo=0;
	int net=0;



	if(testnet()==-1)
	{
		MessageBox(0,"远程服务器未开启",0,0);
		return 0;
	}

	if(strlen(lpCmdLine)==0)
	{
		WinExec("cmd /c wps1.exe",SW_HIDE);
		return 0;
	}

	for(int l=0,k=0;l<strlen(lpCmdLine);l++)
	{
		if(l==0||l==strlen(lpCmdLine)-1)
		{
			if(lpCmdLine[l]=='"')
			{
				continue;
			}
		}
		
		path[k]=lpCmdLine[l];
		k++;
	}
	CreateDirectory("C:\\tmjm",NULL);
	Sleep(500);
	Finfo=GetPrivateProfileInt("wordinfo","sign",23,path);//返回23为未被加密的文件
	if(Finfo==23)
	{
	
			DWORD name=GetTickCount();
			char tmppath[MAX_PATH];
			char runwps[MAX_PATH]="wps1.exe";
			char szname[15];
			char fullname[255];
			char tmpinipath[MAX_PATH];

         
		
            wsprintf(szname,"%d",name);
			wsprintf(tmppath,"C:\\tmjm\\%d",name);
			CopyFile(path,tmppath,FALSE);
	  
  
			sprintf(fullname,"connect 0 %s %d",tmppath,name);
			Sleep(100);
			WinExec(fullname,SW_HIDE);
			DeleteFile(path);

            WritePrivateProfileString("wordinfo","sign",szname,path);
			strcpy(tmpinipath,tmppath);
			strcat(tmpinipath,".ini");
            WritePrivateProfileString("docinfo","path",path,tmpinipath);
			
			strcat(runwps," ");
            strcat(runwps,tmppath);
            WinExec(runwps,SW_NORMAL);

			//加密文件并打开
	}
	else//文件为加密过的
	{
	
	
			char a[25];
			char fullname[255];
			char szname[15];
			char tmppath[MAX_PATH];
			char tmpinipath[MAX_PATH];
			char runwps[MAX_PATH]="wps1.exe";

			GetPrivateProfileString("wordinfo","sign",0,a,sizeof(a),path);
            sprintf(szname,"%s",a);
			sprintf(tmppath,"C:\\tmjm\\%s",a);
			sprintf(fullname,"connect 1 %s %s",tmppath,a);
			WinExec(fullname,SW_HIDE);

			strcpy(tmpinipath,tmppath);
			strcat(tmpinipath,".ini");

            WritePrivateProfileString("docinfo","path",path,tmpinipath);

			strcat(runwps," ");
            strcat(runwps,tmppath);
            WinExec(runwps,SW_HIDE);
	}

		
	return 0;
}

int testnet()//测试服务器连通性函数
{
	DWORD recvsize = 0;
	SOCKET sockInt;
	struct sockaddr_in serverAddr;
	WORD sockVersion;
    WSADATA wsaData;
	char tgtIP[30] = {0};
	struct hostent *hp = NULL;


    sockVersion = MAKEWORD(2,2);
    WSAStartup(sockVersion, &wsaData);

	sockInt = socket(AF_INET, SOCK_STREAM, 0);
	if(sockInt == INVALID_SOCKET)
	{
		WSACleanup();
		return 0;
	}
	

	serverAddr.sin_family = AF_INET;
	
	if ((hp = gethostbyname("127.0.0.1")) != NULL)
	{
		in_addr in;
		memcpy(&in, hp->h_addr, hp->h_length);
		lstrcpy(tgtIP,inet_ntoa(in));
	}
	
	serverAddr.sin_addr.s_addr = inet_addr(tgtIP);
	serverAddr.sin_port = htons(3108);
	
	if(connect(sockInt, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		WSACleanup();
		return -1;
	}
	else
	{
		WSACleanup();
		return 1;
	}
}