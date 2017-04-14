#include "stdafx.h"
#include "windows.h"
#include "winsock.h"
#include "stdlib.h"
#pragma comment(lib,"ws2_32.lib")
#define sockethead              0x35343332
typedef struct
{
	BYTE	Flags;
	DWORD	Buffer_Size;
	BYTE	Buffer[1024];
	BYTE	Info[256];
	DWORD   File_Size;
}NET_DATA, *LPNET_DATA;
/*
typedef struct header
{
	DWORD head;
	DWORD SRsign;
	int FileSize;
	char FileNum[256];
}*pheader;

BYTE* SendFilep(DWORD size)//保存文件的时候
{

	char TmpPath1[MAX_PATH];
	char TmpPath2[MAX_PATH];
	DWORD recvsize = 0;
	SOCKET sockInt;
	struct sockaddr_in serverAddr;
	WORD sockVersion;
    WSADATA wsaData;
	char tgtIP[30] = {0};
	struct hostent *hp = NULL;
	NET_DATA MyData;
	BYTE request[256] = {0};

	char sign[256] = {0};
	DWORD dwBytes;
	header addr;
	
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
		
		MessageBox(0,"链接失败``````",0,0);
		WSACleanup();
		return 0;
	}
	
	addr.head=sockethead;
	addr.SRsign=0;
	addr.FileSize=size;
	if(addr.FileSize==0)
	{
		MessageBox(0,"获取文件大小失败","错误",0);
		WSACleanup();
		return 0;
	}
	Sleep(100);
	send( sockInt, (char*)&addr, sizeof(header), NULL );

	wchar_t *file=new wchar_t[size+1];
	char *file1=new char[size+1];
	memset(file,0,size+1);
	
	memcpy(file,(void*)newwrite,size);
	WideCharToMultiByte(CP_ACP,400,file,sizeof(file),file1,0,NULL,FALSE);
	MessageBox(0,file1,"123123123123412",0);
	
	if (send( sockInt, (char*)&file, size, NULL ) == SOCKET_ERROR)
	{MessageBox(0,"发送文件失败",0,0);return 0;}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	recv( sockInt, (char*)&sign, sizeof(sign), NULL );
	if(strlen(sign)==0)
	{
		MessageBox(0,"接受文件标记出错",0,0);
		WSACleanup();
		return 0;
	}	
	addr.SRsign=1;
	memset(addr.FileNum,0,256);
	strcpy(addr.FileNum,sign);
	GetCurrentDirectory(MAX_PATH,TmpPath1);
	lstrcatA( TmpPath1, "\\file.dat" );
	
    HANDLE hFile = CreateFile( TmpPath1, GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL );
	WriteFile(hFile, &addr, sizeof(header), &dwBytes, NULL);

	CloseHandle(hFile);
	closesocket(sockInt);
	WSACleanup();
	return 0;
}

BYTE* RecvFile(header *addr,DWORD bsize)
{

	DWORD recvsize = 0;
	SOCKET sockInt;
	struct sockaddr_in serverAddr;
	WORD sockVersion;
    WSADATA wsaData;
	char tgtIP[30] = {0};
	struct hostent *hp = NULL;
	NET_DATA MyData;
	BYTE request[256] = {0};
	DWORD size=0;
	char sign[256] = {0};
	DWORD dwBytes;

	if(addr->head!=sockethead)
	{
		return 0;
	}


	
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
		
		MessageBox(0,"链接失败``````",0,0);
		WSACleanup();
		return 0;
	}
	Sleep(100);



	char *file=new char[addr->FileSize+1];
	memset(file,0,addr->FileSize+1);
	send( sockInt, (char*)addr, sizeof(header), NULL );
    recv( sockInt, (char*)file, addr->FileSize, NULL );
	MessageBox(0,file,0,0);
	newread=(DWORD)file;
	NumOfRead=addr->FileSize;
	return 0;
	
}*/