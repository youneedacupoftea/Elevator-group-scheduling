#include "elevatorsystem.h"
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QDebug>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;
Elevatorsystem::Elevatorsystem()
{
    SystemTime = 0;
    for (int i = 0; i < 30; i++)//初始化列表
    {
        outFloorPeople[i] = new Passenger();
    }
    initialize();  // 读取内外乘客请求
}

void Elevatorsystem::stepRunElevator()
{

    respondInOutRequest();
    for (int eleNo = 0; eleNo < 8; eleNo++)
    {

        //检测电梯运行状态是否改变
        changeElevatorStatus(eleNo);
        switch (elevators[eleNo].elevatorStatus.status)
        {
        case 0:
            checkOpenDoor(eleNo);
            break; // 检测开门
        case 1:
            elevatorMove(eleNo);
            break;   // 运行中
        case 2:
            openDoor(eleNo);
            break;	// 开门中
        case 3:
            closeDoor(eleNo);
            break;	 // 关门中
        case 4:
            enterElevator(eleNo);
            break; // 进人中
        case 5:
            leaveElevator(eleNo);
            break; // 出人中
        case -1:
            checkOverWait(eleNo);
            break;// 检测超时
        case -2:
            break; // 就绪等待
        }
        if (elevators[eleNo].doorStatus.status && elevators[eleNo].elevatorStatus.status != 3)
        {
            checkCloseDoor(eleNo);
        }
    }

    SystemTime++;
}

// 分配电梯的目标楼层
void Elevatorsystem::allocTargetFloor(int eleNo, int tarFloor)
{
    TargetFloorList *cur = elevators[eleNo].targetFloor;
    for (; cur->next != nullptr; cur = cur->next)
    {
        if (cur->next->floor == tarFloor) return;
    }
    cur->next = new TargetFloorList(tarFloor);
}

void Elevatorsystem::cancelTargetFloor(int eleNo, int tarFloor)
{
    TargetFloorList *cur = elevators[eleNo].targetFloor;
    // 查找删除该目标楼层
    for (; cur->next != nullptr; cur = cur->next)
    {
        if (cur->next->floor == tarFloor)
        {
            cur->next = cur->next->next;
            return;
        }
    }
}
ElevatorMessage Elevatorsystem::getElevatorMessage(int eleNo)
{
    ElevatorMessage message(elevators[eleNo - 1]);
    return message;
}
//添加内部乘客请求
void Elevatorsystem::addElevatorPassenger(int eleNo, int no, int time, int src, int tar)
{
    Passenger *cur;
    for (cur = elevators[eleNo].inElePeople; cur->next != nullptr; cur = cur->next)
    {
        // 乘客按时间顺序插入
        if (cur->next->requestTime > time)
        {
            Passenger *newNode = new Passenger(no, SystemTime, src, tar);
            newNode->next = cur->next;
            cur->next = newNode;
            return;
        }
    }
    cur->next = new Passenger(no, SystemTime, src, tar);
}

// 添加楼层乘客
void Elevatorsystem::addFloorPassenger(int no, int time, int src, int tar)
{
    Passenger *cur;
    for (cur = outFloorPeople[src - 1]; cur->next != nullptr; cur = cur->next)
    {
        // 乘客存在，不加入
        if (cur->next->peoNo == no)
        {
            return;
        }
        else if (cur->next->requestTime > time)
        {
            auto *newNode = new Passenger(no, time, src, tar);
            newNode->next = cur->next;
            cur->next = newNode;
            return;
        }
    }
    cur->next = new Passenger(no, time, src, tar);
}

//读入乘客数据
void Elevatorsystem::initialize()
{
    ifstream fp("D:/qt_files/ElevatorGroup/data.csv");
    string line;
    while(getline(fp,line))
    {
        int data[4];
        string num;
        istringstream readstr(line);//string数据流化, 将一行数据按'，'分割
        for (int i = 0; i < 4; i++)
        {
            getline(readstr, num, ',');//循环读取数据
            data[i]=atoi(num.c_str());//字符串传int
        }
      addFloorPassenger(data[0], data[1], data[2], data[3]);
    }
}

