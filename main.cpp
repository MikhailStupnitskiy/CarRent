#include <QApplication>
#include "registrationwindow.h" // Включаем заголовочный файл класса RegistrationWindow
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(":/style.css");
    file.open(QFile::ReadOnly);
    a.setStyleSheet(file.readAll());

    registrationwindow registrationWindow;

    registrationWindow.show();

    return a.exec();
}
