#include "dashtermo.h"
#include "../dialog/dashsettingsdialog.h"

DashTermo::DashTermo(){
    index = -1;
    canID = -1;
    isSelected = false;
    setPos(10,10);
    setSize(25,100);

    orientation = Qt::Vertical;
    value = 0;
    value1 = 0;
    value2 = 100;
    tick = 15;
    value = 25;
    color = Qt::red;
}

DashTermo::DashTermo(int index, int canID, QJsonObject json) : DashWidget(index, canID, json){
    this->orientation = static_cast<Qt::Orientation>(json["orientation"].toInt());
    this->value1 = json["min"].toInt();
    this->value2 = json["max"].toInt();
    this->value = qMin(value1,value2);
    this->tick = json["tick"].toInt();
    this->value = (value1+value2)/2;
    this->color = QColor(json["color"].toString());
}


QWidget* DashTermo::getSettingsWidget(int setting){
    QComboBox *combo;
    QSpinBox *spin;
    QToolButton *btn;
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
            spin->setObjectName("Tacche");
            spin->setMinimum(3);
            spin->setMaximum(99);
            spin->setSingleStep(2);
            spin->setValue(tick);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setTick(int)));
            return spin;
        case 8:
            spin = new QSpinBox;
            spin->setObjectName("Min");
            spin->setMinimum(INT_MIN);
            spin->setMaximum(INT_MAX);
            spin->setValue(value1);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setValue1(int)));
            return spin;
        case 9:
            spin = new QSpinBox;
            spin->setObjectName("Max");
            spin->setMinimum(INT_MIN);
            spin->setMaximum(INT_MAX);
            spin->setValue(value2);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setValue2(int)));
            return spin;
        case 10:
            btn = new QToolButton;
            btn->setObjectName("Colore");
            btn->setText("...");
            connect(btn, SIGNAL(clicked()), this, SLOT(setColor()));
            return btn;
            /*
        case 11:
            spin = new QSpinBox;
            spin->setObjectName("Value");
            spin->setMinimum(INT_MIN);
            spin->setMaximum(INT_MAX);
            spin->setValue(value);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
            return spin;*/
        default:
            return DashWidget::getSettingsWidget(setting);
    }
}

void DashTermo::repaint(QPainter &painter){
    float off;
    float min = qMin(value1,value2);
    float max = qMax(value1,value2);
    if(value>max)value = max;
    if(value<min)value = min;
    int size = orientation == Qt::Horizontal? width-10 : height-10;
    float v = (qAbs(min)+value)/(max-min)*size;

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

    painter.setPen(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if(orientation == Qt::Vertical){
        for(int i=0; i<tick; i++){
            off = (float)size/(tick-1)*i+5;
            if(i%2==0){
                painter.drawLine(
                            width+3,off,
                            width+8,off
                            );
            }else{
                painter.drawLine(
                            width+3,off,
                            width+6,off
                            );
            }
            painter.drawText(QRectF(
                        width+13,off-25,
                        50,50),
                        Qt::AlignVCenter,
                        QString::number(qRound((max-min)/(float)(tick-1)*(tick-1-i)+min))
                        );
        }
        painter.drawLine(width+3, 5, width+3, size+5);

    }else{
        for(int i=0; i<tick; i++){
            off = (float)size/(tick-1)*i+5;
            if(i%2==0){
                painter.drawLine(
                                off,height+3,
                                 off,height+8
                                 );
            }else{
                painter.drawLine(
                                off,height+3,
                                 off,height+6
                                 );
            }
            painter.drawText(QRectF(
                        off-25,height+13,
                        50,50),
                        Qt::AlignHCenter,
                        QString::number(qRound((max-min)/(float)(tick-1)*i+min))
                        );
        }
        painter.drawLine(5, height+3, size+5, height+3);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(isSelected ? color : color.darker());
    painter.setBrush(QBrush(isSelected ? color : color.darker()));

    if(orientation == Qt::Vertical){
        painter.drawRect(6,height-5-v,width-12,v);
    }else{
        painter.drawRect(5,6,v,height-12);
    }
    painter.restore();
}

void DashTermo::setOrientation(int o){
    this->orientation = o==0? Qt::Vertical : Qt::Horizontal;
    emit paintRequest();
}

void DashTermo::setValue(int v){
    this->value = v;
    emit paintRequest();
}
void DashTermo::setValue1(int v){
    this->value1 = v;
    emit paintRequest();
}
void DashTermo::setValue2(int v){
    this->value2 = v;
    emit paintRequest();
}

void DashTermo::setColor(){
    QColorDialog dialog;
    if(dialog.exec()){
        this->color = dialog.selectedColor();
        emit paintRequest();
    }
}

void DashTermo::setTick(int t){
    this->tick = t;
    emit paintRequest();
}

void DashTermo::updateValue(QVariant v){
    if(v.type() == QVariant::Double){
        this->value = v.toDouble();
        emit paintRequest();
    }
}

QString DashTermo::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["width"] = width;
    obj["height"] = height;
    obj["orientation"] = orientation;
    obj["min"] = value1;
    obj["max"] = value2;
    obj["tick"] = tick;
    obj["color"] = color.name();
    return QString(QJsonDocument(obj).toJson());
}

int DashTermo::type(){
    return THERMOMETER;
}

DashWidget* DashTermo::getCopy(){
    return new DashTermo(-1, canID, QJsonDocument::fromJson(toJson().toUtf8()).object());
}

DashTermo::~DashTermo(){

}

