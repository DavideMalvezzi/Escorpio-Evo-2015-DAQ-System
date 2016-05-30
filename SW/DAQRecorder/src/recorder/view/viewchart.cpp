#include "viewchart.h"
#include "ui_viewchart.h"
#include <QWidget>
#include <QSqlQueryModel>
#include "../../mainwindow.h"


ChartView::ChartView(QWidget *parent) : QWidget(parent), ui(new Ui::ChartView){
    ui->setupUi(this);
    charts = new QMap<int, Chart*>;
    viewsParams = new QVector<QVector<ChartParams*>*>;

    ui->plotter->setBackground(QColor(66,66,66,255));
    ui->plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plotter->plotLayout()->clear();

    connect(ui->viewCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loadView()));

    replotTimer = new QTimer;
    replotTimer->setSingleShot(true);
    replotTimer->setInterval(200);
    connect(replotTimer, SIGNAL(timeout()), ui->plotter, SLOT(replot()));
}

void ChartView::setup(){
    viewsParams->clear();
    while(charts->count()){
        delete charts->take(charts->firstKey());
    }

    //Carica impostazioni grafici viste
    int lastCod = -1;
    ChartParams *params;
    QSqlQuery q(MainWindow::database);
    q.exec("SELECT CodFinestra, CodCan, Nome, Colore FROM FinestraGrafici ORDER BY CodFinestra,CodCan ASC");
    while(q.next()){
        if(lastCod != q.value("CodFinestra").toInt()){
            viewsParams->append(new QVector<chartParams*>);
            lastCod = q.value("CodFinestra").toInt();
        }
        params = new ChartParams;
        params->canID = q.value("CodCan").toInt();
        params->name = q.value("Nome").toString();
        params->color = QColor(q.value("Colore").toString());
        viewsParams->last()->append(params);
    }

    //Crea grafici
    int i = 0,j;
    Chart* chart;

    q.exec("SELECT DISTINCT CodCan FROM FinestraGrafici ORDER BY CodCan ASC");
    while(q.next()){
         chart = new Chart(ui->plotter);
         chart->hideXaxis(true);
         charts->insert(q.value("CodCan").toInt(), chart);
         i++;
    }

    for(i=0; i<charts->size(); i++){
        for(j=0; j<charts->size(); j++){
            if(i!=j){
                connect(charts->values().at(i)->rectAxis()->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),
                        charts->values().at(j)->rectAxis()->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
            }
        }
    }

    //Carica viste nella combo
    QSqlQueryModel *viewModel = new QSqlQueryModel;
    viewModel->setQuery("SELECT Nome FROM Finestra ORDER BY IDFinestra", MainWindow::database);
    ui->viewCombo->setModel(viewModel);
    ui->viewCombo->setCurrentIndex(0);

}

void ChartView::loadView(){
    if(viewsParams->count()>0){
        QVector<ChartParams*>* currentViewParams = viewsParams->at(ui->viewCombo->currentIndex());
        Chart* chart;
        ChartParams* params;

        for(int i=0; i<ui->plotter->plotLayout()->rowCount(); i++){
            ui->plotter->plotLayout()->element(i,0)->setVisible(false);
            ui->plotter->plotLayout()->take(ui->plotter->plotLayout()->element(i,0));
        }
        ui->plotter->plotLayout()->simplify();

        for(int i=0; i<currentViewParams->count(); i++){
            params = currentViewParams->at(i);
            chart = charts->value(params->canID);
            chart->setChartName(params->name/*.replace(" ", "\n")*/);
            chart->setPen(QPen(params->color, 1));
            chart->hideXaxis(true);
            chart->setVisible(true);
            ui->plotter->plotLayout()->addElement(i,0,chart->rectAxis());
        }
        chart->hideXaxis(false);

        ui->plotter->replot();
    }
}



void ChartView::onChannelUpdate(int ID, unsigned int packetIndex, QVariant value){
    Chart *chart = charts->value(ID, Q_NULLPTR);
    if(chart != Q_NULLPTR){
        chart->addData(packetIndex, value.toDouble(), ui->checkAutoScroll->isChecked());
        //ui->plotter->replot();
        if(!replotTimer->isActive()){
            replotTimer->start();
        }
    }
}

ChartView::~ChartView(){
    qDebug()<<"CHART_VIEW: Deleting";
    delete ui;
}

void ChartView::on_buttonZoomXplus_clicked(){
    charts->value(charts->firstKey())->zoomInX();
}

void ChartView::on_buttonZoomXminus_clicked(){
    charts->value(charts->firstKey())->zoomOutX();
}

