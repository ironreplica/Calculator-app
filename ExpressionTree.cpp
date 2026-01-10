#include "ExpressionTree.h"
#include <stack>
#include <string>
#include <map>
#include <sstream>
#include <mutex>

ExpressionTree::operator_map ExpressionTree::operators;
std::mutex ExpressionTree::operatorsMutex;

ExpressionTree::ExpressionTree(const std::string& str)
{
	// Thread-safe initialization of operators map
	std::lock_guard<std::mutex> lock(operatorsMutex);
	if (operators.empty()) {
		operators["+"] = ExpressionTree::OperatorInfo(0, ExpressionTree::Add);
		operators["-"] = ExpressionTree::OperatorInfo(0, ExpressionTree::Subtract);
		operators["*"] = ExpressionTree::OperatorInfo(1, ExpressionTree::Multiply);
		operators["/"] = ExpressionTree::OperatorInfo(1, ExpressionTree::Divide);
		operators["("] = ExpressionTree::OperatorInfo(-1, NULL);
		operators[")"] = ExpressionTree::OperatorInfo(-1, NULL);
		operators["#"] = ExpressionTree::OperatorInfo(-1, NULL);
	}
	FromString(str);
}

// Destructor - properly deletes the tree to prevent memory leaks
ExpressionTree::~ExpressionTree()
{
	DeleteTree(root);
}

void ExpressionTree::DeleteTree(Node* node)
{
	if (node == nullptr) {
		return;
	}
	DeleteTree(node->Left);
	DeleteTree(node->Right);
	delete node;
}

double ExpressionTree::Evaluate(ExpressionTree::Node* node) const
{
	if (node == nullptr) {
		node = root;
	}

	if (node == nullptr) {
		throw std::runtime_error("Empty expression tree");
	}

	operator_map::iterator it = operators.find(node->Value);
	if (it != operators.end()) {
		// Found an operator, recursively evaluate left and right subtrees
		double leftVal = Evaluate(node->Left);
		double rightVal = Evaluate(node->Right);
		return (it->second.Func)(leftVal, rightVal);
	}
	
	// Found a number, convert and return
	std::istringstream ss(node->Value);
	double val;
	if (!(ss >> val)) {
		throw std::runtime_error("Invalid number: " + node->Value);
	}

	return val;
}

std::string ExpressionTree::Expression() const
{
	return std::string();
}

void AddWhitespace(int idx, int insertAt, std::string& str) {
	if (idx >= 0 && insertAt >= 0 && idx < static_cast<int>(str.length()) && 
	    insertAt < static_cast<int>(str.length()) && str[idx] != ' ') {
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
				(operators.find(std::string(1, str[i - 1])) != operators.end() && str[i - 1] != ')');

			if (isUnary) {
				// Insert "0" before '-'
				str.insert(i, "0");
				i += 1; // skip the inserted 0
			}
		}
	}

	

	// Add whitespace around operators for tokenization
	for (size_t i = 0; i < str.length(); ++i) {
		std::string op;
		op.push_back(str[i]);
		if (operators.find(op) != operators.end()) {
			// DON'T split negative numbers
			if (str[i] == '-') {
				bool partOfNumber =
					(i + 1 < str.length() && isdigit(str[i + 1])) &&
					(i == 0 || str[i - 1] == '(' || str[i - 1] == ' ');

				if (partOfNumber)
					continue;
			}

			AddWhitespace(static_cast<int>(i + 1), static_cast<int>(i + 1), str);
			if (i > 0) {
				AddWhitespace(static_cast<int>(i - 1), static_cast<int>(i), str);
			}
		}
	}

	std::stack<std::string> operatorStack;
	operatorStack.push("#");
	std::stack<Node*> operandStack;

	// Parse expression using Shunting Yard algorithm
	std::istringstream ss(str);
	std::string s;
	while (ss >> s) {
		if (operators.find(s) != operators.end()) {
			if (s == "(") {
				operatorStack.push(s);
			}
			else if (s == ")") {
				while (!operatorStack.empty() && operatorStack.top() != "(") {
					if (operandStack.size() < 2) {
						throw std::runtime_error("Invalid expression: insufficient operands");
					}
					PopOperator(operatorStack, operandStack);
				}
				if (operatorStack.empty() || operatorStack.top() != "(") {
					throw std::runtime_error("Invalid expression: mismatched parentheses");
				}
				operatorStack.pop();
			}
			else {
				while (!operatorStack.empty() && operatorStack.top() != "#" && 
				       operatorStack.top() != "(" &&
				       operators[operatorStack.top()].Precedence >= operators[s].Precedence) {
					if (operandStack.size() < 2) {
						throw std::runtime_error("Invalid expression: insufficient operands");
					}
					PopOperator(operatorStack, operandStack);
				}
				operatorStack.push(s);
			}
		}
		else {
			// operand value
			if (s.empty()) continue;
			Node* n = new Node(s);
			operandStack.push(n);
		}
	}

	// Process remaining operators
	while (!operatorStack.empty() && operatorStack.top() != "#") {
		if (operatorStack.top() == "(") {
			throw std::runtime_error("Invalid expression: unclosed parentheses");
		}
		if (operandStack.size() < 2) {
			throw std::runtime_error("Invalid expression: insufficient operands");
		}
		PopOperator(operatorStack, operandStack);
	}

	if (operandStack.empty()) {
		throw std::runtime_error("Invalid expression: no operands");
	}

	root = operandStack.top();
	operandStack.pop();
	
	if (!operandStack.empty()) {
		throw std::runtime_error("Invalid expression: too many operands");
	}
}

