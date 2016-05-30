#include "dashlabel.h"
#include "../dialog/dashsettingsdialog.h"

DashLabel::DashLabel(){
    index = -1;
    canID = -2;
    isSelected = false;
    text = "Label";
    recalculateSize();
    setPos(0,0);
}

DashLabel::DashLabel(int index, int canID, QJsonObject json){
    this->index = index;
    this->canID = canID;
    this->isSelected = false;

    this->x = json["x"].toInt();
    this->y = json["y"].toInt();
    this->text = json["text"].toString();
    this->font.fromString(json["font"].toString());
    recalculateSize();
}

QWidget* DashLabel::getSettingsWidget(int setting){
    QToolButton* btn;
    QLineEdit *line;
    QSpinBox *spin;
    switch(setting){
        case 0:
            line = new QLineEdit;
            line->setObjectName("Nome");
            line->setText(item->text(0));
            line->setMaxLength(14);
            connect(line, SIGNAL(textChanged(QString)), this, SLOT(setName(QString)));
            return line;
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
            line = new QLineEdit;
            line->setObjectName("Testo");
            line->setText(text);
            connect(line, SIGNAL(textChanged(QString)), this, SLOT(setText(QString)));
            return line;
        case 5:
            btn = new QToolButton;
            btn->setObjectName("Font");
            btn->setText("...");
            connect(btn, SIGNAL(clicked()), this, SLOT(setFont()));
            return btn;
        default:
            return DashWidget::getSettingsWidget(setting);
    }
}

void DashLabel::repaint(QPainter &painter){
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(font);

    if(isSelected){
        painter.setPen(Qt::white);
    }else{
        painter.setPen(Qt::darkGray);
    }
    painter.drawText(x,y+height,text);
    painter.restore();
}

DashWidget* DashLabel::getCopy(){
    return new DashLabel(-1, canID, QJsonDocument::fromJson(toJson().toUtf8()).object());
}

void DashLabel::updateValue(QVariant value){
    if(value.type() == QVariant::Double || value.type() == QVariant::String){
        setText(value.toString());
    }else if(value.type() == QVariant::BitArray){
        QString text;
        QBitArray bit = value.toBitArray();
        for(int i=0; i<bit.count(); i++){
            text.append(bit[i]?'1':'0');
            setText(text);
        }
    }

}

QString DashLabel::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["text"] = text;
    obj["font"] = font.toString();
    return QString(QJsonDocument(obj).toJson());
}

int DashLabel::type(){
    return LABEL;
}

void DashLabel::setText(QString text){
    this->text = text;
    recalculateSize();
    emit paintRequest();
}

void DashLabel::setFont(){
    bool ok;
    QFont selectFont = QFontDialog::getFont(&ok, font, this);
    if(ok){
        this->font = selectFont;
        recalculateSize();
        emit paintRequest();
    }
}

void DashLabel::recalculateSize(){
    QFontMetrics fm(font);
    QRect bound = fm.boundingRect(text);
    width = bound.width();
    height = bound.height();
}

DashLabel::~DashLabel(){

}

