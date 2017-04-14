/*
 *
 *                                ����ע�뺯��ͷ�ļ�
 *
 *                     ��Dll�ļ�ͨ��CreateRemoteThread����ע�뵽ָ������
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-20
 */

///////////////////////////////////////////////////////////////////
//�������ݽṹ
typedef struct _PidModuleList
{
	_PidModuleList *Next;		//ָ����һ���ṹ
	DWORD Pid;					//Pid
	DWORD dwDllModuleBase;		//ע����Dllģ���ַ
}PidModuleList, *pPidModuleList;

typedef struct _PidModuleListHead
{
	_PidModuleList *First;		//ָ�����е�һ���ṹ
	_PidModuleList *Last;		//ָ���������һ���ṹ
	ULONG ulCount;			//�����нṹ����
}PidModuleListHead, *pPidModuleListHead;

/////////////////////////////////////////////////////
//ͨ��Pid,�������в�ѯ��Ӧ��Dllģ���ַ
//
//  IN
//    Pid                     - Ҫ��ѯ��Pid
//
//  OUT
//    DWORD                   - ���ص�Dllģ���ַ,���ʧ��,���� ~0 (�� -1)
//
//
DWORD LookupDllModuleBaseByPid(DWORD Pid);

/////////////////////////////////////////////////////
//��Dll�ļ�ͨ��CreateRemoteThread����ע�뵽ָ������
//
//  IN
//    Pid                     - Ҫע�뵽��Ŀ�����Pid
//
//    szDllFileName           - Ҫע���Dll�ļ���(������ȫ·����)
//
//  OUT
//    BOOL                    - TRUE �ɹ�, FALSE ʧ��
//
//    hDllModuleBase          - ����Dll���ص�Զ�߳̽��̵Ļ���ַ
//
//
BOOL CodeInject(IN DWORD Pid, IN char *szDllFileName, OUT LPDWORD hDllModuleBase);

/////////////////////////////////////////////////////
//ж��Ŀ�������ע���Dllģ��
//
//  IN
//    Pid                     - Ҫж��ע��ģ���Ŀ�����Pid
//
//    hDllModuleBase          - Ҫж�ص�Զ��ģ���ַ
//
//  OUT
//    BOOL                    - TRUE �ɹ�, FALSE ʧ��
//
//
BOOL UnCodeInject(IN DWORD Pid, IN DWORD hDllModuleBase);

/////////////////////////////////////////////////////
//��ӡ�����е���Ϣ
//
//  IN
//    void                    - ��
//
//  OUT
//    void                    - ��
//
//
void PrintList();

/////////////////////////////////////////////////////
//��Ŀ��Dllע�����н���
//
//  IN
//    szDllFileName           - Ҫע���Dll�ļ���(������ȫ·����)
//
//  OUT
//    BOOL                    - TRUE �ɹ�, FALSE ʧ��
//
//
BOOL CodeInjectAll(IN char *szDllFileName);

/////////////////////////////////////////////////////
//ж��������ע���Dllģ��
//
//  IN
//    void                    - ��
//
//  OUT
//    BOOL                    - TRUE �ɹ�, FALSE ʧ��
//
//
BOOL UnCodeInjectAll();

