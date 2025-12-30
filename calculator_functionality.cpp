#include "calculator_functionality.h"
#include <windows.h>  // Optional if already included in header
#include <vector> //  Vector used to create a list
#include <iostream>
#include "Calculator_app.h"
#include "ExpressionTree.h"
#include <locale>
#include <codecvt>


HWND windowHandle;

void calculator_functionality::CreatePopup(HWND hwnd) {
    MessageBox(hwnd, L"Hello from CreatePopup!", L"Info", MB_OK);
}
/**
* @brief Inserts a char into the text box.
* @param character takes in a character to insert.
* @param hWnd takes in the HWND handle.
*/
void calculator_functionality::InsertChar(const wchar_t* character, HWND hWnd) {

    wchar_t selectedChar = character[0];

    if (!IsWindow(windowHandle)) {
        windowHandle = hWnd;
    }

    HWND hEdit = GetDlgItem(hWnd, 1000); // Get handle to edit control

    // ---------- handle multi-character buttons first ----------
    if (wcscmp(character, L"CE") == 0) { // works like C button for now
        // Clear current entry: set edit to "0"
        SetWindowTextW(hEdit, L" ");
        return;
    }

    if (wcscmp(character, L"C") == 0) {
        // Clear ALL: clear expression state and display
        expression.clear();          // your expression storage
        // if you track more state (storedValue, pendingOp, etc.), reset here
        SetWindowTextW(hEdit, L" ");
        return;
    }

    if (wcscmp(character, L"DEL") == 0) { // deletes last character 
        // Delete last character from the edit control
        int length = GetWindowTextLengthW(hEdit);
        if (length > 0) {
            std::wstring currentText(length, L'\0');
            GetWindowTextW(hEdit, &currentText[0], length + 1);

            if (!currentText.empty()) {
                currentText.pop_back();
                if (currentText.empty()) {
                    currentText = L" ";
                }
                SetWindowTextW(hEdit, currentText.c_str());
            }
        }
        return;
    }

    // Get the current text length
    int length = GetWindowTextLengthW(hEdit);
    switch (selectedChar) {
        // --- Special buttons ---
    case L'=':
        // Allocate buffer for current text; +1 for null terminator
        expression = (length, L'\0');

        Compute(GetCurrentExpression());
        return;
        break;
    case L'CE':
        ClearEntry();
        break;
        // --- Operators ---
    case L'×':
        // set character to *, now the expression tree can recognize it
        selectedChar = L'*';
        break;
    case L'÷':
        selectedChar = L'/';
        break;
    default:

        break;
    }
    // Default case for anything thats not part of the expression tree inherently

    // Get the current text length
    //int length = GetWindowTextLengthW(hEdit);

    // Allocate buffer for current text; +1 for null terminator
    std::wstring currentText(length, L'\0');

    GetWindowTextW(hEdit, &currentText[0], length + 1);

    // Convert single wide character to temp wide string
    std::wstring ws(1, selectedChar);

    // Convert the incoming wchar_t* to std::wstring
    std::wstring wTextToAppend(ws);

    // Append the incoming character(s)
    currentText += wTextToAppend;

    // Set the new text back to the edit control
    SetWindowTextW(hEdit, currentText.c_str());
}
std::wstring calculator_functionality::GetCurrentExpression() {
    HWND hEdit = GetDlgItem(windowHandle, 1000); // Get handle to edit control
    if (!hEdit) {
        return L"";
    }

    // Get the current text length (excluding null terminator)
    int length = GetWindowTextLengthW(hEdit);
    if (length <= 0) {
        return L"";
    }

    // Allocate buffer for current text; +1 for null terminator
    std::wstring text(length + 1, L'\0');

    // Read the text into the buffer
    GetWindowTextW(hEdit, &text[0], length + 1);

    // Remove the trailing null so the wstring has exactly 'length' characters
    text.resize(length);

    return text;
}

/**
*  @brief Clears the last item on the expression string.
*/
void calculator_functionality::ClearEntry()
{
    HWND hEdit = GetDlgItem(windowHandle, 1000); // Get handle to edit control
    
    // Getting current expression
    std::wstring curExpression = calculator_functionality::GetCurrentExpression();

    // Removing the last typed character.
    curExpression.pop_back();

    // Setting the window text
    SetWindowText(hEdit, curExpression.c_str());
}
/**
* @brief Pre processes the expression from a wstring and resets the text box.
* @param expression takes in a wide string expression to compute.
*/
std::string calculator_functionality::PreProcess(std::wstring expression)
{
    HWND hEdit = GetDlgItem(windowHandle, 1000); // Get handle to edit control

    // creating a converter object to convert UTF-16 wide strings (wstring) to UTF-8 narrow strings (string)
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    // converts the wstring into a string (UTF-8 bytes) for use in the expression tree
    std::string expressionString = converter.to_bytes(expression);
    std::cout << expressionString << std::endl;

    int depth = 0;
    for (char c : expressionString) {
        if (c == '(') {
            ++depth;
        }
        else if (c == ')') {
            if (depth == 0) {
                // More closing than opening → syntax error
                MessageBoxA(windowHandle, "Syntax error", "Error", MB_OK | MB_ICONERROR);
                return "Err";
            }
            --depth;
        }
    }
    if (depth != 0) {
        // Some '(' were never closed
        MessageBoxA(windowHandle, "Syntax error", "Error", MB_OK | MB_ICONERROR);
        return "Err";
    }
    return expressionString;
}

/**
* @brief Computes the expression from a wstring and resets the text box.
* @param expression takes in a wide string expression to compute.
*/
void calculator_functionality::Compute(std::wstring expression) {
    
    // Testing preprocessing, REMOVE THIS
    std::string processedStr = calculator_functionality::PreProcess(expression);

    // Get handle to edit control
    HWND hEdit = GetDlgItem(windowHandle, 1000); 

    if (processedStr == "Err") {
        
        SetWindowTextW(hEdit, L"SYNTAX ERROR");
        return;
    }
    else {
        // creating the expression tree object
        ExpressionTree exp(processedStr);

        // convert from a double to a wstring
        std::wstring wstringEvaluation = std::to_wstring(exp.Evaluate());

        // gets a constant pointer to the wstring null terminated buffer, makes it safe for win32 api calls
        LPCWSTR lpcwstrEvaluation = wstringEvaluation.c_str();

        // setting the edit text
        SetWindowTextW(hEdit, lpcwstrEvaluation);

        // console print, also re-evaulates the expresion tree
        std::cout << exp.Evaluate() << std::endl;
    }
}



