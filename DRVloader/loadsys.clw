; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CLoadsysDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "loadsys.h"

ClassCount=3
Class1=CLoadsysApp
Class2=CLoadsysDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_LOADSYS_DIALOG

[CLS:CLoadsysApp]
Type=0
HeaderFile=loadsys.h
ImplementationFile=loadsys.cpp
Filter=N

[CLS:CLoadsysDlg]
Type=0
HeaderFile=loadsysDlg.h
ImplementationFile=loadsysDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CLoadsysDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=loadsysDlg.h
ImplementationFile=loadsysDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_LOADSYS_DIALOG]
Type=1
Class=CLoadsysDlg
ControlCount=3
Control1=IDC_EDIT_SYSPATHNAME,edit,1350631552
Control2=IDC_BUTTON_LOADSYS,button,1342242816
Control3=IDC_BUTTON_UNLOADSYS,button,1342242816

