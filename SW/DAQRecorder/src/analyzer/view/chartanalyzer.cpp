#include "chartanalyzer.h"
#include "ui_chartanalyzer.h"
#include "../../recorder/recordingsession.h"


Qt::GlobalColor ChartAnalyzer::colors[6] = {Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan, Qt::yellow};

ChartAnalyzer::ChartAnalyzer(QWidget *parent) : QWidget(parent),ui(new Ui::ChartAnalyzer){
    ui->setupUi(this);
    ui->plotter->setBackground(QColor(66,66,66,255));
    ui->plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plotter->plotLayout()->clear();
    chartCount = 0;
    currentColor = 0;

    ui->plotter->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plotter, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contexMenu(QPoint)));
    /*
    ui->plotter->plotLayout()->clear();



    layout->addElement(0,0,axis1);
    layout->addElement(1,0,axis2);
    ui->plotter->plotLayout()->addElement(0,0,layout);

    axis1->axis(QCPAxis::atBottom)->setVisible(false);
    axis2->axis(QCPAxis::atBottom)->setVisible(false);

    ui->plotter->addGraph(axis1->axis(QCPAxis::atBottom),axis1->axis(QCPAxis::atLeft));
    connect(ui->plotter, SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,QMouseEvent*))
    ui->plotter->addGraph(axis2->axis(QCPAxis::atBottom),axis2->axis(QCPAxis::atLeft));
    connect(axis1->axis(QCPAxis::atBottom),SIGNAL(rangeChanged(QCPRange)),axis2->axis(QCPAxis::atBottom),SLOT(setRange(QCPRange)));
    connect(axis2->axis(QCPAxis::atBottom),SIGNAL(rangeChanged(QCPRange)),axis1->axis(QCPAxis::atBottom),SLOT(setRange(QCPRange)));


    QVector<double> xc,yc;
    for(int i=0; i<100; i++){
        xc.append(i);
        yc.append(qSin(i*0.017));
    }
    QVector<double> xc2,yc2;
    for(int i=0; i<100; i++){
        xc2.append(i);
        yc2.append(qCos(i*0.017));
    }
    ui->plotter->graph(0)->setData(xc,yc);
    ui->plotter->graph(1)->setData(xc2,yc2);
    ui->plotter->rescaleAxes();



    ui->plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plotter->axisRect(0)->setAutoMargins(QCP::msLeft);
    ui->plotter->axisRect(1)->setAutoMargins(QCP::msLeft);
    ui->plotter->axisRect(0)->setMargins(QMargins(0,0,0,0));
    ui->plotter->axisRect(1)->setMargins(QMargins(0,0,0,0));

    /*
    QCPCurve *trackCurve = new QCPCurve(ui->plotter->xAxis, ui->plotter->yAxis);

    QFile lat("C:\\Users\\user\\Desktop\\lat.txt");
    QFile lon("C:\\Users\\user\\Desktop\\lon.txt");
    lat.open(QFile::ReadOnly);
    lon.open(QFile::ReadOnly);


    for(int i=0; i<101300;i++){
        trackCurve->addData(0,QString(lon.readLine()).toFloat(),QString(lat.readLine()).toFloat());
    }


    trackCurve->setPen(QPen(Qt::white, 3));

    ui->plotter->addPlottable(trackCurve);

    ui->plotter->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->plotter->setBackground(QColor(66,66,66,255));
    ui->plotter->xAxis->setVisible(false);
    ui->plotter->yAxis->setVisible(false);
    ui->plotter->axisRect()->setAutoMargins(QCP::msNone);
    ui->plotter->axisRect()->setMargins(QMargins(0,0,0,0));

    ui->plotter->rescaleAxes();
    ui->plotter->replot();
    ui->plotter->setNoAntialiasingOnDrag(true);
    ui->plotter->installEventFilter(this);
    */

}

void ChartAnalyzer::addChart(ChannelValues *channel){
    if(channel->getType() == SIGN_NUMBER_CONV || channel->getType() == UNSIGN_NUMBER_CONV){
        qDebug()<<"CHART_ANALYZER: add chart";
        Chart *c = new Chart(ui->plotter);
        c->setChartName(channel->getName());
        c->setPen(QPen(colors[currentColor]));
        currentColor = (currentColor+1)%6;

        for(int i=0; i<channel->getCount(); i++){
            c->addData(i, channel->getData()[i].toDouble(), false);
        }

        if(ui->plotter->plotLayout()->rowCount()){
            c->rectAxis()->axis(QCPAxis::atBottom)->setRange(ui->plotter->axisRect()->axis(QCPAxis::atBottom)->range());

            connect(ui->plotter->axisRect()->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),
                    c->rectAxis()->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
            connect(c->rectAxis()->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),
                    ui->plotter->axisRect()->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
       }

       ui->plotter->plotLayout()->simplify();
       ui->plotter->plotLayout()->addElement(chartCount++,0,c->rectAxis());
       ui->plotter->rescaleAxes();
       ui->plotter->replot();
    }
}
/*
void ChartAnalyzer::removeChart(QMouseEvent*evt){
    if(evt->button() == Qt::MidButton){
        ui->plotter->graph()->
    }

}
*/

void ChartAnalyzer::contexMenu(QPoint pos){
    QPoint globalPos = ((QWidget*)(QObject::sender()))->mapToGlobal(pos);
    QMenu menu;
    QAction *save = new QAction("Salva come png", &menu);
    QAction *remove = new QAction("Rimuovi", &menu);

    menu.addAction(save);
    menu.addAction(remove);

    QAction* selectedItem = menu.exec(globalPos);
    if(selectedItem == save){
        QString url = QFileDialog::getSaveFileName(this, "Salva", "", "Immagine (*.png)");
        if(!url.isNull()){
            ui->plotter->savePng(url);
        }
    }else if(selectedItem == remove){
        if(chartCount){
            int h = ui->plotter->height()/chartCount;
            int selectedGraph = pos.y()/h;
            ui->plotter->plotLayout()->remove(ui->plotter->plotLayout()->element(selectedGraph, 0));
            ui->plotter->plotLayout()->simplify();
            ui->plotter->replot();
            chartCount--;
        }
    }
}


bool ChartAnalyzer::eventFilter(QObject *sender, QEvent *evt){
    /*
    if(sender == ui->plotter && evt->type() == QEvent::Resize){
        ui->plotter->rescaleAxes();

        float yUpper = ui->plotter->yAxis->range().upper;
        float yLower = ui->plotter->yAxis->range().lower;

        ui->plotter->yAxis->setRangeLower(yLower - ui->plotter->yAxis->range().size()/100);
        ui->plotter->yAxis->setRangeUpper(yUpper + ui->plotter->yAxis->range().size()/100);

        float height = size().height();
        float Vwidth = height / 1.5f;
        float Rwidth = size().width();
        float centerX = ui->plotter->xAxis->range().center();
        float deltaX = ui->plotter->xAxis->range().size();
        deltaX = deltaX * Rwidth / Vwidth;

        ui->plotter->xAxis->setRangeLower(centerX-deltaX/2);
        ui->plotter->xAxis->setRangeUpper(centerX+deltaX/2);
        ui->plotter->replot();
        return QWidget::eventFilter(sender,evt);
    }
    */
    return false;
}


ChartAnalyzer::~ChartAnalyzer(){
    qDebug()<<"CHART_ANALYZER: Deleting";
    delete ui;
}
