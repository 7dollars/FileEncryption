#include <ntddk.h>

PETHREAD pThreadObj = NULL;
BOOLEAN bTerminated = FALSE;
UCHAR szProcessName[12] = "connect.exe";

extern POBJECT_TYPE *PsProcessType;
extern POBJECT_TYPE *PsThreadType;

VOID DriverUnload(PDRIVER_OBJECT pDriverObject);
VOID AntiDbgThread(PVOID pContext);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject,PUNICODE_STRING pRegistryPath)
{
  OBJECT_ATTRIBUTES ObjAddr = {0};
  HANDLE ThreadHandle = 0;
  NTSTATUS NtStatus = STATUS_SUCCESS;

  KdPrint(("Driver Entry"));

  pDriverObject->DriverUnload = DriverUnload;

  InitializeObjectAttributes(&ObjAddr,NULL,OBJ_KERNEL_HANDLE,0,NULL);

  NtStatus = PsCreateSystemThread(&ThreadHandle,THREAD_ALL_ACCESS,&ObjAddr,NULL,NULL,AntiDbgThread,NULL);

  if(NT_SUCCESS(NtStatus))
  {
    KdPrint(("Thread Created"));

    NtStatus = ObReferenceObjectByHandle(ThreadHandle,THREAD_ALL_ACCESS,*PsThreadType,KernelMode,&pThreadObj,NULL);

    ZwClose(ThreadHandle);

    if(!NT_SUCCESS(NtStatus))
    {
      bTerminated = TRUE;
    }
  }

  return NtStatus;
}

VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
  bTerminated = TRUE;
  KeWaitForSingleObject(pThreadObj,Executive,KernelMode,FALSE,NULL);

  ObDereferenceObject(pThreadObj);
}

	   // win7 32
       //  +0x16c ImageFileName   
	   //  +0x0b8 ActiveProcessLinks  //进程链
	   //  +0x0ec DebugPort     
   
	   //xp sp3
       //  +0x174 ImageFileName    
	   //  +0x088 ActiveProcessLinks  //进程链
	   //  +0x0bc DebugPort    

VOID AntiDbgThread(PVOID pContext)
{
  PEPROCESS pCurrentProcess = NULL;
  PEPROCESS pFirstProcess = NULL;
  LARGE_INTEGER inteval;
char *ProcessName;
ULONG PID;

  inteval.QuadPart = -20000000;

  KeSetPriorityThread(KeGetCurrentThread(),LOW_REALTIME_PRIORITY);

  while(1)
  {
    if(bTerminated)
    {
      break;
    }
    pCurrentProcess = IoGetCurrentProcess();
    pFirstProcess = pCurrentProcess;


    while(RtlCompareMemory(szProcessName,(PUCHAR)((ULONG)pCurrentProcess + 0x174),10) != 10) //当前进程名
    {
      pCurrentProcess= (PEPROCESS)(*(PULONG)((ULONG)pCurrentProcess + 0x88) - 0x88);


      
      if(pCurrentProcess == pFirstProcess)
      {
		// KdPrint(("进程链表遍历结束，退出进程链表遍历"));
        goto END;
      }
    }

	  
         *(PULONG)((ULONG)pCurrentProcess + 0xbc) = 0;//debug端口清零
		 ProcessName = (char*)((ULONG)pCurrentProcess+ 0x174);
	     PID=PsGetProcessId(pCurrentProcess);
		 KdPrint(("Debug端口清零成功！被保护的进程名为: %s  \n", ProcessName));

END:
    KeDelayExecutionThread(KernelMode,FALSE,&inteval);
  }
}