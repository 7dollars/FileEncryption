/*
 *
 *                             ���ӽ���������������ͷ�ļ�
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */

/*

///////////////////////////////////////////////
//��������
//
//  IN
//    lpFileName       - ����ȫ·����
//
//  OUT
//    HANDLE           - �������غ�ľ��
//
HANDLE LoadDriver(IN LPCTSTR lpFileName);

///////////////////////////////////////////////
//ж������
//
//  IN
//    hDriver          - �������غ�ľ��
//
//  OUT
//    void             - ��
//
void UnloadDriver(IN HANDLE hDriver);

///////////////////////////////////////////////
//��ȡ�����Ľ�����Ϣ
//
//  IN
//    hDriver          - �������غ�ľ��
//    pProcInfo        - ��¼������Ϣ�ṹ��ָ��
//
//  OUT
//    BOOL             - TRUE �ɹ�, FALSE ʧ��
//
BOOL GetProcInfo(IN HANDLE hDriver, OUT PCALLBACK_INFO pProcInfo);

*/


///////////////////////////////////////////////
//�����¼���ʾ
//
//  IN
//    lpFileName              - ����ȫ·����
//
//    lpfHandleCreateProcess  - �н��̴���ʱ,ִ�еĺ�����ַ,��ü���static,����̬����
//        lpfHandleCreateProcess ������һ������ -> PCALLBACK_INFO
//        lpfHandleCreateProcess ��������Ҫ�ͷ� PCALLBACK_INFO �������ڴ�ռ�
//
//  OUT
//    BOOL                    - TRUE �ɹ�, FALSE ʧ��
//
//
//  ע��:
//        Ҫ�������̴�����ʾʱ,��Ҫ�����˳��¼�Ϊ��Ч
//        extern HANDLE g_hShutdownEvent;		//�����ر��¼����
//        ::SetEvent(g_hShutdownEvent);	//�����˳��¼�,ʹ����������ʾ����,���Զ�ж������
//
BOOL CreateEventNotifyRing3(IN LPCTSTR lpFileName, IN DWORD lpfHandleCreateProcess);

