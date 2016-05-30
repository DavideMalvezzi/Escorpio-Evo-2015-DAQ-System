#include "dashpainter.h"

DashPainter::DashPainter(QWidget *parent) : DashWidget(parent){
    widgets = new QList<DashWidget*>;
    selectedWidget = Q_NULLPTR;
    setMinimumWidth(1920);
    setMinimumHeight(1280);
    setAcceptDrops(true);
}

QWidget* DashPainter::getSettingsWidget(int setting){
    Q_UNUSED(setting)
    /*
    QSpinBox *spin;
    switch(setting){
        case 0:
            width = QWidget::width();
            spin = new QSpinBox;
            spin->setObjectName("Larghezza");
            spin->setMinimum(100);
            spin->setMaximum(1920);
            spin->setValue(width);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setMinimumWidth(int)));
            return spin;
        case 1:
            height = QWidget::height();
            spin = new QSpinBox;
            spin->setObjectName("Altezza");
            spin->setMinimum(100);
            spin->setMaximum(1280);
            spin->setValue(height);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setMinimumHeight(int)));
            return spin;
    }*/
    return Q_NULLPTR;
}

void DashPainter::setSelected(DashWidget *w){
    if(selectedWidget)selectedWidget->deselect();
    selectedWidget = w;
    if(selectedWidget)selectedWidget->select();
    emit changeSelected(selectedWidget);
    QWidget::repaint();
}

void DashPainter::setMinimumWidth(int w){
    QWidget::setMinimumWidth(w);
    width = QWidget::width();
}

void DashPainter::setMinimumHeight(int h){
    QWidget::setMinimumHeight(h);
    height = QWidget::height();
}

//Events
void DashPainter::mousePressEvent(QMouseEvent *event){
    int i=0;
    while(i<widgets->size() && !widgets->at(i)->geometry().contains(event->pos()))i++;

    if(i<widgets->size()){
        setSelected(widgets->at(i));
    }else{
        setSelected(this);
    }
    emit changeSelected(selectedWidget);

    //Drag
    if(event->button() == Qt::LeftButton){
        dragStartPos = event->pos();
    }
}

void DashPainter::mouseMoveEvent(QMouseEvent *event){
    if(!(event->buttons() & Qt::LeftButton))return;
    if((event->pos()-dragStartPos).manhattanLength()<QApplication::startDragDistance())
        return;
    if(selectedWidget && selectedWidget!=this){
        QDrag *drag = new QDrag(this);
        QMimeData *data = new QMimeData;
        QPixmap p = selectedWidget->toPixmap();
        data->setText("drag");
        drag->setMimeData(data);
        drag->setPixmap(p);
        drag->setHotSpot(QPoint(p.width()/2,p.height()/2));
        drag->exec(Qt::CopyAction | Qt::MoveAction);
        delete drag;
    }
}

void DashPainter::dragEnterEvent(QDragEnterEvent *event){
    if(event->mimeData()->hasFormat("text/plain")){
        event->acceptProposedAction();
    }
}

void DashPainter::dropEvent(QDropEvent *event){
    if(event->mimeData()->text() == "drag"){
        if(selectedWidget && selectedWidget!=this){

            if(event->dropAction() == Qt::MoveAction){
                selectedWidget->setPos(event->pos().x()-selectedWidget->size().width()/2,
                                       event->pos().y()-selectedWidget->size().height()/2);
                emit changeSelected(selectedWidget);

            }else if(event->dropAction() == Qt::CopyAction){
                DashWidget *copy = selectedWidget->getCopy();
                copy->setPos(event->pos().x()-selectedWidget->size().width()/2,
                             event->pos().y()-selectedWidget->size().height()/2);
                copy->setItem(new QTreeWidgetItem(*(selectedWidget->getItem())));
                copy->getItem()->setData(0,Qt::UserRole, QVariant(QMetaType::QObjectStar, &copy));
                selectedWidget->getItem()->treeWidget()->addTopLevelItem(copy->getItem());

                addWidget(copy);
                setSelected(copy);

            }
        }
    }
}

void DashPainter::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Delete){
        removeWidget(selectedWidget);
    }
}

void DashPainter::paintEvent(QPaintEvent *evt){
    Q_UNUSED(evt)

    QPainter painter(this);
    for(int i=0; i<widgets->size(); i++){
        widgets->at(i)->repaint(painter);
    }

}


void DashPainter::addWidget(DashWidget*w, bool conn){
    widgets->append(w);
    if(conn){
        connect(w, SIGNAL(paintRequest()), this, SLOT(repaint()));
    }
    QWidget::repaint();
}

void DashPainter::removeWidget(DashWidget* widget){
    if(widget && widget!=this){
        QTreeWidgetItem* item = widget->getItem();
        item->treeWidget()->setCurrentItem(Q_NULLPTR);
        item->treeWidget()->removeItemWidget(item, 0);
        delete item;

        emit widgetDeleted(widget->getIndex());
        widgets->removeOne(widget);
        selectedWidget = Q_NULLPTR;
        emit changeSelected(selectedWidget);
        QWidget::repaint();
    }
}

QList<DashWidget*>* DashPainter::getWidgetsList(){
    return widgets;
}

DashPainter::~DashPainter(){
    selectedWidget = Q_NULLPTR;
    delete widgets;
}

