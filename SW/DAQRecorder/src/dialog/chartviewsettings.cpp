#include "chartviewsettings.h"
#include "ui_chartviewsettings.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>
#include "../mainwindow.h"

ChartViewSettings::ChartViewSettings(QWidget *parent) : QDialog(parent), ui(new Ui::ChartViewSettings){
    ui->setupUi(this);

    sqlModel = new QSqlRelationalTableModel(ui->table, MainWindow::database);
    sqlModel->setTable("FinestraGrafici");
    sqlModel->setRelation(sqlModel->fieldIndex("CodCan"), QSqlRelation("Canale", "CanID", "Nome"));
    sqlModel->setRelation(sqlModel->fieldIndex("CodFinestra"), QSqlRelation("Finestra", "IDFinestra", "Nome"));
    sqlModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    sqlModel->select();

    sqlModel->setHeaderData(1, Qt::Horizontal, "Nome Finestra");
    sqlModel->setHeaderData(2, Qt::Horizontal, "CAN ID");
    sqlModel->setHeaderData(3, Qt::Horizontal, "Nome Grafico");
    sqlModel->setHeaderData(4, Qt::Horizontal, "Colore");

    ui->table->setModel(sqlModel);
    ui->table->setItemDelegate(new ChartViewSettingsDelegate(this));
    ui->table->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->table->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->table->setSortingEnabled(true);
    ui->table->hideColumn(0);
    ui->table->hideColumn(1);
    ui->table->verticalHeader()->setVisible(false);

    connect(ui->addWindowButton, SIGNAL(clicked()), this, SLOT(addWindow()));
    connect(ui->removeWindowButton, SIGNAL(clicked()), this, SLOT(removeWindow()));
    connect(ui->addChartButton, SIGNAL(clicked()), this, SLOT(addChart()));
    connect(ui->removeChartButton, SIGNAL(clicked()), this, SLOT(removeChart()));
    connect(ui->windowsCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(save()));
    connect(ui->windowsCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(selectWindow()));
    connect(ui->OKButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));
    connect(this, SIGNAL(rejected()), this, SLOT(cancelAndClose()));

    QSqlQueryModel *windowsModel = new QSqlQueryModel();
    ui->windowsCombo->setModel(windowsModel);
    reloadWindow();

}

int ChartViewSettings::exec(){
    sqlModel->select();
    sqlModel->relationModel(1)->select();
    sqlModel->relationModel(2)->select();
    reloadWindow();

    QSqlQuery q(MainWindow::database);
    q.exec("SELECT CanID FROM Canale LIMIT 1");
    q.next();
    defaultCanID = q.value(0).toInt();

    return QDialog::exec();
}

void ChartViewSettings::reloadWindow(){
    QSqlQueryModel *windowsModel = (QSqlQueryModel*)ui->windowsCombo->model();
    windowsModel->setQuery("SELECT Nome FROM Finestra", MainWindow::database);
    ui->windowsCombo->setCurrentIndex(0);
}

void ChartViewSettings::addWindow(){
    QSqlQuery q(MainWindow::database);
    if(!ui->newWindowName->text().isEmpty()){
        q.prepare("INSERT INTO Finestra(Nome) VALUES(:nome)");
        q.bindValue(":nome", ui->newWindowName->text());
        q.exec();
        reloadWindow();
        ui->windowsCombo->setCurrentIndex(ui->windowsCombo->findText(ui->newWindowName->text()));
    }
}

void ChartViewSettings::removeWindow(){
    QSqlQuery q(MainWindow::database);
    q.prepare("DELETE FROM Finestra WHERE Nome=:nome");
    q.bindValue(":nome", ui->windowsCombo->currentText());
    q.exec();
    reloadWindow();
}

void ChartViewSettings::selectWindow(){
    QSqlQuery q(MainWindow::database);
    q.prepare("SELECT IDFinestra FROM Finestra WHERE Nome=:nome LIMIT 1");
    q.bindValue(":nome", ui->windowsCombo->currentText());
    q.exec();
    q.next();
    currentWindowID = q.value(0).toInt();
    sqlModel->setFilter(QString("CodFinestra=%1").arg(currentWindowID));
}

void ChartViewSettings::addChart(){
    save();
    QSqlQuery q(MainWindow::database);
    q.prepare("INSERT INTO FinestraGrafici(CodFinestra, CodCan, Nome, Colore) VALUES(:idFinestra, :idCan, 'Nuovo Grafico','#000000')");
    q.bindValue(":idFinestra", currentWindowID);
    q.bindValue(":idCan", defaultCanID);
    q.exec();
    sqlModel->select();

}

void ChartViewSettings::removeChart(){
    int index = ui->table->currentIndex().row();
    save();
    QSqlQuery q(MainWindow::database);
    q.prepare("DELETE FROM FinestraGrafici WHERE IDFinestraGrafici=:id");
    q.bindValue(":id", sqlModel->record(index).value(0).toInt());
    q.exec();
    sqlModel->select();
}

void ChartViewSettings::saveAndClose(){
    if(!save()){
        QMessageBox::critical(this, "Errore", QString("Errore durante il salvataggio: %1").arg(sqlModel->lastError().text()));
    }else{
        accept();
    }
}

bool ChartViewSettings::save(){
    sqlModel->database().transaction();
    if (sqlModel->submitAll()) {
        sqlModel->database().commit();
        return true;
    }
    sqlModel->database().rollback();
    return false;
}

void ChartViewSettings::cancelAndClose(){
    sqlModel->database().rollback();
}


ChartViewSettingsDelegate::ChartViewSettingsDelegate(QWidget *parent) : QSqlRelationalDelegate(parent){}

void ChartViewSettingsDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{

    if(index.column()!=4){
        QSqlRelationalDelegate::paint(painter, option, index);
    }else{
       painter->fillRect(option.rect, QColor(index.data().toString()));
    }
}

bool ChartViewSettingsDelegate::editorEvent(
        QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index){

    if(index.column()!=4)
        return QSqlRelationalDelegate::editorEvent(event, model, option, index);

    if(index.column()==4 && event->type() == QEvent::MouseButtonPress) {
        QColorDialog d(MainWindow::mainWindow);
        if(d.exec() == 1){
            model->setData(index, d.selectedColor().name());
        }
        return false;
    }

    return true;
}

QWidget* ChartViewSettingsDelegate::createEditor(
    QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const{

/*
    if(index.column()==2){
          QSqlQueryModel *model = new QSqlQueryModel();
          model->setQuery("SELECT Nome FROM Canale WHERE (CodTipoDato=1 OR CodTipoDato=2) AND (CodTipoIO=1 OR CodTipoIO=3)", MainWindow::database);

          QComboBox *combo = new QComboBox(aParent);
          combo->setModel(model);
          combo->setModelColumn(0);
          combo->setCurrentIndex(combo->findText(index.data().toString()));

          return combo;
    }*/

    if(index.column()!=4)
        return QSqlRelationalDelegate::createEditor(aParent, option, index);
    return Q_NULLPTR;

}

ChartViewSettings::~ChartViewSettings(){
    delete ui;
}

