#ifndef VIEWCHANNELSPANEL_H
#define VIEWCHANNELSPANEL_H

#include <QWidget>

namespace Ui {
    class ChannelsPanelView;
}

class ChannelsPanelView : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelsPanelView(QWidget *parent = 0);
    void setup();
    ~ChannelsPanelView();

private:
    Ui::ChannelsPanelView *ui;
};

#endif // VIEWCHANNELSPANEL_H
