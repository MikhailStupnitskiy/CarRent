#include "onb6.h"
#include "ui_onb6.h"
#include "authenticationmanager.h"
#include "databasehelper.h"
#include "menuwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QDateTime>
#include <QHBoxLayout>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
onb6::onb6(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb6)
{
    ui->setupUi(this);

    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->btn3, SIGNAL(clicked(bool)), this, SLOT(remove()));
    connect(ui->btn4, SIGNAL(clicked(bool)), this, SLOT(clean()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb6::search);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb6::generateReportRequest);
    QTimer::singleShot(0, this, &onb6::selectAll);

    ui->tw->setColumnCount(9);

    ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("ID клиента"));
    ui->tw->setHorizontalHeaderItem(2, new QTableWidgetItem("ФИО"));
    ui->tw->setHorizontalHeaderItem(3, new QTableWidgetItem("ID авто"));
    ui->tw->setHorizontalHeaderItem(4, new QTableWidgetItem("Цена"));
    ui->tw->setHorizontalHeaderItem(5, new QTableWidgetItem("ID поездки"));
    ui->tw->setHorizontalHeaderItem(6, new QTableWidgetItem("Дата"));
    ui->tw->setHorizontalHeaderItem(7, new QTableWidgetItem("Длительность"));
    ui->tw->setHorizontalHeaderItem(8, new QTableWidgetItem("Километраж"));


    ui->tw->horizontalHeader()->setStretchLastSection(true);

    ui->tw->setAutoScroll(true);

    ui->tw->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tw->setSortingEnabled(true);

    ui->tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

    populate1ComboBox();
    populate2ComboBox();

}

onb6::~onb6()
{
    delete ui;
}


void onb6::on_btnExit_clicked()
{
    AuthenticationManager authenticationManager;

    QString currentUser = authenticationManager.getCurrentUserLogin();

    QStringList allowedButtons = authenticationManager.getAllowedButtons(currentUser);

    close();

    menuwindow *menuWindow = new menuwindow(allowedButtons);
    menuWindow->show();
}



void onb6::populate1ComboBox()
{
    ui->cmb1->clear();
    QSqlQuery query("SELECT client_id FROM client ORDER BY client_id");
    while (query.next()) {
        QString pu1 = query.value("client_id").toString();
        ui->cmb1->addItem(pu1);
    }
}
void onb6::populate2ComboBox()
{
    ui->cmb2->clear();
    QSqlQuery query("SELECT auto_id FROM automobile ORDER BY auto_id");
    while (query.next()) {
        QString pu1 = query.value("auto_id").toString();
        ui->cmb2->addItem(pu1);
    }
}
void onb6::selectAll()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString sqlstr = "SELECT rent_id, rent.client_id, client_name, rent.auto_id, price, rent.trip_id, trip_date, duration, trip_mileage FROM rent "
                    "JOIN client ON client.client_id = rent.client_id "
                    "JOIN automobile ON automobile.auto_id = rent.auto_id "
                    "JOIN trip ON trip.trip_id = rent.trip_id "
                    "JOIN model ON model.model_name = automobile.model_name";

    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }


    ui->tw->clearContents();


    ui->tw->setRowCount(0);

    int rowCount = 0;


    while (query.next())
    {

        ui->tw->insertRow(rowCount);

        // Устанавливаем идентификатор строки
        QTableWidgetItem *idItem = new QTableWidgetItem(query.value("rent_id").toString());
        ui->tw->setItem(rowCount, 0, idItem);


        ui->tw->setItem(rowCount, 1, new QTableWidgetItem(query.value("client_id").toString()));
        ui->tw->setItem(rowCount, 2, new QTableWidgetItem(query.value("client_name").toString()));
        ui->tw->setItem(rowCount, 3, new QTableWidgetItem(query.value("auto_id").toString()));
        ui->tw->setItem(rowCount, 4, new QTableWidgetItem(query.value("price").toString()));
        ui->tw->setItem(rowCount, 5, new QTableWidgetItem(query.value("trip_id").toString()));
        ui->tw->setItem(rowCount, 6, new QTableWidgetItem(query.value("trip_date").toString()));
        ui->tw->setItem(rowCount, 7, new QTableWidgetItem(query.value("duration").toString()));
        ui->tw->setItem(rowCount, 8, new QTableWidgetItem(query.value("trip_mileage").toString()));

        for (int j = 0; j < ui->tw->columnCount(); ++j)
        {
            ui->tw->item(rowCount, j)->setTextAlignment(Qt::AlignCenter);
        }

        rowCount++;
    }


    ui->tw->resizeColumnsToContents();
}

