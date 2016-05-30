#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    Q_UNUSED(type);Q_UNUSED(context);
    QFile outFile("DAQlog.txt");
    outFile.open(QIODevice::Append);
    QTextStream ts(&outFile);
    ts << msg << endl;
    outFile.close();

    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "%s\n", localMsg.constData());
    fflush(stderr);
}

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qInstallMessageHandler(myMessageHandler);
    qDebug()<<"================= LOG "<<QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss")<<"=================";

    QPixmap p(":/images/SplashScreen.png");
    QSplashScreen splash(p, Qt::WindowStaysOnTopHint);
    QTimer::singleShot(2000, &splash, SLOT(close()));
    splash.show();

    MainWindow w;
    w.show();


    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(175, 175, 175));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);


    //qApp->removeLibraryPath(qApp->libraryPaths().first());

    foreach (const QString &path, qApp->libraryPaths())
        qDebug()<<"LIBRARY_PATH: "<<path;

    qApp->setPalette(darkPalette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    return a.exec();
}


