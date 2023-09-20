#include "AI.h"
#include "config.h"
#include "MainWidget.h"
#include <Qdebug>
#include <Windows.h>
#include <iostream>

struct FoundRoad
{
    int SN;
    int lastIndex;
    tagHuman *man;
    QPointF targetPos;
};

static QMap<int, FoundRoad> xunluMap;


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


//using namespace My_Struct;
//下一步
QPointF calNextStep(int Sn, mapInfo myMap[72][72],  bool zhuang, int celue=1 ) // 策略0 转向  策略1 随机
{
//    if(!xunluMap.contains(Sn)){
//        return
//    }
    tagHuman human = *xunluMap[Sn].man;
    FoundRoad f = xunluMap[Sn];
    // 上下左右四个方向可走
    QPoint origin = QPoint(human.BlockL, human.BlockU);
    QPoint leftPos =  QPoint(-1, 0);
    QPoint rightPos = QPoint(1, 0);
    QPoint topPos = QPoint(0, 1);
    QPoint bottomPos = QPoint(0, -1);


    QList<QPoint> kk = QList<QPoint>() << leftPos << topPos << rightPos << bottomPos;

    int& lastIndex = xunluMap[Sn].lastIndex;

    // 有效的方向
    QList<int> validIndex;
    for(int i=0; i< 4; i++){
        int l = human.BlockL + kk[i].x();
        int u = human.BlockU +kk[i].y();
        if(l<1 || l >=72 || u < 1 || u>=72){
            continue;
        }
        if(zhuang && i == lastIndex){
            continue;  //上次撞了的索引丢弃
        }
        if(myMap[ human.BlockL +3*kk[i].x()][ human.BlockU + 3*kk[i].y()].IsArrived){
            continue;
        }
        validIndex << i;
    }

    if(celue == 0){

        if(zhuang){
            int i = 1;
            int inds = (lastIndex +i) %4;
            while(!validIndex.contains(inds)){
                i++;
                inds = (lastIndex +i) %4;
            }
            lastIndex = inds;

        }
        else if(!validIndex.contains(lastIndex)){
            lastIndex = (lastIndex + 1) %4;
        }

        QPoint targetPos = kk[lastIndex];


        qDebug()   << "target" << targetPos;

        QPointF re = QPointF(human.L + targetPos.x() * BLOCKSIDELENGTH, human.U + targetPos.y() * BLOCKSIDELENGTH);
        if(re.rx() < 0 || re.ry() < 0){
             qDebug() << re;
        }


        return re;

    }



    QList<int> notArriveIndex; // 未到达的点
    QList<int> realPosIndex;


    int range = 1;
    while(notArriveIndex.isEmpty() || range < 5){
        realPosIndex.clear();
        for(auto i : validIndex){
            QPoint p  = kk[i];
            int l = human.BlockL + p.x()*range;
            int u = human.BlockU +p.y()*range;
            if(l<1 || l >=72 || u < 1 || u>=72){
                continue;
            }
            if(myMap[l][u].IsEmpty && !myMap[l][u].IsArrived){
                notArriveIndex << i;
            }

            if(myMap[l][u].IsEmpty || (myMap[l][u].SN == human.SN)){
                realPosIndex << i;
            }
        }
        range++;
    }

    for(auto i : validIndex){
        QPoint p  = kk[i];
        int l = human.BlockL + p.x()*range;
        int u = human.BlockU +p.y()*range;
        if(l<1 || l >=72 || u < 1 || u>=72){
            continue;
        }
        if(myMap[l][u].IsEmpty && !myMap[l][u].IsArrived){
            notArriveIndex << i;
        }

        if(myMap[l][u].IsEmpty || (myMap[l][u].SN == human.SN)){
            realPosIndex << i;
        }
    }

     int targetIndex;
/*    int random = QRandomGenerator::global()->bounded(0, 2);
    if(!random){
        targetIndex = realPosIndex[0];
    }
    else */if(notArriveIndex.size() == 1){
        targetIndex = notArriveIndex[0];
    }
    else if(notArriveIndex.size() > 1){
        int index = QRandomGenerator::global()->bounded(0, notArriveIndex.size());
        targetIndex = notArriveIndex[index];
    }
    else if(realPosIndex.size() == 1){
            targetIndex = realPosIndex[0];
    }
    else if(realPosIndex.size() > 1){
        int index = QRandomGenerator::global()->bounded(0, realPosIndex.size());
        targetIndex = realPosIndex[index];
    }
    else{
        int index = QRandomGenerator::global()->bounded(0, validIndex.size());
        targetIndex = validIndex[index];

        qDebug() << "找不到可移动的点";
    }

     if(zhuang && targetIndex == lastIndex){
         qDebug()   << "重复碰撞" ;
     }

    QPoint targetPos = kk[targetIndex];
    lastIndex = targetIndex;

    qDebug()   << "target" << targetPos;

    QPointF re = QPointF(human.L + targetPos.x() * BLOCKSIDELENGTH, human.U + targetPos.y() * BLOCKSIDELENGTH);
    if(re.rx() < 0 || re.ry() < 0){
         qDebug() << re;
    }


    return re;
}

