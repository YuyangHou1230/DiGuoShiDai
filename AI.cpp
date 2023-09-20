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

bool hasBuilding(int type)
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

QPointF AI::calNextStep(tagHuman human, mapInfo myMap[72][72], bool zhuang)
{
    // 上下左右四个方向可走
    QPoint origin = QPoint(human.BlockL, human.BlockU);
    QPoint leftPos =  QPoint(-1, 0);
    QPoint rightPos = QPoint(1, 0);
    QPoint topPos = QPoint(0, 1);
    QPoint bottomPos = QPoint(0, -1);


    QList<QPoint> kk = QList<QPoint>() << leftPos << topPos << rightPos << bottomPos;

    static int lastIndex = 0;

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



    QList<int> notArriveIndex; // 未到达的点
    QList<int> realPosIndex;


    int range = 1;
//    while(notArriveIndex.isEmpty()){
//        realPosIndex.clear();
//        for(auto i : validIndex){
//            QPoint p  = kk[i];
//            int l = human.BlockL + p.x()*range;
//            int u = human.BlockU +p.y()*range;
//            if(l<1 || l >=72 || u < 1 || u>=72){
//                continue;
//            }
//            if(myMap[l][u].IsEmpty && !myMap[l][u].IsArrived){
//                notArriveIndex << i;
//            }

//            if(myMap[l][u].IsEmpty || (myMap[l][u].SN == human.SN)){
//                realPosIndex << i;
//            }
//        }
//        range++;
//    }

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

//    QPoint targetPos = kk[targetIndex];
//    lastIndex = targetIndex;

//    qDebug()   << "target" << targetPos;

//    QPointF re = QPointF(human.L + targetPos.x() * BLOCKSIDELENGTH, human.U + targetPos.y() * BLOCKSIDELENGTH);
//    if(re.rx() < 0 || re.ry() < 0){
//         qDebug() << re;
//    }


//    return re;
}

int AI::findResSN(tagHuman human, int resouce, int &index)
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

void AI::processData()
{
    ProcessDataWork = 1;
    ///构建地图
    static mapInfo myMap[72][72];
    static QMap<int, int> feixuList;
//    static QList<tagHuman*> humanList;
    //取资源
    for(int a=0;a < AIGame.resource_n;a++){
        myMap[AIGame.resource[a].BlockL][AIGame.resource[a].BlockU] = AIGame.resource[a];
        myMap[AIGame.resource[a].BlockL][AIGame.resource[a].BlockU].IsEmpty = false;
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
            }
        }
    }
    //取人
    for (int c=0;c < AIGame.human_n;c++) {
        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU] = AIGame.human[c];
//        humanList.append(&AIGame.human[c]);
        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU].IsEmpty = false;
//        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU].IsArrived = true;
    }

    if(feixuList.size() == 3){
        qDebug() <<"废墟标记完毕";
    }


    ///第一阶段
//    GameFirstStep()
//    mapInfo humanPathMap[5][5];

    tagHuman firstHuman = AIGame.human[0];
    int blockHumanX = AIGame.human[0].BlockL;
    int blockHumanY = AIGame.human[0].BlockU;
    static QPointF humanGoTo;

    if(AIGame.GameFrame == 0){
        humanGoTo = QPointF(-3*BLOCKSIDELENGTH,0*BLOCKSIDELENGTH) + QPointF(AIGame.human[0].L, AIGame.human[0].U);
//        qDebug()<<"humanGoTo ...111111"<<humanGoTo.x();
//        qDebug()<<"humanGoTo ...111111"<<humanGoTo.y();
        HumanMove(AIGame.human[0].SN,humanGoTo.rx(),humanGoTo.ry());
        ProcessDataWork = 0;
        return ;
    }

    const int step = 2;
    static QMap<int, int> pengzhuangPosMap;
    if(AIGame.human[0].NowState == HUMAN_STATE_STOP){ // 遇到障碍物
        //取视野地图5x5
        myMap[blockHumanX][blockHumanY].IsArrived = true;
        if(!pengzhuangPosMap.contains(blockHumanX)){
            pengzhuangPosMap[blockHumanX] = blockHumanY;
            humanGoTo = calNextStep(AIGame.human[0], myMap, true);

            qDebug()  << "碰撞" << humanGoTo;

            //记录即将要去的坐标
    //        humanGoTo.setX(firHumanX+newPos.x()*BLOCKSIDELENGTH);
    //        humanGoTo.setY(firHumanY+newPos.y()*BLOCKSIDELENGTH);
            HumanMove(AIGame.human[0].SN,humanGoTo.x(),humanGoTo.y());
        }
        else{


            QPointF temp = calNextStep(AIGame.human[0], myMap, true);
            auto pos = temp - (humanGoTo - temp); //有可能超出边界
             qDebug()  << "碰撞多次"  << pos << humanGoTo;
             humanGoTo = temp;

            HumanMove(AIGame.human[0].SN,humanGoTo.x(),humanGoTo.y()); // 手动移动
        }




    }else {
        // 判断是否到达目标点

        double fL = fabs(AIGame.human[0].L - humanGoTo.x());
        double fU = fabs(AIGame.human[0].U - humanGoTo.y());
        //            qDebug()<<"fL ..."<<fL;
        //            qDebug()<<"fU ..."<<fU;
        if((fL < 1.1  && fU < 1.1)){ //到达目标点则发送新的移动指令

            pengzhuangPosMap.clear(); //清空碰撞map
            //取视野地图5x5
            myMap[blockHumanX][blockHumanY].IsArrived = true;

            humanGoTo = calNextStep(AIGame.human[0],myMap);

            //记录即将要去的坐标
//            humanGoTo.setX(firHumanX+newPos.x()*BLOCKSIDELENGTH);
//            humanGoTo.setY(firHumanY+newPos.y()*BLOCKSIDELENGTH);
            HumanMove(firstHuman.SN,humanGoTo.x(),humanGoTo.y());
        }
    }


    ProcessDataWork = 0;
    return;


    // 采集浆果
    static int Human2Action = 0;
    static int bushIndex = -1;
    if(Human2Action == 0 || AIGame.human[1].NowState == HUMAN_STATE_IDLE){
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
