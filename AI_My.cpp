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


    static int raoL = 0;
    static int raoU = 0;
    static list<pair<double ,double>> raoList;
    if(AIGame.human[0].NowState == HUMAN_STATE_STOP){ // 遇到障碍物，绕行
        // 判断障碍方向
        if( (AIGame.human[0].L - ListL[stepi]) > 0 && ((AIGame.human[0].U - ListU[stepi]) == 0) ){ // 左
            raoList.clear();
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U + BLOCKSIDELENGTH));
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U));
            HumanMove(AIGame.human[0].SN, AIGame.human[0].L, AIGame.human[0].U + BLOCKSIDELENGTH);
        }
        if( (AIGame.human[0].L - ListL[stepi]) == 0 && ((AIGame.human[0].U - ListU[stepi]) < 0) ){ // 上
            raoList.clear();
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U + BLOCKSIDELENGTH));
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U));
            HumanMove(AIGame.human[0].SN, ListL[stepi], ListU[stepi] + BLOCKSIDELENGTH);
        }
        if( (AIGame.human[0].L - ListL[stepi]) < 0 && ((AIGame.human[0].U - ListU[stepi]) == 0) ){ // 右
            raoList.clear();
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U - BLOCKSIDELENGTH));
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U));
            HumanMove(AIGame.human[0].SN, AIGame.human[0].L, AIGame.human[0].U + BLOCKSIDELENGTH);
        }
        if( (AIGame.human[0].L - ListL[stepi]) == 0 && ((AIGame.human[0].U - ListU[stepi]) > 0) ){ // 下
            raoList.clear();
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U + BLOCKSIDELENGTH));
            raoList.push_back(pair<double, double>(AIGame.human[0].L+ BLOCKSIDELENGTH*2, AIGame.human[0].U));
            HumanMove(AIGame.human[0].SN, ListL[stepi], ListU[stepi] + BLOCKSIDELENGTH);
        }
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
                stepi++;
            }else{
                double targetL;
                double targetU;
                // 判断是否到达目标点
                double fL = fabs(AIGame.human[0].L - ListL[stepi-1]);
                double fU = fabs(AIGame.human[0].U - ListU[stepi-1]);
                if((fL < 1.1  && fU < 1.1) ){ //到达目标点则发送新的移动指令

                    HumanMove(AIGame.human[0].SN, ListL[stepi], ListU[stepi]);
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
    if(Human2Action == 0){
        int bushSN = findResSN(AIGame.human[1], RESOURCE_BUSH);
        if(bushSN != 0){
            HumanAction(AIGame.human[1].SN, bushSN);
            Human2Action = 1;
        }
    }

    // 砍树
    static int Human3Action = 0;
    if(Human3Action == 0){
        int bushSN = findResSN(AIGame.human[2], RESOURCE_TREE);
        if(bushSN != 0){
            HumanAction(AIGame.human[2].SN, bushSN);
            Human3Action = 1;
        }
    }

    // 生产农民
    if(AIGame.human_n < AIGame.Human_MaxNum){
        if(AIGame.building[0].Project == BUILDING_FREE){
            BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_UPGRADE);
        }

    }


    ProcessDataWork = 0;
}

int AI_MY::findResSN(tagHuman human, int resouce)
{
    int dis = 1e6;
    int targetResSN = 0;
    for(int i = 0; i < AIGame.resource_n; i++) {
        if(AIGame.resource[i].Type == resouce) {
            double tempDistance = calDistance(human.L, human.U, AIGame.resource[i].L, AIGame.resource[i].U);
            if(dis > tempDistance) {
                dis = tempDistance;
                targetResSN = AIGame.resource[i].SN;
            }
        }
    }

    return targetResSN;
}