double calDistance1(double L0, double U0, double TL0, double TU0)
{
    return pow(pow(TL0 - L0, 2) + pow(TU0 - U0, 2), 0.5);
}
//找资源的SN
int findResSN(tagHuman human, int resouce, int &index)
{
    int dis = 1e6;
    int targetResSN = 0;

    for(int i = 0; i < AIGame.resource_n; i++) {
        if(AIGame.resource[i].Type == resouce) {
            double tempDistance = calDistance1(human.L, human.U, AIGame.resource[i].L, AIGame.resource[i].U);
            if(dis > tempDistance) {
                dis = tempDistance;
                index = i;
                targetResSN = AIGame.resource[i].SN;
            }
        }
    }

    return targetResSN;
}

int HumanBuild1(int SN, int BuildingNum, int BlockL, int BlockU)
{
    tagAction HumanBuild;
    HumanBuild.A = 3;

    // SN判断
    bool judgeHumanSN = false;
    for(int i = 0; i < AIGame.human_n; i++)
    {
        if(AIGame.human[i].SN == SN)
        {
            judgeHumanSN = true;
            break;
        }
    }
    if(judgeHumanSN == false) return ACTION_INVALID_SN;
    HumanBuild.SN = SN;

    // BuildingNum判断
    bool judgeBuildingNum = false;
    if(BuildingNum != 0 && BuildingNum != 1 && BuildingNum != 3 && BuildingNum != 4 && BuildingNum != 5 && BuildingNum != 6)
        return ACTION_INVALID_BUILDINGNUM;
    HumanBuild.BuildingNum = BuildingNum;

    if(BlockL > 72 || BlockL < 0) return ACTION_INVALID_LOCATION;
    HumanBuild.BlockL = BlockL;
    if(BlockU > 72 || BlockU < 0) return ACTION_INVALID_LOCATION;
    HumanBuild.BlockU = BlockU;

    g_AiAction[SN] = HumanBuild;
    return ACTION_SUCCESS;
}

//在某个位置修房屋
void createBuliding(tagHuman* human, int BuildingNum, int BlockL, int BlockU){
    static int StockFrame = 0; // 记录建造房屋时的帧数
    static int BuildingStockX = BlockL, BuildingStockY = BlockU; // 记录准备建造仓库的位置
    if(AIGame.Wood > BUILDING_HOME && human->NowState != HUMAN_STATE_BUILDING)
    {

        HumanBuild1(human->SN, BuildingNum, BuildingStockX, BuildingStockY);
        StockFrame = AIGame.GameFrame + 5;
    }
    // 如果5帧以后，人还没有进入到工作状态，则换一个位置建造
    if(StockFrame == AIGame.GameFrame && (human->NowState == HUMAN_STATE_IDLE || human->NowState == HUMAN_STATE_STOP))
    {
        BuildingStockX ++;
        BuildingStockY ++;
        // 在坐标不超过地图的情况下，继续尝试在其他位置建造
        if(BuildingStockX >= 72 || BuildingStockY >= 72) StockFrame = 0;
        else {
//            emit AIDebugText("AI获取到建造失败，尝试在(" + QString::number(BuildingStockX) + "," + QString::number(BuildingStockY) + ")处再次建立仓库");
            StockFrame = AIGame.GameFrame + 5;
            HumanBuild1(human->SN, BuildingNum, BuildingStockX, BuildingStockY);
        }
    }
}

//using namespace My_Function;
AI::AI(QObject *parent)
    : QObject(parent)
{

}


