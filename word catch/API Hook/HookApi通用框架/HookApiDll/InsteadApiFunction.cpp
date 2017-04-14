/*
 *
 *                         Hook Api Dll �ļ�
 *
 *                      ʵ�ֽ��̵�Api Hook����
 *                    �µ�Api Hook�ڴ��ļ������
 *
 *                          Made By Adly
 *
 *                       Email: Adly369046978@163.com
 *
 *                           2008-7-22
 *
 */
#define	_WIN32_WINNT	0x0500		//ʹ�� CopyFileExW �ж���
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "HookApiDll.h"


/////////////////////////////////////////////////////////////////////////////////////
//////////  ע��:                                                        ////////////
//////////      �����õ���API(��Kernel32)��Ҫ�ö�̬��ȡ����              ////////////
//////////      ����GetModuleHandle,���ʧ��,����LoadLibraryA������Ӧ��dll�ļ� //////
//////////      Ȼ������GetProcAddress�õ�API��ַ                        ////////////
/////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//��̬��ȡApi��ַ
//
//  ����TRUE,ʹ�ø�Dll���سɹ�
//  ����FALSE,ʹ�ø�Dll����ʧ��
BOOL GetApiAddress()
{

	return TRUE;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// �������API�滻���������


//--------------------------------------------------------------------
// MessageBoxA
//
// ˵��:
//     MessageBoxA Hook ����
//
// ע��:
//     �滻����������� WINAPI ��ָ�����ĵ��÷�ʽΪ "��׼����" ����������ʱ
//     ��ջ��ƽ��,�������쳣!!!!!!!!!!!
//
int WINAPI NewMessageBoxA(
						  HWND hWnd,          // handle to owner window
						  LPCTSTR lpText,     // text in message box
						  LPCTSTR lpCaption,  // message box title
						  UINT uType          // message box style
						  )
{
	//ͨ��Api��,�õ���Ϣ���
	pHookApiInfo pNode;
	pNode = GetNodeByApi("MessageBoxA");
	if(!pNode)
	{
		return FALSE;
	}

	//��ԭԭAPI
	HookOnOrOff(pNode->pfApiAddress, pNode->OldApiMachineCode, 5);

	////////////////////////////////////////////
	// Add Invoke Old Function
	int iRet = MessageBoxA(hWnd, "����,�㱻HOOK��!!!!!!! ^_^", "����", MB_OK);

	//����ԭ API ��ʵ����
	iRet = MessageBoxA(hWnd, lpText, lpCaption, uType);

	//����Hook
	HookOnOrOff(pNode->pfApiAddress, pNode->NewJmpMachineCode, 5);

	return iRet;
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//HOOK API ��ں���
void HookApi()
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//�������API��HOOK����

	if( \
		!InitHook("User32", "User32.dll", "MessageBoxA", (DWORD)NewMessageBoxA) \
		)
	{
		//Hook ʧ��
		//////////////////////////////
		//Do Something here
		
	}
	else
	{
		//Hook �ɹ�
		//////////////////////////////
		//Do Something here

	}
}