// 响应内外部请求
void Elevatorsystem::respondInOutRequest()
{
    for (int floorNo = 0; floorNo < 30; floorNo++)
    {
        for (Passenger *cur = outFloorPeople[floorNo]; cur->next != nullptr; cur = cur->next)
        {
            if ((cur->next->requestTime <= SystemTime) && (!cur->next->flag))
            {
                getOutRequest(cur->next->srcFloor, cur->next->runStatus);
                cur->next->flag=true;
            }
            else break;
        }
    }
    for (int eleNo = 0; eleNo < 8; eleNo++)
    {
        for (Passenger *cur = elevators[eleNo].inElePeople; cur->next != nullptr; cur = cur->next)
        {
            allocTargetFloor(eleNo, cur->next->tarFloor);
        }
    }
}
void Elevatorsystem::getOutRequest(int srcFloor,int runStatus)
{     
    //对每个请求，计算分配给不同电梯的函数值，选取使函数值最小的作为分配电梯
    int index=0;
    int max=INT_MAX;
     for(int i=0;i<8;i++)//对每部电梯求指标值
     {
         int NF=30;
         int PT=7;
         int Ti=0;
         int ns=0;//停几次到达请求楼层
         for(TargetFloorList *cur=elevators[i].targetFloor;cur->next!=nullptr;cur=cur->next)
         {
            ns++;
         }

         if(elevators[i].runStatus.status==1)//电梯在上行
         {
             if(srcFloor>=elevators[i].nowFloor&&runStatus==1)
             {
                 Ti=(srcFloor-elevators[i].nowFloor)*RUN_TIME+PT*ns;
             }
             else if(runStatus==-1)//请求下行
             {
                 Ti=(2*NF-srcFloor-elevators[i].nowFloor)*RUN_TIME+PT*ns;
             }
             else if(srcFloor<elevators[i].nowFloor&&runStatus==1)
             {
                 Ti=(2*NF+srcFloor-elevators[i].nowFloor-2)*RUN_TIME+PT*ns;
             }
         }
         else
         {
            if(srcFloor<=elevators[i].nowFloor&&runStatus==-1)
            {
               Ti=(elevators[i].nowFloor-srcFloor)*RUN_TIME+PT*ns;
            }
            else if(runStatus==1)
            {
                Ti=(elevators[i].nowFloor+srcFloor-2)*RUN_TIME+PT*ns;
            }
            else if(srcFloor>elevators[i].nowFloor&&runStatus==-1)
            {
                 Ti=(2*NF+elevators[i].nowFloor-srcFloor-2)*RUN_TIME+PT*ns;
            }
         }
         if (max > Ti)//选择小的
        {
            max = Ti;
           index = i;
        }
     }
         TargetFloorList *cur = elevators[index].targetFloor;
         if (cur->next == nullptr)
         {
             allocTargetFloor(index, srcFloor);
         }
         else if (elevators[index].runStatus.status == 1 && runStatus == 1)
         {
             if (elevators[index].nowFloor <= srcFloor)
             {
                 allocTargetFloor(index, srcFloor);
             }
             else return;
         }
         else if (elevators[index].runStatus.status == -1 && runStatus == -1)
         {
             if (elevators[index].nowFloor >= srcFloor)
             {
                 allocTargetFloor(index, srcFloor);
             }
             else return;
         }
}

Passenger* Elevatorsystem::searchIn(int eleNo)
{
    // 电梯满载时
    if (elevators[eleNo].nowLoad == MAX_LOAD)
    {
        return nullptr;
    }
    int nowFloor = (int)elevators[eleNo].nowFloor;
    for (Passenger *cur = outFloorPeople[nowFloor - 1]; cur->next != nullptr; cur = cur->next)
    {
        if (cur->next->requestTime > SystemTime)
        {
            return nullptr;
        }
        // 乘客与电梯运行方向相同，才可进入电梯
        else if (cur->next->runStatus == elevators[eleNo].runStatus.status)
        {
            return cur;
        }
    }
    return nullptr;
}

