#include "dashsegment.h"
#include "../dialog/dashsettingsdialog.h"
#include "../recorder/recordingsession.h"

DashSegment::DashSegment(){
    index = -1;
    canID = -1;
    isSelected = false;
    setPos(0,0);

    mode = DecimalMode;
    digit = 2;
    decimalDigit = 0;
    font = QFont(QFontDatabase::applicationFontFamilies(0).at(0));
    font.setPointSize(32);
    color = QColor(0,162,255,255);
    reloadValue();
}

DashSegment::DashSegment(int index, int canID, QJsonObject json){
    this->index = index;
    this->canID = canID;
    this->isSelected = false;

    this->font = QFont(QFontDatabase::applicationFontFamilies(0).at(0));

    this->x = json["x"].toInt();
    this->y = json["y"].toInt();
    this->mode = static_cast<DigitType>(json["mode"].toInt());
    this->digit = json["digit"].toInt();
    this->decimalDigit = json["decimal"].toInt();
    this->font.setPointSize(json["size"].toInt());
    this->color = QColor(json["color"].toString());
    reloadValue();
}

void DashSegment::repaint(QPainter &painter){
    int traslX = 0;
    QFontMetrics fm(font);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(font);

    if(isSelected){
         painter.setPen(color);
    }else{
         painter.setPen(color.darker());
    }

    for(int i=0; i<text.size(); i++){
        painter.drawText(x+traslX, y+font.pointSize(), text.at(i));
        if(text.at(i)==':' || text.at(i)=='.'){
            traslX += fm.width(text.at(i));
        }else{
            traslX += fm.width("0");
        }
    }


    painter.restore();
}

QWidget* DashSegment::getSettingsWidget(int setting){
    QSpinBox *spin;
    QToolButton *btn;
    QComboBox *combo;
    //QLineEdit *line;
    switch(setting){
        case 4:
            combo = new QComboBox;
            combo->setObjectName("Tipo");
            combo->addItem("Numero");
            combo->addItem("Tempo");
            combo->setCurrentIndex(mode);
            connect(combo, SIGNAL(activated(int)), this, SLOT(setMode(int)));
            return combo;
        case 5:
            spin = new QSpinBox;
            spin->setObjectName("Cifre");
            spin->setValue(digit);
            spin->setMinimum(1);
            spin->setMaximum(10);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setDigitCount(int)));
            return spin;
        case 6:
            spin = new QSpinBox;
            spin->setObjectName("Decimali");
            spin->setValue(decimalDigit);
            spin->setMinimum(0);
            spin->setMaximum(3);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setDecimalCount(int)));
            return spin;
        case 7:
            spin = new QSpinBox;
            spin->setObjectName("Dimensione");
            spin->setValue(font.pointSize());
            spin->setMinimum(3);
            spin->setMaximum(100);
            connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setDigitSize(int)));
            return spin;
        case 8:
            btn = new QToolButton;
            btn->setText("...");
            btn->setObjectName("Colore");
            connect(btn, SIGNAL(clicked()), this, SLOT(setColor()));
            return btn;
        /*
        case 9:
            line = new QLineEdit;
            connect(line, SIGNAL(textChanged(QString)), this, SLOT(setText(QString)));
            return line;
            */
        default:
            return DashWidget::getSettingsWidget(setting);
    }
}

void DashSegment::updateValue(QVariant value){
    double v;
    QString text,intPart,decPart;
    if(value.type() == QVariant::Double){
        v = value.toDouble();
        intPart = QString::number((int)v);
        decPart = QString::number((double)(v-(int)v));
        if(mode == DecimalMode){
            if(decPart.count()>0){
                if(decPart.at(0)=='-'){
                    decPart.remove(0,1);
                    if(intPart.at(0)!='-'){
                        intPart.insert(0,'-');
                    }
                }
                decPart.remove(0,2);
            }

            while(intPart.count()<digit){
                intPart.insert(0, ' ');
            }
            while(intPart.count()>digit){
                intPart.remove(0, 1);
            }

            text.append(intPart);

            if(decimalDigit>0){
                text.append(".");
                for(int i=0; i<decimalDigit; i++){
                    if(i<decPart.count()){
                        text.append(decPart.at(i));
                    }else{
                        text.append('0');
                    }
                }
            }
            setText(text);

        }else{
            QTime t(0,0,0);
            t = t.addMSecs(v);
            text = t.toString("hh:mm:ss.zzz");

            if(decimalDigit==0)text.chop(1);
            text.chop(3-decimalDigit);
            if(digit<3)text.remove(0,3);
            if(digit<2)text.remove(0,3);
            setText(text);
        }
    }
}

int DashSegment::type(){
    return DIGIT;
}

void DashSegment::setText(QString text){
    this->text = text;
    emit paintRequest();
}

DashWidget* DashSegment::getCopy(){
    return new DashSegment(-1, canID, QJsonDocument::fromJson(toJson().toUtf8()).object());
}

QString DashSegment::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["mode"] = mode;
    obj["digit"] = digit;
    obj["decimal"] = decimalDigit;
    obj["size"] = font.pointSize();
    obj["color"] = color.name();
    return QString(QJsonDocument(obj).toJson());
}

void DashSegment::setMode(int mode){
    this->mode = static_cast<DigitType>(mode);
    reloadValue();
    emit paintRequest();
}

void DashSegment::setDigitSize(int size){
    font.setPointSize(size);
    recalculateSize();
    emit paintRequest();
}

void DashSegment::setDigitCount(int count){
    this->digit = count;
    reloadValue();
    emit paintRequest();
}

void DashSegment::setDecimalCount(int count){
    this->decimalDigit = count;
    reloadValue();
    emit paintRequest();
}

void DashSegment::setColor(){
    QColorDialog dialog;
    if(dialog.exec()){
        color = dialog.selectedColor();
        emit paintRequest();
    }
}

void DashSegment::recalculateSize(){
    QFontMetrics fm(font);
    QRect bound = fm.boundingRect(text);
    width = bound.width();
    height = bound.height();
}

void DashSegment::reloadValue(){
    int i;
    text = "";
    if(mode == DecimalMode){
        for(i=0; i<digit; i++){
            text.append("0");
        }
        if(decimalDigit){
            text.append(".");
            for(int i=0; i<decimalDigit; i++){
                text.append("0");
            }
        }
    }else{
        for(i=0; i<qMin(digit,3); i++){
            text.append("00:");
        }
        if(decimalDigit){
            text.chop(1);
            text.append(".");
            for(i=0; i<decimalDigit; i++){
                text.append("0");
            }
        }else{
            text.remove(text.size()-1,1);
        }
    }
    recalculateSize();
}



DashSegment::~DashSegment(){

}

