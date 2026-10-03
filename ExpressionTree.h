#pragma once
#include <string>
#include <map>
#include <cmath>

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
	static double Percentage(double x, double y) { return x / 100 * 20; }
	// static double UnaryMinus(double a, double b) { return -a; }

	// this is a function pointer, return value first, then parameters 
	typedef double (*OperatorFunc)(double, double);
	struct OperatorInfo {
		OperatorInfo() {}
		OperatorInfo(int p, OperatorFunc f) : Precedence(p), Func(f) {}
		int Precedence;
		OperatorFunc Func;
	};

	typedef std::map<std::string, OperatorInfo> operator_map;
	void DeleteTree(Node* node);
	void FromString(const std::string& str); // what this for

private:
	Node* root; 
	std::string expression;

	static operator_map operators;
};

