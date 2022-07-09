#include "DlgProc.h"
#include "HookProc.h"
#include "processthreadsapi.h"

BOOL g_bIsRecord = TRUE;
BOOL g_bLoad = FALSE;
HWND g_hwnd;
DWORD g_thread_id;
int thread_id;


BEGIN_DLG_MSGMAP(RecordDlg)
	ON_DLG_MSGMAP(WM_CLOSE, OnClose)
	ON_DLG_MSGMAP(WM_COMMAND, OnCommand)
	ON_DLG_MSGMAP(WM_MOUSEMSG, OnMouseMsg)
	ON_DLG_MSGMAP(WM_KBMSG, OnKBMsg)
	ON_DLG_MSGMAP(WM_CWMSG, OnChooseWindow)
END_DLG_MSGMAP()

BTNMSG g_BtnMsg[] = {
	{IDB_COPY, OnCopy},
	{IDB_PLAYBACK, OnPlayBack},
	{IDB_SAVE, OnSave},
	{IDB_LOAD, OnLoad},
	{IDC_CHWD, OnChwd},
	{0, NULL},
};

IMPLEMENT_DLGPROC(RecordDlg)

BOOL OnClose(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	FreeMsgList();
	EndDialog(hDlg,WM_CLOSE);
	return TRUE;
}


BOOL OnChwd(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE g_hInstanceDll = NULL;
	g_hInstanceDll = LoadLibrary("HookDLL.dll");
	if (NULL == g_hInstanceDll) {
		MessageBox(NULL, "Load dll failed!", "Message", MB_ICONWARNING);
		return false;
	}
	typedef BOOL(CALLBACK* ChooseWindowProc)(HWND hwnd, int thread_id);
	ChooseWindowProc chooseWindowProc = (ChooseWindowProc)::GetProcAddress(g_hInstanceDll, "ChooseWindow");
	if (NULL == chooseWindowProc)
	{
		MessageBox(NULL, "Get proc adress failed!", "Message", MB_ICONWARNING);
		return false;
	}
	if (chooseWindowProc(GetActiveWindow(), 0))
		MessageBox(NULL, "Start hook success!", "Message", MB_ICONWARNING);
	else
		MessageBox(NULL, "Start hook failed!", "Message", MB_ICONWARNING);
	FreeLibrary(g_hInstanceDll);
	return true;
}

BOOL OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	BTNMSG *pBtnMsg = g_BtnMsg;

	while (0 != pBtnMsg->uBtnID)
	{
		if (LOWORD(wParam) == pBtnMsg->uBtnID)
		{
			return pBtnMsg->pBtnProc(hDlg,wParam, lParam);
		}
		pBtnMsg++;
	}

	return FALSE;
}

BOOL OnCopy(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
	{
		if (g_bIsRecord)
		{
			SetWindowText(GetDlgItem(hDlg, IDB_COPY), "Stop");
			g_bIsRecord = FALSE;
			startHookTool();
		}
		else
		{
			SetWindowText(GetDlgItem(hDlg, IDB_COPY), "Record");
			g_bIsRecord = TRUE;
			stopHookTool();
		}
		if (g_bIsRecord)
		{
			EnableWindow(GetDlgItem(hDlg, IDB_PLAYBACK),TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDB_PLAYBACK),FALSE);
		}
	}
	return TRUE;
}

BOOL OnPlayBack(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
	LPEVENTNODE p = g_pMsgList;
	while (p != NULL)
	{
		if (p->wParam == WM_LBUTTONDOWN)
		{
			p = p->pNextMsg;
			continue;
		}
		else if (p->wParam == WM_LBUTTONUP)
		{
			RECT rect;
			::GetWindowRect(g_hwnd, &rect);
			mouseClick(rect.left * getScale() + p->mouseX, rect.top * getScale() + p->mouseY);
		}
		else if (p->wParam == WM_KEYDOWN)
		{
			kbClickDown(p->keyCode);
		}
		else if (p->wParam == WM_KEYUP)
		{
			kbClickUp(p->keyCode);
		}
		p = p->pNextMsg;
	}
	MessageBox(NULL, "Playback finished", "Message", MB_ICONWARNING);
	return TRUE;
}

