// MabinogiPackageTool.Htmlayout.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MabinogiPackageTool.Htmlayout.h"

#include "htmlayout.h"

#pragma comment(lib, "htmlayout")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

LPCSTR lpszHtml = "<html><body>Hellow World<table border=\"1\"><tr><td>11111</td><td>11111</td></tr><tr><td>222</td><td>222</td></tr></table></body></html>";

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_MABINOGIPACKAGETOOLHTMLAYOUT, szWindowClass, MAX_LOADSTRING);
	lstrcpy(szWindowClass, HTMLayoutClassNameT());
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MABINOGIPACKAGETOOLHTMLAYOUT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MABINOGIPACKAGETOOLHTMLAYOUT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MABINOGIPACKAGETOOLHTMLAYOUT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   //hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   UINT style = WS_POPUP | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;
   hWnd = CreateWindowExW(WS_EX_LAYERED, szWindowClass, NULL, style,
	   100, 100, 640, 480, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   

   HTMLayoutLoadHtml(hWnd, (LPCBYTE)lpszHtml, lstrlenA(lpszHtml) + 1);


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	LRESULT lResult;
	BOOL    bHandled;

	// HTMLayout +
	// HTMLayout could be created as separate window 
	// using CreateWindow API.
	// But in this case we are attaching HTMLayout functionality
	// to the existing window delegating windows message handling to 
	// HTMLayoutProcND function.
	lResult = HTMLayoutProcND(hWnd, message, wParam, lParam, &bHandled);
	if (bHandled)
		return lResult;

	switch (message)
	{
	case WM_NCHITTEST:
		//if (me)
		//	return me->hit_test(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_NCCALCSIZE:  return 0; // we have no non-client areas.
	case WM_NCPAINT:     return 0; // we have no non-client areas.
	case WM_NCACTIVATE:  return (wParam == 0) ? TRUE : FALSE; // we have no non-client areas.

	case WM_GETMINMAXINFO:
	{
		LRESULT lr = DefWindowProcW(hWnd, message, wParam, lParam);
		MINMAXINFO* pmmi = (MINMAXINFO*)lParam;
		pmmi->ptMinTrackSize.x = ::HTMLayoutGetMinWidth(hWnd);
		RECT rc; GetWindowRect(hWnd, &rc);
		pmmi->ptMinTrackSize.y = ::HTMLayoutGetMinHeight(hWnd, rc.right - rc.left);
		return lr;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


//int       window::hit_test(int x, int y)
//{
//
//	POINT pt; pt.x = x; pt.y = y;
//	::MapWindowPoints(HWND_DESKTOP, hwnd, &pt, 1);
//
//	if (caption.is_valid() && caption.is_inside(pt))
//		return HTCAPTION;
//
//	if (button_icon.is_valid() && button_icon.is_inside(pt))
//		return HTSYSMENU;
//
//	if (corner.is_valid() && corner.is_inside(pt))
//		return HTBOTTOMRIGHT;
//
//	RECT body_rc = body.get_location(ROOT_RELATIVE | CONTENT_BOX);
//
//	if (PtInRect(&body_rc, pt))
//		return HTCLIENT;
//
//	if (pt.y < body_rc.top + 10)
//	{
//		if (pt.x < body_rc.left + 10)
//			return HTTOPLEFT;
//		if (pt.x > body_rc.right - 10)
//			return HTTOPRIGHT;
//	}
//	else if (pt.y > body_rc.bottom - 10)
//	{
//		if (pt.x < body_rc.left + 10)
//			return HTBOTTOMLEFT;
//		if (pt.x > body_rc.right - 10)
//		{
//			htmlayout::debug_output_console dc;
//			dc.printf("HTBOTTOMRIGHT\n");
//
//			return HTBOTTOMRIGHT;
//		}
//	}
//
//	if (pt.y < body_rc.top)
//		return HTTOP;
//	if (pt.y > body_rc.bottom)
//		return HTBOTTOM;
//	if (pt.x < body_rc.left)
//		return HTLEFT;
//	if (pt.x > body_rc.right)
//		return HTRIGHT;
//
//
//	return HTCLIENT;
//
//}