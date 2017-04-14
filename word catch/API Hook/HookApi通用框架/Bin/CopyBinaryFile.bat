@echo off

echo copy NtNotifyProcessCreate.sys
copy /y ..\NtNotifyProcessCreate\i386\NtNotifyProcessCreate.sys .
echo.


echo copy NtNotifyProcessCreateExeTEST.exe
if exist ..\NtNotifyProcessCreateExeTEST\Release\NtNotifyProcessCreateExeTEST.exe (copy /y ..\NtNotifyProcessCreateExeTEST\Release\NtNotifyProcessCreateExeTEST.exe .) else (copy /y ..\NtNotifyProcessCreateExeTEST\Debug\NtNotifyProcessCreateExeTEST.exe .)
echo.

echo copy HookApiDll.dll
if exist ..\HookApiDll\Release\HookApiDll.dll (copy /y ..\HookApiDll\Release\HookApiDll.dll .) else (copy /y ..\HookApiDll\Debug\HookApiDll.dll .)
echo.

echo copy TEST.exe
copy ..\TEST³ÌÐò\Release\TEST.exe .
echo.


ping -n 1 -l 8 -w 500 1.1.1.1>nul
