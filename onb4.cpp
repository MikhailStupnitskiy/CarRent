#include "onb4.h"
#include "ui_onb4.h"
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
onb4::onb4(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb4)
{
    ui->setupUi(this);

    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->btn3, SIGNAL(clicked(bool)), this, SLOT(remove()));
    connect(ui->btn4, SIGNAL(clicked(bool)), this, SLOT(clean()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb4::search);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb4::generateReportRequest);
    QTimer::singleShot(0, this, &onb4::selectAll);

    ui->tw->setColumnCount(4);

    ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("ФИО"));
    ui->tw->setHorizontalHeaderItem(2, new QTableWidgetItem("Контакт"));
    ui->tw->setHorizontalHeaderItem(3, new QTableWidgetItem("Автопарк"));


    ui->tw->horizontalHeader()->setStretchLastSection(true);

    ui->tw->setAutoScroll(true);

    ui->tw->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tw->setSortingEnabled(true);

    ui->tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

    populate1ComboBox();

}

onb4::~onb4()
{
    delete ui;
}

void onb4::on_btnExit_clicked()
{
    AuthenticationManager authenticationManager;

    QString currentUser = authenticationManager.getCurrentUserLogin();

    QStringList allowedButtons = authenticationManager.getAllowedButtons(currentUser);


    close();

    menuwindow *menuWindow = new menuwindow(allowedButtons);
    menuWindow->show();
}



void onb4::populate1ComboBox()
{
    ui->cmb1->clear();
    QSqlQuery query("SELECT address FROM autopark ORDER BY address");
    while (query.next()) {
        QString pu1 = query.value("address").toString();
        ui->cmb1->addItem( pu1);
    }
}
void onb4::selectAll()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }


    QSqlQuery query(DatabaseHelper::getDatabaseConnection());


    QString sqlstr = "SELECT * FROM administrator ORDER BY address";

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


        for (int col = 0; col < ui->tw->columnCount(); ++col)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, query.value(col));
            ui->tw->setItem(rowCount, col, item);
        }

        rowCount++;
    }


    ui->tw->resizeColumnsToContents();
}


void onb4::add()
{
    // Получаем введенные значения
    //QString name = ui->cmb1->currentText();
    //QString costStr = ui->le2->text();
    //QString periodNumberStr = ui->le3->text();
    QString address = ui->cmb1->currentText();
    QString name = ui->le2->text();
    QString contact = ui->le3->text();
    if (name.contains(QRegularExpression("\\d"))) {
        QMessageBox::critical(this, "Ошибка", "Имя не должно содержать цифр.");
        return;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO administrator (admin_name, admin_phone, address) "
                  "VALUES (:name, :phone, :address)");
    query.bindValue(":name", name);
    query.bindValue(":phone", contact);
    query.bindValue(":address", address);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        ui->teResult->append("Ошибка: " + query.lastError().text());
        return;
    }

    qDebug() << "Данные успешно добавлены в базу данных.";
    ui->teResult->append("Успех: данные успешно добавлены.");

    selectAll();
}



void onb4::remove()
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

    if (QMessageBox::question(this, "Удалить", "Удалить строку?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
        return;

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString pu1 = ui->tw->item(curRow, 0)->text();

    QString sqlstr = "DELETE FROM administrator WHERE admin_id = :AdminId";

    query.prepare(sqlstr);
    query.bindValue(":AdminId", pu1);

    if (!query.exec())
    {
        if (query.lastError().text().contains("foreign key", Qt::CaseInsensitive))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно удалить, так как есть связанные записи.");
        }
        else
        {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
        }
        return;
    }

    ui->teResult->append(QString("Удалено %1 строк").arg(query.numRowsAffected()));

    selectAll();
}

void onb4::edit()
{
    QString cmb = ui->cmb1->currentText();
    QString pu1 = ui->le1->text();
    QString pu2 = ui->le2->text();
    QString pu3 = ui->le3->text();

    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->teResult->append("Внимание: строка не выбрана!");
        return;
    }

    if (pu2.contains(QRegularExpression("\\d"))) {
        QMessageBox::critical(this, "Ошибка", "Имя не должно содержать цифр.");
        return;
    }

    QString AdminId = ui->tw->item(curRow, 0)->text();

    QSqlQuery query;
    query.prepare("UPDATE administrator SET admin_name = :1, admin_phone = :2, address = :3 "
                  "WHERE admin_id = :4");
    query.bindValue(":1", pu2);
    query.bindValue(":2", pu3);
    query.bindValue(":3", cmb);
    query.bindValue(":4", AdminId);

    if (!query.exec()) {
        ui->teResult->append("Ошибка: " + query.lastError().text());
        return;
    }

    ui->teResult->append("Успех: данные успешно обновлены.");
    selectAll(); // Обновить таблицу после обновления
}


void onb4::search()
{
    QString currentSearchQuery = ui->leSearch->text().trimmed(); // Обрезаем начальные и конечные пробелы

    if (currentSearchQuery.isEmpty())
    {
        // Если поле ввода пустое, вызываем функцию selectAll для выделения всех строк
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
void onb4::generateReportRequest()
{
    selectAll();

    QString reportText = "Отчет об администраторах:\n\n";
    reportText += "Подготовлен отчет об администраторах:\n";


    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString name = ui->tw->item(row, 1)->text();
        QString contact = ui->tw->item(row, 2)->text();
        QString address = ui->tw->item(row, 3)->text();
        reportText += "- Администратор " + name + ", тел.: " + contact + ", автопарк: " + address + ";\n";
    }

    QString fileName = "administrator.txt";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Отчет об администраторах успешно сохранен в файле " + fileName);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета о поставщиках.");
    }
}


void onb4::on_tw_itemSelectionChanged()
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

    QString pu1 = ui->tw->item(curRow, 0)->text(); // Изменил индекс столбца на 0, если это первый столбец с индексом 0
    QString pu2 = ui->tw->item(curRow, 1)->text(); // Изменил индекс столбца на 1
    QString pu3 = ui->tw->item(curRow, 2)->text(); // Изменил индекс столбца на 2
    QString cmb = ui->tw->item(curRow, 3)->text(); // Изменил индекс столбца на 3

    ui->le1->setText(pu1);
    ui->le2->setText(pu2);
    ui->le3->setText(pu3);
    ui->cmb1->setCurrentText(cmb);
}


void onb4::clean()
{
    ui->cmb1->setCurrentIndex(0);
    ui->le1->clear();
    ui->le2->clear();
    ui->le3->clear();
}

