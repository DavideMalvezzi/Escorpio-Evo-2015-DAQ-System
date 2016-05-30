#include "gsminterface.h"
#include "../mainwindow.h"

GSMInterface::GSMInterface(){
    port = new QSerialPort(this);
    isTextMode = false;
    responseCount = 0;
    connect(port, SIGNAL(readyRead()), this, SLOT(readFromSerial()));
    connect(&responseTimer, SIGNAL(timeout()), this, SLOT(request()));
}

bool GSMInterface::open(QString name){
   port->setPortName(name);
   port->setBaudRate(QSerialPort::Baud19200);
   port->setDataBits(QSerialPort::Data8);
   port->setStopBits(QSerialPort::OneStop);
   port->setParity(QSerialPort::NoParity);
   port->setFlowControl(QSerialPort::NoFlowControl);
   if(port->open(QSerialPort::ReadWrite)){
       loadSettings();
       setState(CHECK_MODULE_ON);
       return true;
   }
   return false;
}

void GSMInterface::close(){
    RXbuffer.clear();
    RXmsg.clear();
    TXmsg.clear();
    responseTimer.stop();
    if(port){
        port->close();
    }
}

void GSMInterface::writeOnSerial(QByteArray bytes){
    port->write(bytes);
    if(!port->waitForBytesWritten(1000)){
        qDebug()<<"Bytes not sent "<<bytes;
    }
    //qDebug()<<"TX -> "<<b<<" bytes";
    //qCritical()<<"ERR -> "<<port->errorString();
    /*

    if(b<=0){
        qDebug()<<port->errorString();
    }else{
        //port->flush();
        if(!port->waitForBytesWritten(1000)){
            qDebug()<<"Bytes not sent "<<bytes;
        }
    }
    */

}

void GSMInterface::readFromSerial(){
    int end = -1;
    QByteArray msg;
    RXbuffer.append(port->readAll());

    end = RXbuffer.indexOf("\r\n",0);
    while(end>=0){
        msg = RXbuffer.mid(0, end);
        RXbuffer.remove(0, end+2);

        if(msg.size())addRXmsg(msg);
        end = RXbuffer.indexOf("\r\n",0);
    }
}

void GSMInterface::addRXmsg(QByteArray data){
    QString msg(data);
    qDebug()<<"RX -> "<<msg;

    if(msg.contains("NORMAL POWER DOWN")){
        QMessageBox::critical(MainWindow::mainWindow, "Errore", "Il modulo GSM si è spento!");
        setState(CHECK_MODULE_ON);
    }

    else if(msg.contains("+PDP: DEACT")){
        QMessageBox::critical(MainWindow::mainWindow, "Errore", "Connessione interrotta: PDP DEACT");
        setState(RESET_NETWORK);
    }

    else if(msg.contains("CLOSED")){
        QMessageBox::critical(MainWindow::mainWindow, "Errore", "Connessione interrotta: CLOSED");
        setState(RESET_NETWORK);
    }

    else if(msg.contains("+CFUN: 1")){
        setState(CHECK_MODULE_ON);
    }

    else if(msg.contains("+IPSERVER")){
        serverIP = msg.remove("+IPSERVER:");

        QSettings lastUsedIP("config.ini", QSettings::IniFormat);
        lastUsedIP.beginGroup("GSMCONFIG");
        lastUsedIP.setValue("lastIP", serverIP);

        qDebug()<<"Server IP "<<serverIP;
        if(currentState == START_TCP_CONNECTION){
            setState(START_TCP_CONNECTION);
        }else{
            setState(RESET_NETWORK);
        }
    }

    else if(msg.contains("RING")){
    //else if(msg.contains("+CLCC") && currentState != VOICE_CALL){
        qDebug()<<"Chiamata";
        //RXmsg.append(data);
        setState(VOICE_CALL);
        response();
    }

    //+CLCC: id,from,status,0,0,"+393663154577",145,"""
    else if(msg.contains("+CLCC")){
        QString callInfo = msg;
        int callID = callInfo.at(7).digitValue();
        int callFrom = callInfo.at(9).digitValue();
        int callStatus = callInfo.at(11).digitValue();
        callInfo.remove(0,callInfo.indexOf("\"")+1);
        callInfo.chop(callInfo.size()-callInfo.indexOf("\""));

        if(callStatus == 0){
            sendCmd("AT+SGPIO = 0,1,1,1",-1);
        }else if(callStatus == 6){
            sendCmd("AT+SGPIO = 0,1,1,0",-1);
        }
        qDebug()<<"chiamata";
        emit callStatusUpdate(callID, callFrom, callStatus, callInfo);
    }

    else{
        RXmsg.append(data);
        responseCount--;
        if(responseCount<=0){
            response();
        }
    }
}

