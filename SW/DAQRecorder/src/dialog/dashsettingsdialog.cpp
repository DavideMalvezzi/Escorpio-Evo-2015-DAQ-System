#include "dashsettingsdialog.h"
#include "ui_dashsettingsdialog.h"
#include "../mainwindow.h"

DashSettingsDialog::DashSettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DashSettingsDialog){
    ui->setupUi(this);

    sqlModel = new QSqlRelationalTableModel(this, MainWindow::database);
    sqlModel->setTable("WidgetCruscotto");
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();

    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Root");
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &ui->painter));
    ui->painter->setItem(item);
    ui->widgetsTree->addTopLevelItem(item);
    ui->widgetsTree->installEventFilter(this);

    QSqlRecord record;
    for(int i=0; i<sqlModel->rowCount(); i++){
        record = sqlModel->record(i);
        ui->painter->addWidget(createWidget(
                                   i,
                                   record.value(1).toInt(),
                                   record.value(2).toInt(),
                                   record.value(3).toString()
                                   ));
    }

    connect(ui->painter, SIGNAL(changeSelected(DashWidget*)), this, SLOT(loadWidgetSettings(DashWidget*)));
    connect(ui->painter, SIGNAL(widgetDeleted(int)), this, SLOT(deleteWidget(int)));
    connect(ui->widgetsTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(selectWidget(QTreeWidgetItem*)));

}

DashWidget* DashSettingsDialog::createWidget(int index, int canID, int type, QString config){
    DashWidget *widget;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(config.toUtf8());
    QJsonObject obj = jsonResponse.object();
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->widgetsTree);

    switch (type) {
        case LED:
            widget = new DashLed(index, canID, obj);
            item->setText(0,obj["name"].toString());
            item->setIcon(0,QIcon(":/images/icons/led.png"));
            item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
            widget->setItem(item);
            ui->widgetsTree->addTopLevelItem(item);
            return widget;
        case LABEL:
            widget = new DashLabel(index, canID, obj);
            item->setText(0,obj["name"].toString());
            item->setIcon(0,QIcon(":/images/icons/label.png"));
            item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
            widget->setItem(item);
            ui->widgetsTree->addTopLevelItem(item);
            return widget;
        case DIGIT:
            widget = new DashSegment(index, canID, obj);
            item->setText(0,obj["name"].toString());
            item->setIcon(0,QIcon(":/images/icons/digit.png"));
            item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
            widget->setItem(item);
            ui->widgetsTree->addTopLevelItem(item);
            return widget;
        case ANGOLMETER:
            widget = new DashSpeedometer(index, canID, obj);
            item->setText(0,obj["name"].toString());
            item->setIcon(0,QIcon(":/images/icons/angol.png"));
            item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
            widget->setItem(item);
            ui->widgetsTree->addTopLevelItem(item);
            return widget;
        case BAR:
            widget = new DashBar(index, canID, obj);
            item->setText(0,obj["name"].toString());
            item->setIcon(0,QIcon(":/images/icons/bars.png"));
            item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
            widget->setItem(item);
            ui->widgetsTree->addTopLevelItem(item);
            return widget;
        case THERMOMETER:
            widget = new DashTermo(index, canID, obj);
            item->setText(0,obj["name"].toString());
            item->setIcon(0,QIcon(":/images/icons/thermometer.png"));
            item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
            widget->setItem(item);
            ui->widgetsTree->addTopLevelItem(item);
            return widget;
        case ALARM:
            widget = new DashAlarm(index, canID, obj);
            item->setText(0,obj["name"].toString());
            item->setIcon(0,QIcon(":/images/icons/warning.png"));
            item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
            widget->setItem(item);
            ui->widgetsTree->addTopLevelItem(item);
            return widget;
    }
    return new DashWidget();
}

void DashSettingsDialog::loadWidgetSettings(DashWidget* widget){
    int i=0;
    QWidget* setting;
    QTableWidgetItem *item;

    while(ui->widgetSettings->rowCount()){
        ui->widgetSettings->removeRow(0);
    }

    if(widget){
        ui->widgetsTree->setCurrentItem(widget->getItem());

        setting = widget->getSettingsWidget(i);
        while(setting!=Q_NULLPTR) {
            ui->widgetSettings->insertRow(i);
            item = new QTableWidgetItem(setting->objectName());
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->widgetSettings->setItem(i,0,item);
            ui->widgetSettings->setCellWidget(i,1,setting);
            if(i%2==1)setting->setStyleSheet("background-color: #353535;");
            setting = widget->getSettingsWidget(++i);
        }
        ui->widgetSettings->resizeRowsToContents();
    }
}

void DashSettingsDialog::deleteWidget(int index){
    sqlModel->removeRow(index);
}

void DashSettingsDialog::reject(){
    if(save()){
        QDialog::reject();
    }else{
        QMessageBox::critical(this, "Errore", "Impossibile salvare! Trovato widget con CanID non valido!");
    }
}

