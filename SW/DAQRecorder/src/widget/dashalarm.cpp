#include "dashalarm.h"
#include "../dialog/dashsettingsdialog.h"

QImage* DashAlarm::icon = Q_NULLPTR;

DashAlarm::DashAlarm(){
    if(!icon){
        icon = new QImage(":/images/icons/alarm.png");
    }
    index = -1;
    canID = -1;
    isSelected = false;
    isActived = false;
    setPos(10,10);
    setSize(icon->width(),icon->height());

    this->alarmText = "Allarme";
    this->condition = Cond_EQ;
    this->player = Q_NULLPTR;
}

DashAlarm::DashAlarm(int index, int canID, QJsonObject json) : DashWidget(index, canID, json){
    if(!icon){
        icon = new QImage(":/images/icons/alarm.png");
    }
    setSize(icon->width(),icon->height());
    this->alarmText = json["text"].toString();
    this->alarmSound = json["sound"].toString();
    this->condition = static_cast<OnCondition>(json["cond"].toInt());
    this->alarmCondition = json["value"].toString();

    this->isActived = false;
    QFile sound(alarmSound);
    if(sound.exists()){
        player = new QMediaPlayer;
        player->setMedia(QUrl(alarmSound));
        player->setVolume(100);
    }else{
        this->player = Q_NULLPTR;
    }
}

QWidget* DashAlarm::getSettingsWidget(int setting){
    QToolButton *btn;
    QComboBox *combo;
    QLineEdit *line;
    switch (setting) {
        case 2:
            btn = new QToolButton;
            btn->setText(alarmSound.isNull() | alarmSound.isEmpty() ? "Nessun suono" : "Suono selezionato");
            btn->setObjectName("Suono");
            connect(btn, SIGNAL(clicked()), this, SLOT(setAlarmSound()));
            return btn;
        case 3:
            line = new QLineEdit;
            line->setObjectName("Errore");
            line->setText(alarmText);
            connect(line, SIGNAL(textChanged(QString)), this, SLOT(setAlarmText(QString)));
            return line;
        case 4:
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
        case 5:
            line = new QLineEdit;
            line->setObjectName("Valore");
            line->setText(alarmCondition);
            //line->setValidator(new QDoubleValidator);
            connect(line, SIGNAL(textChanged(QString)), this, SLOT(setAlarmCondition(QString)));
            return line;

        default:
            return DashWidget::getSettingsWidget(setting);
    }
}

void DashAlarm::repaint(QPainter &painter){
    painter.save();

    if(!isSelected){
        painter.setOpacity(0.75f);
    }
    painter.drawImage(x,y,*icon);
    painter.restore();
}

void DashAlarm::updateValue(QVariant value){
    bool ok, prevState;
    prevState = isActived;

    if(value.type() == QVariant::Double){
        double cv = alarmCondition.toDouble(&ok);
        if(ok){
            if(condition == Cond_EQ){
                isActived = value.toDouble() == cv;
            }else if(condition == Cond_GE){
                isActived = value.toDouble() >= cv;
            }else if(condition == Cond_LE){
                isActived = value.toDouble() <= cv;
            }else if(condition == Cond_NE){
                isActived = value.toDouble() != cv;
            }
        }

    }else if(value.type() == QVariant::BitArray){
        int cv = alarmCondition.toInt(&ok);
        QBitArray v = value.toBitArray();
        if(ok && condition == Cond_BITON && cv>=0 && cv<v.size()){
            isActived = v.at(cv);
        }

    }else if(value.type() == QVariant::String){
        QString v = value.toString();
        if(condition == Cond_EQ){
            //qDebug()<<v.toUtf8().toHex()<<"   "<<alarmCondition.toUtf8().toHex();
            isActived = v.contains(alarmCondition);
        }else if(condition == Cond_NE){
            isActived = (v != alarmCondition);
        }
    }

    if(prevState == false && isActived == true){
        if(player && player->state() != QMediaPlayer::PlayingState){
            player->play();
        }
        QMessageBox::critical(this, "ALLARME", alarmText);
    }

}

QString DashAlarm::toJson(){
    QJsonObject obj;
    obj["name"] = item->text(0);
    obj["x"] = x;
    obj["y"] = y;
    obj["text"] = alarmText;
    obj["sound"] = alarmSound;
    obj["cond"] = condition;
    obj["value"] = alarmCondition;
    return QString(QJsonDocument(obj).toJson());
}

int DashAlarm::type(){
    return ALARM;
}

DashWidget* DashAlarm::getCopy(){
    return new DashAlarm(-1, canID, QJsonDocument::fromJson(toJson().toUtf8()).object());
}

void DashAlarm::setCondition(int i){
    this->condition = static_cast<OnCondition>(i);
}
void DashAlarm::setAlarmText(QString s){
    this->alarmText = s;
}
void DashAlarm::setAlarmSound(){
    QToolButton *btn = (QToolButton*)QObject::sender();
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    d.setNameFilter("MP3 Files (*.mp3)");
    d.setDirectory("");
    if(d.exec() == 1){
        this->alarmSound = d.selectedFiles().first();
    }
    btn->setText(alarmSound.isNull() | alarmSound.isEmpty() ? "Nessun suono" : "Suono selezionato");
}
void DashAlarm::setAlarmCondition(QString s){
    this->alarmCondition = s;
}

DashAlarm::~DashAlarm(){
    if(player){
        player->stop();
        delete player;
    }
}

