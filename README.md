# Expression Converter and Visualizer 

A C++ GUI application developed using the **Qt Framework** for the Data Structures and Algorithms (DSA) Mini-Project.

## Overview
This tool converts mathematical expressions between **Infix, Postfix, and Prefix** notations using the **Shunting-Yard Algorithm**. It also features a real-time visualization table to help students understand how the stack operates during conversion.

## Key Features
- **Algorithm Visualization:** A step-by-step breakdown of the conversion process.
- **Custom DSA:** Built using a manually coded **Stack** and **Singly Linked List** (no `std::stack` used).
- **Expression Evaluation:** Computes the final result of postfix expressions.
- **GUI Interface:** Interactive and responsive design using Qt Widgets.

## Technical Details
- **Language:** C++
- **Framework:** Qt 6
- **Core Logic:** Shunting-Yard Algorithm & Postfix Evaluation.
- **Memory Management:** Manual node allocation/deallocation within the custom Linked List.

## Repository Structure
- `/` : Contains `.cpp`, `.h`, and `.ui` source files.
- `ExpressionCalculator.pro`: Qt Project configuration.
- `ProjectReport_Pragyee_10.pdf`: Comprehensive project documentation and system design.

## How to Run
1. Ensure **Qt Creator** is installed.
2. Clone this repo: `git clone https://github.com/Pragyee2024/dsa_miniproject.git`
3. Open `ExpressionCalculator.pro` in Qt Creator.
4. Build and Run.
