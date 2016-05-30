#ifndef DASHPAINTER_H
#define DASHPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QRect>
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QDropEvent>
#include <QApplication>
#include <QTreeWidgetItem>
#include "dashwidget.h"

class DashPainter : public DashWidget{
    Q_OBJECT
public:
    explicit DashPainter(QWidget *parent = 0);
    void addWidget(DashWidget*w, bool conn=true);
    void removeWidget(DashWidget* widget);
    QWidget* getSettingsWidget(int setting);
    QList<DashWidget*>* getWidgetsList();
    ~DashPainter();
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void dropEvent(QDropEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void keyPressEvent(QKeyEvent *);

signals:
    void changeSelected(DashWidget *);
    void widgetDeleted(int);

public slots:
    void setMinimumWidth(int w);
    void setMinimumHeight(int h);
    void setSelected(DashWidget*);

private:
    QList<DashWidget*>*widgets;
    DashWidget *selectedWidget;
    QPoint dragStartPos;
};

#endif // DASHPAINTER_H
