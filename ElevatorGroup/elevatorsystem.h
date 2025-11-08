#ifndef ELEVATORSYSTEM_H
#define ELEVATORSYSTEM_H
#include"structure.h"

class Elevatorsystem
{
public:
    Elevatorsystem();

    void stepRunElevator();   // 单步电梯运行电梯
    ElevatorMessage getElevatorMessage(int eleNo);
    int flag={0};

private:
    int SystemTime;
    static const int MAX_LOAD = 15;// 电梯最大负载
    static const int INOUT_TIME = 3;//乘客进出时间
    static const int RUN_TIME = 2;//电梯上下一楼用时
    static const int OPEN_TIME = 2;//开门时间
    static const int CLOSE_TIME = 2;//关门时间
    static const int CHECK_TIME = 4;//检测时间
    static const int MAX_STAY_TIME = 30;//最大停留时间
    Elevator elevators[8];// 电梯数据数组
    Passenger *outFloorPeople[30];	 // 楼层乘客链表
    void initialize();// 初始化乘客数据
    void addElevatorPassenger(int eleNo, int no, int time, int src, int tar);
    void addFloorPassenger(int no, int time, int src, int tar);
    void respondInOutRequest();// 内外乘客请求处理
    void getOutRequest(int srcFloor,int runStatus);
    Passenger* searchIn(int eleNo);
    Passenger * searchOut(int eleNo);

    void changeElevatorStatus(int eleNo); // 电梯状态改变
    void elevatorMove(int eleNo);
    void openDoor(int eleNo);
    void closeDoor(int eleNo);
    void enterElevator(int eleNo);
    void leaveElevator(int eleNo);

    // 分配电梯的目标楼层
    void allocTargetFloor(int eleNo, int tarFloor);
    void cancelTargetFloor(int eleNo, int tarFloor);

    void checkOpenDoor(int eleNo);
    void checkCloseDoor(int eleNo);
    void checkOverWait(int eleNo);

};

#endif // ELEVATORSYSTEM_H
