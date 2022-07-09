#ifndef __HOOKPROC_H__
#define __HOOKPROC_H__
#include "windows.h"
struct EVENTNODE
{
	WPARAM wParam;
	LONG mouseX;
	LONG mouseY;
	char keyCode;
	EVENTNODE* pNextMsg;
};

typedef EVENTNODE* LPEVENTNODE;

//LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK LowLevelKeyBoardProc(int code, WPARAM wParam, LPARAM lParam);


void AddMsgList(EVENTNODE *pNewMsg);
void FreeMsgList();

//extern HHOOK g_hMouse;
//extern HHOOK g_hKeyboard;
extern LPEVENTNODE g_pMsgList;//����ͷָ��
extern UINT uLen;//������

#endif