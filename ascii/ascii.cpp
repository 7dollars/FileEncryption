// ascii.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <malloc.h>


DWORD convert(char *b,int num);
DWORD unconvert(char *b,int num);
typedef DWORD (__cdecl *AsmConvert)(char*,int);
typedef DWORD (__cdecl *AsmUnConvert)(char*,int);

//////////////////////////////////////////////////////////////////////////////////////////////
DWORD convert(char *b,int num)
{
	for(int i=0;i<num-1;i++)
	{
		if(b[i]=='\xFF')
			b[i]='\x00';
		else
		b[i]=b[i]+1;
	}
	return 0;
}

unsigned char data[45] = {
	0x8B, 0x44, 0x24, 0x08, 0x56, 0x33, 0xC9, 0x8D, 0x70, 0xFF, 0x85, 0xF6, 0x7E, 0x1B, 0x8B, 0x54, 
	0x24, 0x08, 0x8A, 0x04, 0x11, 0x3C, 0xFF, 0x75, 0x06, 0xC6, 0x04, 0x11, 0x00, 0xEB, 0x05, 0xFE, 
	0xC0, 0x88, 0x04, 0x11, 0x41, 0x3B, 0xCE, 0x7C, 0xE9, 0x33, 0xC0, 0x5E, 0xC3
};
/*
00401040  /$  8B4424 08     mov eax,dword ptr ss:[esp+0x8]
00401044  |.  56            push esi
00401045  |.  33C9          xor ecx,ecx
00401047  |.  8D70 FF       lea esi,dword ptr ds:[eax-0x1]
0040104A  |.  85F6          test esi,esi
0040104C  |.  7E 1B         jle Xascii.00401069
0040104E  |.  8B5424 08     mov edx,dword ptr ss:[esp+0x8]
00401052  |>  8A0411        /mov al,byte ptr ds:[ecx+edx]
00401055  |.  3C FF         |cmp al,0xFF
00401057  |.  75 06         |jnz Xascii.0040105F
00401059  |.  C60411 00     |mov byte ptr ds:[ecx+edx],0x0
0040105D  |.  EB 05         |jmp Xascii.00401064
0040105F  |>  FEC0          |inc al
00401061  |.  880411        |mov byte ptr ds:[ecx+edx],al
00401064  |>  41            |inc ecx
00401065  |.  3BCE          |cmp ecx,esi
00401067  |.^ 7C E9         \jl Xascii.00401052
00401069  |>  33C0          xor eax,eax
0040106B  |.  5E            pop esi
0040106C  \.  C3            retn
*/
//////////////////////////////////////////////////////////////////////////////////////////////
DWORD unconvert(char *b,int num)
{
	for(int i=0;i<num-1;i++)
	{
		if(b[i]=='\x00')
			b[i]='\xFF';
		else
		b[i]=b[i]-1;
	}
	return 0;
}

unsigned char undata[45] = {
	0x8B, 0x44, 0x24, 0x08, 0x56, 0x33, 0xC9, 0x8D, 0x70, 0xFF, 0x85, 0xF6, 0x7E, 0x1B, 0x8B, 0x54, 
	0x24, 0x08, 0x8A, 0x04, 0x11, 0x84, 0xC0, 0x75, 0x06, 0xC6, 0x04, 0x11, 0xFF, 0xEB, 0x05, 0xFE, 
	0xC8, 0x88, 0x04, 0x11, 0x41, 0x3B, 0xCE, 0x7C, 0xE9, 0x33, 0xC0, 0x5E, 0xC3
};
/*
00401040  /$  8B4424 08     mov eax,dword ptr ss:[esp+0x8]
00401044  |.  56            push esi
00401045  |.  33C9          xor ecx,ecx
00401047  |.  8D70 FF       lea esi,dword ptr ds:[eax-0x1]
0040104A  |.  85F6          test esi,esi
0040104C  |.  7E 1B         jle Xascii.00401069
0040104E  |.  8B5424 08     mov edx,dword ptr ss:[esp+0x8]
00401052  |>  8A0411        /mov al,byte ptr ds:[ecx+edx]
00401055  |.  84C0          |test al,al
00401057  |.  75 06         |jnz Xascii.0040105F
00401059  |.  C60411 FF     |mov byte ptr ds:[ecx+edx],0xFF
0040105D  |.  EB 05         |jmp Xascii.00401064
0040105F  |>  FEC8          |dec al
00401061  |.  880411        |mov byte ptr ds:[ecx+edx],al
00401064  |>  41            |inc ecx
00401065  |.  3BCE          |cmp ecx,esi
00401067  |.^ 7C E9         \jl Xascii.00401052
00401069  |>  33C0          xor eax,eax
0040106B  |.  5E            pop esi
0040106C  \.  C3            retn
*/
//////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char          a[100];
	AsmConvert    pasmconvert;
	AsmUnConvert  pasmunconvert;

	pasmconvert   = (AsmConvert)malloc(46);
	pasmunconvert = (AsmUnConvert)malloc(46);

	for(int i=0;i<99;i++)
		a[i]='\x66';

	memcpy(pasmconvert,data,45);
	memcpy(pasmunconvert,undata,45);

	pasmconvert(a,sizeof(a));
	pasmunconvert(a,sizeof(a));

	MessageBox(0,a,0,0);
	return 0;
}



