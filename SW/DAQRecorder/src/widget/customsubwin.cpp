#include "customsubwin.h"
#include <QLayout>
#include <QDebug>
#include <QIcon>

CustomSubWin::CustomSubWin(QWidget *parent) : QMdiSubWindow(parent){
    setStyleSheet("background-color:#424242;");
    setWindowIcon(QIcon());
}

void CustomSubWin::closeEvent(QCloseEvent *event){
    event->ignore();
    hide();
    emit onClose(false);
}


