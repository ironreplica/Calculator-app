#pragma once

#include <string>
#include <vector> //  Vector used to create a list


class calculator_functionality {
public: 
	void CreatePopup();
	void InsertChar();
	std::string Compute(std::wstring expression);
	std::wstring GetCurrentExpression();
	std::string PreProcess(std::wstring expression);
private:
	std::string ConvertString(std::wstring wstrToConvert);

};
