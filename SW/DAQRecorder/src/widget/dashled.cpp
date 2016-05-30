#include "dashled.h"
#include "../dialog/dashsettingsdialog.h"

DashLed::DashLed(){
    index = -1;
    canID = -1;
    isSelected = false;
    setPos(0,0);
    setSize(32,32);

    condition = Cond_EQ;
    onColor1 =  QColor(0,255,0);
    onColor2 =  QColor(0,192,0);
    offColor1 = QColor(0,28,0);
    offColor2 = QColor(0,128,0);
}

DashLed::DashLed(int index, int canID, QJsonObject json) : DashWidget(index, canID, json){
    this->onColor1 = QColor(json["color"].toString());
    this->onColor2 = onColor1.darker();
    this->offColor1 = onColor2.darker();
    this->offColor2 = offColor2.darker();
    this->condition = static_cast<OnCondition>(json["condition"].toInt());
    this->condValue = json["value"].toString();
}

void DashLed::repaint(QPainter &painter){
    QRadialGradient gradient;
    QPen pen(Qt::black);
    pen.setWidth(1);

    painter.save();

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width/2, height/2);

    gradient = QRadialGradient(QPointF(x-width/2,y-height/2), width*3, QPointF(x-width/2,y-height/2));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPointF(x,y), width*0.5f, height*0.5f);

    gradient = QRadialGradient(QPointF(x+width/2,y+height/2), width*3, QPointF(x+width/2,y+height/2));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPointF(x,y), width*0.45f, height*0.45f);

    painter.setPen(pen);
    if(isSelected) {
      gradient = QRadialGradient(QPointF(x-width/2,y-height/2), width*3, QPointF(x-width/2,y-height/2));
      gradient.setColorAt(0, onColor1);
      gradient.setColorAt(1, onColor2);
    } else {
      gradient = QRadialGradient(QPointF(x+width/2,y+height/2), width*3, QPointF(x+width/2,y+height/2));
      gradient.setColorAt(0, offColor1);
      gradient.setColorAt(1, offColor2);
    }
    painter.setBrush(gradient);
    painter.drawEllipse(QPointF(x,y), width*0.4f, height*0.4f);

    painter.restore();
}

int DashLed::type(){
    return LED;
}

QWidget* DashLed::getSettingsWidget(int setting){
    QToolButton *btn;
    QComboBox *combo;
    QLineEdit *line;
    switch (setting) {
        case 6:
            btn = new QToolButton;
            btn->setText("...");
            btn->setObjectName("Colore");
            connect(btn, SIGNAL(clicked()), this, SLOT(setColor()));
            return btn;
        case 7:
            combo = new QComboBox;
            combo->setObjectName("Condizione");
            combo->addItem(">=");
            combo->addItem("<=");
            combo->addItem("==");
            combo->addItem("!=");
            combo->addItem("BIT");
            combo->setCurrentIndex(condition);
            connect(combo, SIGNAL(activated(int)), this, SLOT(setCondition(int)));
            return combo;
        case 8:
            line = new QLineEdit;
            line->setObjectName("Valore");
            line->setText(condValue);
            //line->setValidator(new QDoubleValidator);
            connect(line, SIGNAL(textChanged(QString)), this, SLOT(setValue(QString)));
            return line;
        default:
            return DashWidget::getSettingsWidget(setting);
    }
}

void DashLed::setColor(){
    QColorDialog dialog;
    if(dialog.exec()){
        onColor1 = dialog.selectedColor();
        onColor2 = onColor1.darker();
        offColor1 = onColor2.darker();
        offColor2 = offColor2.darker();
        emit paintRequest();
    }
}

void DashLed::setCondition(int index){
    this->condition = static_cast<OnCondition>(index);
}

void DashLed::setValue(QString v){
    this->condValue = v;
}

void DashLed::updateValue(QVariant value){
    bool ok;
    if(value.type() == QVariant::Double){
        double cv = condValue.toDouble(&ok);
        if(ok){
            if(condition == Cond_EQ){
                isSelected = value.toDouble() == cv;
                emit paintRequest();
            }else if(condition == Cond_GE){
                isSelected = value.toDouble() >= cv;
                emit paintRequest();
            }else if(condition == Cond_LE){
                isSelected = value.toDouble() <= cv;
                emit paintRequest();
            }else if(condition == Cond_NE){
                isSelected = value.toDouble() != cv;
                emit paintRequest();
            }
        }
    }else if(value.type() == QVariant::BitArray){
        int cv = condValue.toInt(&ok);
        QBitArray v = value.toBitArray();
        if(ok && condition == Cond_BITON && cv>=0 && cv<v.size()){
            isSelected = v.at(cv);
            emit paintRequest();
        }
    }else if(value.type() == QVariant::String){
        QString cv = condValue;
        if(condition == Cond_EQ){
            isSelected = cv == value.toString();
        }else if(condition == Cond_NE){
            isSelected = cv != value.toString();
        }
    }

}

DashWidget* DashLed::getCopy(){
    return new DashLed(-1, canID, QJsonDocument::fromJson(toJson().toUtf8()).object());
}

QString DashLed::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["width"] = width;
    obj["height"] = height;
    obj["color"] = onColor1.name();
    obj["condition"] = condition;
    obj["value"] = condValue;
    return QString(QJsonDocument(obj).toJson());
}

DashLed::~DashLed(){

}

