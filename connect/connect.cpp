// connect.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "../common/server.h"
#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")
BYTE Endata[100]={'\0'},Dedata[100]={'\0'};
void main(int argc, char* argv[])
{
	RecvData(Endata,Dedata);                     //���ռӽ��ܴ���
	if(strcmp(argv[1],"0")==0)                   //����1Ϊ�շ���ǣ�����2Ϊ�ļ�·��,3Ϊ�ļ���(tmp��)
	{
		SendFile(argv[2],atoi(argv[3]));         //��Ϊ�ж�
	}
	else if(strcmp(argv[1],"1")==0)
	{
		RecvFile(argv[2],atoi(argv[3]));
	}
	else
	return;
}
