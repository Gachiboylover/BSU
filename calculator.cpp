#include "calculator.h"
#include <QKeyEvent>
#include <cmath>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), resultSoFar(0.0), waitingForOperand(true)
{
    createWidgets();
    createLayout();
    createConnections();

    setWindowTitle("Калькулятор");
    setFixedSize(300, 400);
}

void Calculator::createWidgets()
{
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);
    display->setStyleSheet("QLineEdit { font-size: 20px; padding: 10px; }");

    // Кнопки
    for (int i = 0; i < 10; ++i)
    {
        digitButtons.append(new QPushButton(QString::number(i)));
        digitButtons[i]->setStyleSheet("QPushButton { font-size: 16px; padding: 10px; }");
    }

    // Операции
    pointButton = new QPushButton(".");
    equalsButton = new QPushButton("=");
    plusButton = new QPushButton("+");
    minusButton = new QPushButton("-");
    multiplyButton = new QPushButton("×");
    divideButton = new QPushButton("÷");
    clearButton = new QPushButton("C");
    clearAllButton = new QPushButton("CE");
    backspaceButton = new QPushButton("⌫");

    // Стили
    QString opStyle = "QPushButton { font-size: 16px; padding: 10px; background-color: #ff9500; color: white; }";
    QString digitStyle = "QPushButton { font-size: 16px; padding: 10px; background-color: #f0f0f0; }";
    QString specialStyle = "QPushButton { font-size: 16px; padding: 10px; background-color: #a6a6a6; }";

    for (int i = 0; i < 10; ++i)
    {
        digitButtons[i]->setStyleSheet(digitStyle);
    }

    pointButton->setStyleSheet(digitStyle);
    equalsButton->setStyleSheet(opStyle);
    plusButton->setStyleSheet(opStyle);
    minusButton->setStyleSheet(opStyle);
    multiplyButton->setStyleSheet(opStyle);
    divideButton->setStyleSheet(opStyle);
    clearButton->setStyleSheet(specialStyle);
    clearAllButton->setStyleSheet(specialStyle);
    backspaceButton->setStyleSheet(specialStyle);
}

void Calculator::createLayout()
{
    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->addWidget(display, 0, 0, 1, 4);

    // Первая строка
    mainLayout->addWidget(clearAllButton, 1, 0);
    mainLayout->addWidget(clearButton, 1, 1);
    mainLayout->addWidget(backspaceButton, 1, 2);
    mainLayout->addWidget(divideButton, 1, 3);

    // Вторая строка
    mainLayout->addWidget(digitButtons[7], 2, 0);
    mainLayout->addWidget(digitButtons[8], 2, 1);
    mainLayout->addWidget(digitButtons[9], 2, 2);
    mainLayout->addWidget(multiplyButton, 2, 3);

    // Третья строка
    mainLayout->addWidget(digitButtons[4], 3, 0);
    mainLayout->addWidget(digitButtons[5], 3, 1);
    mainLayout->addWidget(digitButtons[6], 3, 2);
    mainLayout->addWidget(minusButton, 3, 3);

    // Четвертая строка
    mainLayout->addWidget(digitButtons[1], 4, 0);
    mainLayout->addWidget(digitButtons[2], 4, 1);
    mainLayout->addWidget(digitButtons[3], 4, 2);
    mainLayout->addWidget(plusButton, 4, 3);

    // Пятая строка
    mainLayout->addWidget(digitButtons[0], 5, 0, 1, 2);
    mainLayout->addWidget(pointButton, 5, 2);
    mainLayout->addWidget(equalsButton, 5, 3);

    setLayout(mainLayout);
}

void Calculator::createConnections()
{
}

void Calculator::digitClicked()
{
}

void Calculator::operatorClicked()
{
}

void Calculator::equalClicked()
{
}

void Calculator::clearClicked()
{
}

void Calculator::clearAllClicked()
{
}

void Calculator::pointClicked()
{
}

void Calculator::backspaceClicked()
{
}

double Calculator::calculate(double operand1, double operand2, const QString &operation)
{
    return 0;
}