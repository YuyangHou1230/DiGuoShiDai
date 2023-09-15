#include "AI_My.h"


AI_MY::AI_MY()
{

}

void AI_MY::processData()
{
    ProcessDataWork = 1;

    if(AIGame.GameFrame == 0) {
        emit cheatRes();
        emit cheatRes();
        emit cheatRes();
        // 也可以再初始化时更改农民上限或进入下一时代
//        emit cheatFarmerLimit();
//        emit cheatAge();

        // 打印市镇中心左边
        qDebug() << "市镇中心：" << AIGame.building[0].BlockL << AIGame.building[0].BlockU;

        ProcessDataWork = 0;
        return ;
    } //这些代码仅用于方便调试，模拟实际运行环境时应删除

    static pair<int, int> feixuPosList; // 废墟位置列表

    // 探路
    auto converFunc = [](pair<int, int> value) -> pair<double, double> {
        pair<double, double> ret(0, 0);
        ret.first = value.first * BLOCKSIDELENGTH;
        ret.second = value.second * BLOCKSIDELENGTH;
    };
    // 第一个人绕市镇中心一圈探路
    double L = AIGame.human[0].L;
    double U = AIGame.human[0].U;
    const static int stepn = 6;
    // 相对坐标
    int a = 4;
    const int b = 3;
    static int circleNum = 1; // 第几圈



    static double ListL[stepn];
    static double ListU[stepn];
    static int stepi = 0;
    static double targetL = ListL[stepi];
    static double targetU = ListL[stepi];


    static int corectL = 0;     //修正参数L
    static int corectU = 0;     //修正参数U
    static list<pair<double ,double>> raoList;
    static int correctTimes = 0;
    if(AIGame.human[0].NowState == HUMAN_STATE_STOP){ // 遇到障碍物，绕行
        // 判断障碍方向
        double LD = AIGame.human[0].L - ListL[stepi - 1];
        double UD = AIGame.human[0].U - ListU[stepi - 1];

        int tCorrectL = 0;
        int tCorrectU = 0;
        if(LD > 1.1 && qAbs(UD) < 1.1){
            qDebug() << "左边有障碍";

            //            raoList.push_back(pair<double, double>(targetL, targetU));
            tCorrectL = 0;
            tCorrectU = 1;
        }
        if(LD < -1.1 && qAbs(UD) < 1.1){
//            raoList.push_back(pair<double, double>(AIGame.human[0].L, AIGame.human[0].U - BLOCKSIDELENGTH));
            qDebug() << "右边有障碍";

            tCorrectL = 0;
            tCorrectU = -1;
        }
        if(qAbs(LD) < 1.1 && UD > 1.1){
//            raoList.push_back(pair<double, double>(AIGame.human[0].L, AIGame.human[0].U - BLOCKSIDELENGTH));
            qDebug() << "下边有障碍";

            tCorrectL = -1;
            tCorrectU = 0;
        }
        if(qAbs(LD) < 1.1 && UD < -1.1){
//            raoList.push_back(pair<double, double>(AIGame.human[0].L, AIGame.human[0].U + BLOCKSIDELENGTH));
            qDebug() << "上边有障碍";

            tCorrectL = 1;
            tCorrectU = 0;
        }
        targetL = AIGame.human[0].L + tCorrectL * BLOCKSIDELENGTH;
        targetU = AIGame.human[0].U + tCorrectU * BLOCKSIDELENGTH;

        corectL += tCorrectL;
        corectU += tCorrectU;

        correctTimes+= 1;

        HumanMove(AIGame.human[0].SN, targetL, targetU);
    }
    else{ // 普通移动
        if(!raoList.empty()){
            HumanMove(AIGame.human[0].SN, raoList.front().first, raoList.front().second);
            raoList.pop_front();
        }
        else{
            if(stepi == 0){
                int dL[stepn] = {-b, 0, (2*a*circleNum+b), 0, -(2*a*circleNum+b)};
                int dU[stepn] = {0, a*circleNum+b, 0, -(2*a*circleNum+b), 0, };
                double L1=L,U1=U;
                for(int i=0;i<stepn;i++){
                    L1 += dL[i] * BLOCKSIDELENGTH;
                    ListL[i] = L1;
                    U1 += dU[i] * BLOCKSIDELENGTH;
                    ListU[i] = U1;
                }
                HumanMove(AIGame.human[0].SN, ListL[stepi], ListU[stepi]);
                targetL = ListL[stepi];
                targetU = ListU[stepi];
                stepi++;
            }else{
                // 判断是否到达目标点
                double fL = fabs(AIGame.human[0].L - targetL);
                double fU = fabs(AIGame.human[0].U - targetU);
                if((fL < 1.1  && fU < 1.1) ){ //到达目标点则发送新的移动指令

                    targetL = ListL[stepi] - corectL * BLOCKSIDELENGTH;
                    targetU = ListU[stepi] - corectU * BLOCKSIDELENGTH;
                    corectL = 0;
                    corectU = 0;
                    HumanMove(AIGame.human[0].SN, targetL, targetU);

                    stepi++;

                    if(stepi == stepn){
                        stepi = 0;
                        circleNum ++; // 圈数增加
                    }
                }
            }
        }


    }



//    if(AIGame.GameFrame > 1 && (fL < 1.1  && fU < 1.1) && stepi<stepn - 1) {
//    if(AIGame.GameFrame > 1 && stepi<stepn - 1) {




//    }

    // 盖住房



    // 采集浆果
    static int Human2Action = 0;
    static int bushIndex = -1;
    if(Human2Action == 0){
        int bushSN = findResSN(AIGame.human[1], RESOURCE_BUSH, bushIndex);
        if(bushSN != 0){
            HumanAction(AIGame.human[1].SN, bushSN);
            Human2Action = 1;
        }
    }

    // 砍树
    static int Human3Action = 0;
    int index;
    if(Human3Action == 0){
        int bushSN = findResSN(AIGame.human[2], RESOURCE_TREE,index);
        if(bushSN != 0){
            HumanAction(AIGame.human[2].SN, bushSN);
            Human3Action = 1;
        }
    }

    // 生产农民
    if(AIGame.human_n < AIGame.Human_MaxNum){
        if(AIGame.building[0].Project == BUILDING_FREE){
            BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_CREATEFARMER);
        }
    }

    // 生产之后首先应该建造谷仓和房屋和仓库
    static bool hasGuCang = false;
    static bool isBuilding = false;
    hasGuCang = hasBuilding(BUILDING_GRANARY); // 每帧检测是否有谷仓
    if(!hasGuCang && !isBuilding){ // 没有则建造
        // 使用第四个人建造
        if(AIGame.human_n > 3){
            // 在旁边建造
            isBuilding = true;
            HumanBuild(AIGame.human[3].SN, BUILDING_GRANARY, 10, 10);
        }
    }
    else{
        if(AIGame.human[3].NowState == HUMAN_STATE_IDLE){
            // 计算建造房屋的位置（默认一个，然后判断是否可以）

            // 建造房屋
        }
    }

    // 得到当前空闲的人的SN
    list<int> emptyFarmerSN;
    for(int i = 1; i < AIGame.human_n; i++) {
        if(AIGame.human[i].NowState == HUMAN_STATE_IDLE){
            emptyFarmerSN.push_back(AIGame.human[i].SN);
        }
    }
    // 大于4个人时 先拿出一个人去建造仓库
    static int CangkuNum = 0;
    static bool isBuiling = false;

    if(CangkuNum == 0 && !isBuiling && AIGame.human_n> 4){
        if(emptyFarmerSN.empty()){
            HumanBuild(emptyFarmerSN.front(), BUILDING_STOCK, 37, 34);
        }
        else{
            HumanBuild(AIGame.human[4].SN, BUILDING_STOCK, 37, 34);
        }

        isBuiling = true;
    }

    // 升级至工具时代

    // 建造箭塔 ，采集石头


    ProcessDataWork = 0;
}

int AI_MY::findResSN(tagHuman human, int resouce, int &index)
{
    int dis = 1e6;
    int targetResSN = 0;
    for(int i = 0; i < AIGame.resource_n; i++) {
        if(AIGame.resource[i].Type == resouce) {
            double tempDistance = calDistance(human.L, human.U, AIGame.resource[i].L, AIGame.resource[i].U);
            if(dis > tempDistance) {
                dis = tempDistance;
                index = i;
                targetResSN = AIGame.resource[i].SN;
            }
        }
    }

    return targetResSN;
}

bool AI_MY::hasBuilding(int type)
{
    bool has = false;
    for(int i = 0; i < AIGame.building_n; i++) {
        if(AIGame.building[i].Type == type){
            has = true;
            break;
        }
    }

    return has;
}