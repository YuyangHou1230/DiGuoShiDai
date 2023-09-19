#ifndef AI_MY_H
#define AI_MY_H

#include "AI.h"

#include "GlobalVariate.h"
struct mapInfo{
    bool IsEmpty = true;
    bool IsArrived = false;
    int Type;
    int SN;
    //human
    int NowState;
    //buliding
    int Foundation;
    int Project;//当前建筑忙碌于
    mapInfo(){

    }
    mapInfo(tagResource resource){
        Type = resource.Type;
        SN = resource.SN;
    }

    mapInfo(tagHuman resource){
        NowState = resource.NowState;
        SN = resource.SN;
    }

    mapInfo(tagBuilding resource){
        Type = resource.Type;
        Foundation = resource.Foundation;
        Project = resource.Project;
        SN = resource.SN;
    }
};

class AI_MY : public AI
{
public:
    AI_MY();

    virtual void processData();

    ///第一阶段
    void GameFirstStep(mapInfo* myMap,int m, int n);

    // 查找距离某个村民最近的资源
    int findResSN(tagHuman human, int resouce, int &index);
    bool hasBuilding(int type);
};

#endif // AI_MY_H
