#include "AI.h"
#include "config.h"
#include "MainWidget.h"
#include <Qdebug>
#include <Windows.h>
#include <iostream>

AI::AI(QObject *parent)
    : QObject(parent)
{
}

void AI::processData()
{
    // 与主线程通讯
    ProcessDataWork = 1;

    // 使用作弊函数获取资源
    if(AIGame.GameFrame == 0) {
//        emit cheatRes();
//        emit cheatRes();
//        emit cheatRes();
        // 也可以再初始化时更改农民上限或进入下一时代
//        emit cheatFarmerLimit();
//        emit cheatAge();
        ProcessDataWork = 0;
        return ;
    } //这些代码仅用于方便调试，模拟实际运行环境时应删除

    // dis用于存储对象和对象之间的距离，targetResSN用于存储目的资源的全局编号
    double dis = 1e6;
    int targetResSN = 0;

    // 升级时代完成后，AI停止工作
    if(AIGame.civilizationStage == CIVILIZATION_TOOLAGE) {
        ProcessDataWork = 0;
        return ;
    }

    static bool tryBuild = false; // 令某些建筑指令只发一次，不重复
    static bool GranaryJudge = false; // 判断谷仓是否建好
    static bool StockJudge = false; // 判断仓库是否建好

    // 如果同时拥有粮仓和仓库，且食物足够、市镇中心空闲时，尝试升级到下一个时代
    if(AIGame.civilizationStage == CIVILIZATION_STONEAGE && GranaryJudge == true && StockJudge == true && AIGame.building[0].Project == BUILDING_FREE && AIGame.Meat >= BUILDING_CENTER_UPGRADE_FOOD)
        BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_UPGRADE);

    // 如果食物足够、市镇中心空闲且农民数量小于5时，尝试生产一个农民，且优先级高于升级时代
    if(AIGame.building[0].Project == BUILDING_FREE && AIGame.Meat >= BUILDING_CENTER_CREATEFARMER_FOOD && AIGame.human_n < 4)
        BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_CREATEFARMER);

    for(int i = 0; i < AIGame.building_n; i++) {
        // 判断是否存在粮仓和仓库，此为升级时代的前置条件
        if(AIGame.building[i].Type == BUILDING_GRANARY && GranaryJudge == false) {
            emit AIDebugText("存在粮仓");
            GranaryJudge = true;
        }
        else if(AIGame.building[i].Type == BUILDING_STOCK && StockJudge == false) {
            emit AIDebugText("存在仓库");
            StockJudge = true;
        }
    }

    // 第一个人绕市镇中心一圈探路
    double L = AIGame.human[0].L;
    double U = AIGame.human[0].U;
    const static int stepn = 6;
    // 相对坐标
    static int dL[stepn] = {0, -3, 0, 12, 0, -15};
    static int dU[stepn] = {-1, 0, 11, 0, -15, 0};

    static double ListL[stepn];
    static double ListU[stepn];
    static int stepi;
    if(AIGame.GameFrame == 1)
    {
        double L1=L,U1=U;
        for(int i=0;i<stepn;i++){
            L1 += dL[i] * BLOCKSIDELENGTH;
            ListL[i] = L1;
            U1 += dU[i] * BLOCKSIDELENGTH;
            ListU[i] = U1;
        }
        stepi = 0;
        HumanMove(AIGame.human[0].SN, ListL[stepi], ListU[stepi]);
    }
    double fL = fabs(AIGame.human[0].L - ListL[stepi]);
    double fU = fabs(AIGame.human[0].U - ListU[stepi]);

    if(AIGame.GameFrame > 1 && (fL < 1.1  && fU < 1.1) && stepi<stepn - 1) {
        stepi++;
        HumanMove(AIGame.human[0].SN, ListL[stepi], ListU[stepi]);
    }

    // 第二个人尝试去(40, 31)的位置建立一个仓库
    static int StockFrame = 0; // 记录建造仓库时的帧数
    static int BuildingStockX = 40, BuildingStockY = 31; // 记录准备建造仓库的位置
    if(AIGame.Wood > BUILD_STOCK_WOOD && !tryBuild)
    {
        HumanBuild(AIGame.human[1].SN, BUILDING_STOCK, BuildingStockX, BuildingStockY);
        StockFrame = AIGame.GameFrame + 5;
    }
    // 如果5帧以后，人还没有进入到工作状态
    if(StockFrame == AIGame.GameFrame && (AIGame.human[1].NowState == HUMAN_STATE_IDLE || AIGame.human[1].NowState == HUMAN_STATE_STOP))
    {
        BuildingStockX ++;
        BuildingStockY ++;

        // 在坐标不超过地图的情况下，继续尝试在其他位置建造
        if(BuildingStockX >= 72 || BuildingStockY >= 72)
        {
            StockFrame = 0;
        }
        else {
            emit AIDebugText("AI获取到建造失败，尝试在(" + QString::number(BuildingStockX) + "," + QString::number(BuildingStockY) + ")处再次建立仓库");
            StockFrame = AIGame.GameFrame + 5;
            HumanBuild(AIGame.human[1].SN, BUILDING_STOCK, BuildingStockX, BuildingStockY);
        }
    }
    // 第2个村民建完建筑后，让他去砍最近的树
    static int Human2Action = 0;
    if(AIGame.building[1].Percent == 100 && Human2Action == 0) {
        // 计算出距离第2个村民最近的树的全局编号SN
        dis = 1e6;
        targetResSN = 0;
        for(int i = 0; i < AIGame.resource_n; i++) {
            if(AIGame.resource[i].Type == RESOURCE_TREE) {
                double tempDistance = calDistance(AIGame.human[1].L, AIGame.human[1].U, AIGame.resource[i].L, AIGame.resource[i].U);
                if(dis > tempDistance) {
                    dis = tempDistance;
                    targetResSN = AIGame.resource[i].SN;
                }
            }
        }
        if(targetResSN != 0) {
            int t = HumanAction(AIGame.human[1].SN, targetResSN);
            Human2Action = 1;
        }
    }

    // 第三个人尝试去(39, 39)的位置建立一个谷仓
    static int GranaryFrame = 0;
    static int BuildingGranaryX = 39, BuildingGranaryY = 39;
    if(AIGame.Wood >= BUILD_GRANARY_WOOD && !tryBuild) {
        HumanBuild(AIGame.human[2].SN, BUILDING_GRANARY, BuildingGranaryX, BuildingGranaryY);
        GranaryFrame = AIGame.GameFrame + 5;
    }
    // 如果5帧以后，人还没有进入到工作状态
    if(GranaryFrame == AIGame.GameFrame && (AIGame.human[2].NowState == HUMAN_STATE_IDLE || AIGame.human[2].NowState == HUMAN_STATE_STOP)) {
        BuildingGranaryX ++;
        BuildingGranaryY ++;

        // 在坐标不超过地图的情况下，继续尝试在其他位置建造
        if(BuildingGranaryX >= 72 || BuildingGranaryY >= 72) {
            GranaryFrame = 0;
        }
        else {
            emit AIDebugText("AI获取到建造失败，尝试在(" + QString::number(BuildingGranaryX) + "," + QString::number(BuildingGranaryY) + ")处再次建立粮仓");
            GranaryFrame = AIGame.GameFrame + 5;
            HumanBuild(AIGame.human[2].SN, BUILDING_GRANARY, BuildingGranaryX, BuildingGranaryY);
        }
    }
    // 第3个村民建完建筑后，让他去采集最近的浆果丛
    static int Human3Action = 0;
    if(AIGame.building[2].Percent == 100 && Human3Action == 0) {
        // 计算出距离第3个村民最近的树的全局编号SN
        dis = 1e6;
        targetResSN = 0;
        for(int i = 0; i < AIGame.resource_n; i++) {
            if(AIGame.resource[i].Type == RESOURCE_BUSH) {
                double tempDistance = calDistance(AIGame.human[2].L, AIGame.human[2].U, AIGame.resource[i].L, AIGame.resource[i].U);
                if(dis > tempDistance) {
                    dis = tempDistance;
                    targetResSN = AIGame.resource[i].SN;
                }
            }
        }
        if(targetResSN != 0) {
            int t = HumanAction(AIGame.human[2].SN, targetResSN);
            Human3Action = 1;
        }
    }

    if(AIGame.human_n >= 4) {
        // 让第四个村民去采集离他最近的浆果丛
        if(AIGame.human[3].NowState == HUMAN_STATE_IDLE) {
            // 计算出距离第4个村民最近的浆果丛的全局编号SN
            dis = 1e6;
            targetResSN = 0;
            for(int i = 0; i < AIGame.resource_n; i++) {
                if(AIGame.resource[i].Type == RESOURCE_BUSH) {
                    double tempDistance = calDistance(AIGame.human[3].L, AIGame.human[3].U, AIGame.resource[i].L, AIGame.resource[i].U);
                    if(dis > tempDistance) {
                        dis = tempDistance;
                        targetResSN = AIGame.resource[i].SN;
                    }
                }
            }
            if(targetResSN != 0) int t = HumanAction(AIGame.human[3].SN, targetResSN);
        }
    }

    if(!tryBuild) tryBuild = true; // 只尝试建造一次

    // 与主线程通讯

    ProcessDataWork = 0;
}
