#include "menuwindow.h"
#include "ui_menuwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include "databasehelper.h"
#include <QMessageBox>
#include "registrationwindow.h"
#include "onb1.h"
#include "onb2.h"
#include "onb3.h"
#include "onb4.h"
#include "onb5.h"
#include "onb6.h"
#include "onb7.h"
#include <QFile>
menuwindow::menuwindow(const QStringList &allowedButtons, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::menuwindow)
{
    ui->setupUi(this); // Подключение формы

    QVBoxLayout *layout = new QVBoxLayout(this); // Создание layout

    for (const QString &buttonText : allowedButtons) {
        QPushButton *button = new QPushButton(buttonText, this);
        button->setStyleSheet("QPushButton {"
                              "    background-color: #A9A9A9;" // Серый цвет фона
                              "    border-radius: 10px;"       // Закругленные углы
                              "}"
                              "QPushButton:hover {"
                              "    background-color: #808080;" // Цвет при наведении
                              "}"
                              "QPushButton:pressed {"
                              "    background-color: #696969;" // Цвет при нажатии
                              "}");
        layout->addWidget(button);

        if (buttonText == "Клиент") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb11);
        } else if (buttonText == "Автомобиль") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb22);
        } else if (buttonText == "Автопарк") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb33);
        } else if (buttonText == "Администратор") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb44);
        } else if (buttonText == "Марка") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb55);
        } else if (buttonText == "Аренда") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb66);
        } else if (buttonText == "Модель") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb77);
        }
    }

    returnToRegistrationButton = new QPushButton("Вернуться к регистрации", this);
    returnToRegistrationButton->setStyleSheet("QPushButton {"
                                              "    background-color: blue;" // Синий цвет фона
                                              "    border-radius: 10px;"       // Закругленные углы
                                              "}"
                                              "QPushButton:hover {"
                                              "    background-color: #808080;" // Цвет при наведении
                                              "}"
                                              "QPushButton:pressed {"
                                              "    background-color: #696969;" // Цвет при нажатии
                                              "}");
    layout->addWidget(returnToRegistrationButton);
    connect(returnToRegistrationButton, &QPushButton::clicked, this, &menuwindow::onReturnToRegistrationClicked);

    exitButton = new QPushButton("Выход", this);
    exitButton->setStyleSheet("QPushButton {"
                              "    background-color: blue;" // Серый цвет фона
                              "    border-radius: 10px;"       // Закругленные углы
                              "}"
                              "QPushButton:hover {"
                              "    background-color: #808080;" // Цвет при наведении
                              "}"
                              "QPushButton:pressed {"
                              "    background-color: #696969;" // Цвет при нажатии
                              "}");
    layout->addWidget(exitButton);
    connect(exitButton, &QPushButton::clicked, this, &menuwindow::onExitButtonClicked);
}

menuwindow::~menuwindow()
{
    delete ui;
}

void menuwindow::onb11()
{

    if (DatabaseHelper::isDatabaseConnected()) {

        onb1 *onbDialog = new onb1(this);
        onbDialog->show();
        this->hide();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}
void menuwindow::onb22()
{
    if (DatabaseHelper::isDatabaseConnected()) {
        onb2 *onbDialog = new onb2(this);
        onbDialog->show();
        this->hide();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb33()
{
    if (DatabaseHelper::isDatabaseConnected()) {
        onb3 *onbDialog = new onb3(this);
        onbDialog->show();
        this->hide();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}
void menuwindow::onb44()
{
    if (DatabaseHelper::isDatabaseConnected()) {

        onb4 *onbDialog = new onb4(this);
        onbDialog->show();
        this->hide();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb55()
{
    if (DatabaseHelper::isDatabaseConnected()) {
        onb5 *onbDialog = new onb5(this);
        onbDialog->show();
        this->hide();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb66()
{
    if (DatabaseHelper::isDatabaseConnected()) {
        onb6 *onbDialog = new onb6(this);
        onbDialog->show();
        this->hide();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb77()
{
    if (DatabaseHelper::isDatabaseConnected()) {
        onb7 *onbDialog = new onb7(this);
        onbDialog->show();
        this->hide();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onReturnToRegistrationClicked()
{
    close();
    registrationwindow *registrationWindow = new registrationwindow();
    registrationWindow->show();
}
void menuwindow::onExitButtonClicked()
{
    close();
}


