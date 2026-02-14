# C++ Calculator

A simple calculator built in **C++** that can currently do basic operations such as addition, subtraction, multiplication, and division. 

---

## 🚀 Features

- Performs addition, subtraction, multiplication, and division  
- Handles division by zero "safely"  

---

## 📂 File Explanations

**Calculator_app.cpp**  
> The current basic ui frontend of the calculator. Also defines what buttons we have.

**Calculator_app.h**  
> Includes "resource.h"

**calculator_functionality.cpp**  
> Connects buttons to what mathmatical function they serve. As well as defining use for buttons that are not used in math. Contains our preprocessing.

**calculator_functionality.h**  
> Declares functions used the .cpp file for calculator_functionality.

**ExpressionTree.cpp**  
> Using CodeMasters ExpressionTree tutorial for **C++** this is where all our main computing happens. This handles precedence when handling equations and much more.

**utils.cpp**  
> Declares what is used in ExpressionTree.cpp
