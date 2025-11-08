#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QImage>
#include <QIcon>
#include <QPainter>
#include<QPushButton>
#include "structure.h"
#include "elevatorsystem.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void Pause();


private:
    Ui::Widget *ui;
    Elevatorsystem elesystem;
    int SystemTime;                                                 // 系统时钟
    bool buttonOption;                                              // 电梯运行控制
    int floorPassenger[30];                                         // 楼层乘客数目
    ElevatorMessage eleMessage[8];                                  // 电梯信息参数
    static const int Height=30;
    static const int Width=22;

    QIcon endIco, pauseIco,continueIco;
    QImage buildingImg,eleOpenImg,eleCloseImg, eleHalfOpenImg, inImg,outImg;
    QPushButton *start;
    QPushButton *end;

    void addSystemTime();                                          // 递增系统时钟
    void updateMessage();
    void paintEvent(QPaintEvent *);
    void drawElevator(QPainter &painter);                           // 绘制电梯
    QRect getEleFloorRect(int eleNo, float nowFloor);               // 获取电梯绘制区域
};
#endif // WIDGET_H
