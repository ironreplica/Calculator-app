#pragma once

#include <windows.h>  // Make sure this is included if HWND is used in the header
#include <string>
#include <vector> //  Vector used to create a list


class calculator_functionality {
public: 
		void CreatePopup(HWND hwnd);
		void InsertChar(const wchar_t* character, HWND hWnd);
		void InsertOperator(char op);
		
private:
	std::vector<std::string> numberList;
	std::wstring expression;
	HWND windowHandle;
	void Compute(std::wstring expression);
	std::wstring GetCurrentExpression();
	void ClearEntry();
};
