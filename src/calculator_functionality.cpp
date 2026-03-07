#include "include/calculator_functionality.h"
#include "include/ExpressionTree.h"

#include <iostream>
#include <vector> //  Vector used to create a list
#include <locale>
#include <codecvt>
#include <cmath> 
#include <string>

std::wstring calculator_functionality::GetCurrentExpression() 
{
    // insert code here...
}

std::string calculator_functionality::PreProcess(std::wstring expression)
{

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
                //! Add something to send to front end syntax error
                return "Err";
            }
            --depth;
        }
    }
    if (depth != 0) {
        // Some '(' were never closed
        // Add something to send to front end syntax error
        return "Err";
    }
    return expressionString;
}

void calculator_functionality::Compute(std::wstring expression) {
    
    // Testing preprocessing, REMOVE THIS
    std::string processedStr = calculator_functionality::PreProcess(expression);

    if (processedStr == "Err") {
        
        // Add something to send to front end syntax error
        return;
    }
    else {
        // creating the expression tree object
        ExpressionTree exp(processedStr);

        // convert from a double to a wstring
        std::wstring wstringEvaluation = std::to_wstring(exp.Evaluate());

        // gets a constant pointer to the wstring null terminated buffer, makes it safe for win32 api calls
        // LPCWSTR lpcwstrEvaluation = wstringEvaluation.c_str();

        // setting the edit text
        // SetWindowTextW(hEdit, lpcwstrEvaluation);

        // console print, also re-evaulates the expresion tree
        std::cout << exp.Evaluate() << std::endl;
    }
}