// 查询是否还有乘客出电梯
Passenger* Elevatorsystem::searchOut(int eleNo)
{
    if (elevators[eleNo].nowLoad == 0) {
        return nullptr;
    }

    int nowFloor = (int)elevators[eleNo].nowFloor;
    for (Passenger *cur = elevators[eleNo].inElePeople; cur->next != nullptr; cur = cur->next)
    {
        // 乘客目标楼层为当前楼层
        if (cur->next->tarFloor == nowFloor)
        {
            return cur;
        }
        else if (cur->next->tarFloor == 1)
        {
            if (nowFloor == 1) return cur;
            else continue;
        }
    }
    return nullptr;
}
//变更电梯运行状态,上下层均无请求时，回到一楼
void Elevatorsystem::changeElevatorStatus(int eleNo)
{
    bool downTag = false, upTag = false, nowTag = false;
    float nowFloor = elevators[eleNo].nowFloor;
    TargetFloorList *cur = elevators[eleNo].targetFloor;
    // 就绪等待状态
    if (elevators[eleNo].elevatorStatus.status == -2)
    {
        if (cur->next != nullptr)
        {
            elevators[eleNo].elevatorStatus = { 0, 0 };
        }
    }
    // 无请求状态
    else if (cur->next == nullptr)
    {
        if (elevators[eleNo].elevatorStatus.status == -1) return;
        else elevators[eleNo].elevatorStatus = { -1, 0 };
    }
    //空载超时回归基层后，置就绪等待态
    else if (elevators[eleNo].nowLoad == 0 && nowFloor == 1 && cur->next->floor == 1 && cur->next->next == nullptr)
    {
        elevators[eleNo].elevatorStatus = { -2, 0 };
        cancelTargetFloor(eleNo, 1);
    }
    // 其他操作状态
    else if (elevators[eleNo].elevatorStatus.status == -1)
    {
        elevators[eleNo].elevatorStatus = { 0, 0 };
    }

     //改变运行状态
    for (; cur->next != nullptr; cur = cur->next)
    {
        if (nowFloor == (float)cur->next->floor)
        {
            nowTag = true;
        }
        else if ((float)cur->next->floor > nowFloor)
        {
            upTag = true;
        }
        else if ((float)cur->next->floor < nowFloor)
        {
            downTag = true;
        }
    }
    if (nowFloor == (float)1)//到一楼
    {
        elevators[eleNo].runStatus.status = 1;
    }
   else if (nowFloor == (float)30)//到顶层
    {
        elevators[eleNo].runStatus.status = -1;
    }
    else if (nowTag && !upTag && !downTag) // 到达目标楼层 上行行均无请求
   {
        int status = elevators[eleNo].elevatorStatus.status;
        if (status == 0 || status == 2)
        {
            if (searchIn(eleNo) == nullptr)
            {
                elevators[eleNo].runStatus.status =-1;
            }
        }
    }
    else if (!nowTag)
    {
        if (!upTag && !downTag)
        {
            elevators[eleNo].elevatorStatus = { -1, 0 };//置为无请求状态
        }
        // 电梯行进方向无请求，转向
        else if (!upTag && elevators[eleNo].runStatus.status == 1)
        {
            elevators[eleNo].runStatus.status = -1;
        }
        else if (!downTag && elevators[eleNo].runStatus.status == -1)
        {
            elevators[eleNo].runStatus.status = 1;
        }
    }
   // 电梯移动到目标楼层，检测是否开门
    if (nowTag && elevators[eleNo].elevatorStatus.status == 1)
    {
        checkOpenDoor(eleNo);
    }
}

// 电梯移动
void Elevatorsystem::elevatorMove(int eleNo)
{
    switch (elevators[eleNo].runStatus.status)
    {
    case 1:
        elevators[eleNo].nowFloor += 0.5;
        break;
    case -1:
        elevators[eleNo].nowFloor -= 0.5;
        break;
    }
}

/* 开电梯门 */
void Elevatorsystem::openDoor(int eleNo)
{
    int gapTime = elevators[eleNo].elevatorStatus.timer;
    if ( gapTime < OPEN_TIME)
    {
        elevators[eleNo].elevatorStatus.timer++;
    }
    // 完成开门
    else if (gapTime == OPEN_TIME)
    {
        elevators[eleNo].doorStatus = { 1, 0 };

        //完成开门后 有人出入
        if (searchOut(eleNo) != nullptr)
        {
            elevators[eleNo].elevatorStatus = { 5, 0 };
            leaveElevator(eleNo);
        }
        else if (searchIn(eleNo) != nullptr)
        {
            elevators[eleNo].elevatorStatus = { 4, 0 };
            enterElevator(eleNo);
        }
        else elevators[eleNo].elevatorStatus = { 0, 0 };
    }
}

// 关电梯门
void Elevatorsystem::closeDoor(int eleNo)
{
    int gapTime = elevators[eleNo].elevatorStatus.timer;
    if ( gapTime < CLOSE_TIME)
    {
        elevators[eleNo].elevatorStatus.timer++;
       // 如果关门期间有人进入
        if (searchIn(eleNo) != nullptr)
        {
            if (!gapTime)
            {
                elevators[eleNo].elevatorStatus = { 4, 0 };
                enterElevator(eleNo);
            }
              else if (gapTime < CLOSE_TIME)
            {
                elevators[eleNo].elevatorStatus = { 2, OPEN_TIME - gapTime };
                openDoor(eleNo);
            }
        }
    }
    // 完成关门
    else if (gapTime == CLOSE_TIME)
    {
        elevators[eleNo].doorStatus = { 0, 0 };
        cancelTargetFloor(eleNo, (int)elevators[eleNo].nowFloor);
        elevators[eleNo].elevatorStatus = { 1, 0 };//继续运行
    }
}
void Elevatorsystem::enterElevator(int eleNo)
{
    int gapTime = elevators[eleNo].elevatorStatus.timer;
    if (gapTime < INOUT_TIME)
    {
        elevators[eleNo].elevatorStatus.timer++;
    }
    else if (gapTime == INOUT_TIME)
    {
        Passenger *cur = searchIn(eleNo);

        allocTargetFloor(eleNo, cur->next->tarFloor); // 响应进电梯乘客请求

        addElevatorPassenger(eleNo, cur->next->peoNo, cur->next->requestTime, cur->next->srcFloor, cur->next->tarFloor);
        cur->next = cur->next->next;
        elevators[eleNo].nowLoad++;

        if (searchIn(eleNo) != nullptr)
        {
            elevators[eleNo].elevatorStatus = { 4, 0 };
            enterElevator(eleNo);
        }
        else elevators[eleNo].elevatorStatus = { 0, 0 };
    }
}

