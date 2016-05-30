#include "dashbar.h"
#include "../dialog/dashsettingsdialog.h"
DashBar::DashBar(){
    index = -1;
    canID = -1;
    isSelected = false;
    setPos(10,10);
    setSize(75,200);

    orientation = Qt::Vertical;
    value = 0;
    value1 = 0;
    value2 = 100;
}

DashBar::DashBar(int index, int canID, QJsonObject json) : DashWidget(index, canID, json){
    this->orientation = static_cast<Qt::Orientation>(json["orientation"].toInt());
    this->value1 = json["min"].toInt();
    this->value2 = json["max"].toInt();
    this->value = qMin(value1,value2);
}

QWidget* DashBar::getSettingsWidget(int setting){
    QComboBox *combo;
    QSpinBox *spin;
    switch (setting) {
        case 6:
            combo = new QComboBox;
            combo->setObjectName("Orientamento");
            combo->addItem("Verticale");
            combo->addItem("Orizzontale");
            combo->setCurrentIndex(orientation == Qt::Vertical? 0 : 1);
            connect(combo, SIGNAL(activated(int)), this, SLOT(setOrientation(int)));
            return combo;
        case 7:
            spin = new QSpinBox;
            spin->setObjectName("Min");
            spin->setMinimum(INT_MIN);
            spin->setMaximum(INT_MAX);
            spin->setValue(value1);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setValue1(int)));
            return spin;
        case 8:
            spin = new QSpinBox;
            spin->setObjectName("Max");
            spin->setMinimum(INT_MIN);
            spin->setMaximum(INT_MAX);
            spin->setValue(value2);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setValue2(int)));
            return spin;
            /*
        case 9:
            spin = new QSpinBox;
            spin->setObjectName("Value");
            spin->setMinimum(INT_MIN);
            spin->setMaximum(INT_MAX);
            spin->setValue(value);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
            return spin;
            */
        default:
            return DashWidget::getSettingsWidget(setting);
    }
}

void DashBar::setOrientation(int o){
    this->orientation = o==0? Qt::Vertical : Qt::Horizontal;
    emit paintRequest();
}

void DashBar::setValue(int v){
    this->value = v;
    emit paintRequest();
}
void DashBar::setValue1(int v){
    this->value1 = v;
    emit paintRequest();
}
void DashBar::setValue2(int v){
    this->value2 = v;
    emit paintRequest();
}

void DashBar::repaint(QPainter &painter){
    float v;
    float min = qMin(value1,value2);
    float max = qMax(value1,value2);
    int size = orientation == Qt::Horizontal? width-10 : height-10;

    QRadialGradient gradient;
    QPen pen(Qt::black);
    pen.setWidth(1);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(x,y);

    gradient = QRadialGradient(QPointF(0,0), qSqrt(width*width+height*height)*2, QPointF(0,0));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawRect(0,0,width, height);

    gradient = QRadialGradient(QPointF(width,height), qSqrt(width*width+height*height)*2, QPointF(width,height));
    gradient.setColorAt(0, QColor(88,88,88));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawRect(5,5,width-10,height-10);

    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);

    if(value>max)value = max;
    if(value<min)value = min;
    v = (max-value)/(max-min)*size;

    for(int i=0; i<size; i+=4){
        if(i>size*0.4f){
            painter.setPen(i>=v || isSelected? Qt::green : QColor(Qt::green).darker());
        }else if(i>=size*0.1f && i<=size*0.4f){
            painter.setPen(i>=v || isSelected? Qt::yellow : QColor(Qt::yellow).darker());
        }else{
            painter.setPen(i>=v || isSelected? Qt::red : QColor(Qt::red).darker());
        }

        if(orientation == Qt::Horizontal){ //Orizzontale
            painter.drawLine(width-6-i,6,width-6-i,height-6); //Linee verticali
        }else{
            painter.drawLine(6,6+i,width-6,6+i);
        }

    }

    painter.restore();
}

void DashBar::updateValue(QVariant v){
    if(v.type() == QVariant::Double){
        this->value = v.toDouble();
        emit paintRequest();
    }
}

QString DashBar::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["width"] = width;
    obj["height"] = height;
    obj["orientation"] = orientation;
    obj["min"] = value1;
    obj["max"] = value2;
    return QString(QJsonDocument(obj).toJson());
}

int DashBar::type(){
    return BAR;
}

DashWidget* DashBar::getCopy(){
    return new DashBar(-1,canID,QJsonDocument::fromJson(toJson().toUtf8()).object());
}

DashBar::~DashBar(){

}

