#ifndef AI_MY_H
#define AI_MY_H

#include "AI.h"

#include "GlobalVariate.h"

class AI_MY : public AI
{
public:
    AI_MY();

    virtual void processData();

    // 查找距离某个村民最近的资源
    int findResSN(tagHuman human, int resouce, int &index);
    bool hasBuilding(int type);
};

#endif // AI_MY_H
