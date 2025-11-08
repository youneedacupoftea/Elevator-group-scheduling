#ifndef STRUCTURE_H
#define STRUCTURE_H
struct Passenger
{
    int peoNo;							    // 乘客个人编号
    int requestTime;					    // 乘客请求时间
    int srcFloor;						    // 乘客起始楼层
    int tarFloor;						    // 乘客目标楼层
    bool flag=false;
    int runStatus;						    // 乘客前行方向
    Passenger *next;

    Passenger()
    {
        peoNo = 0;
        requestTime = 0;
        srcFloor = 0;
        tarFloor = 0;
        runStatus = 0;
        next = nullptr;
    }
    Passenger(int peono, int request, int src, int tar)
    {
        peoNo = peono;
        requestTime = request;
        srcFloor = src;
        tarFloor = tar;
        if (tarFloor > srcFloor) runStatus = 1;
        else runStatus = -1;
        next = nullptr;
    }
};

struct TargetFloorList
{
    int floor;							    // 目标楼层
    TargetFloorList *next;

    TargetFloorList(int f = 0)
    {
        floor = f;
        next = nullptr;
    }
};

struct Command
{
    int status;
    int timer;
};

struct Elevator
{
    float nowFloor;						    // 电梯当前楼层
    int nowLoad;						    // 电梯当前负载

    Command runStatus;					    // UP = 1, DOWN = -1
    Command doorStatus;					    // CLOSE = 0, OPEN = 1
    Command elevatorStatus;				    /* READYWAIT = -2, NOREQUEST = -1  STAY = 0, RUN = 1
                                             DOOROPEN = 2, DOORCLOSE = 3   INELE = 4, OUTELE = 5 */
    Passenger *inElePeople;			    // 电梯乘客链表
    TargetFloorList *targetFloor;		    // 目标楼层链表

    Elevator()
    {
        nowFloor = 1;
        nowLoad = 0;

        runStatus = { 1, 0 };
        doorStatus = { 0, 0 };
        elevatorStatus = { -1, 0 };

        inElePeople = new Passenger;
        targetFloor = new TargetFloorList(0);
    }

};


struct ElevatorMessage
{
    float nowFloor;
    int nowLoad;
    Command runStatus;
    Command doorStatus;
    Command elevatorStatus;

    ElevatorMessage()
    {
        nowFloor = 1;
        nowLoad = 0;

        runStatus = {0, 0};
        doorStatus = {0, 0};
        elevatorStatus = {-2, 0};
    }
    ElevatorMessage(Elevator ele)
    {
        nowFloor = ele.nowFloor;
        nowLoad = ele.nowLoad;
        runStatus = ele.runStatus;
        doorStatus = ele.doorStatus;
        elevatorStatus = ele.elevatorStatus;
    }
};


#endif // STRUCTURE_H
