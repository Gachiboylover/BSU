#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVector>

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);

private slots:
    void digitClicked();
    void operatorClicked();
    void equalClicked();
    void clearClicked();
    void clearAllClicked();
    void pointClicked();
    void backspaceClicked();

private:
    void createWidgets();
    void createLayout();
    void createConnections();
    double calculate(double operand1, double operand2, const QString &operation);

    QLineEdit *display;
    QVector<QPushButton *> digitButtons;
    QPushButton *pointButton;
    QPushButton *equalsButton;
    QPushButton *plusButton;
    QPushButton *minusButton;
    QPushButton *multiplyButton;
    QPushButton *divideButton;
    QPushButton *clearButton;
    QPushButton *clearAllButton;
    QPushButton *backspaceButton;

    QString pendingOperation;
    double resultSoFar;
    bool waitingForOperand;
};

#endif