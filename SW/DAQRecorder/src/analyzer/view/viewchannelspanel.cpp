#include "viewchannelspanel.h"
#include "ui_viewchannelspanel.h"

ChannelsPanelView::ChannelsPanelView(QWidget *parent) : QWidget(parent),ui(new Ui::ChannelsPanelView){
    ui->setupUi(this);
}

void ChannelsPanelView::setup(){
    ui->channelsList->clear();
}

ChannelsPanelView::~ChannelsPanelView(){
    delete ui;
}