void GSMInterface::addTXmsg(QByteArray data){
    int size = 0;

    TXmsg.append(data);

    if(!isTextMode){
        isTextMode = true;

        sendCount = 0;
        while(sendCount<TXmsg.count() && (size + TXmsg.at(sendCount).size()+4)<PACKET_MAX_SIZE){
            size += TXmsg.at(sendCount).size()+4;
            sendCount++;
        }
        sendCmd(QString("AT+CIPSEND = 0,%1").arg(size), -1);
        QTimer::singleShot(100, this, SLOT(sendPacket()));

    }
}

void GSMInterface::sendCmd(QString cmd, int timeout, int count){
    qDebug()<<"TX -> "<<cmd;
    writeOnSerial(cmd.append(0xD).append(0xA).toUtf8());
    responseCount = count;
    if(timeout>0){
        responseTimer.setInterval(timeout);
        responseTimer.start();
    }else{
        responseTimer.stop();
    }
}

void GSMInterface::sendPacket(){
    QByteArray msg;
    for(int i=0; i<sendCount; i++){
        msg = TXmsg.value(i).prepend(0xA).prepend(0xD).append(0xD).append(0xA);
        qDebug()<<"TX -> "<<msg.toHex()<<"size -> "<<msg.size();
        writeOnSerial(msg);
    }
    TXmsg.remove(0, sendCount);
}

void GSMInterface::request(){
    switch (currentState) {
        case CHECK_MODULE_ON:
            qDebug()<<"CHECK_MODULE_ON";
            sendCmd("AT",3000);
            break;

        case CHECK_SIM_PIN:
            qDebug()<<"CHECK_SIM_PIN";
            sendCmd("AT+CPIN?",1000,2);
            break;

        case CHECK_GSM_CONNECTION:
            qDebug()<<"CHECK_GSM_CONNECTION";
            sendCmd("AT+CCALR?",3000,2);
            break;

        case RESET_NETWORK:
            qDebug()<<"RESET_NETWORK";
            serverIP = "";
            sendCmd("AT+CIPSHUT",10000);
            break;

        case SETUP_NETWORK_SETTINGS:
            qDebug()<<"SETUP_NETWORK_SETTINGS";
            sendCmd(QString(
                    "ATS0 = 1;"
                    "+CIPMUX = 1;"
                    "+CSTT = \"%1\", \"%2\", \"%3\";"
                    "+CIPUDPMODE = 1,1;"
                    "+CLPORT = 1,\"UDP\",%4;"
                    "+CIPQSEND = 1;"
                    "+CIPSPRT = 0;"
                    "+CLCC=1;"
                    "+CLVL = 100;" //Max volume
                    "+CMIC = 0,11" //Max mic gain
                    )
                    .arg(apn).arg(user).arg(psw).arg(UDPport),3000);
            break;

        case BRING_UP_WIRELESS:
            qDebug()<<"BRING_UP_WIRELESS";
            sendCmd("AT+CIICR",50000);
            break;

        case GET_CURRENT_IP:
            qDebug()<<"GET_CURRENT_IP";
            sendCmd("AT+CIFSR",5000);
            break;

        case START_TCP_CONNECTION:
            qDebug()<<"START_TCP_CONNECTION";
            //if(!serverIP.isEmpty()){
                //sendCmd(QString("AT+CIPSTART = 0,\"TCP\", \"%1\", \"%2\"").arg(serverIP).arg(TCPport),90000);

                sendCmd(QString("AT+CIPSTART = 0,\"TCP\", \"%1\", \"%2\"").arg("188.218.73.172").arg(TCPport),90000);
            //}
            break;

        case START_UDP_CONNECTION:
            qDebug()<<"START_UDP_CONNECTION";
            sendCmd(QString("AT+CIPSTART = 1,\"UDP\", \"%1\", \"%2\"").arg(serverIP).arg(UDPport),90000);
            break;

        /*
        case CONNECTED:
            qDebug()<<"START_UDP_CONNECTION";
            sendCmd("AT+CIPRXGET = 2,1,64",1000);
            break;
        */
            /*
        case VOICE_CALL:
            qDebug()<<"VOICE_CALL";
            sendCmd("AT+CLCC",-1);
            break;
            */
    }

}