void onb6::add()
{
    QString cmb1 = ui->cmb1->currentText(); // Получаем данные из первого выпадающего списка
    QString cmb2 = ui->cmb2->currentText(); // Получаем данные из второго выпадающего списка
    QString pu2 = ui->le2->text(); // Получаем введенное имя
    QString pu3 = ui->le3->text(); // Получаем введенную сумму долга
    QString pu4 = ui->le4->text(); // Получаем введенное имя
    QString pu5 = ui->le5->text(); // Получаем введенную сумму долга

    int client = cmb1.toInt();
    int Auto = cmb2.toInt();
    int Trip = pu2.toInt();
    int Duration = pu4.toInt();
    int Mileage = pu5.toInt();
    QDate pu33 = QDate::fromString(pu3, "dd.MM.yyyy");
    QString pu333 = pu33.toString("yyyy-MM-dd");

    QSqlQuery query;
    query.prepare("INSERT INTO trip(trip_id, trip_date, duration, trip_mileage) "
                  "VALUES (:1, :2, :3, :4)");

    query.bindValue(":1", Trip); //
    query.bindValue(":2", pu333); //
    query.bindValue(":3", Duration); //
    query.bindValue(":4", Mileage); //

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        ui->teResult->append("Error: " + query.lastError().text());
        return;
    }
    query.prepare("INSERT INTO rent(client_id, auto_id, trip_id) "
                  "VALUES (:5, :6, :1)");
    query.bindValue(":1", Trip); //
    query.bindValue(":5", client); //
    query.bindValue(":6", Auto); //


    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        ui->teResult->append("Error: " + query.lastError().text());
        return;
    }

    qDebug() << "Данные успешно добавлены в базу данных.";

    ui->teResult->append("Успех: данные успешно добавлены.");
    selectAll(); // Обновить таблицу, чтобы отобразить новые данные
}


void onb6::remove()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не выбрана строка!");
        return;
    }

    if (QMessageBox::question(this, "Удалить", "Удалить строку?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
        return;

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString pu1 = ui->tw->item(curRow, 0)->text();

    query.prepare("DELETE FROM rent WHERE rent_id = :pupu");
    query.bindValue(":pupu", pu1);

    if (!query.exec()) {
        // Проверяем текст ошибки на наличие фразы "foreign key"
        if (query.lastError().text().contains("foreign key", Qt::CaseInsensitive))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно удалить, так как есть связанные.");
        }
        else
        {
            // Если запрос не выполнен по другой причине, выводим сообщение об ошибке
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
        }
        return;
    }

    ui->teResult->append(QString("Deleted %1 rows").arg(query.numRowsAffected()));

    selectAll();
}

void onb6::edit()
{
    QString cmb1 = ui->cmb1->currentText(); // Получаем данные из первого выпадающего списка
    QString cmb2 = ui->cmb2->currentText(); // Получаем данные из второго выпадающего списка
    QString pu2 = ui->le2->text(); // Получаем введенное имя
    QString pu3 = ui->le3->text(); // Получаем введенную сумму долга
    QString pu4 = ui->le4->text(); // Получаем введенное имя
    QString pu5 = ui->le5->text(); // Получаем введенную сумму долга


    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->teResult->append("Внимание: строка не выбрана!");
        return;
    }

    int client = cmb1.toInt();
    int Auto = cmb2.toInt();
    int Duration = pu4.toInt();
    int Mileage = pu5.toInt();
    QDate pu33 = QDate::fromString(pu3, "dd.MM.yyyy");
    QString pu333 = pu33.toString("yyyy-MM-dd");
    QString RentId = ui->tw->item(curRow, 0)->text();
    QString TripId = ui->tw->item(curRow, 5)->text();


    QSqlQuery query;
    query.prepare("UPDATE trip SET trip_date = :Date, duration = :Duration, trip_mileage = :Mileage "
                  "WHERE trip_id = :TripId");
    query.bindValue(":Date", pu33);
    query.bindValue(":Duration", Duration);
    query.bindValue(":Mileage", Mileage);
    query.bindValue(":TripId", TripId);

    if (!query.exec()) {
        ui->teResult->append("Ошибка: " + query.lastError().text());
        return;
    }
    query.prepare("UPDATE rent SET client_id = :Client, auto_id = :Auto, trip_id = :Trip "
                  "WHERE rent_id = :RentId");
    query.bindValue(":Client", client);
    query.bindValue(":Auto", Auto);
    query.bindValue(":Trip", TripId);
    query.bindValue(":RentId", RentId);
    if (!query.exec()) {
        ui->teResult->append("Ошибка: " + query.lastError().text());
        return;
    }
    ui->teResult->append("Успех: данные успешно обновлены.");
    selectAll(); // Обновить таблицу после обновления
}

