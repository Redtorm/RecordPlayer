#ifndef __DLGPROC_H__
#define __DLGPROC_H__
#include "windows.h"
#include "DlgMsgMap.h"
#include "resource.h"

#define WM_MOUSEMSG WM_USER + 106
#define WM_KBMSG WM_USER + 107
#define WM_CWMSG WM_USER + 108

DECLARE_DLGMSGMAP(RecordDlg);
DECLARE_DLGMSGPROC(RecordDlg);

typedef BOOL (*PBTNPROC)(HWND hDlg,WPARAM wParam, LPARAM lParam);

struct BTNMSG
{
	WORD uBtnID;
	PBTNPROC pBtnProc;
};

BOOL OnClose(HWND hDlg, WPARAM wParam, LPARAM lParam);
BOOL OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);
BOOL OnChwd(HWND hDlg, WPARAM wParam, LPARAM lParam);


BOOL OnCopy(HWND hDlg,WPARAM wParam, LPARAM lParam);
BOOL OnPlayBack(HWND hDlg,WPARAM wParam, LPARAM lParam);
BOOL OnSave(HWND hDlg,WPARAM wParam, LPARAM lParam);
BOOL OnLoad(HWND hDlg,WPARAM wParam, LPARAM lParam);
BOOL OnMouseMsg(HWND hDlg, WPARAM wParam, LPARAM lParam);
BOOL OnKBMsg(HWND hDlg, WPARAM wParam, LPARAM lParam);
BOOL OnChooseWindow(HWND hDlg, WPARAM wParam, LPARAM lParam);


void mouseClick(LONG x, LONG y);
void kbClickDown(BYTE c);
void kbClickUp(BYTE c);
void kbClick(BYTE c);
double getScale();
BOOL startHookTool();
BOOL stopHookTool();
HANDLE getThreadHandle(HWND hwnd);

#endif