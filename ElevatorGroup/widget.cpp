#include "widget.h"
#include "ui_widget.h"
#include<Windows.h>
#include<QLCDNumber>
#include<QTimer>
#include<QPushButton>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    endIco.addFile(":/label/end.png");
    pauseIco.addFile(":/label/pause.png");
    continueIco.addFile(":/label/continue.png");

    buildingImg.load(":/label/ele.png");
    eleOpenImg.load(":/label/open.png");
    eleCloseImg.load(":/label/close.png");
    eleHalfOpenImg.load(":/label/halfopen.png");
    inImg.load(":/label/in.png");
    outImg.load(":/label/out.png");

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/label/elevator.ico"));

    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("background-color: rgb(255, 255, 255)");

     start=new QPushButton;
     start->move(100,360);
     start->resize(81,71);
     start->setParent(this);
     start->setIcon(continueIco);
     end=new QPushButton;
     end->move(200,360);
     end->resize(81,71);
     end->setParent(this);
     end->setIcon(endIco);

     connect(end,&QPushButton::clicked,this,&QWidget::close);
     connect(start,&QPushButton::clicked,this,&Widget::Pause);
    SystemTime = 0;
    buttonOption =false;
    updateMessage();
}
void Widget::addSystemTime()
{
    elesystem.stepRunElevator();
    updateMessage();
    Sleep(100);
    SystemTime++;

    // 视图更新
//    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
//    timer->start(50);
    update();
}

void Widget::updateMessage()
{
    // 更新电梯信息参数
    for (int eleNo = 1; eleNo <= 8; eleNo++)
    {
        eleMessage[eleNo - 1] = elesystem.getElevatorMessage(eleNo);
    }
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    ui->lcdNumber->display(SystemTime);
    ui->lcdNumber_2->display(eleMessage[0].nowLoad);
    ui->lcdNumber_3->display(eleMessage[1].nowLoad);
    ui->lcdNumber_4->display(eleMessage[2].nowLoad);
    ui->lcdNumber_5->display(eleMessage[3].nowLoad);
    ui->lcdNumber_6->display(eleMessage[4].nowLoad);
    ui->lcdNumber_7->display(eleMessage[5].nowLoad);
    ui->lcdNumber_8->display(eleMessage[6].nowLoad);
    ui->lcdNumber_9->display(eleMessage[7].nowLoad);

    painter.drawImage(450,20, buildingImg);
    drawElevator(painter);
    //更新
    if (buttonOption)
    {
        addSystemTime();
    }
}

void Widget::drawElevator(QPainter &painter)
{
    for (int eleNo = 0; eleNo < 8; eleNo++)
    {
        ElevatorMessage message = eleMessage[eleNo];
        QRect elevatorRect = getEleFloorRect(eleNo + 1, message.nowFloor);
        QImage eleImg;
        // 关门状态
        if (message.doorStatus.status == 0)
        {
            if (message.elevatorStatus.status == 2 && message.elevatorStatus.timer == 1)
            {
                eleImg = eleHalfOpenImg;
            }
            else if (message.elevatorStatus.status == 2 && message.elevatorStatus.timer == 2)
            {
                eleImg = eleOpenImg;
            }
            else eleImg = eleCloseImg;
        }
        // 开门状态
        else if (message.doorStatus.status == 1)
        {
            if (message.elevatorStatus.status == 3 && message.elevatorStatus.timer == 1)
            {
                eleImg = eleHalfOpenImg;
            }
            else if (message.elevatorStatus.status == 3 && message.elevatorStatus.timer == 2)
            {
                eleImg = eleCloseImg;
            }
            else eleImg = eleOpenImg;
        }

        painter.drawImage(elevatorRect, eleImg);
    }
}


// 获取电梯绘制位置
QRect Widget::getEleFloorRect(int eleNo, float nowFloor)
{
    int x=480+(eleNo-1)*(Width+0.5);
    int y=903-(nowFloor-1)*(Height+0.5);
    return QRect(x, y, Width, Height);
}

void Widget::Pause()
{
    buttonOption = !buttonOption;
    if (buttonOption)
    {
        start->setIcon(pauseIco);
    }
    else
    {
       start->setIcon(continueIco);
    }
}
Widget::~Widget()
{
    delete ui;
}
