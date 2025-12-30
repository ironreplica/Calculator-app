#include "ExpressionTree.h"
#include <stack>
#include <string>
#include <map>
#include <sstream>

ExpressionTree::operator_map ExpressionTree::operators;
ExpressionTree::ExpressionTree(const std::string& str)
{
	if (operators.empty()) {
		// populate list when the operator list is empty. Not thread safe operation
		// make thread safe
		operators["+"] = ExpressionTree::OperatorInfo(0, ExpressionTree::Add);
		operators["-"] = ExpressionTree::OperatorInfo(0, ExpressionTree::Subtract);
		operators["*"] = ExpressionTree::OperatorInfo(1, ExpressionTree::Multiply);
		operators["/"] = ExpressionTree::OperatorInfo(1, ExpressionTree::Divide);
		//operators["u-"] = ExpressionTree::OperatorInfo(2, ExpressionTree::UnaryMinus);
		operators["("] = ExpressionTree::OperatorInfo(-1, NULL);
		operators[")"] = ExpressionTree::OperatorInfo(-1, NULL);
		operators["#"] = ExpressionTree::OperatorInfo(-1, NULL);

	}
	FromString(str);
}

// Deconstructor
ExpressionTree::~ExpressionTree()
{
}

double ExpressionTree::Evaluate(ExpressionTree::Node* node) const
{
	if (node == NULL) node = root; // what is root

	operator_map::iterator it = operators.find(node->Value);
	if (it != operators.end()) {
		// we know we have found an operator, do operation on the two numbers
		return (it->second.Func)(Evaluate(node->Left), Evaluate(node->Right)); // gets the pointer to the function, parenthesis is how you call a function pointer
	}
	// if we didnt find an operator, dont create a new tree
	std::istringstream ss(node->Value);
	double val;
	ss >> val;

	return val;
}

std::string ExpressionTree::Expression() const
{
	return std::string();
}

void AddWhitespace(int idx, int insertAt, std::string& str) {
	// look into this if statement
	if (idx >= 0 && insertAt >= 0 && idx < str.length() && insertAt < str.length() && str[idx] != ' ') {
		str.insert(insertAt, 1, ' ');
	}
}
void PopOperator(std::stack<std::string> &operatorStack, std::stack<ExpressionTree::Node*> &operandStack) {
	ExpressionTree::Node* n = new ExpressionTree::Node(operatorStack.top());
	operatorStack.pop();

	n->Right = operandStack.top();
	operandStack.pop();
	n->Left = operandStack.top();
	operandStack.pop();

	operandStack.push(n);
}
void ExpressionTree::FromString(const std::string &expressionString)
{
	std::string str(expressionString);

	// 1) Insert 0 before truly unary minus: "-6" -> "0-6", "(-6" -> "(0-6", "*-6" -> "*0-6"
	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == '-') {
			bool isUnary =
				(i == 0) ||
				(str[i - 1] == '(') ||
				(operators.find(std::string(1, str[i - 1])) != operators.end());

			if (isUnary) {
				size_t start = i;       // position of '-'
				size_t j = i + 1;

				// skip whitespace
				while (j < str.size() && str[j] == ' ') j++;

				// read number
				while (j < str.size() && (isdigit(str[j]) || str[j] == '.')) j++;

				// wrap "-number" as "(-number)"
				str.insert(j, ")");
				str.insert(start, "(");

				i += 1; // move past '('
			}
		}
	}

	

	// need more explanation on size_t
	for (size_t i = 0; i < str.length(); ++i) {
		std::string op;
		op.push_back(str[i]); // what this do
		if (operators.find(op) != operators.end()) {

			// DON'T split negative numbers
			if (str[i] == '-') {
				bool partOfNumber =
					(i + 1 < str.length() && isdigit(str[i + 1])) &&
					(i == 0 || str[i - 1] == '(' || str[i - 1] == ' ');

				if (partOfNumber)
					continue;
			}

			AddWhitespace(i + 1, i + 1, str);
			AddWhitespace(i - 1, i, str);
		}

	}

	std::stack<std::string> operatorStack;
	operatorStack.push("#");
	std::stack<Node*> operandStack;

	// most important part of the code
	std::istringstream ss(str);
	while (!ss.eof())
	{
		std::string s;
		ss >> s;

		if (operators.find(s) != operators.end()) {
			if (s == "(") {
				operatorStack.push(s);
			}
			else if (s == ")") {
				while (operatorStack.top() != "(") {
					PopOperator(operatorStack, operandStack);
				}
				operatorStack.pop(); // seperates parentheses into its own string

			}
			else if (operators[operatorStack.top()].Precedence >= operators[s].Precedence) {
				PopOperator(operatorStack, operandStack);
				operatorStack.push(s);
			}
			else {
				// operator had a higher precedence evaluate this first
				operatorStack.push(s);
			}
		}
		else {
			// operand value
			Node* n = new Node(s);
			operandStack.push(n);
		}
	}

	// special character
	while (operatorStack.top() != "#") {
		PopOperator(operatorStack, operandStack);
	}

	root = operandStack.top();
}

