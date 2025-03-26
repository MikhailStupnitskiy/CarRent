#include "onb3.h"
#include "ui_onb3.h"
#include "menuwindow.h"
#include <QMessageBox>         // Подключаем заголовочный файл для вывода сообщений
#include <QSqlQuery>           // Подключаем заголовочный файл для выполнения SQL-запросов
#include <QSqlError>           // Подключаем заголовочный файл для работы с ошибками SQL
#include <QTimer>              // Подключаем заголовочный файл для работы с таймерами
#include <QDebug>              // Подключаем заголовочный файл для вывода отладочной информации
#include "authenticationmanager.h"
#include "databasehelper.h"
#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QDate>

onb3::onb3(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb3)
{
    ui->setupUi(this);

    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));  // Подключаем кнопку добавления
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(edit()));  // Подключаем кнопку обновления
    connect(ui->btn3, SIGNAL(clicked(bool)), this, SLOT(remove()));  // Подключаем кнопку удаления
    connect(ui->btn4, SIGNAL(clicked(bool)), this, SLOT(clean()));
    connect(ui->tw, SIGNAL(itemSelectionChanged()), this, SLOT(on_twGuests_itemSelectionChanged()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb3::search);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb3::generateReportRequest);
    QTimer::singleShot(0, this, &onb3::selectAll);
    ui->tw->setColumnCount(2);

    ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("Адрес"));



    ui->tw->horizontalHeader()->setStretchLastSection(true);

    ui->tw->setAutoScroll(true);

    ui->tw->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tw->setSortingEnabled(true);

    ui->tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

onb3::~onb3()
{
    delete ui;
}



void onb3::on_btnExit_clicked()
{
    AuthenticationManager authenticationManager;

    QString currentUser = authenticationManager.getCurrentUserLogin();

    QStringList allowedButtons = authenticationManager.getAllowedButtons(currentUser);

    close();

    menuwindow *menuWindow = new menuwindow(allowedButtons);
    menuWindow->show();
}




void onb3::selectAll()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString sqlstr = "SELECT * FROM autopark ORDER BY autopark_id";

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

        QTableWidgetItem *idItem = new QTableWidgetItem(query.value("autopark_id").toString());
        ui->tw->setItem(rowCount, 0, idItem);

        ui->tw->setItem(rowCount, 1, new QTableWidgetItem(query.value("address").toString()));



        for (int j = 0; j < ui->tw->columnCount(); ++j)
        {
            ui->tw->item(rowCount, j)->setTextAlignment(Qt::AlignCenter);
        }

        rowCount++;
    }

    ui->tw->resizeColumnsToContents();
}


void onb3::add()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    QString pu1 = ui->le2->text(); // Адрес

    QSqlQuery query;  // Создаем объект запроса

    QString sqlstr = "INSERT INTO autopark(address) VALUES (?)";  // Формируем строку SQL-запроса для вставки данных в таблицу

    query.prepare(sqlstr);

    query.bindValue(0, pu1);

    if (!query.exec())
    {
        ui->teResult->append(query.lastQuery());
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    ui->teResult->append(QString("Добавлено %1 строк").arg(query.numRowsAffected()));

    selectAll();
}


void onb3::on_tw_itemSelectionChanged()
{
    int curRow = ui->tw->currentRow();

    if (curRow < 0 || curRow >= ui->tw->rowCount()) {
        ui->le1->clear();
        ui->le2->clear();
        return;
    }

    QString pu1 = ui->tw->item(curRow, 0) ? ui->tw->item(curRow, 0)->text() : "";
    QString pu2 = ui->tw->item(curRow, 1) ? ui->tw->item(curRow, 1)->text() : "";


    ui->le1->setText(pu1);
    ui->le2->setText(pu2);

}



void onb3::remove()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    int currow = ui->tw->currentRow();

    if (currow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не выбрана строка!");
        return;
    }

    if (QMessageBox::question(this, "Удалить", "Удалить строку?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
        return;

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString pu1 = ui->tw->item(currow, 0)->text();

    QString sqlstr = "DELETE FROM autopark WHERE autopark_id = '" + pu1 + "'";

    if (!query.exec(sqlstr))
    {
        if (query.lastError().text().contains("foreign key", Qt::CaseInsensitive))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно удалить, так как есть связанные.");
        }
        else
        {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
        }
        return;
    }

    ui->teResult->append(QString("Deleted %1 rows").arg(query.numRowsAffected()));

    selectAll();
}




void onb3::clean()
{
    ui->le1->clear();
    ui->le2->clear();
}

void onb3::edit()
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



    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString sqlstr = "UPDATE autopark SET address = ? WHERE autopark_id = ?";

    query.prepare(sqlstr);

    query.bindValue(0, pu2);
    query.bindValue(1, pu1);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    ui->teResult->append(QString("Updated %1 rows").arg(query.numRowsAffected()));

    selectAll();
}


void onb3::search()
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

void onb3::generateReportRequest()
{
    selectAll();


    QString reportText = "Отчет об автопарках:\n\n";

    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString address = ui->tw->item(row, 1)->text();
        QString Id = ui->tw->item(row, 0)->text();
        reportText += "-Автопарк№ " + Id + ": "+ address + "\n";

    }

    QString fileName = "Autopark.txt"; // Укажите путь к файлу отчета
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Отчет успешно сохранен в файле: " + fileName);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета.");
    }
}



