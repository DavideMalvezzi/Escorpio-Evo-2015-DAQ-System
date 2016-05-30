#ifndef DASHWIDGET_H
#define DASHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QGraphicsView>
#include <QSpinBox>
#include <QComboBox>
#include <QSqlQueryModel>
#include <QJsonObject>
#include <QTreeWidgetItem>

class DashWidget : public QWidget{
    Q_OBJECT
public:
    explicit DashWidget(QWidget *parent = 0);
    explicit DashWidget(int index, int canID, QJsonObject json);
    virtual QWidget* getSettingsWidget(int setting);
    virtual void repaint(QPainter &painter);
    virtual void updateValue(QVariant value);
    virtual QString toJson();
    virtual int type();
    virtual DashWidget* getCopy();

    virtual void setItem(QTreeWidgetItem* data);
    virtual QTreeWidgetItem* getItem();
    virtual int getIndex();
    virtual int getCanID();
    virtual void select();
    virtual void deselect();
    virtual void setIndex(int index);
    virtual void setSize(int w, int h);
    virtual void setPos(int x, int y);
    virtual QString getName();
    virtual QSize size();
    virtual QPoint pos();
    virtual QRect geometry();
    virtual QSize sizeHint();
    virtual QPixmap toPixmap();
    ~DashWidget();

protected:
    bool isSelected;
    int index,canID,x,y,width,height;
    QTreeWidgetItem* item;

signals:
    void paintRequest();

public slots:
    void setName(QString name);
    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);

protected slots:
    void setCanID(int value);
};

#endif // DASHWIDGET_H
