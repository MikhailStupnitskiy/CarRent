

#include "onb2.h"
#include "ui_onb2.h"
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
onb2::onb2(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb2)
{
    ui->setupUi(this);

    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->btn3, SIGNAL(clicked(bool)), this, SLOT(remove()));
    connect(ui->btn4, SIGNAL(clicked(bool)), this, SLOT(clean()));
    connect(ui->cmb1, SIGNAL(currentIndexChanged(int)), this, SLOT(populate2ComboBox()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb2::search);
    connect(ui->btn5, &QPushButton::clicked, this, &onb2::filter);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb2::generateReportRequest);
    QTimer::singleShot(0, this, &onb2::selectAll);

     ui->tw->setColumnCount(7);

     ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
     ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("Марка"));
     ui->tw->setHorizontalHeaderItem(2, new QTableWidgetItem("Модель"));
     ui->tw->setHorizontalHeaderItem(3, new QTableWidgetItem("Цена"));
     ui->tw->setHorizontalHeaderItem(4, new QTableWidgetItem("Автопарк"));
     ui->tw->setHorizontalHeaderItem(5, new QTableWidgetItem("Год выпуска"));
     ui->tw->setHorizontalHeaderItem(6, new QTableWidgetItem("Пробег"));


    ui->tw->horizontalHeader()->setStretchLastSection(true);

    ui->tw->setAutoScroll(true);

    ui->tw->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tw->setSortingEnabled(true);
    //ui->tw->sortByColumn(0, Qt::AscendingOrder);

    ui->tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

    populate1ComboBox();
    populate2ComboBox();
    populate3ComboBox();

}

onb2::~onb2()
{
    delete ui;
}



void onb2::on_btnExit_clicked()
{
    AuthenticationManager authenticationManager;

    QString currentUser = authenticationManager.getCurrentUserLogin();

    QStringList allowedButtons = authenticationManager.getAllowedButtons(currentUser);


    close();

    menuwindow *menuWindow = new menuwindow(allowedButtons);
    menuWindow->show();
}



void onb2::populate1ComboBox()
{
    ui->cmb1->clear();
    QSqlQuery query("SELECT brand_name FROM brand ORDER BY brand_name");
    while (query.next()) {
        QString pu1 = query.value("brand_name").toString();
        ui->cmb1->addItem( pu1);
    }
}

void onb2::populate2ComboBox()
{
    ui->cmb2->clear();
    QString pu1 = ui->cmb1->currentText();

    QSqlQuery query;
    query.prepare("SELECT model_name FROM model "
                  "JOIN brand ON brand.brand_name = model.brand_name "
                  "WHERE brand.brand_name = ? "
                  "ORDER BY model_name");
    query.bindValue(0, pu1);

    if (query.exec()) {
        while (query.next()) {
            QString model_name = query.value("model_name").toString();
            ui->cmb2->addItem(model_name);
        }
    } else {
        // Обработка ошибки выполнения запроса
        qDebug() << "Query execution error: " << query.lastError().text();
    }
}

void onb2::populate3ComboBox()
{
    ui->cmb3->clear();
    QSqlQuery query("SELECT address FROM autopark ORDER BY address");
    while (query.next()) {
        QString pu1 = query.value("address").toString();
        ui->cmb3->addItem( pu1);
    }
}

void onb2::selectAll()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }


    QSqlQuery query(DatabaseHelper::getDatabaseConnection());


    QString sqlstr = "SELECT auto_id, brand_name, automobile.model_name, price, address, auto_year, mileage FROM automobile "
                     "JOIN model ON model.model_name = automobile.model_name "
                     "ORDER BY auto_id";

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

        QTableWidgetItem *idItem = new QTableWidgetItem(query.value("auto_id").toString());
        ui->tw->setItem(rowCount, 0, idItem);


        ui->tw->setItem(rowCount, 1, new QTableWidgetItem(query.value("brand_name").toString()));
        ui->tw->setItem(rowCount, 2, new QTableWidgetItem(query.value("model_name").toString()));
        ui->tw->setItem(rowCount, 3, new QTableWidgetItem(query.value("price").toString()));
        ui->tw->setItem(rowCount, 4, new QTableWidgetItem(query.value("address").toString()));
        ui->tw->setItem(rowCount, 5, new QTableWidgetItem(query.value("auto_year").toString()));
        ui->tw->setItem(rowCount, 6, new QTableWidgetItem(query.value("mileage").toString()));


        for (int j = 0; j < ui->tw->columnCount(); ++j)
        {
            ui->tw->item(rowCount, j)->setTextAlignment(Qt::AlignCenter);
        }

        rowCount++;
    }


    ui->tw->resizeColumnsToContents();
}


