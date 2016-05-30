#include "packetsettings.h"
#include "ui_packetsettings.h"
#include "../mainwindow.h"

#include <QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>
#include <QModelIndex>

#include <QLineEdit>
#include <QSpinBox>
#include <QIntValidator>

#include <QSerialPortInfo>

PacketSettingsDialog::PacketSettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PacketSettingsDialog){
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportConfig()));
    connect(ui->buttonAddRow, SIGNAL(clicked()), this, SLOT(addRow()));
    connect(ui->buttonOK, SIGNAL(clicked()), this, SLOT(saveAndClose()));
    connect(ui->buttonSave, SIGNAL(clicked()), this, SLOT(save()));
    connect(this, SIGNAL(rejected()), this, SLOT(cancelAndClose()));

    sqlModel = new QSqlRelationalTableModel(ui->table, MainWindow::database);
    sqlModel->setTable("Canale");
    sqlModel->setRelation(sqlModel->fieldIndex("CodTipoDato"), QSqlRelation("TipoDato", "IDTipoDato", "Tipo"));
    sqlModel->setRelation(sqlModel->fieldIndex("CodTipoIO"), QSqlRelation("TipoIO", "IDTipoIO", "Tipo"));
    sqlModel->setRelation(sqlModel->fieldIndex("CodTipoConversione"), QSqlRelation("TipoConversione", "IDTipoConversione", "Tipo"));
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();

    sqlModel->setHeaderData(0, Qt::Horizontal, "CAN ID");
    sqlModel->setHeaderData(1, Qt::Horizontal, "Nome Canale");
    sqlModel->setHeaderData(2, Qt::Horizontal, "Tipo Dato");
    sqlModel->setHeaderData(3, Qt::Horizontal, "Dimensione (Byte)");
    sqlModel->setHeaderData(4, Qt::Horizontal, "I/O");
    sqlModel->setHeaderData(5, Qt::Horizontal, "Tipo Conversione");

    ui->table->setModel(sqlModel);
    ui->table->setItemDelegate(new PacketSettingsDelegate(this));
    ui->table->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->table->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->table->verticalHeader()->setVisible(false);
}

int PacketSettingsDialog::exec(){
    sqlModel->select();
    ui->table->resizeColumnsToContents();
    ui->table->resizeRowsToContents();
    return QDialog::exec();
}

void PacketSettingsDialog::addRow(){  
    QSqlRecord record = sqlModel->record();
    record.setValue("Nome", "Nuovo canale");
    record.setValue(record.fieldName(2), 0);
    record.setValue("Dimensione", 1);
    record.setValue(record.fieldName(4), 1);
    record.setValue(record.fieldName(5), 0);
    record.setValue("a", 0);
    record.setValue("b", 0);
    record.setValue("c", 0);
    sqlModel->insertRecord(0, record);
    sqlModel->selectRow(0);

    ui->table->resizeColumnsToContents();
    ui->table->resizeRowsToContents();
}

void PacketSettingsDialog::saveAndClose(){
    if(save()){
        accept();
    }
}

bool PacketSettingsDialog::save(){
    sqlModel->database().transaction();
    if (sqlModel->submitAll()) {
        sqlModel->database().commit();
        return true;
    }
    sqlModel->database().rollback();
    QMessageBox::critical(this, "Errore", QString("Errore durante il salvataggio: %1").arg(sqlModel->lastError().text()));
    return false;
}

void PacketSettingsDialog::cancelAndClose(){
    sqlModel->database().rollback();
}

PacketSettingsDelegate::PacketSettingsDelegate(QWidget *parent) : QSqlRelationalDelegate(parent){}

void PacketSettingsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    if(index.column() == 0){
            int value = index.data().toInt();

            QStyleOptionToolButton label;
            label.rect = option.rect;
            label.text = QString("0x%1").arg(QString::number(value,16));

            QApplication::style()->drawControl(QStyle::CE_ToolButtonLabel, &label, painter);
    }else{
        QSqlRelationalDelegate::paint(painter, option, index);
    }
}

QWidget* PacketSettingsDelegate::createEditor(
    QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const{

    if(index.column()==0){
        QSpinBox *canID = new QSpinBox(aParent);
        canID->setRange(1, 2047);
        canID->setDisplayIntegerBase(16);
        return canID;
    }

    if(index.column() == 3){
        QSpinBox *dim = new QSpinBox(aParent);
        dim->setRange(1, 8);
        return dim;
    }
    return QSqlRelationalDelegate::createEditor(aParent, option, index);
}


PacketSettingsDialog::~PacketSettingsDialog(){
    delete ui;
}

void PacketSettingsDialog::exportConfig(){
    if(save()){
        QFileDialog d(this);
        d.setAcceptMode(QFileDialog::AcceptOpen);
        d.setOption(QFileDialog::ShowDirsOnly);
        d.setFileMode(QFileDialog::Directory);
        if(d.exec() == 1){
            QFile file(d.selectedFiles().first().append("/CanConf.txt"));
            if (file.open(QIODevice::WriteOnly)){
                QTextStream stream(&file);
                QSqlQuery q(MainWindow::database);
                q.exec("SELECT COUNT(*) FROM Canale WHERE CodTipoIO <> 2");
                q.next();
                stream<<q.value(0).toInt()<<endl;
                q.exec("SELECT * FROM Canale WHERE CodTipoIO <> 2");
                while(q.next()){
                    QSqlRecord r = q.record();
                    for(int j=0; j<r.count()-1; j++){
                        stream<<r.value(j).toString()<<",";
                    }
                    stream<<endl;
                }
                file.close();
            }
        }
    }
}

int PacketSettingsDialog::getRecordNum(){
    return sqlModel->rowCount();
}