void GSMInterface::response(){
    QStringList msg;
    for(int i=0; i<RXmsg.size(); i++){
        msg<<QString(RXmsg.at(i));
    }

    if(msg.size()>0){
        switch (currentState){
            case CHECK_MODULE_ON:
               if(msg.contains("OK")){
                   qDebug()<<"Modulo acceso";
                   RXmsg.clear();
                   writeOnConsole("INFOAccensione modulo GSM");
                   setState(CHECK_SIM_PIN);
               }
               break;

           case CHECK_SIM_PIN:
               if(msg.contains("+CPIN: SIM PIN")){
                   RXmsg.clear();
                   sendCmd(QString("AT+CPIN = %1").arg(pin),3000);
               }else if(msg.contains("OK") || msg.contains("+CPIN: READY")){
                   qDebug()<<"PIN OK";
                   RXmsg.clear();
                   writeOnConsole("INFOPIN inserito corretto");
                   setState(CHECK_GSM_CONNECTION);
               }else if(msg.contains("ERROR")){
                   qDebug()<<"PIN errato";
                   RXmsg.clear();
                   writeOnConsole("ERRPIN inserito errato. Modulo bloccato");
                   setState(BLOCK);
               }
               break;

            case CHECK_GSM_CONNECTION:
                if(msg.contains("+CCALR: 1")){
                    qDebug()<<"Connesso alla rete GSM";
                    RXmsg.clear();
                    writeOnConsole("INFOConnessione alla rete GSM riuscita");
                    setState(RESET_NETWORK);
                }else{
                    RXmsg.clear();
                }
                break;

            case RESET_NETWORK:
                if(msg.contains("SHUT OK")){
                    qDebug()<<"Reset OK";
                    RXmsg.clear();
                    writeOnConsole("INFOReset delle impostazioni GSM");
                    setState(SETUP_NETWORK_SETTINGS);
                }
                break;

            case SETUP_NETWORK_SETTINGS:
                if(msg.contains("OK")){
                    RXmsg.clear();
                    setState(BRING_UP_WIRELESS);
                }
                break;

            case BRING_UP_WIRELESS:
                if(msg.contains("OK")){
                    qDebug()<<"Wireless OK";
                    RXmsg.clear();
                    writeOnConsole("INFOConnessione senza fili abilitata");
                    setState(GET_CURRENT_IP);
                }else if(msg.contains("ERROR")){
                    qDebug()<<"Wireless ERROR";
                    RXmsg.clear();
                    writeOnConsole("ERRConnessione senza fili fallita");
                    setState(RESET_NETWORK);
                }
                break;

            case GET_CURRENT_IP:
                if(msg.contains("ERROR")){
                    qDebug()<<"IP ERROR";
                    RXmsg.clear();
                    setState(RESET_NETWORK);
                }else{
                    clientIP = msg.first();
                    RXmsg.clear();
                    writeOnConsole("INFOModulo GSM pronto");
                    setState(START_TCP_CONNECTION);
                }
                break;

            case START_TCP_CONNECTION:
                //if(!serverIP.isEmpty()){
                    if(msg.contains("0, CONNECT OK")){
                        qDebug()<<"Connesso al server";
                        RXmsg.clear();
                        writeOnConsole("INFOConnesso al server");
                        //setState(START_UDP_CONNECTION);
                        setState(CONNECTED);
                    }else if(msg.contains("OK")){
                        qDebug()<<"Parametri di connessione corretti";
                        RXmsg.clear();
                    }else if(msg.contains("0, ALREADY CONNECT")){
                        qDebug()<<"Connessione già esistente";
                        RXmsg.clear();
                        setState(START_UDP_CONNECTION);
                    }else if(msg.contains("0, CONNECT FAIL")){
                        qDebug()<<"Connessione fallita";
                        writeOnConsole("ERRConnessione al server fallita");
                        RXmsg.clear();
                        setState(START_TCP_CONNECTION);
                    }else if(msg.contains("ERROR")){
                        qDebug()<<"Errore durante la connessione";
                        RXmsg.clear();
                        writeOnConsole("ERRErrore di connessione al server");
                        setState(RESET_NETWORK);
                    }
               // }else{
                 //   RXmsg.clear();
                //}
                break;

            case START_UDP_CONNECTION:
                if(msg.contains("1, CONNECT OK")){
                    qDebug()<<"Connesso al server";
                    RXmsg.clear();
                    setState(CONNECTED);
                }else if(msg.contains("OK")){
                    qDebug()<<"Parametri di connessione corretti";
                    RXmsg.clear();
                }else if(msg.contains("1, ALREADY CONNECT")){
                    qDebug()<<"Connessione già esistente";
                    RXmsg.clear();
                    setState(CONNECTED);
                }else if(msg.contains("1, CONNECT FAIL")){
                    qDebug()<<"Connessione fallita";
                    RXmsg.clear();
                    setState(START_UDP_CONNECTION);
                }else if(msg.contains("1, ERROR")){
                    qDebug()<<"Errore durante la connessione";
                    RXmsg.clear();
                    setState(RESET_NETWORK);
                }
                break;

            case CONNECTED:
                if(msg.at(0).contains("DATA ACCEPT")){
                    qDebug()<<"Pacchetto inviato";
                    RXmsg.removeFirst();
                    isTextMode = false;
                }else if(msg.at(0).startsWith("CMD")){
                    if(!isTextMode){
                        sendCmd(msg.first().remove(0,3),-1);
                        RXmsg.removeFirst();
                    }
                }else if(msg.at(0).contains("DAT")){
                    qDebug()<<"Pacchetto ricevuto"<<RXmsg.first().toHex();
                    //addTXmsg(QByteArray("ACK").append(RXmsg.first().at(3)).append(RXmsg.first().at(4)));
                    emit dataPacketReceived(RXmsg.first());
                    RXmsg.removeFirst();

                }else{
                    RXmsg.removeFirst();
                }
                break;

            case VOICE_CALL:
                if(msg.at(0).contains("NO CARRIER") || msg.at(0).contains("BUSY") || msg.at(0).contains("ERROR")){
                    emit callMessage(msg.at(0));
                    RXmsg.removeFirst();
                }else{
                    RXmsg.removeFirst();
                }

                break;
        }
    }
}

