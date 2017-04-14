/*
 *
 *                         Hook Api Dll ͷ�ļ�
 *
 *                      ʵ�ֽ��̵�Api Hook����
 *
 *                          Made By Adly
 *
 *                       Email: Adly369046978@163.com
 *
 *                           2008-7-22
 *
 */


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//����  ����API HOOK
extern "C" __declspec(dllexport) void HookApi();
//UNHOOK API ��ں���
extern "C" __declspec(dllexport) void UnHookApi();
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//������ṹ��
typedef struct _HookApiInfo
{
	char szLibModuleName[100];		//��ģ���� - ͨ��Ϊȥ��".dll"��dllģ���� (��:kernel32)
	char szLibDllModuleName[100];	//Dll����  - ͨ��Ϊ��ģ��������".dll" (��:kernel32.dll)
	char szApiName[100];			//Hook �� Api ����(��: MessageBoxA)
	FARPROC pfApiAddress;			//Api��ַ
	UCHAR OldApiMachineCode[5];		//ԭʼApi��ǰ5���ֽڻ�����
	UCHAR NewJmpMachineCode[5];		//�¹����JMP������(5�ֽ�)
	_HookApiInfo *Next;
}HookApiInfo, *pHookApiInfo;


//HookOn��HookOff����
BOOL HookOnOrOff(LPVOID lpfFunctionAddress, UCHAR *ucFunctionJmpCode, DWORD dwModifyLen);
//ͨ��Api���õ����ڽ��
pHookApiInfo GetNodeByApi(char *szApiName);
//��ʼ��Hook����
BOOL InitHook(char *szLibModuleName/*����*/, \
			  char *szLibDllModuleName/*���Dll��*/, \
			  char *szApiName/*Api��*/, \
			  DWORD NewFunctionAddress/*�滻������ַ*/ \
			  );

//��̬��ȡApi��ַ
BOOL GetApiAddress();