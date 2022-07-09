// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_MOUSEMSG WM_USER + 106
#define WM_KBMSG WM_USER + 107
#define WM_CWMSG WM_USER + 108


#pragma data_seg("SHARED")
static HHOOK g_hMouse = NULL;
static HHOOK g_hKeyboard = NULL;
static HINSTANCE g_hInstance = NULL;
static HWND g_hWnd = NULL;
#pragma data_seg()
#pragma comment(linker,"/section:SHARED,rws")

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (g_hWnd != NULL && nCode == HC_ACTION)
	{
		::SendMessage(g_hWnd, WM_MOUSEMSG, wParam, lParam);
	}

	return CallNextHookEx(g_hMouse, nCode, wParam, lParam);

}

LRESULT CALLBACK LowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (g_hWnd != NULL && nCode == HC_ACTION)
	{
		::SendMessage(g_hWnd, WM_KBMSG, wParam, lParam);
	}

	return CallNextHookEx(g_hKeyboard, nCode, wParam, lParam);
}

LRESULT CALLBACK ChooseWindowProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (g_hWnd != NULL && nCode == HC_ACTION)
	{
		::SendMessage(g_hWnd, WM_CWMSG, wParam, lParam);
	}

	return CallNextHookEx(g_hKeyboard, nCode, wParam, lParam);
}

BOOL WINAPI StartHook(HWND hwnd, int thread_id)
{
	g_hWnd = hwnd;
	g_hInstance = ::GetModuleHandle("HookDLL");
	g_hMouse = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LowLevelMouseProc, g_hInstance, (DWORD)0);
	g_hKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyBoardProc, g_hInstance, (DWORD)0);
	if (NULL == g_hMouse || NULL == g_hKeyboard)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL WINAPI ChooseWindow(HWND hwnd, int thread_id)
{
	g_hWnd = hwnd;
	g_hInstance = ::GetModuleHandle("HookDLL");
	g_hMouse = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)ChooseWindowProc, g_hInstance, (DWORD)0);
	if (NULL == g_hMouse)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

VOID WINAPI StopHook()
{
	if (g_hMouse != NULL)
	{
		::UnhookWindowsHookEx(g_hMouse);	
	}
	if (g_hKeyboard != NULL) {
		::UnhookWindowsHookEx(g_hKeyboard);
	}
}