void onb2::filter()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    QString pu3 = ui->cmb3->currentText();
    QSqlQuery query(DatabaseHelper::getDatabaseConnection());


    QString sqlstr = "SELECT auto_id, brand_name, automobile.model_name, price, address, auto_year, mileage FROM automobile "
                     "JOIN model ON model.model_name = automobile.model_name "
                     "WHERE address = ?";
    query.prepare(sqlstr);
    query.bindValue(0, pu3);
    if (!query.exec())
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

        QTableWidgetItem *idItem = new QTableWidgetItem(query.value("auto_id").toString());
        ui->tw->setItem(rowCount, 0, idItem);


        ui->tw->setItem(rowCount, 1, new QTableWidgetItem(query.value("brand_name").toString()));
        ui->tw->setItem(rowCount, 2, new QTableWidgetItem(query.value("model_name").toString()));
        ui->tw->setItem(rowCount, 3, new QTableWidgetItem(query.value("price").toString()));
        ui->tw->setItem(rowCount, 4, new QTableWidgetItem(query.value("address").toString()));
        ui->tw->setItem(rowCount, 5, new QTableWidgetItem(query.value("auto_year").toString()));
        ui->tw->setItem(rowCount, 6, new QTableWidgetItem(query.value("mileage").toString()));


        for (int j = 0; j < ui->tw->columnCount(); ++j)
        {
            ui->tw->item(rowCount, j)->setTextAlignment(Qt::AlignCenter);
        }

        rowCount++;
    }


    ui->tw->resizeColumnsToContents();
}

void onb2::add()
{
    QString pu1 = ui->cmb2->currentText();
    int pu2 = ui->le2->text().toInt();
    QString pu3 = ui->cmb3->currentText();
    int pu4 = ui->le3->text().toInt();
    int pu5 = ui->le4->text().toInt();


    if (pu2 < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Цена не может быть отрицательной.");
        return;
    }
    if (pu4 < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Год выпуска не может быть отрицательным.");
        return;
    }
    if (pu5 < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Пробег не может быть отрицательным.");
        return;
    }



    QSqlQuery query;
    query.prepare("INSERT INTO automobile (model_name, price, address, auto_year, mileage) "
                  "VALUES (:1, :2, :3, :4, :5)");

    query.bindValue(":1", pu1);
    query.bindValue(":2", pu2);
    query.bindValue(":3", pu3);
    query.bindValue(":4", pu4);
    query.bindValue(":5", pu5);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        ui->teResult->append("Error: " + query.lastError().text());
        return;
    }

    qDebug() << "Данные успешно добавлены в базу данных.";

    ui->teResult->append("Успех: данные успешно добавлены.");
    selectAll();
}


void onb2::remove()
{
    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->teResult->append("Внимание: строка не выбрана!");
        return;
    }

    QString pu1 = ui->tw->item(curRow, 0)->text();

    if (QMessageBox::question(this, "Удалить", "Удалить строку?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
        return;

    QSqlQuery query;
    query.prepare("DELETE FROM automobile WHERE auto_id = :AutoId");
    query.bindValue(":AutoId", pu1);

    if (!query.exec()) {
        // Проверяем текст ошибки на наличие фразы "foreign key"
        if (query.lastError().text().contains("foreign key", Qt::CaseInsensitive))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно удалить запись, так как есть связанные данные.");
        }
        else
        {
            // Если запрос не выполнен по другой причине, выводим сообщение об ошибке
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
        }
        return;
    }

    ui->teResult->append("Успех: запись успешно удалена.");
    selectAll();
}

