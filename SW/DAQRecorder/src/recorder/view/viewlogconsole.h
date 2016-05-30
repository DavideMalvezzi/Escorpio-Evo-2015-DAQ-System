#ifndef LOGCONSOLE_H
#define LOGCONSOLE_H

#include <QWidget>

namespace Ui {
    class LogConsole;
}

class LogConsole : public QWidget
{
    Q_OBJECT

public:
    enum Type{
        INFO,
        WARNING,
        ERRORS
    };

    explicit LogConsole(QWidget *parent = 0);
    ~LogConsole();

public slots:
    void addToLog(QString info);
    void addToLog(Type type, QString info);

private:
    Ui::LogConsole *ui;
};

#endif // LOGCONSOLE_H
