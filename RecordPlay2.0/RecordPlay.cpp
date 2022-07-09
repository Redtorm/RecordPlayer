// RecordPlay.cpp : Defines the entry point for the application.
//

#include "DlgProc.h"
HINSTANCE g_hDlg;
int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    // TODO: Place code here.
    g_hDlg = hInstance;
    //SetCursorPos(639, 447);
    DialogBox(g_hDlg, MAKEINTRESOURCE(IDD_DIALOG1), NULL, GET_DLGPROC(RecordDlg));
    return 0;
}