void onb2::edit()
{
    QString cmbb1 = ui->cmb1->currentText();
    QString cmbb2 = ui->cmb2->currentText();
    QString cmbb3 = ui->cmb3->currentText();
    QString pu1 = ui->le1->text();
    QString pu2 = ui->le2->text();
    QString pu3 = ui->le3->text();
    QString pu4 = ui->le4->text();

    if (pu2.toInt() <= 0)
    {
        QMessageBox::critical(this, "Ошибка", "Сумма должна быть больше 0.");
        return;
    }
    if (pu3.toInt() <= 0)
    {
        QMessageBox::critical(this, "Ошибка", "Год выпуска должен быть больше 0.");
        return;
    }
    if (pu4.toInt() <= 0)
    {
        QMessageBox::critical(this, "Ошибка", "Пробег должен быть больше 0.");
        return;
    }

    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->teResult->append("Внимание: строка не выбрана!");
        return;
    }

    QString AutoId = ui->tw->item(curRow, 0)->text();

    QSqlQuery query;
    query.prepare("UPDATE automobile SET model_name = :Model, price = :Price, address = :Address, auto_year = :Year, mileage = :Mileage "
                  "WHERE auto_id = :AutoId");
    query.bindValue(":Model", cmbb2);
    query.bindValue(":Price", pu2);
    query.bindValue(":Address", cmbb3);
    query.bindValue(":Year", pu3);
    query.bindValue(":Mileage", pu4);
    query.bindValue(":AutoId", AutoId);

    if (!query.exec()) {
        ui->teResult->append("Ошибка: " + query.lastError().text());
        return;
    }

    ui->teResult->append("Успех: данные успешно обновлены.");
    selectAll(); // Обновить таблицу после обновления
}

void onb2::search()
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
void onb2::generateReportRequest()
{

    selectAll();

    QString reportText = "Запрос на формирование отчета об автомобилях:\n\n";
    reportText += "Подготовлен отчет об автомобилях:\n";


    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString AutoId = ui->tw->item(row, 0)->text();
        QString brand = ui->tw->item(row, 1)->text();
        QString model = ui->tw->item(row, 2)->text();
        QString price = ui->tw->item(row, 3)->text();
        QString address = ui->tw->item(row, 4)->text();
        QString year = ui->tw->item(row, 5)->text();
        QString mileage = ui->tw->item(row, 6)->text();
        reportText += "- Автомобиль № " + AutoId + ": " + brand + " " + model + ", цена за час: " + price + ". Автопарк: " + address + ". Год выпуска: "
                      + year + ". Пробег: " + mileage + ";\n";
    }


    QString fileName = "Auto_report_request.txt";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Запрос на формирование отчета об автомобилях успешно сохранен в файле " + fileName);


    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета об автомобилях.");
    }
}


void onb2::on_tw_itemSelectionChanged()
{
    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->cmb1->setCurrentIndex(0);
        ui->le1->clear();
        ui->le2->clear();
        ui->le3->clear();
        return;
    }

    QString pu1 = ui->tw->item(curRow, 0)->text();
    QString pu2 = ui->tw->item(curRow, 3)->text();
    QString pu3 = ui->tw->item(curRow, 5)->text();
    QString pu4 = ui->tw->item(curRow, 6)->text();
    QString cmbb1 = ui->tw->item(curRow, 1)->text();
    QString cmbb2 = ui->tw->item(curRow, 2)->text();
    QString cmbb3 = ui->tw->item(curRow, 4)->text();

    ui->le1->setText(pu1);
    ui->le2->setText(pu2);
    ui->le3->setText(pu3);
    ui->le4->setText(pu4);
    ui->cmb1->setCurrentText(cmbb1);
    ui->cmb2->setCurrentText(cmbb2);
    ui->cmb3->setCurrentText(cmbb3);
}


void onb2::clean()
{
    ui->cmb1->setCurrentIndex(0);
    ui->cmb2->setCurrentIndex(0);
    ui->cmb3->setCurrentIndex(0);
    ui->le1->clear();
    ui->le2->clear();
    ui->le3->clear();
    ui->le4->clear();
    selectAll();
}

