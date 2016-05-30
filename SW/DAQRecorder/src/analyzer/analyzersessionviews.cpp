#include "analyzersessionviews.h"
#include "ui_analyzersessionviews.h"
#include "../mainwindow.h"
#include "view/chartanalyzer.h"

AnalyzerSessionViews::AnalyzerSessionViews(QWidget *parent) : QWidget(parent),ui(new Ui::AnalyzerSessionViews){
    ui->setupUi(this);
}

void AnalyzerSessionViews::setup(){
    ui->channelList->clear();
    QList<ChannelValues*> values = MainWindow::analyzerSession->getChannelsValues().values();
    QListWidgetItem *item;

    for(int i=0; i<values.size(); i++){
        item = new QListWidgetItem;
        item->setText(values.at(i)->getName());
        item->setData(Qt::UserRole, qVariantFromValue((void*)values.at(i)));
        ui->channelList->insertItem(i,item);
    }
    connect(ui->channelList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addToChart(QListWidgetItem*)));
}

void AnalyzerSessionViews::addToChart(QListWidgetItem* item){
    qDebug()<<"ANALYZER_SESSION_VIEWS: Item click";
    ui->plotter->addChart((ChannelValues*)item->data(Qt::UserRole).value<void*>());
}

AnalyzerSessionViews::~AnalyzerSessionViews(){
    qDebug()<<"ANALYZER_SESSION_VIEWS: Deleting";
    delete ui;
}
