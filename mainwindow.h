#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

template <typename T>
class Stack {
private:
    QVector<T> data;
public:
    void push(T val)     { data.append(val); }
    void pop()           { if (!isEmpty()) data.removeLast(); }
    T    top()     const { return data.last(); }
    bool isEmpty() const { return data.isEmpty(); }
    int  size()    const { return data.size(); }
    void clear()         { data.clear(); }
};


struct HistoryNode {
    QString input;
    QString output;
    QString conversionType;
    HistoryNode* next;

    HistoryNode(const QString& in, const QString& out, const QString& type)
        : input(in), output(out), conversionType(type), next(nullptr) {}
};

class LinkedList {
private:
    HistoryNode* head;
    int count;
public:
    LinkedList() : head(nullptr), count(0) {}
    ~LinkedList() { clear(); }


    void insertFront(const QString& in, const QString& out, const QString& type) {
        HistoryNode* node = new HistoryNode(in, out, type);
        node->next = head;
        head = node;
        count++;
    }


    QVector<HistoryNode*> getAll() const {
        QVector<HistoryNode*> result;
        HistoryNode* curr = head;
        while (curr) { result.append(curr); curr = curr->next; }
        return result;
    }

    int  size()    const { return count; }
    bool isEmpty() const { return head == nullptr; }


    void clear() {
        while (head) {
            HistoryNode* temp = head;
            head = head->next;
            delete temp;
        }
        count = 0;
    }
};


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInfixToPostfix();
    void onInfixToPrefix();
    void onPostfixToInfix();
    void onPrefixToInfix();
    void onClearInput();
    void onClearHistory();

private:
    Ui::MainWindow *ui;


    LinkedList history;


    QTextEdit* algorithmTextEdit;


    int     precedence(QChar op);
    bool    isOperator(QChar ch);
    bool    isRightAssociative(QChar op);
    QString infixToPostfix(const QString& expr);
    QString infixToPrefix(const QString& expr);
    QString postfixToInfix(const QString& expr);
    QString prefixToInfix(const QString& expr);


    void doConversion(const QString& convType);
    void applyDarkTheme();
    void refreshHistory();
    void showAlgorithmSteps(const QString& convType, const QString& input);
    QString generateStepTable(const QString& convType, const QString& input);
};

#endif // MAINWINDOW_H
