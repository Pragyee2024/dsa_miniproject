#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QScrollBar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Expression Notation Converter");


    connect(ui->pushButton,   &QPushButton::clicked, this, &MainWindow::onInfixToPostfix);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onInfixToPrefix);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onPostfixToInfix);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::onPrefixToInfix);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::onClearInput);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &MainWindow::onClearHistory);


    connect(ui->inputlineEdit, &QLineEdit::returnPressed, this, &MainWindow::onInfixToPostfix);


    ui->outputTypeLabel->setText("—");
    ui->outputValueLabel->setText("—");
    ui->historySizeLabel->setText("History: 0 entries  |  Linked List nodes: 0");


    algorithmTextEdit = findChild<QTextEdit*>("algorithmTextEdit");
    if (algorithmTextEdit) {
        algorithmTextEdit->setReadOnly(true);
        showAlgorithmSteps("", "");
    }

}

MainWindow::~MainWindow() { delete ui; }

// DSA Stack
bool MainWindow::isOperator(QChar ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
}

bool MainWindow::isRightAssociative(QChar op) {
    return op == '^';
}

int MainWindow::precedence(QChar op) {
    if (op == '^')               return 3;
    if (op == '*' || op == '/')  return 2;
    if (op == '+' || op == '-')  return 1;
    return 0;
}

QString MainWindow::infixToPostfix(const QString& expr) {
    Stack<QChar> stack;
    QStringList output;
    QString token;

    auto flush = [&]() {
        if (!token.isEmpty()) { output << token; token.clear(); }
    };

    for (QChar ch : expr) {
        if (ch == ' ') { flush(); continue; }

        if (ch.isLetterOrNumber() || ch == '.') {
            token += ch;
        } else {
            flush();
            if (ch == '(') {
                stack.push(ch);
            } else if (ch == ')') {
                while (!stack.isEmpty() && stack.top() != '(') {
                    output << QString(stack.top()); stack.pop();
                }
                if (!stack.isEmpty()) stack.pop();
            } else if (isOperator(ch)) {
                while (!stack.isEmpty() && stack.top() != '(' &&
                       (precedence(stack.top()) > precedence(ch) ||
                        (precedence(stack.top()) == precedence(ch) && !isRightAssociative(ch)))) {
                    output << QString(stack.top()); stack.pop();
                }
                stack.push(ch);
            }
        }
    }
    flush();
    while (!stack.isEmpty()) { output << QString(stack.top()); stack.pop(); }
    return output.join(" ");
}


QString MainWindow::infixToPrefix(const QString& expr) {
    QString rev = expr;
    std::reverse(rev.begin(), rev.end());
    for (QChar& ch : rev) {
        if      (ch == '(') ch = ')';
        else if (ch == ')') ch = '(';
    }
    QStringList tokens = infixToPostfix(rev).split(" ", Qt::SkipEmptyParts);
    std::reverse(tokens.begin(), tokens.end());
    return tokens.join(" ");
}


QString MainWindow::postfixToInfix(const QString& expr) {
    Stack<QString> stack;
    QStringList tokens = expr.trimmed().split(" ", Qt::SkipEmptyParts);

    for (const QString& token : tokens) {
        if (token.length() == 1 && isOperator(token[0])) {
            if (stack.size() < 2) return "Invalid postfix expression";
            QString b = stack.top(); stack.pop();
            QString a = stack.top(); stack.pop();
            stack.push("(" + a + " " + token + " " + b + ")");
        } else {
            stack.push(token);
        }
    }
    if (stack.size() != 1) return "Invalid postfix expression";
    return stack.top();
}


QString MainWindow::prefixToInfix(const QString& expr) {
    Stack<QString> stack;
    QStringList tokens = expr.trimmed().split(" ", Qt::SkipEmptyParts);

    for (int i = tokens.size() - 1; i >= 0; i--) {
        const QString& token = tokens[i];
        if (token.length() == 1 && isOperator(token[0])) {
            if (stack.size() < 2) return "Invalid prefix expression";
            QString a = stack.top(); stack.pop();
            QString b = stack.top(); stack.pop();
            stack.push("(" + a + " " + token + " " + b + ")");
        } else {
            stack.push(token);
        }
    }
    if (stack.size() != 1) return "Invalid prefix expression";
    return stack.top();
}


