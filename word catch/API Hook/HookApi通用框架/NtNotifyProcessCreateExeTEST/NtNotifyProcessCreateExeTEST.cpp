/*
 *
 *                                 ���̴������Ӳ���
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */

#include <stdio.h>
#include <windows.h>
#include "..\\Common\\CodeInject.h"		//����ע��ͷ�ļ�


int main()
{
	//////////////////////////////////////////////////////////
	////---------------���Դ���
	char szDllName[] = "HookApiDll.dll";

	//���Դ���ע��
	if(!CodeInjectAll(szDllName))
	{
		printf("CodeInjectAll Failed!\n");
		//�����������
		::TerminateProcess(NULL, -1);
	}
	else
	{
		printf("CodeInjectAll Success!\n");
	}
	
	//��ӡ����
	PrintList();


	getchar();	//������з���ʼж��
	//����ע�����ж��
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
