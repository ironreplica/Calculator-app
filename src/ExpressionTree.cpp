#include "include/ExpressionTree.h"
#include <stack>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <cctype>

ExpressionTree::operator_map ExpressionTree::operators;

// === Constructor ===
ExpressionTree::ExpressionTree(const std::string& str) {
    std::cout << "CONSTRUCTOR CALLED with: '" << str << "'\n";

    // Initialize static operator table if not already set
    if (operators.empty()) {
        std::cout << "Initializing operators...\n";

        operators["+"] = OperatorInfo(1, Add);
        operators["-"] = OperatorInfo(1, Subtract);
        operators["*"] = OperatorInfo(2, Multiply);
        operators["/"] = OperatorInfo(2, Divide);
        operators["^"] = OperatorInfo(3, Exponents);
        operators["√"] = OperatorInfo(3, SquareRoot);
        operators["("] = OperatorInfo(-2, nullptr);
        operators[")"] = OperatorInfo(-2, nullptr);
        operators["#"] = OperatorInfo(-2, nullptr);
    }

    std::cout << "About to call FromString\n";
    FromString(str);
    std::cout << "FromString finished. root=" << (root ? "NOT NULL" : "NULL") << "\n";
}

// === Destructor ===
ExpressionTree::~ExpressionTree() {
    DeleteTree(root);
    root = nullptr;
}

// === Tree deletion ===
void ExpressionTree::DeleteTree(Node* node) {
    if (!node) return;

    DeleteTree(node->Left);
    DeleteTree(node->Right);
    delete node;
}

// === Evaluation ===
double ExpressionTree::Evaluate(Node* node) const {
    node = node ? node : root;

    auto it = operators.find(node->Value);
    if (it != operators.end()) {
        double left = node->Left ? Evaluate(node->Left) : 0.0;
        double right = node->Right ? Evaluate(node->Right) : 0.0;
        return (it->second.Func)(left, right);
    }

    std::istringstream ss(node->Value);
    double val;
    ss >> val;
    return val;
}

// === Expression representation (to be implemented) ===
std::string ExpressionTree::Expression() const {
    return {};
}

// === Helper: Add whitespace around operators ===
void AddWhitespace(int idx, int insertAt, std::string& str) {
    if (idx >= 0 && insertAt >= 0 &&
        idx < static_cast<int>(str.length()) &&
        insertAt < static_cast<int>(str.length()) &&
        str[idx] != ' ')
    {
        str.insert(insertAt, 1, ' ');
    }
}

// === Helper: Pop operator from stack and form new node ===
void PopOperator(std::stack<std::string>& operatorStack,
    std::stack<ExpressionTree::Node*>& operandStack) {
    std::string op = operatorStack.top();
    operatorStack.pop();

    ExpressionTree::Node* n = new ExpressionTree::Node(op);

    if (op == "√") { // unary operator
        n->Left = operandStack.top();
        operandStack.pop();
        n->Right = nullptr;
    }
    else { // binary operator
        n->Right = operandStack.top();
        operandStack.pop();
        n->Left = operandStack.top();
        operandStack.pop();
    }

    operandStack.push(n);
}

// === Core parsing logic ===
void ExpressionTree::FromString(const std::string& expressionString) {
    std::string str(expressionString);

    // Insert 0 before unary minus
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '-') {
            bool isUnary =
                (i == 0) ||
                (str[i - 1] == '(') ||
                (operators.find(std::string(1, str[i - 1])) != operators.end() && str[i - 1] != ')');

            if (isUnary) {
                str.insert(i, "0");
                i += 1;
            }
        }
    }

    // Add spaces around operators for tokenization
    for (size_t i = 0; i < str.length(); ++i) {
        std::string op(1, str[i]);
        if (operators.find(op) != operators.end()) {
            // Skip minus if part of a number
            if (str[i] == '-') {
                bool isPartOfNumber =
                    (i + 1 < str.length() && isdigit(str[i + 1])) &&
                    (i == 0 || str[i - 1] == '(' || str[i - 1] == ' ');
                if (isPartOfNumber) continue;
            }
            AddWhitespace(i + 1, i + 1, str);
            AddWhitespace(i - 1, i, str);
        }
    }

    std::stack<std::string> operatorStack;
    std::stack<Node*> operandStack;
    operatorStack.push("#");

    // Build expression tree using Shunting Yard algorithm

    // Uses Dijkstra's Shunting Yard algorithm: it reads an infix expression left-to-right,
    // manages operators and parentheses with stacks based on precedence/associativity,
    // and produces an order that is easy for the computer to evaluate (postfix/tree).

    std::istringstream ss(str);
    while (!ss.eof()) {
        std::string s;
        ss >> s;

        if (operators.find(s) != operators.end()) {
            if (s == "(") {
                operatorStack.push(s);
            }
            else if (s == ")") {
                while (operatorStack.top() != "(")
                    PopOperator(operatorStack, operandStack);
                operatorStack.pop(); // discard "("
            }
            else {
                // Define right-associative operators
                auto isRightAssociative = [](const std::string& op) {
                    return op == "^" || op == "√";
                    };

                while (operatorStack.top() != "#" &&
                    operators.find(operatorStack.top()) != operators.end() &&
                    ((!isRightAssociative(s) &&
                        operators[operatorStack.top()].Precedence >= operators[s].Precedence) ||
                        (isRightAssociative(s) &&
                            operators[operatorStack.top()].Precedence > operators[s].Precedence)))
                {
                    PopOperator(operatorStack, operandStack);
                }

                operatorStack.push(s);
            }
        }
        else {
            // Operand
            Node* n = new Node(s);
            operandStack.push(n);
        }
    }

    // Pop remaining operators
    while (operatorStack.top() != "#")
        PopOperator(operatorStack, operandStack);

    root = operandStack.top();
}