BOOL OnMouseMsg(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_LBUTTONUP)
	{
		static PMSLLHOOKSTRUCT mouse = (PMSLLHOOKSTRUCT)lParam;
		HWND hwnd = ::WindowFromPoint(mouse->pt);
		DWORD procID = 0;
		GetWindowThreadProcessId(hwnd, &procID);
		if (procID == g_thread_id)
		{
			RECT rect;
		    ::GetWindowRect(g_hwnd, &rect);
			EVENTNODE* pNewMsg = new EVENTNODE;
			pNewMsg->mouseX = mouse->pt.x - rect.left * getScale();
			pNewMsg->mouseY = mouse->pt.y - rect.top * getScale();
			pNewMsg->wParam = wParam;
			AddMsgList(pNewMsg);
			uLen++;
		}
	}
	return true;
}

BOOL OnKBMsg(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	LPKBDLLHOOKSTRUCT key = (LPKBDLLHOOKSTRUCT)lParam;
	EVENTNODE* pNewMsg = new EVENTNODE;
	pNewMsg->wParam = wParam;
	pNewMsg->keyCode = key->vkCode;
	AddMsgList(pNewMsg);
	uLen++;
	return true;
}

BOOL OnChooseWindow(HWND hDlg, WPARAM wParam, LPARAM lParam) 
{
	if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP)
	{
		stopHookTool();
		static PMSLLHOOKSTRUCT mouse = (PMSLLHOOKSTRUCT)lParam;
		HWND hwnd = ::WindowFromPoint(mouse->pt);
		g_hwnd = hwnd;
		DWORD procID = 0;
		GetWindowThreadProcessId(hwnd, &procID);
		HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, procID);
		if (handle)
		{
			g_thread_id = procID;
			TCHAR szFileName[MAX_PATH] = { 0 };
			DWORD dwLen = MAX_PATH;
			QueryFullProcessImageName(handle, 0, szFileName, &dwLen);
			g_hwnd = hwnd;
			TCHAR proName[MAX_PATH] = { 0 };
			int c = 0;
			for (int i = strlen(szFileName); 0 <= i; i--) 
			{
				if (szFileName[i] != '\\')
				{
					proName[c] = szFileName[i];
					c++;
				}else
				{
					break;
				}
			}
			for (int i = 0; i < c; i++)
			{
				szFileName[i] = proName[c - i - 1];
			}
			SetWindowText(GetDlgItem(hDlg, IDC_TEXT), szFileName);
			//MessageBox(NULL, szFileName, "Message", MB_ICONWARNING);
		}
	}
	return true;
}

