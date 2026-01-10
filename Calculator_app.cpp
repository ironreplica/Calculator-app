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

   // Calculate window size based on button layout
   int windowWidth = static_cast<int>(buttonWidth * 4) + 16; // 4 buttons wide + margins
   int windowHeight = static_cast<int>(textBoxHeight + (BUTTON_COUNT / 4 + 1) * buttonHeight) + 60; // Buttons + title bar
   
   // Center window on screen
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);
   int x = (screenWidth - windowWidth) / 2;
   int y = (screenHeight - windowHeight) / 2;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, 
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      x, y, windowWidth, windowHeight, 
      nullptr, nullptr, hInstance, nullptr);

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
            // Create the display/edit box with better styling
            HWND hInputBox = CreateWindowW(
                L"EDIT",                 // Class name for edit control
                L"0",                    // Default text
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_READONLY | ES_AUTOHSCROLL, 
                8, 8,                    // X, Y position with margin
                static_cast<int>(buttonWidth * 4) - 16, static_cast<int>(textBoxHeight), // Width, Height
                hWnd,                    // Parent window handle
                (HMENU)1000,             // ID for input box
                hInst,                   // Instance handle
                NULL                     // Pointer not needed
            );
            
            // Set font for better readability
            HFONT hFont = CreateFontW(
                static_cast<int>(textBoxHeight * 0.6),  // Height
                0,                                      // Width (0 = auto)
                0, 0,                                   // Escapement, Orientation
                FW_NORMAL,                              // Weight
                FALSE,                                  // Italic
                FALSE,                                  // Underline
                FALSE,                                  // Strikeout
                DEFAULT_CHARSET,                        // Charset
                OUT_DEFAULT_PRECIS,                     // Output precision
                CLIP_DEFAULT_PRECIS,                    // Clipping precision
                DEFAULT_QUALITY,                        // Quality
                DEFAULT_PITCH | FF_DONTCARE,            // Pitch and family
                L"Consolas"                             // Font name
            );
            SendMessage(hInputBox, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
            // Create buttons in a grid layout
            HFONT hButtonFont = CreateFontW(
                static_cast<int>(buttonHeight * 0.4),  // Height
                0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI"
            );
            
            for (int i = 0; i < BUTTON_COUNT; ++i) {
                HWND hButton = CreateWindowW(
                    L"BUTTON",                         // Button class
                    buttons[i],                        // Text label
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    8 + static_cast<int>(buttonWidth * (i % 4)),           // X position with margin
                    static_cast<int>(textBoxHeight + 16 + (i / 4) * buttonHeight), // Y position
                    static_cast<int>(buttonWidth - 4), static_cast<int>(buttonHeight - 4), // Slightly smaller with spacing
                    hWnd,
                    (HMENU)(i),                        // Assign unique ID
                    hInst,
                    NULL
                );
                SendMessage(hButton, WM_SETFONT, reinterpret_cast<WPARAM>(hButtonFont), TRUE);
            }

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
    case WM_KEYDOWN:
        {
            // Keyboard input support for calculator
            wchar_t keyChar = 0;
            bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            
            // Handle number keys and basic operators
            if (wParam >= '0' && wParam <= '9') {
                keyChar = static_cast<wchar_t>(wParam);
                calculator_func.InsertChar(&keyChar, hWnd);
            }
            else if (wParam == VK_OEM_PLUS || wParam == VK_ADD || (wParam == 0xBB && !shiftPressed)) {
                keyChar = L'+';
                calculator_func.InsertChar(&keyChar, hWnd);
            }
            else if (wParam == VK_OEM_MINUS || wParam == VK_SUBTRACT || (wParam == 0xBD && !shiftPressed)) {
                keyChar = L'-';
                calculator_func.InsertChar(&keyChar, hWnd);
            }
            else if (wParam == VK_MULTIPLY || (wParam == '8' && shiftPressed)) {
                keyChar = L'×';
                calculator_func.InsertChar(&keyChar, hWnd);
            }
            else if (wParam == VK_DIVIDE || (wParam == VK_OEM_2 && !shiftPressed)) {
                keyChar = L'÷';
                calculator_func.InsertChar(&keyChar, hWnd);
            }
            else if (wParam == VK_OEM_PERIOD || wParam == VK_DECIMAL) {
                keyChar = L'.';
                calculator_func.InsertChar(&keyChar, hWnd);
            }
            else if (wParam == VK_RETURN || wParam == VK_SEPARATOR) {
                keyChar = L'=';
                calculator_func.InsertChar(&keyChar, hWnd);
            }
            else if (wParam == VK_OEM_4 || wParam == 0x39) { // '9' key with shift or ')' key
                if (shiftPressed) {
                    keyChar = L'(';
                    calculator_func.InsertChar(&keyChar, hWnd);
                }
            }
            else if (wParam == VK_OEM_6 || wParam == 0x30) { // '0' key with shift or ')' key
                if (shiftPressed) {
                    keyChar = L')';
                    calculator_func.InsertChar(&keyChar, hWnd);
                }
            }
            else if (wParam == VK_BACK || wParam == VK_DELETE) {
                const wchar_t* del = L"DEL";
                calculator_func.InsertChar(del, hWnd);
            }
            else if (wParam == VK_ESCAPE || (wParam == 'C' && !shiftPressed)) {
                const wchar_t* clear = L"C";
                calculator_func.InsertChar(clear, hWnd);
            }
            return 0;
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