bool DashSettingsDialog::save(){
    QList<DashWidget*>* widgets = ui->painter->getWidgetsList();
    DashWidget* widget;
    QSqlRecord record;

    for(int i=0; i<widgets->count(); i++){
        widget = widgets->at(i);
        if(widget->getIndex() == -1){ //new widget
            //if(widget->getCanID() != -1){
                record = sqlModel->record();
                if(widget->getCanID()>0){
                    record.setValue(1, widget->getCanID());
                }
                record.setValue(2, widget->type());
                record.setValue(3, widget->toJson());
                sqlModel->insertRecord(-1, record);
                widget->setIndex(sqlModel->rowCount()-1);
            //}else{
              //  return false;
            //}
        }else{ //old widget
            record = sqlModel->record(widget->getIndex());
            if(widget->getCanID()>0){
                record.setValue(1, widget->getCanID());
            }
            record.setValue(2, widget->type());
            record.setValue(3, widget->toJson());
            sqlModel->setRecord(widget->getIndex(), record);
        }
    }

    if(sqlModel->database().transaction()){
        return sqlModel->submitAll() && sqlModel->database().commit();
    }

    return false;
}

bool DashSettingsDialog::eventFilter(QObject *obj, QEvent *evt){
    if(obj == ui->widgetsTree){
        if(evt->type() == QEvent::KeyPress){
            QKeyEvent *event = (QKeyEvent*)evt;
            if(event->key() == Qt::Key_Delete && ui->widgetsTree->selectedItems().count()){
                QObject *obj = qvariant_cast<QObject*>(ui->widgetsTree->selectedItems().first()->data(0, Qt::UserRole));
                DashWidget *widget = qobject_cast<DashWidget*>(obj);
                if(widget){
                    int index = ui->widgetsTree->indexOfTopLevelItem(widget->getItem());
                    ui->painter->removeWidget(widget);
                    ui->widgetsTree->setCurrentItem(
                                index<ui->widgetsTree->topLevelItemCount() ?
                                ui->widgetsTree->topLevelItem(index) :
                                ui->widgetsTree->topLevelItem(index-1)
                                );
                    obj = qvariant_cast<QObject*>(ui->widgetsTree->selectedItems().first()->data(0, Qt::UserRole));
                    widget = qobject_cast<DashWidget*>(obj);
                    if(widget)loadWidgetSettings(widget);
                    return true;
                }
            }
        }
    }
    return false;
}

void DashSettingsDialog::selectWidget(QTreeWidgetItem* item){
    QObject *obj = qvariant_cast<QObject*>(item->data(0, Qt::UserRole));
    DashWidget *widget = qobject_cast<DashWidget*>(obj);
    ui->painter->setSelected(widget);
}

void DashSettingsDialog::on_addLabelButton_clicked(){
    DashWidget *widget = new DashLabel;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Label");
    item->setIcon(0,QIcon(":/images/icons/label.png"));
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
    widget->setItem(item);
    ui->painter->addWidget(widget);
    ui->widgetsTree->addTopLevelItem(item);
}

void DashSettingsDialog::on_addLedButton_clicked(){
    DashWidget *widget = new DashLed;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Led");
    item->setIcon(0,QIcon(":/images/icons/led.png"));
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
    widget->setItem(item);
    ui->painter->addWidget(widget);
    ui->widgetsTree->addTopLevelItem(item);
}

void DashSettingsDialog::on_addDigitButton_clicked(){
    DashWidget *widget = new DashSegment;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Digit");
    item->setIcon(0,QIcon(":/images/icons/digit.png"));
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
    widget->setItem(item);
    ui->painter->addWidget(widget);
    ui->widgetsTree->addTopLevelItem(item);
}

void DashSettingsDialog::on_addAngularButton_clicked(){
    DashWidget *widget = new DashSpeedometer;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Angolometer");
    item->setIcon(0,QIcon(":/images/icons/angol.png"));
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
    widget->setItem(item);
    ui->painter->addWidget(widget);
    ui->widgetsTree->addTopLevelItem(item);
}

void DashSettingsDialog::on_addBarButton_clicked(){
    DashWidget *widget = new DashBar;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Bar");
    item->setIcon(0,QIcon(":/images/icons/bars.png"));
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
    widget->setItem(item);
    ui->painter->addWidget(widget);
    ui->widgetsTree->addTopLevelItem(item);
}

void DashSettingsDialog::on_addTermoButton_clicked(){
    DashWidget *widget = new DashTermo;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Termometro");
    item->setIcon(0,QIcon(":/images/icons/thermometer.png"));
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
    widget->setItem(item);
    ui->painter->addWidget(widget);
    ui->widgetsTree->addTopLevelItem(item);
}

void DashSettingsDialog::on_addAlarmButton_clicked(){
    DashWidget *widget = new DashAlarm;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->widgetsTree);
    item->setText(0,"Allarme");
    item->setIcon(0,QIcon(":/images/icons/warning.png"));
    item->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &widget));
    widget->setItem(item);
    ui->painter->addWidget(widget);
    ui->widgetsTree->addTopLevelItem(item);
}

DashSettingsDialog::~DashSettingsDialog(){
    QList<DashWidget*> *list = ui->painter->getWidgetsList();
    while(list->count()){
        delete list->takeAt(0);
    }
    delete sqlModel;
    delete ui;
}