BOOL OnSave(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
	if (NULL != g_pMsgList)
	{
		HANDLE hFile = CreateFile("MsgFile.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		EVENTNODE *pTemp = g_pMsgList; 
		DWORD dwRealWrite;//真实写入的大小
		while (NULL != pTemp)
		{
			char t[20];
			if (pTemp->wParam == WM_LBUTTONUP) {
				WriteFile(hFile, "MouseClick:(", 12 * sizeof(char), &dwRealWrite, NULL);
				ltoa(pTemp->mouseX, t, 10);
				WriteFile(hFile, t, strlen(t) * sizeof(char), &dwRealWrite, NULL);
				ltoa(pTemp->mouseY, t, 10);
				WriteFile(hFile, ",", sizeof(char), &dwRealWrite, NULL);
				WriteFile(hFile, t, strlen(t) * sizeof(char), &dwRealWrite, NULL);
				WriteFile(hFile, ")\n", 2 * sizeof(char), &dwRealWrite, NULL);
			}
			if (pTemp->wParam == WM_KEYDOWN) {
				WriteFile(hFile, "KeyDown:", 8 * sizeof(char), &dwRealWrite, NULL);
				WriteFile(hFile, &(pTemp->keyCode), sizeof(char), &dwRealWrite, NULL);
				WriteFile(hFile, "\n", sizeof(char), &dwRealWrite, NULL);
			}
			
			

			pTemp = pTemp->pNextMsg;
		}

		CloseHandle(hFile);
	}
	return TRUE;
}

BOOL OnLoad(HWND hDlg,WPARAM wParam, LPARAM lParam)
{
	/*g_bLoad = TRUE;
	FreeMsgList();
	HANDLE hFile = CreateFile("MsgFile.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwRelRead;
	EVENTNODE *pEndList;

	do 
	{
		if(NULL == g_pMsgList)
		{
			g_pMsgList = new EVENTNODE;
			ReadFile(hFile, &(g_pMsgList->EventMsg), sizeof(EVENTMSG),&dwRelRead, NULL);
			if (0 == dwRelRead)
			{
				break;
			}
			pEndList = g_pMsgList;
		}else
		{
			EVENTNODE *pNewMsg = new EVENTNODE;
			pNewMsg->pNextMsg = NULL;
			ReadFile(hFile, &(pNewMsg->EventMsg), sizeof(EVENTMSG),&dwRelRead, NULL);
			if (0 == dwRelRead)
			{
				break;
			}
			pEndList->pNextMsg = pNewMsg;
			pEndList = pEndList->pNextMsg;
		}

		uLen++;
	} while (TRUE);

	if (g_bLoad && NULL != g_pMsgList)
	{
		EnableWindow(GetDlgItem(hDlg, IDB_PLAYBACK),TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg, IDB_PLAYBACK),FALSE);
	}

	CloseHandle(hFile);*/
	return TRUE;
}

void mouseClick(LONG x, LONG y)
{
	double dpi = getScale();
	SetCursorPos(x / dpi, y / dpi);
	Sleep(200);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(200);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	Sleep(200);
}

void kbClick(BYTE c)
{
	Sleep(200);
	keybd_event(c, 0, 0, 0);
	Sleep(200);
	keybd_event(c, 0, KEYEVENTF_KEYUP, 0);
}

void kbClickDown(BYTE c)
{
	Sleep(200);
	keybd_event(c, 0, 0, 0);
}

void kbClickUp(BYTE c)
{

	Sleep(200);
	keybd_event(c, 0, KEYEVENTF_KEYUP, 0);
}

/*
	get screen scaling
*/
double getScale()
{
	HWND hd = GetDesktopWindow();
	int zoom = GetDpiForWindow(hd);
	double scale = 1;
	switch (zoom) {
	case 96:
		scale = 1;
		break;
	case 120:
		scale = 1.25;
		break;
	case 144:
		scale = 1.5;
		break;
	case 192:
		scale = 2;
		break;
	default:
		break;
	}
	return scale;
}

BOOL startHookTool()
{
	HINSTANCE g_hInstanceDll = NULL;
	g_hInstanceDll = LoadLibrary("HookDLL.dll");
	if (NULL == g_hInstanceDll) {
		MessageBox(NULL, "Load dll failed!", "Message", MB_ICONWARNING);
		return false;
	}
	typedef BOOL(CALLBACK* StartHook)(HWND hwnd, int thread_id);
	StartHook startHook = (StartHook)::GetProcAddress(g_hInstanceDll, "StartHook");
	if (NULL == startHook)
	{
		MessageBox(NULL, "Get proc adress failed!", "Message", MB_ICONWARNING);
		return false;
	}
	if (startHook(GetActiveWindow(), 0))
		MessageBox(NULL, "Start hook success!", "Message", MB_ICONWARNING);
	else
		MessageBox(NULL, "Start hook failed!", "Message", MB_ICONWARNING);
	FreeLibrary(g_hInstanceDll);
}

BOOL stopHookTool()
{
	HINSTANCE g_hInstanceDll = NULL;
	g_hInstanceDll = LoadLibrary("HookDLL.dll");
	if (g_hInstanceDll == NULL)
	{
		MessageBox(NULL, "Load dll failed!", "Message", MB_ICONWARNING);
		return false;
	}
	typedef BOOL(CALLBACK* StopHook)();
	StopHook stopHook = (StopHook)::GetProcAddress(g_hInstanceDll, "StopHook");
	if (stopHook == NULL)
		MessageBox(NULL, "stop hook failed!", "Message", MB_ICONWARNING);
	else
		stopHook();
	MessageBox(NULL, "stop hook success!", "Message", MB_ICONWARNING);
	FreeLibrary(g_hInstanceDll);
}