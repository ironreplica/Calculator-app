#include "calculator_functionality.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "Calculator_app.h"
#include "ExpressionTree.h"
#include <locale>
#include <codecvt>
#include <stdexcept>


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
    if (wcscmp(character, L"CE") == 0) {
        // Clear current entry: set edit to "0"
        SetWindowTextW(hEdit, L"0");
        return;
    }

    if (wcscmp(character, L"C") == 0) {
        // Clear ALL: clear expression state and display
        expression.clear();
        SetWindowTextW(hEdit, L"0");
        return;
    }

    // Handle special mathematical operations
    if (wcscmp(character, L"%") == 0) {
        HandlePercentage();
        return;
    }

    if (wcscmp(character, L"1/x") == 0) {
        HandleReciprocal();
        return;
    }

    if (wcscmp(character, L"x^2") == 0) {
        HandleSquare();
        return;
    }

    if (wcscmp(character, L"√") == 0) {
        HandleSquareRoot();
        return;
    }

    if (wcscmp(character, L"DEL") == 0) {
        // Delete last character from the edit control
        std::wstring currentText = GetCurrentExpression();
        if (currentText.length() > 1 && currentText != L"0") {
            currentText.pop_back();
            SetWindowTextW(hEdit, currentText.empty() ? L"0" : currentText.c_str());
        } else {
            SetWindowTextW(hEdit, L"0");
        }
        return;
    }

    // Handle special character conversions
    switch (selectedChar) {
        // --- Operators ---
    case L'×':
        // set character to *, now the expression tree can recognize it
        selectedChar = L'*';
        break;
    case L'÷':
        selectedChar = L'/';
        break;
    case L'=':
        Compute(GetCurrentExpression());
        return;
    case L'±':
        positivenegative();
        return;
    default:
        break; 
    }
    // Default case: append character to expression
    std::wstring currentText = GetCurrentExpression();
    
    // If display shows "0" or is empty and user enters a number, replace it
    if ((currentText == L"0" || currentText.empty()) && iswdigit(selectedChar)) {
        currentText = std::wstring(1, selectedChar);
        SetWindowTextW(hEdit, currentText.c_str());
        return;
    }
    
    // Prevent multiple decimal points in the same number
    if (selectedChar == L'.') {
        // Find the last number (from end, up to an operator)
        bool hasDecimal = false;
        for (int i = static_cast<int>(currentText.length()) - 1; i >= 0; --i) {
            if (currentText[i] == L'.') {
                hasDecimal = true;
                break;
            }
            if (currentText[i] == L'+' || currentText[i] == L'-' || 
                currentText[i] == L'*' || currentText[i] == L'/' ||
                currentText[i] == L'×' || currentText[i] == L'÷' ||
                currentText[i] == L'(' || currentText[i] == L')') {
                break;
            }
        }
        if (hasDecimal) {
            return; // Don't add another decimal point
        }
    }

    // Append the character
    currentText += selectedChar;
    SetWindowTextW(hEdit, currentText.c_str());
}
std::wstring calculator_functionality::GetCurrentExpression() {
    HWND hEdit = GetDlgItem(windowHandle, 1000); // Get handle to edit control
    if (!hEdit) {
        return L"0";
    }

    // Get the current text length (excluding null terminator)
    int length = GetWindowTextLengthW(hEdit);
    if (length <= 0) {
        return L"0";
    }

    // Allocate buffer for current text; +1 for null terminator
    std::wstring text(length + 1, L'\0');

    // Read the text into the buffer
    GetWindowTextW(hEdit, &text[0], length + 1);

    // Remove the trailing null so the wstring has exactly 'length' characters
    text.resize(length);
    
    // Return "0" if empty or just whitespace
    if (text.empty() || text.find_first_not_of(L" \t\n\r") == std::wstring::npos) {
        return L"0";
    }

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
void calculator_functionality::positivenegative()
{
    HWND hEdit = GetDlgItem(windowHandle, 1000); // Get handle to edit control

    std::wstring expr = calculator_functionality::GetCurrentExpression();
    if (expr.empty())
        return;

    // 1. Find start of last number: scan backward until an operator or start
    int i = static_cast<int>(expr.size()) - 1;

    while (i >= 0 && iswspace(expr[i]))
        --i;

    if (i < 0)
        return;

    // Move left while characters are part of the number (digits, dot, maybe others)
    while (i >= 0 && (iswdigit(expr[i]) || expr[i] == L'.'))
        --i;

    // Now i is at: -1, or an operator, or an existing sign before this number
    int numberStart = i + 1;
    if (numberStart >= static_cast<int>(expr.size()))
        return; // no number found

    // 2. Check if there is already a unary minus *immediately* before the number
    bool hasUnaryMinus = (i >= 0 && expr[i] == L'-');

    std::wstring lastNumber = expr.substr(numberStart);

    if (lastNumber == L"0" || lastNumber == L"-0")
    {
        //don't toggle 0 to -0
        return;
    }

    if (hasUnaryMinus)
    {
        // Remove the unary minus
        expr.erase(i, 1); // remove that '-'
    }
    else
    {
        // Insert a unary minus right before this number
        expr.insert(numberStart, 1, L'-');
    }

    SetWindowText(hEdit, expr.c_str());
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
* @brief Formats a double value to a string with proper decimal handling
* @param value The double value to format
* @return Formatted wide string
*/
std::wstring calculator_functionality::FormatResult(double value) {
    // Handle infinity and NaN
    if (!std::isfinite(value)) {
        if (std::isnan(value)) {
            return L"NaN";
        }
        if (value > 0) {
            return L"∞";
        }
        return L"-∞";
    }

    // Convert to string and remove trailing zeros
    std::wstring result = std::to_wstring(value);
    
    // Remove trailing zeros and unnecessary decimal point
    size_t pos = result.find_last_not_of(L"0");
    if (pos != std::wstring::npos && result[pos] == L'.') {
        pos--; // Remove decimal point if no fractional part
    }
    if (pos != std::wstring::npos && pos < result.length() - 1) {
        result = result.substr(0, pos + 1);
    }

    // For very large or small numbers, use scientific notation
    if (std::abs(value) >= 1e10 || (std::abs(value) < 1e-4 && value != 0.0)) {
        std::wostringstream oss;
        oss << std::scientific << std::setprecision(10) << value;
        std::wstring sci = oss.str();
        // Clean up scientific notation
        size_t ePos = sci.find(L'e');
        if (ePos != std::wstring::npos) {
            std::wstring base = sci.substr(0, ePos);
            std::wstring exp = sci.substr(ePos + 1);
            // Remove trailing zeros from base
            size_t basePos = base.find_last_not_of(L"0");
            if (basePos != std::wstring::npos && base[basePos] == L'.') {
                basePos--;
            }
            if (basePos != std::wstring::npos && basePos < base.length() - 1) {
                base = base.substr(0, basePos + 1);
            }
            // Remove leading zero from exponent if present
            if (exp.length() > 1 && exp[0] == L'0' && exp[1] != L'-') {
                exp = exp.substr(1);
            }
            result = base + L"e" + exp;
        }
    }

    return result;
}

/**
* @brief Computes the expression from a wstring and displays the result.
* @param expression takes in a wide string expression to compute.
*/
void calculator_functionality::Compute(std::wstring expression) {
    std::string processedStr = PreProcess(expression);
    HWND hEdit = GetDlgItem(windowHandle, 1000); 

    if (processedStr == "Err" || processedStr.empty()) {
        SetWindowTextW(hEdit, L"SYNTAX ERROR");
        return;
    }

    try {
        ExpressionTree exp(processedStr);
        double result = exp.Evaluate();
        std::wstring formattedResult = FormatResult(result);
        SetWindowTextW(hEdit, formattedResult.c_str());
        std::cout << "Result: " << result << std::endl;
    }
    catch (const std::exception& e) {
        SetWindowTextW(hEdit, L"MATH ERROR");
        std::cout << "Error: " << e.what() << std::endl;
    }
    catch (...) {
        SetWindowTextW(hEdit, L"ERROR");
    }
}

/**
* @brief Evaluates the last number in the current expression
* @return The numeric value of the last number, or 0 if invalid
*/
double calculator_functionality::EvaluateCurrentNumber() {
    std::wstring expr = GetCurrentExpression();
    if (expr.empty()) {
        return 0.0;
    }

    // Find the last number in the expression
    int i = static_cast<int>(expr.size()) - 1;
    while (i >= 0 && iswspace(expr[i])) {
        --i;
    }

    if (i < 0) {
        return 0.0;
    }

    // Move left while characters are part of the number
    int numberStart = i + 1;
    while (i >= 0 && (iswdigit(expr[i]) || expr[i] == L'.' || expr[i] == L'-' || expr[i] == L'+')) {
        --i;
    }
    numberStart = i + 1;

    if (numberStart >= static_cast<int>(expr.size())) {
        return 0.0;
    }

    std::wstring lastNumber = expr.substr(numberStart);
    
    // Handle unary minus
    if (numberStart > 0 && expr[numberStart - 1] == L'-') {
        bool isUnary = (numberStart == 1) || 
                       (numberStart > 1 && (expr[numberStart - 2] == L'(' || 
                                            expr[numberStart - 2] == L'+' || 
                                            expr[numberStart - 2] == L'-' || 
                                            expr[numberStart - 2] == L'*' || 
                                            expr[numberStart - 2] == L'/' ||
                                            expr[numberStart - 2] == L'×' || 
                                            expr[numberStart - 2] == L'÷'));
        if (isUnary) {
            lastNumber = L"-" + lastNumber;
        }
    }

    try {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::string numStr = converter.to_bytes(lastNumber);
        return std::stod(numStr);
    }
    catch (...) {
        return 0.0;
    }
}

/**
* @brief Handles percentage operation: divides current number by 100
*/
void calculator_functionality::HandlePercentage() {
    HWND hEdit = GetDlgItem(windowHandle, 1000);
    std::wstring expr = GetCurrentExpression();

    if (expr.empty()) {
        SetWindowTextW(hEdit, L"0");
        return;
    }

    // Evaluate the entire expression, then divide by 100
    try {
        std::string processedStr = PreProcess(expr);
        if (processedStr == "Err" || processedStr.empty()) {
            SetWindowTextW(hEdit, L"SYNTAX ERROR");
            return;
        }

        ExpressionTree exp(processedStr);
        double result = exp.Evaluate() / 100.0;
        std::wstring formattedResult = FormatResult(result);
        SetWindowTextW(hEdit, formattedResult.c_str());
    }
    catch (...) {
        SetWindowTextW(hEdit, L"MATH ERROR");
    }
}

/**
* @brief Handles reciprocal operation: 1/x
*/
void calculator_functionality::HandleReciprocal() {
    HWND hEdit = GetDlgItem(windowHandle, 1000);
    std::wstring expr = GetCurrentExpression();

    if (expr.empty()) {
        SetWindowTextW(hEdit, L"MATH ERROR");
        return;
    }

    try {
        std::string processedStr = PreProcess(expr);
        if (processedStr == "Err" || processedStr.empty()) {
            SetWindowTextW(hEdit, L"SYNTAX ERROR");
            return;
        }

        ExpressionTree exp(processedStr);
        double value = exp.Evaluate();
        
        if (std::abs(value) < 1e-10) {
            SetWindowTextW(hEdit, L"DIVIDE BY ZERO");
            return;
        }

        double result = 1.0 / value;
        std::wstring formattedResult = FormatResult(result);
        SetWindowTextW(hEdit, formattedResult.c_str());
    }
    catch (...) {
        SetWindowTextW(hEdit, L"MATH ERROR");
    }
}

/**
* @brief Handles square operation: x^2
*/
void calculator_functionality::HandleSquare() {
    HWND hEdit = GetDlgItem(windowHandle, 1000);
    std::wstring expr = GetCurrentExpression();

    if (expr.empty()) {
        SetWindowTextW(hEdit, L"0");
        return;
    }

    try {
        std::string processedStr = PreProcess(expr);
        if (processedStr == "Err" || processedStr.empty()) {
            SetWindowTextW(hEdit, L"SYNTAX ERROR");
            return;
        }

        ExpressionTree exp(processedStr);
        double value = exp.Evaluate();
        double result = value * value;
        std::wstring formattedResult = FormatResult(result);
        SetWindowTextW(hEdit, formattedResult.c_str());
    }
    catch (...) {
        SetWindowTextW(hEdit, L"MATH ERROR");
    }
}

/**
* @brief Handles square root operation: √x
*/
void calculator_functionality::HandleSquareRoot() {
    HWND hEdit = GetDlgItem(windowHandle, 1000);
    std::wstring expr = GetCurrentExpression();

    if (expr.empty()) {
        SetWindowTextW(hEdit, L"0");
        return;
    }

    try {
        std::string processedStr = PreProcess(expr);
        if (processedStr == "Err" || processedStr.empty()) {
            SetWindowTextW(hEdit, L"SYNTAX ERROR");
            return;
        }

        ExpressionTree exp(processedStr);
        double value = exp.Evaluate();
        
        if (value < 0) {
            SetWindowTextW(hEdit, L"INVALID INPUT");
            return;
        }

        double result = std::sqrt(value);
        std::wstring formattedResult = FormatResult(result);
        SetWindowTextW(hEdit, formattedResult.c_str());
    }
    catch (...) {
        SetWindowTextW(hEdit, L"MATH ERROR");
    }
}

