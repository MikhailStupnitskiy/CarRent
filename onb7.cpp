#include "onb7.h"
#include "ui_onb7.h"
#include "menuwindow.h"
#include "databasehelper.h"
#include "authenticationmanager.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QFile>
onb7::onb7(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb7)
{
    ui->setupUi(this);

    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(remove()));
    connect(ui->btn3, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb7::search);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb7::generateReportRequest);
    QTimer::singleShot(0, this, SLOT(selectAll()));

    ui->tw->setColumnCount(3); // Измените на количество столбцов в вашей таблице

    ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("Марка"));
    ui->tw->setHorizontalHeaderItem(2, new QTableWidgetItem("Модель"));

    ui->tw->horizontalHeader()->setStretchLastSection(true);

    ui->tw->setAutoScroll(true);

    ui->tw->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tw->setSortingEnabled(true);

    ui->tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

    populate1ComboBox();
}

onb7::~onb7()
{
    delete ui;
}

void onb7::selectAll()
{

    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }


    ui->tw->clearContents();

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());
    QString sqlstr = "SELECT model_id, brand_name, model_name FROM model ORDER BY model_id";


    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    ui->tw->clearContents();

    ui->tw->setRowCount(0); // Очищаем все строки

    int rowCount = 0;

    while (query.next())
    {
        ui->tw->insertRow(rowCount);

        QTableWidgetItem *idItem = new QTableWidgetItem(query.value("model_id").toString());
        ui->tw->setItem(rowCount, 0, idItem);

        ui->tw->setItem(rowCount, 1, new QTableWidgetItem(query.value("brand_name").toString()));
        ui->tw->setItem(rowCount, 2, new QTableWidgetItem(query.value("model_name").toString()));


        for (int j = 0; j < ui->tw->columnCount(); ++j)
        {
            ui->tw->item(rowCount, j)->setTextAlignment(Qt::AlignCenter);
        }

        rowCount++;
    }

    ui->tw->resizeColumnsToContents();
}



void onb7::on_tw_itemSelectionChanged()
{
    int curRow = ui->tw->currentRow();

    if (curRow < 0 || curRow >= ui->tw->rowCount()) {
        // Если выбранная строка не существует, сбрасываем значения полей ввода и выходим из функции
        ui->cmb1->setCurrentIndex(0);
        ui->le1->clear();
        ui->le2->clear();
        return;
    }

    QString pu1 = ui->tw->item(curRow, 0) ? ui->tw->item(curRow, 0)->text() : "";
    QString pu2 = ui->tw->item(curRow, 1) ? ui->tw->item(curRow, 1)->text() : "";
    QString pu3 = ui->tw->item(curRow, 2) ? ui->tw->item(curRow, 2)->text() : "";


    ui->le1->setText(pu1);
    ui->cmb1->setCurrentText(pu2);
    ui->le2->setText(pu3);

}


void onb7::clean()
{
    ui->le1->clear();
    ui->cmb1->setCurrentIndex(0);
    ui->le2->clear();
}


void onb7::edit()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Строка не выбрана!");
        return;
    }

    QString pu1 = ui->le1->text();
    QString pu2 = ui->le2->text();
    QString pu3 = ui->cmb1->currentText();


    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString sqlstr = "UPDATE model SET brand_name = ?, model_name = ? WHERE model_id = ?";

    query.prepare(sqlstr);

    query.bindValue(0, pu3);
    query.bindValue(1, pu2);
    query.bindValue(2, pu1);


    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    ui->teResult->append(QString("Updated %1 rows").arg(query.numRowsAffected()));

    ui->tw->item(curRow, 1)->setText(pu3);
    ui->tw->item(curRow, 2)->setText(pu2);

    selectAll();
}
void onb7::add()
{

    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }


    QString pu1 = ui->cmb1->currentText();
    QString pu2 = ui->le2->text();

    QSqlDatabase::database().transaction();

    QSqlQuery query;

    QString sqlstr = "INSERT INTO model(model_name, brand_name) VALUES (?, ?)";

    query.prepare(sqlstr);

    query.bindValue(0, pu2);
    query.bindValue(1, pu1);

    if (!query.exec())
    {
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос: " + query.lastError().text() +
                                                  "\nЗапрос: " + query.executedQuery());
        return;
    }

    QSqlDatabase::database().commit();

    selectAll();
}


void onb7::remove()
{

    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена");
        return;
    }


    int curRow = ui->tw->currentRow();


    if (curRow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Ряд не выбран!");
        return;
    }


    QTableWidgetItem *pu = ui->tw->item(curRow, 0);

    if (!pu)
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить идентификатор бронирования.");
        return;
    }

    QString pu1 = pu->text();


    qDebug() << "Selected ID:" << pu1;


    QSqlQuery query(DatabaseHelper::getDatabaseConnection());


    QString sqlstr = "DELETE FROM model WHERE model_id = :ModelId";


    query.prepare(sqlstr);


    query.bindValue(":ModelId", pu1);


    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить моедль с ID : " + pu1 + "\nОшибка: " + query.lastError().text());
        return;
    }


    qDebug() << "Успешно удалена модель с ID :" << pu1;


    ui->tw->removeRow(curRow);
}





