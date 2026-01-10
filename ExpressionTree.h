#pragma once
#include <string>
#include <map>
#include <stdexcept>
#include <cmath>
#include <mutex>
class ExpressionTree
{
public:
	struct Node {
		Node(std::string v) : Value(v), Left(NULL), Right(NULL) {} // initialization
		std::string Value;
		Node* Left, * Right; // left and right pointers for nodes
	};
	// Constructor
	ExpressionTree(const std::string& str); // Takes in the string of expressions
	~ExpressionTree(); // deconstructor

	double Evaluate(ExpressionTree::Node* node = NULL) const;
	std::string Expression() const;

protected:
	static double Add(double x, double y) { return x + y; }
	static double Subtract(double x, double y) { return x - y; }
	static double Multiply(double x, double y) { return x * y; }
	static double Divide(double x, double y) {
		if (std::abs(y) < 1e-10) {
			throw std::runtime_error("Division by zero");
		}
		return x / y;
	}

	// this is a function pointer, return value first, then parameters 
	typedef double (*OperatorFunc)(double, double);
	struct OperatorInfo {
		OperatorInfo() {}
		OperatorInfo(int p, OperatorFunc f) : Precedence(p), Func(f) {}
		int Precedence;
		OperatorFunc Func;
	};

	// defining the map
	typedef std::map<std::string, OperatorInfo> operator_map;
	void FromString(const std::string& str); // what this for

private:
	Node* root; // holding the root nodes 
	std::string expression;
	static operator_map operators; // all of the operator info will go in here
	static std::mutex operatorsMutex; // Thread safety for operator map initialization
	
	void DeleteTree(Node* node); // Helper function to delete the tree
};

