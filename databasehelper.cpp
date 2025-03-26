// databasehelper.cpp

#include "databasehelper.h"
#include <QCoreApplication> // Для доступа к методу addLibraryPath
#include <QSqlDatabase>
#include <QSqlError>
#include "AuthenticationManager.h"
bool DatabaseHelper::databaseConnected = false;
QSqlError DatabaseHelper::lastDbError;


bool DatabaseHelper::connectToDatabase(const QString &hostName, const QString &databaseName,
                                       const QString &userName, const QString &password)
{
    QCoreApplication::addLibraryPath("/opt/homebrew/share/qt/plugins");
    QCoreApplication::addLibraryPath("/usr/local/lib/qt/plugins/sqldrivers");

    // Подключение к базе данных
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);

    if (db.open()) {
        databaseConnected = true;
        return true;
    } else {
        lastDbError = db.lastError();
        databaseConnected = false;
        return false;
    }
}

bool DatabaseHelper::isDatabaseConnected()
{
    return databaseConnected;
}

QSqlError DatabaseHelper::lastError()
{
    return lastDbError;
}

QSqlDatabase DatabaseHelper::getDatabaseConnection()
{
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "База данных не открыта. Ошибка: " << db.lastError().text();
    }

    return db;
}



bool DatabaseHelper::checkUser(const QString &login, const QString &password)
{

}
