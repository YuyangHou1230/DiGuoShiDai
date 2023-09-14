#include "AI_My.h"

#include "GlobalVariate.h"

AI_MY::AI_MY()
{

}

void AI_MY::processData()
{
    ProcessDataWork = 1;

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


    if(AIGame.human[0].NowState == HUMAN_STATE_STOP){ // 遇到障碍物，绕行
        // 判断障碍方向
        if( (AIGame.human[0].L - ListL[stepi]) < 0 && (AIGame.human[0].U - ListU[stepi] < 0) ){ // 坐下
            HumanMove(AIGame.human[0].SN, ListL[stepi], ListU[stepi] + BLOCKSIDELENGTH);
        }
    }
    else{ // 普通移动
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



//    if(AIGame.GameFrame > 1 && (fL < 1.1  && fU < 1.1) && stepi<stepn - 1) {
//    if(AIGame.GameFrame > 1 && stepi<stepn - 1) {




//    }

    // 盖住房

    // 砍树

    // 采集浆果


    ProcessDataWork = 0;
}
