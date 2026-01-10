#pragma once

#include <windows.h>  // Make sure this is included if HWND is used in the header
#include <string>
#include <vector> //  Vector used to create a list


class calculator_functionality {
public: 
	void CreatePopup(HWND hwnd);
	void InsertChar(const wchar_t* character, HWND hWnd);
private:
	std::vector<std::string> numberList;
	std::wstring expression;
	HWND windowHandle;
	void Compute(std::wstring expression);
	std::wstring GetCurrentExpression();
	void ClearEntry();
	void positivenegative();
	std::string PreProcess(std::wstring expression);
	std::wstring FormatResult(double value);
	void HandlePercentage();
	void HandleReciprocal();
	void HandleSquare();
	void HandleSquareRoot();
	double EvaluateCurrentNumber();
};