void MainWindow::doConversion(const QString& convType) {
    QString input = ui->inputlineEdit->text().trimmed();

    if (input.isEmpty()) {
        QMessageBox::warning(this, "Empty Input", "Please enter an expression first.");
        return;
    }

    QString result;
    if      (convType == "Infix → Postfix") result = infixToPostfix(input);
    else if (convType == "Infix → Prefix")  result = infixToPrefix(input);
    else if (convType == "Postfix → Infix") result = postfixToInfix(input);
    else if (convType == "Prefix → Infix")  result = prefixToInfix(input);


    ui->outputTypeLabel->setText(convType);
    ui->outputValueLabel->setText(result);


    showAlgorithmSteps(convType, input);

    //Push new node to front of Linked List
    history.insertFront(input, result, convType);
    refreshHistory();
}


void MainWindow::onInfixToPostfix() { doConversion("Infix → Postfix"); }
void MainWindow::onInfixToPrefix()  { doConversion("Infix → Prefix");  }
void MainWindow::onPostfixToInfix() { doConversion("Postfix → Infix"); }
void MainWindow::onPrefixToInfix()  { doConversion("Prefix → Infix");  }

void MainWindow::onClearInput() {
    ui->inputlineEdit->clear();
    ui->outputTypeLabel->setText("—");
    ui->outputValueLabel->setText("—");
    ui->inputlineEdit->setFocus();
}

void MainWindow::onClearHistory() {
    history.clear();
    refreshHistory();
}

// DSA Linked List
void MainWindow::refreshHistory() {
    ui->historyTextEdit->clear();

    if (history.isEmpty()) {
        ui->historySizeLabel->setText("History: 0 entries  |  Linked List nodes: 0");
        ui->historyTextEdit->setPlaceholderText("No conversions yet...");
        return;
    }

    ui->historySizeLabel->setText(
        QString("History: %1 entries  |  Linked List nodes: %2  (head = newest)")
            .arg(history.size()).arg(history.size())
        );


    QVector<HistoryNode*> entries = history.getAll();
    for (int i = 0; i < entries.size(); i++) {
        HistoryNode* node = entries[i];
        QString entry = QString(
                            "#%1  [ %2 ]\n"
                            "  Input  : %3\n"
                            "  Output : %4\n"
                            ).arg(i + 1)
                            .arg(node->conversionType)
                            .arg(node->input)
                            .arg(node->output);

        ui->historyTextEdit->append(entry);
    }

    ui->historyTextEdit->verticalScrollBar()->setValue(0);
}


void MainWindow::showAlgorithmSteps(const QString& convType, const QString& input) {
    if (!algorithmTextEdit) return;

    if (convType.isEmpty()) {
        algorithmTextEdit->setHtml(
            "<div style='color:#7eb8f7; font-size:18px; font-weight:bold; margin-bottom:12px;'>"
            "Algorithm Visualizer</div>"
            "<div style='color:#a0a0cc; font-size:13px; line-height:1.6;'>"
            "Enter an expression and click a conversion button to see the algorithm in action with a step-by-step table."
            "</div>"
            );
        return;
    }

    QString html = QString(
                       "<div style='color:#7eb8f7; font-size:18px; font-weight:bold; margin-bottom:10px;'>%1</div>"
                       ).arg(convType);


    if (convType == "Infix → Postfix") {
        html += "<div style='color:#55cc88; font-size:14px; font-weight:bold; margin-top:14px;'>Algorithm: Shunting-Yard (Stack)</div>";
        html += "<ul style='color:#c0c0e0; font-size:12px; line-height:1.9;'>";
        html += "<li>Scan expression left to right</li>";
        html += "<li>If operand → add to output</li>";
        html += "<li>If '(' → push to stack</li>";
        html += "<li>If ')' → pop until '('</li>";
        html += "<li>If operator → pop higher/equal precedence, then push</li>";
        html += "<li>Pop remaining operators</li>";
        html += "</ul>";
    } else if (convType == "Infix → Prefix") {
        html += "<div style='color:#55cc88; font-size:14px; font-weight:bold; margin-top:14px;'>Algorithm: Reverse + Shunting-Yard</div>";
        html += "<ul style='color:#c0c0e0; font-size:12px; line-height:1.9;'>";
        html += "<li>Reverse the infix expression</li>";
        html += "<li>Swap '(' with ')' and vice versa</li>";
        html += "<li>Apply Shunting-Yard algorithm</li>";
        html += "<li>Reverse the result</li>";
        html += "</ul>";
    } else if (convType == "Postfix → Infix") {
        html += "<div style='color:#55cc88; font-size:14px; font-weight:bold; margin-top:14px;'>Algorithm: Stack-based Reconstruction</div>";
        html += "<ul style='color:#c0c0e0; font-size:12px; line-height:1.9;'>";
        html += "<li>Scan postfix left to right</li>";
        html += "<li>If operand → push to stack</li>";
        html += "<li>If operator → pop 2 operands, create (a op b), push back</li>";
        html += "<li>Final stack top = infix</li>";
        html += "</ul>";
    } else if (convType == "Prefix → Infix") {
        html += "<div style='color:#55cc88; font-size:14px; font-weight:bold; margin-top:14px;'>Algorithm: Stack (Right to Left)</div>";
        html += "<ul style='color:#c0c0e0; font-size:12px; line-height:1.9;'>";
        html += "<li>Scan prefix right to left</li>";
        html += "<li>If operand → push to stack</li>";
        html += "<li>If operator → pop 2 operands, create (a op b), push back</li>";
        html += "<li>Final stack top = infix</li>";
        html += "</ul>";
    }


    html += generateStepTable(convType, input);

    algorithmTextEdit->setHtml(html);
}

