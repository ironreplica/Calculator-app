#pragma once
#include <string>
#include <map>
#include <cmath>
#include <vector>
#include <stack>
#include <stdexcept>

// Thrown when the expression can't be parsed (bad syntax)
struct ExpressionError : std::runtime_error {
	using std::runtime_error::runtime_error;
};

// Thrown when the expression parses but has no answer (1/0, square root of a negative, overflow)
struct MathError : ExpressionError {
	using ExpressionError::ExpressionError;
};

class ExpressionTree
{
public:
	struct Node {
		Node(std::string v) : Value(v), Left(NULL), Right(NULL) {} // initialization
		std::string Value;
		Node* Left, * Right; // left and right pointers for nodes
	};

	ExpressionTree(const std::string& str);
	~ExpressionTree(); 

	double Evaluate(ExpressionTree::Node* node = NULL) const;
	std::string Expression() const;

protected: // look into using lambdas here
	
	static double Add(double x, double y) { return x + y; }
	static double Subtract(double x, double y) { return x - y; }
	static double Multiply(double x, double y) { return x * y; }
	static double Divide(double x, double y) { return x / y; }
	static double Exponents(double x, double y) { return std::pow(x, y); }
	static double SquareRoot(double x, double y) { return std::sqrt(x); }
	static double Percentage(double x, double y) { return x / 100; }
	static double UnaryMinus(double x, double y) { return -x; }

	// this is a function pointer, return value first, then parameters 
	typedef double (*OperatorFunc)(double, double);

	// Binary: "x + y"   Prefix: "√x"   Postfix: "x%"   Bracket: "(", ")" and the "#" stack marker
	enum OperatorType { Binary, Prefix, Postfix, Bracket };

	struct OperatorInfo {
		OperatorInfo() {}
		OperatorInfo(int p, OperatorFunc f, OperatorType t) : Precedence(p), Func(f), Type(t) {}
		int Precedence;
		OperatorFunc Func;
		OperatorType Type;
	};

	typedef std::map<std::string, OperatorInfo> operator_map;
	void DeleteTree(Node* node);
	void FromString(const std::string& str); // what this for
	static std::vector<std::string> Tokenize(const std::string& str);
	static void PopOperator(std::stack<std::string>& operatorStack, std::stack<Node*>& operandStack);

	// longest expression accepted, keeps the recursion in Evaluate/DeleteTree shallow
	static const size_t MaxLength = 999999999999999999;

private:
	Node* root = nullptr;
	std::string expression;

	static operator_map operators;
};

