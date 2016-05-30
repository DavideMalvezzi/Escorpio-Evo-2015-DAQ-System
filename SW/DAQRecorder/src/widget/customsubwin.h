#ifndef CUSTOMSUBWIN_H
#define CUSTOMSUBWIN_H

#include <QMdiSubWindow>
#include <QCloseEvent>

class CustomSubWin : public QMdiSubWindow{
    Q_OBJECT

public:
    explicit CustomSubWin(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);



signals:
    void onClose(bool);

public slots:

};

#endif // CUSTOMSUBWIN_H
