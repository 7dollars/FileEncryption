#define    _WIN32_WINNT  0x0500
#include <stdio.h>
#include <windows.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	
	Sleep(500);

	::MessageBox(NULL, "ԭMessageBox�ĵ���!", "TEST", MB_OK);


	return 0;
}