QString MainWindow::generateStepTable(const QString& convType, const QString& input) {
    QString table = "<div style='margin-top:15px;'>";
    table += "<div style='color:#55cc88; font-size:15px; font-weight:bold; margin-bottom:10px;'>Step-by-Step Trace</div>";

    if (convType == "Infix → Postfix") {
        table += "<table style='width:100%; border-collapse:collapse; font-size:13px; font-family:Courier New;'>";
        table += "<tr style='background:#16213e; color:#7eb8f7; font-weight:bold;'>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Step</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Token</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Stack</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Output</td>"
                 "</tr>";


        QVector<QChar> stack;
        QStringList output;
        QString token;
        int step = 0;

        auto addRow = [&](const QString& tok, const QString& stk, const QString& out) {
            QString bg = (step % 2 == 0) ? "#1a1a2e" : "#0f0f1e";
            table += QString("<tr style='background:%1; color:#c0c0e0;'>").arg(bg);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%1</td>").arg(step);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%2</td>").arg(tok);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%3</td>").arg(stk);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%4</td>").arg(out);
            table += "</tr>";
            step++;
        };

        auto getStackStr = [&]() {
            QString str;
            for (const QChar& c : stack) str += c + QString(" ");
            return str.trimmed();
        };

        for (QChar ch : input) {
            if (ch == ' ') continue;

            if (ch.isLetterOrNumber() || ch == '.') {
                token += ch;
            } else {
                if (!token.isEmpty()) {
                    output << token;
                    addRow(token, getStackStr(), output.join(" "));
                    token.clear();
                }

                if (ch == '(') {
                    stack.append(ch);
                    addRow(QString(ch), getStackStr(), output.join(" "));
                } else if (ch == ')') {
                    while (!stack.isEmpty() && stack.last() != '(') {
                        output << QString(stack.last());
                        stack.removeLast();
                    }
                    if (!stack.isEmpty()) stack.removeLast();
                    addRow(QString(ch), getStackStr(), output.join(" "));
                } else if (isOperator(ch)) {
                    while (!stack.isEmpty() && stack.last() != '(' &&
                           (precedence(stack.last()) > precedence(ch) ||
                            (precedence(stack.last()) == precedence(ch) && !isRightAssociative(ch)))) {
                        output << QString(stack.last());
                        stack.removeLast();
                    }
                    stack.append(ch);
                    addRow(QString(ch), getStackStr(), output.join(" "));
                }
            }
        }

        if (!token.isEmpty()) {
            output << token;
            addRow(token, getStackStr(), output.join(" "));
        }

        while (!stack.isEmpty()) {
            output << QString(stack.last());
            stack.removeLast();
            addRow("—", getStackStr(), output.join(" "));
        }

    } else if (convType == "Postfix → Infix") {
        table += "<table style='width:100%; border-collapse:collapse; font-size:13px; font-family:Courier New;'>";
        table += "<tr style='background:#16213e; color:#7eb8f7; font-weight:bold;'>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Step</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Token</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Stack</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Action</td>"
                 "</tr>";

        QVector<QString> stack;
        QStringList tokens = input.trimmed().split(" ", Qt::SkipEmptyParts);
        int step = 0;

        auto addRow = [&](const QString& tok, const QString& stk, const QString& action) {
            QString bg = (step % 2 == 0) ? "#1a1a2e" : "#0f0f1e";
            table += QString("<tr style='background:%1; color:#c0c0e0;'>").arg(bg);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%1</td>").arg(step);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%2</td>").arg(tok);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%3</td>").arg(stk);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%4</td>").arg(action);
            table += "</tr>";
            step++;
        };

        auto getStackStr = [&]() {
            QString str;
            for (const QString& s : stack) str += s + " ";
            return str.trimmed();
        };

        for (const QString& token : tokens) {
            if (token.length() == 1 && isOperator(token[0])) {
                if (stack.size() < 2) {
                    addRow(token, getStackStr(), "Error: Not enough operands");
                    break;
                }
                QString b = stack.last(); stack.removeLast();
                QString a = stack.last(); stack.removeLast();
                QString result = "(" + a + " " + token + " " + b + ")";
                stack.append(result);
                addRow(token, getStackStr(), "Pop " + a + " & " + b + ", Push " + result);
            } else {
                stack.append(token);
                addRow(token, getStackStr(), "Push operand");
            }
        }

    } else if (convType == "Prefix → Infix") {
        table += "<table style='width:100%; border-collapse:collapse; font-size:13px; font-family:Courier New;'>";
        table += "<tr style='background:#16213e; color:#7eb8f7; font-weight:bold;'>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Step</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Token</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Stack</td>"
                 "<td style='border:1px solid #2a2a4a; padding:6px;'>Action</td>"
                 "</tr>";

        QVector<QString> stack;
        QStringList tokens = input.trimmed().split(" ", Qt::SkipEmptyParts);
        int step = 0;

        auto addRow = [&](const QString& tok, const QString& stk, const QString& action) {
            QString bg = (step % 2 == 0) ? "#1a1a2e" : "#0f0f1e";
            table += QString("<tr style='background:%1; color:#c0c0e0;'>").arg(bg);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%1</td>").arg(step);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%2</td>").arg(tok);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%3</td>").arg(stk);
            table += QString("<td style='border:1px solid #2a2a4a; padding:6px;'>%4</td>").arg(action);
            table += "</tr>";
            step++;
        };

        auto getStackStr = [&]() {
            QString str;
            for (const QString& s : stack) str += s + " ";
            return str.trimmed();
        };


        for (int i = tokens.size() - 1; i >= 0; i--) {
            const QString& token = tokens[i];
            if (token.length() == 1 && isOperator(token[0])) {
                if (stack.size() < 2) {
                    addRow(token, getStackStr(), "Error: Not enough operands");
                    break;
                }
                QString a = stack.last(); stack.removeLast();
                QString b = stack.last(); stack.removeLast();
                QString result = "(" + a + " " + token + " " + b + ")";
                stack.append(result);
                addRow(token, getStackStr(), "Pop " + a + " & " + b + ", Push " + result);
            } else {
                stack.append(token);
                addRow(token, getStackStr(), "Push operand");
            }
        }

    } else {

        QString reversed = input;
        std::reverse(reversed.begin(), reversed.end());

        QString swapped;
        for (QChar ch : reversed) {
            if (ch == '(') swapped += ')';
            else if (ch == ')') swapped += '(';
            else swapped += ch;
        }

        table += QString("<tr style='background:#1a1a2e; color:#c0c0e0;'>"
                         "<td style='border:1px solid #2a2a4a; padding:6px;' colspan='4'>"
                         "<div style='font-size:13px; line-height:1.8;'>"
                         "<b>Step 1:</b> Reverse the infix expression<br>"
                         "Original: <span style='color:#7eb8f7;'>%1</span><br>"
                         "Reversed: <span style='color:#7eb8f7;'>%2</span><br><br>"
                         "<b>Step 2:</b> Swap parentheses ( ↔ )<br>"
                         "After swap: <span style='color:#7eb8f7;'>%3</span><br><br>"
                         "<b>Step 3:</b> Convert to postfix using Shunting-Yard<br>"
                         "Postfix result: <span style='color:#a0a0cc;'>%5</span><br><br>"
                         "<b>Step 4:</b> Reverse the postfix to get prefix<br>"
                         "Final Prefix: <span style='color:#55cc88; font-weight:bold;'>%4</span>"
                         "</div>"
                         "</td></tr>")
                     .arg(input)
                     .arg(reversed)
                     .arg(swapped)
                     .arg(infixToPrefix(input))
                     .arg(infixToPostfix(swapped));
    }

    table += "</table></div>";
    return table;
}
