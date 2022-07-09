#include "HookProc.h"

LPEVENTNODE g_pMsgList = NULL;
UINT uLen = 0;

//HHOOK g_hMouse;
//HHOOK g_hKeyboard;

//LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam)
//{
//	if (HC_ACTION == code)
//	{
//		
//	}
//	return ::CallNextHookEx(g_hMouse, code, wParam, lParam);
//}
//
//LRESULT CALLBACK LowLevelKeyBoardProc(int code, WPARAM wParam, LPARAM lParam)
//{
//	if (HC_ACTION == code)
//	{
//		
//	}
//	return ::CallNextHookEx(g_hKeyboard, code, wParam, lParam);
//}

void AddMsgList(EVENTNODE *pNewMsg)
{
	if (NULL == g_pMsgList)
	{
		g_pMsgList = pNewMsg;
		g_pMsgList->pNextMsg = NULL;
	}else
	{
		EVENTNODE *pTemp = g_pMsgList;
		while (NULL != pTemp->pNextMsg)
		{
			pTemp = pTemp->pNextMsg;
		}
		pNewMsg->pNextMsg = NULL;
		pTemp->pNextMsg = pNewMsg;
	}
}

void FreeMsgList()
{
	EVENTNODE *pTemp = g_pMsgList;
	while (NULL != pTemp)
	{
		g_pMsgList = pTemp->pNextMsg;

		delete pTemp;
		pTemp = g_pMsgList;
	}
	uLen = 0;
}