//////**************************************游戏开始***********************************************************
void AI::processData()
{

    ProcessDataWork = 1;
    ///构建地图
    static mapInfo myMap[72][72];
    static QMap<int, int> feixuList;
    QList<tagHuman*> m_humansList;
    static int findRoadHumanNum = 0;
    int bushHumanNum = 0;//采集人数
    int stoneHumanNum = 0;
    int woodHumanNum = 0;

    QList<tagResource*> m_bushList;
    QList<tagResource*> m_woodList;
    QList<tagResource*> m_stoneList;
    QList<tagBuilding*> m_zhuzhaiList;
//    static QList<tagBuilding*> m_gucangList;
//    static QList<tagBuilding*> m_List;


    //取资源
    for(int a=0;a < AIGame.resource_n;a++){
        myMap[AIGame.resource[a].BlockL][AIGame.resource[a].BlockU] = AIGame.resource[a];
        myMap[AIGame.resource[a].BlockL][AIGame.resource[a].BlockU].IsEmpty = false;
        switch (AIGame.resource[a].Type) {
        case RESOURCE_BUSH:m_bushList.append(&AIGame.resource[a]);break;
        case RESOURCE_TREE:m_woodList.append(&AIGame.resource[a]);break;
        case RESOURCE_STONE:m_stoneList.append(&AIGame.resource[a]);break;
        case RESOURCE_GAZELLE:break;
        case RESOURCE_ELEPHANT:break;
        case RESOURCE_LION:break;
        }
    }

    //取建筑
    for (int b=0;b < AIGame.building_n ;b++) {
        mapInfo bulidInfo = AIGame.building[b];
        int  foundTmp = bulidInfo.Foundation;
        if(AIGame.building[b].Type == BUILDING_ARROWTOWERPOSITION ){ //标记废墟
            if(!feixuList.contains(AIGame.building[b].BlockL)){
                feixuList[AIGame.building[b].BlockL] =AIGame.building[b].BlockU;
            }
        }
        //所占格都是Foundation+2
        for (int bi=0;bi < foundTmp+2;bi++) {
            for (int bj=0;bj < foundTmp+2;bj++) {
                myMap[AIGame.building[b].BlockL+bi][AIGame.building[b].BlockU+bj] = AIGame.building[b];
                myMap[AIGame.building[b].BlockL+bi][AIGame.building[b].BlockU+bj].IsEmpty = false;
                if(AIGame.building[b].Type == BUILDING_HOME){
                    m_zhuzhaiList.append(&AIGame.building[b]);
                }
            }
        }
    }
    //取人
    for (int c=0;c < AIGame.human_n;c++) {
        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU] = AIGame.human[c];
        m_humansList.append(&AIGame.human[c]);
        switch (AIGame.human[c].NowState) {
        case HUMAN_STATE_CUTTING:bushHumanNum++;break;
        case HUMAN_STATE_DIGGING_STONE:stoneHumanNum++;break;
        case HUMAN_STATE_GATHERING:woodHumanNum++;break;
        case HUMAN_STATE_FARMING:break;
        }
        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU].IsEmpty = false;
        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU].IsArrived = true;
    }

    if(feixuList.size() == 3){
        qDebug() <<"废墟标记完毕";
    }

    // 生产农民
    if(AIGame.human_n < AIGame.Human_MaxNum && AIGame.human_n <= 24 && AIGame.Meat >= 50){
        if(AIGame.building[0].Project == BUILDING_FREE){
            BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_CREATEFARMER);
        }
    }

    ///第一阶段
