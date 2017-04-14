/*
 *
 *                             监视进程驱动操作函数头文件
 *
 *                                   Made By Adly
 *
 *                                 Email : Raojianhua242@yahoo.com.cn
 *
 *                                    2008-7-17
 */

/*

///////////////////////////////////////////////
//加载驱动
//
//  IN
//    lpFileName       - 驱动全路径名
//
//  OUT
//    HANDLE           - 驱动加载后的句柄
//
HANDLE LoadDriver(IN LPCTSTR lpFileName);

///////////////////////////////////////////////
//卸载驱动
//
//  IN
//    hDriver          - 驱动加载后的句柄
//
//  OUT
//    void             - 无
//
void UnloadDriver(IN HANDLE hDriver);

///////////////////////////////////////////////
//获取创建的进程信息
//
//  IN
//    hDriver          - 驱动加载后的句柄
//    pProcInfo        - 记录进程信息结构体指针
//
//  OUT
//    BOOL             - TRUE 成功, FALSE 失败
//
BOOL GetProcInfo(IN HANDLE hDriver, OUT PCALLBACK_INFO pProcInfo);

*/


///////////////////////////////////////////////
//创建事件提示
//
//  IN
//    lpFileName              - 驱动全路径名
//
//    lpfHandleCreateProcess  - 有进程创建时,执行的函数地址,最好加上static,即静态函数
//        lpfHandleCreateProcess 函数带一个参数 -> PCALLBACK_INFO
//        lpfHandleCreateProcess 函数内需要释放 PCALLBACK_INFO 参数的内存空间
//
//  OUT
//    BOOL                    - TRUE 成功, FALSE 失败
//
//
//  注意:
//        要结束进程创建提示时,需要设置退出事件为有效
//        extern HANDLE g_hShutdownEvent;		//导出关闭事件句柄
//        ::SetEvent(g_hShutdownEvent);	//设置退出事件,使创建进程提示结束,并自动卸载驱动
//
BOOL CreateEventNotifyRing3(IN LPCTSTR lpFileName, IN DWORD lpfHandleCreateProcess);