void GSMInterface::setState(GSMState newState){
    responseTimer.stop();
    if(currentState!=newState){
        prevState = currentState;
    }
    currentState = newState;
    request();
}

void GSMInterface::loadSettings(){
    QList<QString> settings;
    QSqlQuery q(MainWindow::database);
    q.exec("SELECT Valore FROM ImpostazioniGSM");
    while(q.next()){
        settings.append(q.value(0).toString());
    }

    pin = settings.at(GSMSettings::RECEIVER_PIN);
    apn = settings.at(GSMSettings::APN_NAME);
    user = settings.at(GSMSettings::APN_USER);
    psw = settings.at(GSMSettings::APN_PSW);
    serverIP = "";
    TCPport = settings.at(GSMSettings::TCP_PORT).toInt();
    UDPport = settings.at(GSMSettings::UDP_PORT).toInt();
}

void GSMInterface::execExternalCmd(QString cmd){
    CMDmsg.append(cmd);
    processExternalCommand();
}

void GSMInterface::processExternalCommand(){
    if(!isTextMode){
        QString cmd = CMDmsg.first();

        if(cmd.contains("STARTCALL")){
            sendCmd(QString("ATD%1").arg(cmd.remove("STARTCALL")),-1);
            setState(VOICE_CALL);
            writeOnConsole("WARNAvvio chiamata");
        }

        else if(cmd.contains("ENDCALL") && getState() == VOICE_CALL){
            sendCmd("ATH",-1);
            setState(prevState);
            writeOnConsole("WARNFine chiamata");
        }

        else if(cmd.contains("SOLOCALL") && getState() == VOICE_CALL){
            sendCmd(QString("AT+CHLD = 2%1").arg(cmd.remove("SOLOCALL")),-1);
        }

        else if(cmd.contains("GROUPCALL") && getState() == VOICE_CALL){
            sendCmd("AT+CHLD = 3",-1);
            writeOnConsole("WARNAvvio chiamata di gruppo");
        }

        else if(cmd.contains("STARTDAT")){
            qDebug()<<"Inizio ricezione dati";
            addTXmsg("CMDSTARTDAT");
            DATStarted = true;
            writeOnConsole("INFOInizio ricezione dati");
        }

        else if(cmd.contains("ENDDAT")){
            qDebug()<<"Fine ricezione dati";
            addTXmsg("CMDENDDAT");
            DATStarted = false;
            writeOnConsole("INFOFine ricezione dati");
        }

        else if(cmd.contains("CAN")){
            qDebug()<<"Invio msg can";
            addTXmsg(cmd.toUtf8());
            writeOnConsole("INFOMessaggio inviato su can");
        }

        CMDmsg.removeFirst();
    }else{
        QTimer::singleShot(50,this,SLOT(processExternalCommand()));
    }
}

GSMInterface::~GSMInterface(){
    if(port){
        delete port;
    }
}