//    GameFirstStep()
//    mapInfo humanPathMap[5][5];


    static QPointF humanGoTo;

    if(AIGame.GameFrame == 0){
        //前俩个人去寻路
        humanGoTo = QPointF(-3*BLOCKSIDELENGTH,0*BLOCKSIDELENGTH) + QPointF(m_humansList[0]->L, m_humansList[0]->U);
        HumanMove(m_humansList[0]->SN,humanGoTo.rx(),humanGoTo.ry());
        FoundRoad f{m_humansList[0]->SN, 0, m_humansList[0], humanGoTo};
        xunluMap.insert(m_humansList[0]->SN, f);
        findRoadHumanNum++;

        humanGoTo = QPointF(3*BLOCKSIDELENGTH,0*BLOCKSIDELENGTH) + QPointF(m_humansList[1]->L, m_humansList[1]->U);
        HumanMove(m_humansList[1]->SN,humanGoTo.rx(),humanGoTo.ry());
        FoundRoad f2{m_humansList[1]->SN, 0, m_humansList[1], humanGoTo};
        xunluMap.insert(m_humansList[1]->SN, f2);
        findRoadHumanNum++;

        // 第三个人尝试去(40, 31)的位置建立一个房屋

        static int StockFrame = 0; // 记录建造房屋时的帧数
        static int BuildingStockX = 40, BuildingStockY = 31; // 记录准备建造仓库的位置
        if(AIGame.Wood > BUILDING_HOME && m_humansList[2]->NowState != HUMAN_STATE_BUILDING)
        {
            HumanBuild(m_humansList[2]->SN, BUILDING_HOME, BuildingStockX, BuildingStockY);
            StockFrame = AIGame.GameFrame + 5;
        }
        // 如果5帧以后，人还没有进入到工作状态，则换一个位置建造
        if(StockFrame == AIGame.GameFrame && (m_humansList[2]->NowState == HUMAN_STATE_IDLE || m_humansList[2]->NowState == HUMAN_STATE_STOP))
        {
            BuildingStockX ++;
            BuildingStockY ++;
            // 在坐标不超过地图的情况下，继续尝试在其他位置建造
            if(BuildingStockX >= 72 || BuildingStockY >= 72) StockFrame = 0;
            else {
                emit AIDebugText("AI获取到建造失败，尝试在(" + QString::number(BuildingStockX) + "," + QString::number(BuildingStockY) + ")处再次建立仓库");
                StockFrame = AIGame.GameFrame + 5;
                HumanBuild(m_humansList[2]->SN, BUILDING_HOME, BuildingStockX, BuildingStockY);
            }
        }
        ProcessDataWork = 0;
        return ;
    }


    QMap<int, FoundRoad>::iterator it;
    for(it = xunluMap.begin(); it != xunluMap.end(); ++it){
        FoundRoad& p = it.value();

        int blockHumanX = p.man->BlockL;
        int blockHumanY = p.man->BlockU;
        if(p.man->NowState== HUMAN_STATE_STOP){// 遇到障碍物
            //取视野地图5x5
            myMap[blockHumanX][blockHumanY].IsArrived = true;
            p.targetPos = calNextStep(p.man->SN, myMap, true);

            qDebug()  << "碰撞" << humanGoTo;

            //记录即将要去的坐标
            HumanMove(p.man->SN,humanGoTo.x(),humanGoTo.y());
        }
        else if(p.man->NowState >=1 && p.man->NowState <= 10){
            //取视野地图5x5
            myMap[blockHumanX][blockHumanY].IsArrived = true;
            p.targetPos = calNextStep(p.man->SN, myMap, true);

            qDebug()  << "碰撞" << humanGoTo;

            //记录即将要去的坐标
            HumanMove(p.man->SN,p.targetPos.x(),p.targetPos.y());
        }
        else{
            // 判断是否到达目标点

            double fL = fabs(p.man->L - p.targetPos.x());
            double fU = fabs(p.man->U - p.targetPos.y());
            //            qDebug()<<"fL ..."<<fL;
            //            qDebug()<<"fU ..."<<fU;
            if((fL < 1.1  && fU < 1.1)){ //到达目标点则发送新的移动指令

//                pengzhuangPosMap.clear(); //清空碰撞map
                //取视野地图5x5
                myMap[blockHumanX][blockHumanY].IsArrived = true;

                p.targetPos = calNextStep(p.man->SN,myMap,false);

                //记录即将要去的坐标
                HumanMove(p.man->SN,p.targetPos.x(),p.targetPos.y());
            }
        }
    }


    //人物策略
    for (auto i :m_humansList) {
        if(i->SN == AIGame.human[0].SN ||i->SN == AIGame.human[1].SN){
            //跳过前两个寻路的人
            continue;
        }

        if(i->NowState == HUMAN_STATE_IDLE && i->Blood >= 0){
            //不少于2个派人去寻路
//            if(findRoadHumanNum <= 2){

//            }

            //不少于2个人去找果子,生产农民,需判断当前资源是否有剩余，如无iterate()+1
            if(bushHumanNum <= 2){
                for (auto a : m_bushList) {
                    if(a->Cnt <= 5){continue;}
                    HumanAction(i->SN,a->SN);
                    bushHumanNum++;
                }
            }

            //不少于3个人去伐木,因为建筑需要木头
            if(woodHumanNum <= 3){
                for (auto b :m_woodList) {
                    if(b->Cnt <= 2){continue;}
                    HumanAction(i->SN,b->SN);
                    woodHumanNum++;
                }
            }

            //建造建筑
            if(m_zhuzhaiList.size() <= 4){
                int bulidBloL = 32;//AIGame.building[0].BlockL+2;
                int bulidBloU = 32;//AIGame.building[0].BlockU+2;
                createBuliding(i,BUILDING_HOME,bulidBloL,bulidBloU);
            }

            if()
        }



    }

    ProcessDataWork = 0;
    return;

    ///第二阶段
    // 采集浆果 采浆果3个人 暂定为 4 5 6

    for(int ii=0;ii<3;ii++){//采集最近的3组浆果
        tagHuman* human = m_humansList[ii];
        int snH = human->SN;
        int snA = m_bushList[ii]->SN;
        HumanAction(snH,snA);
    }

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
    if(AIGame.human[2].NowState == HUMAN_STATE_IDLE){
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

    ProcessDataWork = 0;
    return;

}
