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
void calculator_functionality::InsertChar(const wchar_t* character, HWND hWnd) {
     
    /* ----- Problems found -----
     * Problem 1: 
     * Code does not currently support negatives. Once you operate on a negative it breaks,
     * since the expression tree treats all negatives as the operator... 
     * 
     * Potential Fix:
     * Will likely have to add a new property with its own precedence for unary negative values.
     * 
     * Problem 2:
     * ClearEntry() does not operate like a normal calculator. CE needs to clear the last number typed.
     * Not the last character typed.
     * 
     * Potential Fix:
     * Some type of preprocessing to seperate the numbers. This will add complexity with the ExpressionTree.
     * 
     * Problem 3:
     * Unclosed parentheses cause crash. 
     * 
     * Potential Fix:
     * Add error handling code to remove unclosed parenthese before computing.
     * --------------------------
    */

    wchar_t selectedChar = character[0];

    if (!IsWindow(windowHandle)) {
        windowHandle = hWnd;
    }
    //if (!numberList.empty() && numberList.back() != "+" && numberList.back() != "-" && numberList.back() != "*" && numberList.back() != "/") {
    //    numberList.back() += std::to_string(num); // Appends the number to the last item
    //}
    //else {
    //    numberList.push_back(std::to_string(num)); // If empty, add as new item
    //}
    HWND hEdit = GetDlgItem(hWnd, 1000); // Get handle to edit control

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
/**
*  @brief Gets the current expression from the next box. 
* 
*/
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
* 
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


void calculator_functionality::Compute(std::wstring expression) {
    
    // Get handle to edit control
    HWND hEdit = GetDlgItem(windowHandle, 1000); 

    // creating a converter object to convert UTF-16 wide strings (wstring) to UTF-8 narrow strings (string)
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    // converts the wstring into a string (UTF-8 bytes) for use in the expression tree, since it expects narrow strings
    std::string expressionString = converter.to_bytes(expression);
  
    // creating the expression tree object
    ExpressionTree exp(expressionString);

    // convert from a double to a wstring
    std::wstring wstringEvaluation = std::to_wstring(exp.Evaluate());

    // gets a constant pointer to the wstring null terminated buffer, makes it safe for win32 api calls
    LPCWSTR lpcwstrEvaluation = wstringEvaluation.c_str();

    // setting the edit text
    SetWindowTextW(hEdit, lpcwstrEvaluation);
    
    // console print, also re-evaulates the expresion tree
    std::cout << exp.Evaluate() << std::endl;

}



