#include "authenticationmanager.h"
#include "databasehelper.h"
#include "menuwindow.h"

QString AuthenticationManager::currentUserLogin;

AuthenticationManager::AuthenticationManager(QObject *parent) : QObject(parent) {}

bool AuthenticationManager::authenticate(const QString &login, const QString &password)
{
    bool authenticated = DatabaseHelper::checkUser(login, password);
    if (authenticated) {
        QStringList allowedButtons = AuthenticationManager::getAllowedButtons(login);
        menuwindow menuWindow(allowedButtons);
        menuWindow.exec();
        currentUserLogin = login;
    } else {
    }
    return authenticated;
}

bool AuthenticationManager::saveUserInfo(const QString &login)
{
    currentUserLogin = login;
    return true;
}

QString AuthenticationManager::getCurrentUserLogin() const
{
    return currentUserLogin;
}

QStringList AuthenticationManager::getAllowedButtons(const QString &login)
{
    QStringList allowedButtons;
    if (login == "system_admin") {
        allowedButtons = {"Клиент" ,"Автомобиль", "Автопарк", "Администратор", "Марка", "Аренда", "Модель"};
    } else if (login == "data_viewer") {
        allowedButtons = {"Автомобиль" ,"Автопарк", "Модель", "Марка"};
    }
    return allowedButtons;
}
