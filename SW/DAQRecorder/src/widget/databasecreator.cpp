#include "databasecreator.h"
#include "../mainwindow.h"

DatabaseCreator::DatabaseCreator(QString url){
    this->url = url;
    progress = new QProgressBar;
    progress->setRange(0,100);
    progress->show();

    connect(this, SIGNAL(updateProgress(int)), progress, SLOT(setValue(int)));

}

void DatabaseCreator::run(){
    QFile dbSql(":/db/EscorpioDB.sql");
    dbSql.open(QFile::ReadOnly);
    QString temp = dbSql.readAll();
    QStringList queries = temp.split(";");

    MainWindow::mainWindow->setEnabled(false);
    for(int i=0; i<queries.size(); i++){
        MainWindow::database.exec(queries.at(i));
        if(MainWindow::database.lastError().isValid()){
            qDebug()<<"DATABASE_CREATOR: Error "<<MainWindow::database.lastError().text();
        }
        emit updateProgress(qCeil(i*100/queries.size()));
    }
    MainWindow::mainWindow->setEnabled(true);
    delete this;
}

DatabaseCreator::~DatabaseCreator(){
    delete progress;
}

