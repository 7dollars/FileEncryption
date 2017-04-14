// server.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "winsock.h"
#pragma comment(lib,"ws2_32.lib")
#define Request_DOWN			0x9
#define Request_UP		    	0x23
#define File_Buffer				0x10
#define File_Buffer_Finish		0x11
#define Request_EnOK            0x7

typedef struct
{
	BYTE	Flags;
	DWORD	Buffer_Size;
	BYTE	Buffer[1024];
	BYTE	Info[256];
	DWORD   File_Size;
	BYTE    Fname[256];
}NET_DATA, *LPNET_DATA;



BYTE* SendFile(char filename[])
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
	DWORD size=0;
	char sign[256] = {0};
	DWORD dwBytes;


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
	
	

	GetCurrentDirectory(MAX_PATH,TmpPath1);
	
	lstrcatA( TmpPath1, "\\file.dat" );


	if ( GetFileAttributes(TmpPath1) == -1 )
	{
		WSACleanup();
		return 0;
	}
					
    HANDLE hFile = CreateFile( TmpPath1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL );
	request[0] = Request_DOWN;

	send( sockInt, (char*)&request, sizeof(request), NULL );

	size=GetFileSize(hFile,NULL);
	

	if(size!=0)
	{

		Sleep(100);
		send( sockInt, (char*)&size, sizeof(size), NULL );

	
	}
	BYTE *file=new BYTE[size];
	memset(file,0,size);
	while(1)
	{	
		memset( &MyData, 0, sizeof(NET_DATA) );
		MyData.Flags = File_Buffer;
		ReadFile(hFile, MyData.Buffer, 1024, &MyData.Buffer_Size, NULL); // 读取数据, 后面发送
		
		if ( MyData.Buffer_Size == 0 )//读取完了，
		{
			CloseHandle(hFile);
			
			
			memset( &MyData, 0, sizeof(NET_DATA) );
			MyData.Flags = File_Buffer_Finish;
			
			MyData.Buffer_Size = 1;
			
			if ( send( sockInt, (char*)&MyData, sizeof(MyData), NULL ) == SOCKET_ERROR ) break;
			break;
		}
		if (send( sockInt, (char*)&MyData, sizeof(MyData), NULL ) == SOCKET_ERROR ) break; // 发送上面读取的数据
	}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	CloseHandle(hFile);
	closesocket(sockInt);
	WSACleanup();
	return 0;
}
BYTE* RecvFile(char filename[])
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
	DWORD size=0;
	char sign[256] = {0};
	DWORD dwBytes;


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
	
	

	GetCurrentDirectory(MAX_PATH,TmpPath1);



	if ( GetFileAttributes(TmpPath1) == -1 )
	{
		WSACleanup();
		return 0;
	}
					
   
	request[0] = Request_UP;

	send( sockInt, (char*)&request, sizeof(request), NULL );

    recv( sockInt, (char*)&size, sizeof(size), NULL );
	BYTE *file=new BYTE[size];
	memset(file,0,size);
	


		memset(file,0,size);
		recvsize=0;
		while (1)
		{
			memset( &MyData, 0, sizeof(NET_DATA) );
			recv( sockInt, (char*)&MyData, sizeof(MyData), NULL );
			
			if ( MyData.Flags != File_Buffer_Finish && MyData.Flags != File_Buffer ) break;
			//	WriteFile(hFile, MyData.Buffer, MyData.Buffer_Size, &dwBytes, NULL);
		
			memcpy(file+recvsize,MyData.Buffer,MyData.Buffer_Size);
			
			recvsize += MyData.Buffer_Size;
			if ( MyData.Flags == File_Buffer_Finish ) break;
		}

		 GetCurrentDirectory(MAX_PATH,TmpPath2);
		
	     lstrcatA( TmpPath2, "\\receivefile.dat" );

		 HANDLE hFile1 = CreateFile( TmpPath2, GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL );
	     	
		 WriteFile(hFile1, file, size, &dwBytes, NULL);
	
		 CloseHandle(hFile1);

	closesocket(sockInt);
	WSACleanup();
	return 0;
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	BYTE *pFile = 0;
    SendFile("");
    return 0;
}



