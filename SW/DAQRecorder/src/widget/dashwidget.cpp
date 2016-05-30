#include "dashwidget.h"
#include "../mainwindow.h"

DashWidget::DashWidget(QWidget *parent) : QWidget(parent){
    index = -1;
    canID = -1;
    isSelected = false;
    setPos(10,10);
    setSize(100,100);
}

DashWidget::DashWidget(int index, int canID, QJsonObject json){
    this->index = index;
    this->canID = canID;
    this->isSelected = false;

    this->x = json["x"].toInt();
    this->y = json["y"].toInt();
    this->width = json["width"].toInt();
    this->height = json["height"].toInt();
}

void DashWidget::repaint(QPainter &painter){
    painter.save();
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::red);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRect(geometry());

    if(isSelected){
         painter.setPen(Qt::white);
         painter.drawRect(geometry());
    }
    painter.restore();
}

QPixmap DashWidget::toPixmap(){
    QPixmap p(geometry().size()+QSize(1,1));
    p.fill(QColor(0,0,0,0));
    QPainter painter(&p);
    painter.translate(-x,-y);
    painter.setOpacity(0.45);
    repaint(painter);
    return p;
}

QWidget* DashWidget::getSettingsWidget(int setting){
    int i=0;
    QLineEdit *line;
    QSpinBox *spin;
    QComboBox *combo;
    QSqlQueryModel *sqlModel;
    switch (setting) {
        case 0:
            line = new QLineEdit;
            line->setObjectName("Nome");
            line->setText(item->text(0));
            line->setMaxLength(14);
            connect(line, SIGNAL(textChanged(QString)), this, SLOT(setName(QString)));
            return line;
        case 1:
            combo = new QComboBox;
            sqlModel = new QSqlQueryModel;
            sqlModel->setQuery("SELECT CanID,Nome FROM Canale", MainWindow::database);
            combo->setModel(sqlModel);
            combo->setModelColumn(1);
            combo->setObjectName("Can ID");
            if(canID){
                while(i<sqlModel->rowCount() && sqlModel->data(sqlModel->index(i,0)).toInt()!=canID)i++;
                if(i<sqlModel->rowCount()){
                    combo->setCurrentIndex(i);
                }else{
                    canID = -1;
                    combo->setCurrentIndex(-1);
                }
            }else{
                combo->setCurrentIndex(-1);
            }
            connect(combo, SIGNAL(activated(int)), this, SLOT(setCanID(int)));
            return combo;
        case 2:
            spin = new QSpinBox;
            spin->setObjectName("X");
            spin->setMinimum(0);
            spin->setMaximum(1920);
            spin->setValue(x);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setX(int)));
            return spin;
        case 3:
            spin = new QSpinBox;
            spin->setObjectName("Y");
            spin->setMinimum(0);
            spin->setMaximum(1920);
            spin->setValue(y);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setY(int)));
            return spin;
        case 4:
            spin = new QSpinBox;
            spin->setObjectName("Larghezza");
            spin->setMinimum(16);
            spin->setMaximum(1920);
            spin->setValue(width);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setWidth(int)));
            return spin;
        case 5:
            spin = new QSpinBox;
            spin->setObjectName("Altezza");
            spin->setMinimum(16);
            spin->setMaximum(1920);
            spin->setValue(height);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setHeight(int)));
            return spin;

    }
    return Q_NULLPTR;
}

DashWidget* DashWidget::getCopy(){
    return new DashWidget(-1, canID, QJsonDocument::fromJson(toJson().toUtf8()).object());
}

void DashWidget::updateValue(QVariant value){

}

QString DashWidget::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["width"] = width;
    obj["height"] = height;
    return QString(QJsonDocument(obj).toJson());
}

void DashWidget::setName(QString name){
    this->item->setText(0,name);
}
QString DashWidget::getName(){
    return item->text(0);
}
void DashWidget::select(){
    isSelected = true;
}
void DashWidget::deselect(){
    isSelected = false;
}
void DashWidget::setItem(QTreeWidgetItem* item){
    this->item = item;
}
QTreeWidgetItem* DashWidget::getItem(){
    return item;
}
void DashWidget::setCanID(int value){
    QComboBox *sender = (QComboBox*)QObject::sender();
    this->canID = sender->model()->data(sender->model()->index(value,0)).toInt();
}
void DashWidget::setX(int x){
    this->x = x;
    emit paintRequest();
}
void DashWidget::setY(int y){
    this->y = y;
    emit paintRequest();
}
void DashWidget::setWidth(int w){
    this->width = w;
    emit paintRequest();
}
void DashWidget::setHeight(int h){
    this->height = h;
    emit paintRequest();
}
void DashWidget::setSize(int w, int h){
    this->width = w;
    this->height = h;
}
void DashWidget::setPos(int x, int y){
    this->x = x;
    this->y = y;
    emit paintRequest();
}
void DashWidget::setIndex(int index){
    this->index = index;
}
int DashWidget::getIndex(){
    return index;
}
int DashWidget::getCanID(){
    return canID;
}
int DashWidget::type(){
    return LED;
}
QSize DashWidget::size(){
    return QSize(width,height);
}
QPoint DashWidget::pos(){
    return QPoint(x,y);
}
QRect DashWidget::geometry(){
    return QRect(x,y,width,height);
}
QSize DashWidget::sizeHint(){
    return size();
}

DashWidget::~DashWidget(){
    //if(item)delete item;
}

