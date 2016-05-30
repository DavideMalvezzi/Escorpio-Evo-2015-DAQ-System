#ifndef DATABASECREATOR_H
#define DATABASECREATOR_H

#include <QThread>
#include <QFile>
#include <QProgressBar>

class DatabaseCreator : public QThread{
    Q_OBJECT
public:
    DatabaseCreator(QString url);
    ~DatabaseCreator();

private:
    QString url;
    QProgressBar *progress;

protected:
    virtual void run();

signals:
    void updateProgress(int);
};

#endif // DATABASECREATOR_H
