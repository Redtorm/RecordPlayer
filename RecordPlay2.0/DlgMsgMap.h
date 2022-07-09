#ifndef __DLGMSGMAP_H__
#define __DLGMSGMAP_H_
#include "windows.h"
typedef BOOL(*PDLGPROC)(HWND, WPARAM, LPARAM);

struct DLGMSGMAP
{
	UINT uMsg;
	PDLGPROC pDlgProc;
};

//ÉùÃ÷
#define DECLARE_DLGMSGMAP(DlgName) extern DLGMSGMAP g_##DlgName##MsgMap[];
#define DECLARE_DLGMSGPROC(DlgName) extern BOOL CALLBACK DlgName##Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//¶¨Òå
#define BEGIN_DLG_MSGMAP(DlgName) DLGMSGMAP g_##DlgName##MsgMap[] = {
#define ON_DLG_MSGMAP(DlgMsg, MsgProc) {DlgMsg, MsgProc},
#define END_DLG_MSGMAP() {WM_NULL, NULL}};

#define IMPLEMENT_DLGPROC(DlgName)\
	BOOL CALLBACK DlgName##Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)\
	{\
		DLGMSGMAP *pDlgMsgMap = g_##DlgName##MsgMap;\
		while (WM_NULL != pDlgMsgMap->uMsg)\
		{\
			if (uMsg == pDlgMsgMap->uMsg)\
			{\
				return pDlgMsgMap->pDlgProc(hDlg, wParam, lParam);\
			}\
			pDlgMsgMap++;\
		}\
		return FALSE;\
	}\

#define GET_DLGPROC(DlgName) DlgName##Proc

#endif