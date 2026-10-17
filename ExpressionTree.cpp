#include "ExpressionTree.h"
#include <stack>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <cctype>

ExpressionTree::operator_map ExpressionTree::operators;

ExpressionTree::ExpressionTree(const std::string& str) {
    std::cout << "CONSTRUCTOR CALLED with: '" << str << "'\n";

    if (operators.empty()) {
        std::cout << "Initializing operators...\n";

        operators["+"] = OperatorInfo(1, Add, Binary);
        operators["-"] = OperatorInfo(1, Subtract, Binary);
        operators["*"] = OperatorInfo(2, Multiply, Binary);
        operators["/"] = OperatorInfo(2, Divide, Binary);
        operators["%"] = OperatorInfo(2, Percentage, Postfix);
        operators["^"] = OperatorInfo(3, Exponents, Binary);
        operators["√"] = OperatorInfo(3, SquareRoot, Prefix);
        operators["neg"] = OperatorInfo(3, UnaryMinus, Prefix);
        operators["("] = OperatorInfo(-2, nullptr, Bracket);
        operators[")"] = OperatorInfo(-2, nullptr, Bracket);
        operators["#"] = OperatorInfo(-2, nullptr, Bracket);
    }

    std::cout << "About to call FromString\n";
    FromString(str);
    std::cout << "FromString finished. root=" << (root ? "NOT NULL" : "NULL") << "\n";
}

ExpressionTree::~ExpressionTree() {
    DeleteTree(root);
    root = nullptr;
}

void ExpressionTree::DeleteTree(Node* node) {
    if (!node) return;

    DeleteTree(node->Left);
    DeleteTree(node->Right);
    delete node;
}

double ExpressionTree::Evaluate(Node* node) const {
    node = node ? node : root;

    auto it = operators.find(node->Value);
    if (it != operators.end()) {
        double left = node->Left ? Evaluate(node->Left) : 0.0;
        double right = node->Right ? Evaluate(node->Right) : 0.0;

        if (node->Value == "/" && right == 0)
            throw MathError("Division by zero");

        double result = (it->second.Func)(left, right);
        if (!std::isfinite(result))
            throw MathError("Math error");
        return result + 0.0; // turns -0 into 0
    }

    std::istringstream ss(node->Value);
    double val = 0.0;
    ss >> val;
    if (!std::isfinite(val))
        throw MathError("Math error");
    return val;
}

// Expression representation (to be implemented)
std::string ExpressionTree::Expression() const {
    return {};
}

// Splits the expression into tokens and checks the syntax, so the tree builder
// never gets an operator that is missing an operand.
// Unary minus comes out as "neg", and "2(3)", "2√9", "(1)(2)", "50%2" get a "*" inserted.
std::vector<std::string> ExpressionTree::Tokenize(const std::string& str) {
    if (str.length() > MaxLength)
        throw ExpressionError("Expression too long");

    const std::string sqrtSymbol = "√";
    std::vector<std::string> tokens;
    bool prevIsValue = false;  // previous token ends a value: a number, ")" or "%"
    bool prevIsNumber = false;
    int depth = 0;             // open parentheses

    for (size_t i = 0; i < str.length();) {
        unsigned char c = str[i];

        if (isspace(c)) {
            ++i;
            continue;
        }

        // Number: digits with at most one decimal point
        if (isdigit(c) || c == '.') {
            size_t start = i;
            int digits = 0, dots = 0;
            while (i < str.length() && (isdigit(static_cast<unsigned char>(str[i])) || str[i] == '.')) {
                str[i] == '.' ? ++dots : ++digits;
                ++i;
            }
            if (digits == 0 || dots > 1)
                throw ExpressionError("Invalid number");
            if (prevIsNumber)
                throw ExpressionError("Missing operator");
            if (prevIsValue)
                tokens.push_back("*");

            tokens.push_back(str.substr(start, i - start));
            prevIsValue = prevIsNumber = true;
            continue;
        }

        std::string op = str.compare(i, sqrtSymbol.size(), sqrtSymbol) == 0 ? sqrtSymbol : std::string(1, c);
        i += op.size();

        if (op == "(") {
            if (prevIsValue)
                tokens.push_back("*");
            ++depth;
            prevIsValue = false;
        }
        else if (op == ")") {
            if (!prevIsValue || depth == 0)
                throw ExpressionError("Syntax error");
            --depth;
        }
        else {
            // minus is a sign unless it follows a value
            if (op == "-" && !prevIsValue)
                op = "neg";

            auto it = operators.find(op);
            if (it == operators.end() || it->second.Type == Bracket)
                throw ExpressionError("Unexpected character");

            switch (it->second.Type) {
            case Prefix:
                if (prevIsValue)
                    tokens.push_back("*");
                prevIsValue = false;
                break;
            case Binary:
                if (!prevIsValue)
                    throw ExpressionError("Syntax error");
                prevIsValue = false;
                break;
            default: // Postfix
                if (!prevIsValue)
                    throw ExpressionError("Syntax error");
                break;
            }
        }

        tokens.push_back(op);
        prevIsNumber = false;
    }

    // empty, ends on an operator, or has an unclosed "("
    if (!prevIsValue || depth != 0)
        throw ExpressionError("Syntax error");

    return tokens;
}

void ExpressionTree::PopOperator(std::stack<std::string>& operatorStack,
    std::stack<Node*>& operandStack) {
    std::string op = operatorStack.top();
    operatorStack.pop();

    Node* n = new Node(op);

    if (operators[op].Type != Binary) { // unary operator
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
    // Throws on bad syntax, before any nodes are allocated
    std::vector<std::string> tokens = Tokenize(expressionString);

    std::stack<std::string> operatorStack;
    std::stack<Node*> operandStack;
    operatorStack.push("#");

    // Build expression tree using Shunting Yard algorithm

    // Uses Dijkstra's Shunting Yard algorithm: it reads an infix expression left-to-right,
    // manages operators and parentheses with stacks based on precedence/associativity,
    // and produces an order that is easy for the computer to evaluate (postfix/tree).

    for (const std::string& s : tokens) {
        auto it = operators.find(s);
        if (it != operators.end()) {
            if (s == "(") {
                operatorStack.push(s);
            }
            else if (s == ")") {
                while (operatorStack.top() != "(")
                    PopOperator(operatorStack, operandStack);
                operatorStack.pop(); // discard "("
            }
            else if (it->second.Type == Postfix) {
                // postfix: applies straight away to the value before it
                operatorStack.push(s);
                PopOperator(operatorStack, operandStack);
            }
            else if (it->second.Type == Prefix) {
                // prefix: nothing on the stack can be finished yet
                operatorStack.push(s);
            }
            else {
                // "^" is right-associative, the other binary operators are left-associative
                while (operatorStack.top() != "#" &&
                    (operators[operatorStack.top()].Precedence > operators[s].Precedence ||
                        (s != "^" &&
                            operators[operatorStack.top()].Precedence == operators[s].Precedence)))
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