// 乘客离开
void Elevatorsystem::leaveElevator(int eleNo)
{
    int gapTime = elevators[eleNo].elevatorStatus.timer;
    if ( gapTime < INOUT_TIME)
    {
        elevators[eleNo].elevatorStatus.timer++;
    }
    else if (gapTime == INOUT_TIME)
    {
        Passenger *cur = searchOut(eleNo);
        cur->next = cur->next->next;
        elevators[eleNo].nowLoad--;
        //有人离开
        if (searchOut(eleNo) != nullptr)
        {
            elevators[eleNo].elevatorStatus = { 5, 0 };
            leaveElevator(eleNo);
        }
        else if (searchIn(eleNo) != nullptr)
        {
            elevators[eleNo].elevatorStatus = { 4, 0 };
            enterElevator(eleNo);
        }
        else elevators[eleNo].elevatorStatus = { 0, 0 };
    }
}


void Elevatorsystem::checkOpenDoor(int eleNo)
{
    int nowFloor = (int)elevators[eleNo].nowFloor;
    if (elevators[eleNo].doorStatus.status == 1) return;
    else
    {
        bool nowTag = false;
        for (TargetFloorList *cur = elevators[eleNo].targetFloor; cur->next != nullptr; cur = cur->next)
        {
            if (nowFloor == (float)cur->next->floor)
            {
                nowTag = true;
                break;
            }
        }
        // 是目标层
        if (nowTag)
        {
            if (searchOut(eleNo) != nullptr || searchIn(eleNo) != nullptr)
            {
                elevators[eleNo].elevatorStatus = { 2, 0 };
                return;
            }
            else
                 elevators[eleNo].elevatorStatus = { 3, 0 };
        }
        elevators[eleNo].elevatorStatus = { 1, 0 };
    }
}

// 检测能否关门模块
void Elevatorsystem::checkCloseDoor(int eleNo)
{
    int stayTimer = elevators[eleNo].doorStatus.timer;
    if (stayTimer < CHECK_TIME)
    {
        elevators[eleNo].doorStatus.timer++;

        // 出人进人中，跳过
        if (elevators[eleNo].elevatorStatus.status == 5 || elevators[eleNo].elevatorStatus.status == 4);
        else if (searchIn(eleNo) != nullptr)
        {
            elevators[eleNo].elevatorStatus = { 4, 0 };
            enterElevator(eleNo);
        }
    }
    else if (stayTimer == CHECK_TIME)
    {
        // 如果出人进入中，重置计时
        if (elevators[eleNo].elevatorStatus.status == 5 || elevators[eleNo].elevatorStatus.status == 4)
        {
            elevators[eleNo].doorStatus.timer = 0;
        }
        else if (searchIn(eleNo) != nullptr)
        {
            elevators[eleNo].doorStatus.timer = 0;
            elevators[eleNo].elevatorStatus = { 4, 0 };
            enterElevator(eleNo);
        }
        // 处于停留态 无人进入 关门
        else if (elevators[eleNo].elevatorStatus.status == 0 && searchIn(eleNo) == nullptr)
        {
            elevators[eleNo].doorStatus.timer = 0;
            elevators[eleNo].elevatorStatus = { 3, 0 };
            closeDoor(eleNo);
        }
    }
}

void Elevatorsystem::checkOverWait(int eleNo)
{
    int waitTimer = elevators[eleNo].elevatorStatus.timer;
    if (waitTimer < MAX_STAY_TIME)
    {
        elevators[eleNo].elevatorStatus.timer++;
    }
    else if (waitTimer == MAX_STAY_TIME)
    {
        allocTargetFloor(eleNo, 1);//回到一楼
        elevators[eleNo].elevatorStatus = { 0, 0 };
    }
}



