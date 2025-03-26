#include "registrationwindow.h"
#include "ui_registrationwindow.h"
#include <QMessageBox>
#include "databasehelper.h" // Включаем заголовочный файл с помощником базы данных
#include "authenticationmanager.h"

registrationwindow::registrationwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registrationwindow)
{
    ui->setupUi(this);

    ui->lePassword->setEchoMode(QLineEdit::Password);
}

registrationwindow::~registrationwindow()
{
    delete ui;
}

void registrationwindow::on_btnWelcome_clicked()
{
    QString login = ui->leLogin->text();
    QString password = ui->lePassword->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Пожалуйста, введите логин и пароль.");
        return;
    }

    AuthenticationManager::saveUserInfo(login);

    QStringList allowedButtons = AuthenticationManager::getAllowedButtons(login);

    if (DatabaseHelper::connectToDatabase("localhost", "car_rent", login, password)) {
        menuwindow *menuWindow = new menuwindow(allowedButtons);
        menuWindow->show();
        this->close(); // Закрываем текущее окно регистрации
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка подключения:\n" + DatabaseHelper::lastError().text());
    }
}
