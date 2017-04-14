// connect.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "../common/server.h"
#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")
BYTE Endata[100]={'\0'},Dedata[100]={'\0'};
void main(int argc, char* argv[])
{
	RecvData(Endata,Dedata);                     //接收加解密代码
	if(strcmp(argv[1],"0")==0)                   //参数1为收发标记，参数2为文件路径,3为文件名(tmp名)
	{
		SendFile(argv[2],atoi(argv[3]));         //行为判断
	}
	else if(strcmp(argv[1],"1")==0)
	{
		RecvFile(argv[2],atoi(argv[3]));
	}
	else
	return;
}