void onb7::populate1ComboBox()
{

    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }


    QSqlQuery query(DatabaseHelper::getDatabaseConnection());


    QString sqlstr = "SELECT brand_name FROM brand ORDER BY brand_name";
    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }


    ui->cmb1->clear();


    while (query.next())
    {
        QString pu1 = query.value("brand_name").toString();
        ui->cmb1->addItem(pu1);
    }
}




void onb7::on_btnExit_clicked()
{

    AuthenticationManager authenticationManager;

    // Получаем логин текущего пользователя через созданный объект
    QString currentUser = authenticationManager.getCurrentUserLogin();

    // Получаем список разрешенных кнопок для текущего пользователя
    QStringList allowedButtons = authenticationManager.getAllowedButtons(currentUser);

    // Закрываем текущее окно гостей
    close();

    // Создаем и отображаем новое окно главного меню с передачей списка разрешенных кнопок
    menuwindow *menuWindow = new menuwindow(allowedButtons);
    menuWindow->show();
}

void onb7::search()
{
    // Получаем текст из поля поиска
    QString currentSearchQuery = ui->leSearch->text().trimmed(); // Обрезаем начальные и конечные пробелы

    // Проверяем, пусто ли поле ввода
    if (currentSearchQuery.isEmpty())
    {
        // Если поле ввода пустое, вызываем функцию selectAll для выделения всех строк
        selectAll();
        return; // Завершаем выполнение функции
    }

    // Проверяем, изменилось ли значение поиска с предыдущего запроса
    if (currentSearchQuery != m_lastSearchQuery)
    {
        // Если значение изменилось, обновляем переменную с последним значением поиска
        m_lastSearchQuery = currentSearchQuery;

        // Сбрасываем индекс последнего найденного совпадения
        m_lastFoundIndex = -1;
    }

    // Очищаем выделение в таблице
    ui->tw->clearSelection();

    // Флаг для отслеживания найденных результатов
    bool foundMatch = false;

    // Проходим по всем строкам таблицы и ищем нужную строку
    for (int row = m_lastFoundIndex + 1; row < ui->tw->rowCount(); ++row)
    {
        // Получаем текст в каждой ячейке строки таблицы
        QString rowData;
        for (int col = 0; col < ui->tw->columnCount(); ++col)
        {
            QTableWidgetItem *item = ui->tw->item(row, col);
            if (item)
                rowData += item->text() + " ";
        }

        // Проверяем, содержит ли текст строки искомую подстроку
        if (rowData.contains(currentSearchQuery, Qt::CaseInsensitive))
        {
            // Выделяем найденную строку
            ui->tw->selectRow(row);

            // Прокручиваем таблицу к найденной строке
            ui->tw->scrollToItem(ui->tw->item(row, 0));

            // Устанавливаем флаг найденного элемента в true
            foundMatch = true;

            // Обновляем индекс последнего найденного совпадения
            m_lastFoundIndex = row;

            // Выходим из цикла, так как строка найдена
            break;
        }
    }

    // Если совпадение не было найдено, выводим сообщение об этом
    if (!foundMatch)
    {
        QMessageBox::information(this, "Поиск", "Больше совпадений не найдено.");
        // Сбрасываем индекс последнего найденного совпадения
        m_lastFoundIndex = -1;
    }
}

void onb7::generateReportRequest()
{
    // Вызываем функцию selectAll для получения данных о комиссиях из таблицы
    selectAll();

    QString reportText = "Запрос на формирование отчета о моделях:\n\n";
    reportText += "Подготовлен отчет о моделях автомобилей:\n";

    // Добавляем информацию о каждой комиссии из таблицы
    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString brand = ui->tw->item(row, 1)->text();
        QString model = ui->tw->item(row, 2)->text();
        reportText += "-Марка: " + brand + ", Модель: " + model + ";\n";
    }

    QString fileName = "Models.txt"; // Имя файла для сохранения отчета

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Запрос на формирование отчета о моделях успешно сохранен в файле " + fileName);

        // После сохранения отчета можно продолжить работу с приложением или завершить его
        // Например, добавив опцию для закрытия диалогового окна или для продолжения работы с приложением
        // QMessageBox::StandardButton reply = QMessageBox::question(this, "Вопрос", "Желаете продолжить работу?", QMessageBox::Yes|QMessageBox::No);
        // if (reply == QMessageBox::No) {
        //     QApplication::quit(); // Закрыть приложение
        // }
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета о комиссиях на энергоснабжение.");
    }
}