void onb6::search()
{
    QString currentSearchQuery = ui->leSearch->text().trimmed(); // Обрезаем начальные и конечные пробелы

    if (currentSearchQuery.isEmpty())
    {
        selectAll();
        return; // Завершаем выполнение функции
    }

    if (currentSearchQuery != m_lastSearchQuery)
    {
        m_lastSearchQuery = currentSearchQuery;

        m_lastFoundIndex = -1;
    }

    ui->tw->clearSelection();

    bool foundMatch = false;

    for (int row = m_lastFoundIndex + 1; row < ui->tw->rowCount(); ++row)
    {
        QString rowData;
        for (int col = 0; col < ui->tw->columnCount(); ++col)
        {
            QTableWidgetItem *item = ui->tw->item(row, col);
            if (item)
                rowData += item->text() + " ";
        }

        if (rowData.contains(currentSearchQuery, Qt::CaseInsensitive))
        {
            ui->tw->selectRow(row);

            ui->tw->scrollToItem(ui->tw->item(row, 0));

            foundMatch = true;

            m_lastFoundIndex = row;

            break;
        }
    }

    if (!foundMatch)
    {
        QMessageBox::information(this, "Поиск", "Больше совпадений не найдено.");
        m_lastFoundIndex = -1;
    }
}
void onb6::generateReportRequest()
{
    // Вызываем функцию selectAll для получения данных о потребителях из таблицы
    selectAll();
    QString cmb1 = ui->cmb1->currentText();

    QString reportText = "Запрос на формирование отчета о клиенте:\n\n";
    reportText += "Подготовлен отчет о Клиенте: ";
    int totalTrips = 0;
    int total = 0;
    QString clientName;
    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString client = ui->tw->item(row, 1)->text();
        if (client == cmb1)
        {
            clientName = ui->tw->item(row, 2)->text();
            break;
        }
    }
    reportText += clientName + "\n";
    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString client = ui->tw->item(row, 1)->text();
        if (client == cmb1)
        {
            totalTrips += 1;
            QString auto_id = ui->tw->item(row, 3)->text();
            QString price = ui->tw->item(row, 4)->text();
            QString date = ui->tw->item(row, 6)->text();
            QString duration = ui->tw->item(row, 7)->text();
            reportText += "-Поездка№" +  QString::number(totalTrips);
            reportText += " : Автомобиль №" + auto_id + ", цена за час: " + price + ", дата: " + date + ", продолжительность: " + duration + ";\n";
            total += duration.toInt()*price.toInt();
        }

    }

    reportText += "\nВсего поездок: " +  QString::number(totalTrips);
    reportText += "\nПотрачено: " +  QString::number(total);

    QString fileName = "Client.txt"; // Имя файла для сохранения отчета

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Запрос на формирование отчета о клиенте успешно сохранен в файле " + fileName);


    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета о потребителях.");
    }
}


void onb6::on_tw_itemSelectionChanged()
{
    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->cmb2->setCurrentIndex(0);
        ui->cmb1->setCurrentIndex(0);
        ui->le1->clear();
        ui->le4->clear();
        ui->le3->clear();
        ui->le4->clear();
        ui->le5->clear();
        return;
    }

    QString pu1 = ui->tw->item(curRow, 0)->text(); // Изменил индекс столбца на 0, если это первый столбец с индексом 0
    QString pu2 = ui->tw->item(curRow, 5)->text(); // Изменил индекс столбца на 1
    QString pu3 = ui->tw->item(curRow, 6)->text(); // Изменил индекс столбца на 2
    QString pu4 = ui->tw->item(curRow, 7)->text(); // Изменил индекс столбца на 2
    QString pu5 = ui->tw->item(curRow, 8)->text(); // Изменил индекс столбца на 2
    QString cmb = ui->tw->item(curRow, 3)->text(); // Изменил индекс столбца на 3
    QString cmbb = ui->tw->item(curRow, 1)->text(); // Изменил индекс столбца на 3

    QDate pu33 = QDate::fromString(pu3, "yyyy-MM-dd");
    ui->le1->setText(pu1);
    ui->le4->setText(pu4);
    ui->le2->setText(pu2);
    ui->le5->setText(pu5);
    ui->le3->setDate(pu33);
    ui->cmb2->setCurrentText(cmb);
    ui->cmb1->setCurrentText(cmbb);
}


void onb6::clean()
{
    ui->cmb2->setCurrentIndex(0);
    ui->cmb1->setCurrentIndex(0);
    ui->le1->clear();
    ui->le4->clear();
    ui->le3->clear();
    ui->le2->clear();
    ui->le5->clear();
}

