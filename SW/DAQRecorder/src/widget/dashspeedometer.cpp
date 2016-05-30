#include "dashspeedometer.h"
#include "../dialog/dashsettingsdialog.h"

DashSpeedometer::DashSpeedometer(){
    index = -1;
    canID = -1;
    isSelected = false;

    tick = 25;
    fontSize = 10;
    startAngle = -45;
    arcSize = 270;
    value = qDegreesToRadians(startAngle+arcSize+0.5f);
    value1 = 0;
    value2 = 100;
    setPos(10,10);
    setSize(100,100);
}

DashSpeedometer::DashSpeedometer(int index, int canID, QJsonObject json){
    this->index = index;
    this->canID = canID;
    this->isSelected = false;

    this->x = json["x"].toInt();
    this->y = json["y"].toInt();
    this->width = this->height = json["width"].toInt();
    this->tick = json["tick"].toInt();
    this->fontSize = json["font"].toInt();
    this->startAngle = json["angle"].toInt();
    this->arcSize = json["len"].toInt();;
    this->value = qDegreesToRadians(startAngle+arcSize+0.5f);
    this->value1 = json["min"].toInt();
    this->value2 = json["max"].toInt();
}

QWidget* DashSpeedometer::getSettingsWidget(int setting){
    QSpinBox *spin;
    switch (setting) {
        case 4:
            spin = new QSpinBox;
            spin->setObjectName("Raggio");
            spin->setMinimum(16);
            spin->setMaximum(1920);
            spin->setValue(width);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setWidth(int)));
            return spin;
        case 5:
            spin = new QSpinBox;
            spin->setObjectName("Tacche");
            spin->setMinimum(7);
            spin->setMaximum(35);
            spin->setValue(tick);
            spin->setSingleStep(2);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setTick(int)));
            return spin;
        case 6:
            spin = new QSpinBox;
            spin->setObjectName("Angolo");
            spin->setMinimum(-45);
            spin->setMaximum(135);
            spin->setValue(startAngle);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setStartAngle(int)));
            return spin;
        case 7:
            spin = new QSpinBox;
            spin->setObjectName("Ampiezza");
            spin->setMinimum(90);
            spin->setMaximum(270);
            spin->setValue(arcSize);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setArcLenght(int)));
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
            spin = new QSpinBox;
            spin->setObjectName("Dimensione font");
            spin->setMinimum(5);
            spin->setMaximum(100);
            spin->setValue(fontSize);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setFontSize(int)));
            return spin;

            /*
        case 10:
            spin = new QSpinBox;
            spin->setObjectName("Max");
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

QString DashSpeedometer::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["width"] = width;
    obj["tick"] = tick;
    obj["font"] = fontSize;
    obj["angle"] = startAngle;
    obj["len"] = arcSize;
    obj["min"] = value1;
    obj["max"] = value2;
    return QString(QJsonDocument(obj).toJson());
}

DashWidget* DashSpeedometer::getCopy(){
    return new DashSpeedometer(-1, canID, QJsonDocument::fromJson(toJson().toUtf8()).object());
}

void DashSpeedometer::updateValue(QVariant value){
    float v;
    float min = qMin(value1,value2);
    float max = qMax(value1,value2);

    if(value.type() == QVariant::Double){
        v = value.toDouble();
        if(v<min)v = min;
        if(v>max)v = max;

        v = (max-v)/(max-min)*(float)arcSize+startAngle;
        setValue(v);
    }
}

int DashSpeedometer::type(){
    return ANGOLMETER;
}

void DashSpeedometer::setWidth(int w){
    this->width = this->height = w;
    emit paintRequest();
}

void DashSpeedometer::setFontSize(int t){
    this->fontSize = t;
    emit paintRequest();
}

void DashSpeedometer::setTick(int t){
    this->tick = t;
    emit paintRequest();
}

void DashSpeedometer::setStartAngle(int angle){
    this->startAngle = angle;
    this->value = qDegreesToRadians(startAngle+arcSize+0.5f);
    emit paintRequest();
}

void DashSpeedometer::setArcLenght(int len){
    this->arcSize = len;
    this->value = qDegreesToRadians(startAngle+arcSize+0.5f);
    emit paintRequest();
}

void DashSpeedometer::setValue(float v){
    this->value = qDegreesToRadians((float)v);
    emit paintRequest();
}

void DashSpeedometer::setValue1(int v){
    this->value1 = v;
    emit paintRequest();
}

void DashSpeedometer::setValue2(int v){
    this->value2 = v;
    emit paintRequest();
}

void DashSpeedometer::repaint(QPainter &painter){
    int min = qMin(value1,value2);
    int max = qMax(value1,value2);
    float angle = qDegreesToRadians((float)startAngle);
    QRadialGradient gradient;
    QPen pen(Qt::black);
    pen.setWidth(1);

    //Draw bound
    painter.save();

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width/2, width/2);
    painter.setPen(Qt::white);

    gradient = QRadialGradient(QPointF(x-width/2,y-height/2), width*2, QPointF(x-width/2,y-height/2));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setBrush(QBrush(gradient));

    QPainterPath path,path2;
    if(arcSize>=180){
        path.moveTo(
                    x+width*0.5f*qCos(angle+qDegreesToRadians(arcSize+3.5f)),
                    y+width*0.5f*-qSin(angle+qDegreesToRadians(arcSize+3.5f))
                    );
        path.arcTo(
                    x-width/2,y-width/2,width,width,((float)startAngle-3.5f),(arcSize+7)
                    );
    }else{
        path.moveTo(x,y);
        path.arcTo(
                    x-width/2,y-width/2,width,width,((float)startAngle-7.5f),arcSize+15
                    );
    }
    painter.fillPath(path, painter.brush());


    gradient = QRadialGradient(QPointF(x+width/2,y+height/2), width*2, QPointF(x+width/2,y+height/2));
    gradient.setColorAt(0, QColor(88,88,88));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setBrush(QBrush(gradient));

    if(arcSize>=180){
        path2.moveTo(
                    x+width/2*0.95f*qCos(angle+qDegreesToRadians(arcSize+3.5f)),
                    y+width/2*0.95f*-qSin(angle+qDegreesToRadians(arcSize+3.5f))
                    );
        path2.arcTo(
                    x-width/2*0.95f,y-width/2*0.95f,width*0.95f,width*0.95f,((float)startAngle-3.5f),(arcSize+7)
                    );
    }else{
        path2.moveTo(x,y);
        path2.arcTo(
                    x-width/2*0.95f,y-width/2*0.95f,width*0.95f,width*0.95f,((float)startAngle-7.5f),arcSize+15
                    );
    }
    painter.fillPath(path2, painter.brush());

    //Draw ticks
    QFont font("Calibri");
    QString text;
    font.setPointSize(fontSize);

    pen.setWidth(4);
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.setFont(font);

    for(int i=0; i<tick; i++){
        if(i%2==0){
            text = QString::number(qRound((max-min)/(float)(tick-1)*(tick-1-i)+min));
            painter.drawLine(QLineF(
                    x+width*0.30f*qCos(angle),y+width*0.30f*-qSin(angle),
                    x+width*0.33f*qCos(angle),y+width*0.33f*-qSin(angle)
                    ));
            painter.drawText(QRect(
                        x+width*0.4f*qCos(angle)-75,
                        y+width*0.4*-qSin(angle)-75,
                        150,150),
                        Qt::AlignCenter,
                        text
                    );
        }else{
            painter.drawLine(QLineF(
                    x+width*0.30f*qCos(angle),y+width*0.30*-qSin(angle),
                    x+width*0.315f*qCos(angle),y+width*0.315f*-qSin(angle)
                    ));
        }

        angle += qDegreesToRadians((float)arcSize/(tick-1));
    }

    //Draw hand
    pen.setWidth(1);
    pen.setColor(Qt::red);
    painter.setBrush(QBrush(Qt::red));
    painter.setPen(pen);

    QPolygonF hand;
    hand<<QPointF(x+width*0.016f*qCos(value-M_PI/2),y+width*0.016f*-qSin(value-M_PI/2));
    hand<<QPointF(x+width*0.016f*qCos(value+M_PI/2),y+width*0.016f*-qSin(value+M_PI/2));
    hand<<QPointF(x+width*0.32f*qCos(value),y+width*0.32f*-qSin(value));
    painter.drawPolygon(hand);

    //Draw hand point
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.setBrush(QBrush(QColor(224,224,224)));
    painter.drawEllipse(QPointF(x,y), width*0.035f, width*0.035f);

    painter.restore();
}

DashSpeedometer::~DashSpeedometer(){

}

