// Calculator app.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Calculator_app.h"
#include "calculator_functionality.h"
#define ZERO_BUTTON 2000 // Unique ID for your button
#define ONE_BUTTON 2001 
#define TWO_BUTTON 2002 
#define THREE_BUTTON 2003 
#define FOUR_BUTTON 2004
#define FIVE_BUTTON 2005 
#define SIX_BUTTON 2006
#define SEVEN_BUTTON 2007 
#define EIGHT_BUTTON 2008
#define NINE_BUTTON 2009 
#include <io.h>
#include <fcntl.h>
#include "Calculator_app.h"

#define MAX_LOADSTRING 100

// Global Variables:
calculator_functionality calculator_func;
const int BUTTON_COUNT = 26; // this should be grabbed from the len of buttons
float buttonWidth = 200;
float buttonHeight = 40;
float textBoxHeight = 50;
const wchar_t* buttons[26] = {
    L"%", L"CE", L"C", L"DEL",
    L"1/x", L"x^2", L"√", L"÷",
    L"7", L"8", L"9", L"×",
    L"4", L"5", L"6", L"-",
    L"1", L"2", L"3", L"+",
    L"±", L"0", L".", L"=",
    L"(", L")"
};

// Add a variable to change calulate text size



HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    SetConsoleOutputCP(CP_UTF8); // Optional, for Unicode

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CALCULATORAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CALCULATORAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
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
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CALCULATORAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CALCULATORAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    switch (message)
    {

        case WM_CREATE:
        {
            HWND hInputBox = CreateWindowW(
                L"EDIT",                 // Class name for edit control
                L"",                     // Default text
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, // Styles for single-line input
                0, 0,                // X, Y position (tweak as needed)
                buttonWidth * 4, textBoxHeight,                 // Width, Height (tweak as needed)
                hWnd,                    // Parent window handle
                (HMENU)1000,             // ID for input box (pick a value out of button range)
                hInst,                   // Instance handle
                NULL                     // Pointer not needed
            );
            // find a way to scale this with the len of button arr
            for (int i = 0; i < 26; ++i) {
                CreateWindowW(
                    L"BUTTON",                         // Button class
                    buttons[i],                        // Text label
                    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    buttonWidth * (i % 4), i / 4 * buttonHeight + textBoxHeight, buttonWidth, buttonHeight,
                    hWnd,
                    (HMENU)(i),                 // Assign unique ID
                    hInst,
                    NULL
                );
            }

            //for (int i = 0; i < BUTTON_COUNT; ++i) {
            //    std::wstring buttonLabel = std::to_wstring(abs(i - 9));
            //    CreateWindowW(
            //        L"BUTTON",               // Button class
            //        buttonLabel.c_str(),                    // Text
            //        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles
            //        abs(i-9) == 0 ? buttonStartXPos - buttonWidth : buttonStartXPos - (i % 3) * buttonWidth, i / 3 * buttonHeight + textBoxHeight, buttonWidth, buttonHeight,
            //        hWnd,                    // Parent handle
            //        (HMENU)abs(i-9),     // Button ID
            //        hInst,                   // Instance handle
            //        NULL                     // No menu
            //    );
            //}
            
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            if (wmId >= 0 && wmId <= BUTTON_COUNT) {
                calculator_func.InsertChar(buttons[wmId], hWnd);

            }
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            //case :
            //    { // Handle button click
            //        calculator_func.InsertNumber(0);
            //        break; 
            //    }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
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



