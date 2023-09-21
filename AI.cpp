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

// 计算一个村民视野边界随机位置
QPointF calHumanEdgePos(tagHuman man, mapInfo myMap[72][72])
{
    QList<QPoint> edgePos;
    edgePos << QPoint(-2, 3) << QPoint(-1, 3) << QPoint(0, 3) << QPoint(1, 3) << QPoint(1, 3);
    edgePos << QPoint(-3, -2) << QPoint(-3, -1) << QPoint(-3, 0) << QPoint(-3, 1) << QPoint(-3, 2);
    edgePos << QPoint(-2, -3) << QPoint(-1, -3) << QPoint(0, -3) << QPoint(1, -3) << QPoint(1, -3);
    edgePos << QPoint(3, -2) << QPoint(3, -1) << QPoint(3, 0) << QPoint(3, 1) << QPoint(3, 2);


    // 选出有效的点
    QList<QPoint> validPos;
    for(auto p : edgePos){
        int newL = man.BlockL + p.x();
        int newU = man.BlockU + p.y();
        if(newL < 2 || newL > 68 || newU < 2 || newU > 68){
            continue;
        }
        if(!myMap[newL][newU].IsEmpty){
            continue;
        }


        validPos << p;
    }

     QPoint targetPos;
    if(validPos.size() == 0){
        QPoint leftPos =  QPoint(-1, 0);
        QPoint rightPos = QPoint(1, 0);
        QPoint topPos = QPoint(0, 1);
        QPoint bottomPos = QPoint(0, -1);
        QList<QPoint> kk = QList<QPoint>() << leftPos << topPos << rightPos << bottomPos;

        QList<QPoint> pos;
        QList<QPoint> realPos;
        for(int i=0; i< 4; i++){
            int l = man.BlockL + kk[i].x();
            int u = man.BlockU +kk[i].y();

            pos << QPoint(l, u);

            if(l<1 || l >=72 || u < 1 || u>=72){
                continue;
            }
            if(!myMap[l][u].IsEmpty){
                continue;
            }
            realPos << kk[i];
        }

        if(realPos.size() == 0 ){
            qDebug() <<myMap[pos[0].x()][pos[0].y()].IsEmpty<<myMap[pos[0].x()][pos[0].y()].SN;
            qDebug() <<myMap[pos[1].x()][pos[1].y()].IsEmpty<<myMap[pos[1].x()][pos[1].y()].SN;
            qDebug() <<myMap[pos[2].x()][pos[2].y()].IsEmpty<<myMap[pos[2].x()][pos[2].y()].SN;
            qDebug() <<myMap[pos[3].x()][pos[3].y()].IsEmpty<<myMap[pos[3].x()][pos[3].y()].SN;
            qWarning() << "无路可走" ;

        }

        int index = QRandomGenerator::global()->bounded(0, validPos.size());
        targetPos = realPos[index];

    }
    else{
        int index = QRandomGenerator::global()->bounded(0, validPos.size());
        targetPos = validPos[index];

    }



    qDebug() << "Move:" << QPoint(man.BlockL, man.BlockU) + targetPos;
    QPointF re = QPointF(man.L + targetPos.x() * BLOCKSIDELENGTH, man.U + targetPos.y() * BLOCKSIDELENGTH);
    if(re.rx() < 0 || re.ry() < 0){
         qDebug() << re;
    }

    return re;
}
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
bool hasResource(int resouce)
{
    bool has = false;
    for(int i = 0; i < AIGame.resource_n; i++) {
        if(AIGame.resource[i].Type == resouce) {
            has = true;
            break;
        }
    }

    return has;
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
     mapInfo myMap[72][72];
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
    QList<tagBuilding*> buildingList;
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

        buildingList << &AIGame.building[b];
        mapInfo bulidInfo = AIGame.building[b];
        int  foundTmp = bulidInfo.Foundation;
        if(AIGame.building[b].Type == BUILDING_ARROWTOWERPOSITION ){ //标记废墟
            if(!feixuList.contains(AIGame.building[b].BlockL)){
                feixuList[AIGame.building[b].BlockL] =AIGame.building[b].BlockU;
            }
        }
        if(AIGame.building[b].Type == BUILDING_HOME){
            m_zhuzhaiList.append(&AIGame.building[b]);
        }
        //所占格都是Foundation+2
        for (int bi=0;bi < foundTmp+2;bi++) {
            for (int bj=0;bj < foundTmp+2;bj++) {
                myMap[AIGame.building[b].BlockL+bi][AIGame.building[b].BlockU+bj] = AIGame.building[b];
                myMap[AIGame.building[b].BlockL+bi][AIGame.building[b].BlockU+bj].IsEmpty = false;

                if(AIGame.building[b].Type == BUILDING_HOME){
            }
        }
    }
    //取人
    for (int c=0;c < AIGame.human_n;c++) {
        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU] = AIGame.human[c];
        if(!xunluMap.contains(AIGame.human[c].SN) && AIGame.human[c].Blood > 0){ // 没在探路中且血量大于0
            m_humansList.append(&AIGame.human[c]);
        }

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
    // 人物失效判断
    QList<int> wuxiaoSN;
    QMap<int, FoundRoad>::iterator it;
    for(it = xunluMap.begin(); it != xunluMap.end(); ++it){
        auto p = it.value();
        if(it.value().man->Blood == 0 || p.man->NowState== HUMAN_STATE_STOP || (p.man->NowState != HUMAN_STATE_IDLE )){
            wuxiaoSN << it.key();
        }
        int blockHumanX = p.man->BlockL;
        int blockHumanY = p.man->BlockU;
        if(p.man->NowState== HUMAN_STATE_STOP){// 遇到障碍物

            myMap[blockHumanX][blockHumanY].IsArrived = true;

            int index = -1;
            int bushSN = findResSN(*p.man, RESOURCE_TREE,index);
            if(bushSN != 0){
                HumanAction(p.man->SN, bushSN);
//                xunluMap.remove(p.man->SN);
            }
        }
    }
    for(auto p : wuxiaoSN){
        xunluMap.remove(p);
    }
    if(xunluMap.size() < 2){
        for(int i=0; i<2 - xunluMap.size(); i++){
            for(auto p : m_humansList){
                if(!xunluMap.contains(p->SN)){
                    humanGoTo = QPointF(-3*BLOCKSIDELENGTH,0*BLOCKSIDELENGTH) + QPointF(p->L, p->U);
                    HumanMove(p->SN,humanGoTo.rx(),humanGoTo.ry());
                    FoundRoad f{p->SN, 0, p, humanGoTo};
                    xunluMap.insert(p->SN, f);
                    findRoadHumanNum++;
                    break;
                }
            }
        }
    }

    // 人物失效判断
//    QMap<int, FoundRoad>::iterator it;
    for(it = xunluMap.begin(); it != xunluMap.end(); ++it){
        if(it.value().man->Blood == 0){
            continue;
        }
        FoundRoad& p = it.value();

        int blockHumanX = p.man->BlockL;
        int blockHumanY = p.man->BlockU;
        if(p.man->NowState== HUMAN_STATE_STOP){// 遇到障碍物

//            myMap[blockHumanX][blockHumanY].IsArrived = true;

//            int index = -1;
//            int bushSN = findResSN(*p.man, RESOURCE_TREE,index);
//            if(bushSN != 0){
//                HumanAction(p.man->SN, bushSN);
//                xunluMap.remove(p.man->SN);
//            }
//            //取视野地图5x5

//            p.targetPos = calHumanEdgePos(*p.man, myMap);

//            qDebug()  << "碰撞" << humanGoTo;

            //记录即将要去的坐标
//            HumanMove(p.man->SN,humanGoTo.x(),humanGoTo.y());
        }
        else if(p.man->NowState >=1 && p.man->NowState <= 10){
            //取视野地图5x5
//            myMap[blockHumanX][blockHumanY].IsArrived = true;
//            p.targetPos = calNextStep(p.man->SN, myMap, true);

//            qDebug()  << "碰撞" << humanGoTo;

//            //记录即将要去的坐标
//            HumanMove(p.man->SN,p.targetPos.x(),p.targetPos.y());
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

//                p.targetPos = calNextStep(p.man->SN,myMap,false);
                p.targetPos = calHumanEdgePos(*p.man, myMap);

                //记录即将要去的坐标
                HumanMove(p.man->SN,p.targetPos.x(),p.targetPos.y());
            }
        }
    }



    // 农民分类
    QList<tagHuman*> emptyMans;
    QList<tagHuman*> shuList;
    QList<tagHuman*> guoList;
    QList<tagHuman*> shiList;
    for(auto p :m_humansList){
        switch (p->NowState) {
            case HUMAN_STATE_IDLE:
                emptyMans << p;
                break;
            case HUMAN_STATE_CUTTING:
                shuList << p;
                break;
            case HUMAN_STATE_DIGGING_STONE:
                shiList << p;
                break;
            case HUMAN_STATE_GATHERING:
                guoList << p;
                break;
            case HUMAN_STATE_FARMING:
                break;
        }
    }



    // 建造谷仓
    static bool isBuildingGucang = false;
    bool hasGucang = hasBuilding(BUILDING_GRANARY);
    static int times = 0;
    if(isBuildingGucang && !hasGucang){
        isBuildingGucang = false;
    }
    static bool gucang = false;
    if(hasGucang){
        for(auto p : buildingList){
            if(p->Type == BUILDING_GRANARY && p->Percent == 100){
                gucang = true;
            }
        }
    }

    // 建造仓库
    static bool isBuildingCangku = false;
    bool hasCangku = hasBuilding(BUILDING_STOCK);
    static int Cangkutimes = 0;
    if(isBuildingCangku && !hasCangku){
        isBuildingCangku = false;
    }
    static bool cangku = false;
    if(hasCangku){
        for(auto p : buildingList){
            if(p->Type == BUILDING_STOCK && p->Percent == 100){
                cangku = true;
                break;
            }
        }
    }

    //优先升级时代
    if(AIGame.civilizationStage == CIVILIZATION_STONEAGE && gucang && cangku &&
            AIGame.building[0].Project == BUILDING_FREE && AIGame.Meat >= BUILDING_CENTER_UPGRADE_FOOD){
        BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_UPGRADE);
    }

    if(!hasGucang && hasResource(RESOURCE_BUSH) && AIGame.human[AIGame.human_n - 1].NowState != HUMAN_STATE_BUILDING){ // 并且已经找到果子, 在果子旁边
       // 计算果子的位置
        int index = ((times %2) == 0) ? 1 : -1;
        int buildingX = m_bushList[0]->BlockL + index * ( 2 * (times /2 +1));
        int buildingY = m_bushList[0]->BlockU + index * ( 2 * (times /2 +1));

        HumanBuild(AIGame.human[AIGame.human_n - 1].SN, BUILDING_GRANARY, buildingX, buildingY);
        isBuildingGucang = true;
        times += 1;
        if(times > 10){
            times = 0;
        }

        emptyMans.removeOne(&AIGame.human[AIGame.human_n - 1]);

    }

    // 生产农民
    if((xunluMap.size() + m_humansList.size()) < AIGame.Human_MaxNum){
        if(AIGame.building[0].Project == BUILDING_FREE){
            BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_CREATEFARMER);
        }
    }

    if(!hasCangku && AIGame.human_n > 4 && AIGame.human[4].NowState != HUMAN_STATE_BUILDING){ // 并且已经找到果子, 在果子旁边
        // 计算果子的位置
        int index = ((Cangkutimes %2) == 0) ? 1 : -1;
        int buildingX = 33 +  -1 *Cangkutimes ;
        int buildingY = 35 ;

        HumanBuild(AIGame.human[4].SN, BUILDING_STOCK, buildingX, buildingY);
        Cangkutimes += 1;
        isBuildingCangku = true;

        if(Cangkutimes > 10){
            Cangkutimes = 0;
        }

        emptyMans.removeOne(&AIGame.human[4]);

    }

    // 建造房屋
    static bool isBuildingHome = false;
    static int size = 0;
    if(size == m_zhuzhaiList.size()){
        isBuildingHome = false;
    }
    if(isBuildingHome && m_zhuzhaiList.last()->Percent == 100){
        isBuildingHome = false;
    }
    if( !isBuildingHome && m_zhuzhaiList.size() < 5 && hasCangku && hasGucang && AIGame.human[AIGame.human_n - 1].NowState != HUMAN_STATE_BUILDING){

        bool notFind = true;
        int x, y;
        while(notFind){
             x = QRandomGenerator::global()->bounded(0, 72);
             y = QRandomGenerator::global()->bounded(0, 72);

            bool notEmpty = false;
            if(!myMap[x][y].IsEmpty){
                notEmpty = true;
            }
            if(!myMap[x+1][y].IsEmpty){
                notEmpty = true;
            }
            if(!myMap[x][y+1].IsEmpty){
                notEmpty = true;
            }
            if(!myMap[x+1][y+1].IsEmpty){
                notEmpty = true;
            }
            if(!notEmpty){
                notFind = false;
            }

        }

        HumanBuild(AIGame.human[AIGame.human_n - 1].SN, BUILDING_STOCK, x, y);
        emptyMans.removeOne(&AIGame.human[AIGame.human_n - 1]);
        isBuildingHome = true;
        size++;
    }

    // 空闲分配
    if(emptyMans.size() > 0){
        for(auto p : emptyMans){
            if(guoList.isEmpty()){
                int index = -1;
                int bushSN = findResSN(*p, RESOURCE_BUSH,index);
                if(bushSN != 0){
                    HumanAction(p->SN, bushSN);
                    bushHumanNum +=1;
                }
            }
            else if(shuList.isEmpty()){
                int index = -1;
                int bushSN = findResSN(*p, RESOURCE_TREE,index);
                if(bushSN != 0){
                    HumanAction(p->SN, bushSN);
                    woodHumanNum +=1;
                }

            }
            else{
                if(AIGame.civilizationStage == CIVILIZATION_STONEAGE){
                    if(woodHumanNum < bushHumanNum){
                        int index = -1;
                        int bushSN = findResSN(*p, RESOURCE_TREE,index);
                        if(bushSN != 0){
                            HumanAction(p->SN, bushSN);
                            woodHumanNum +=1;
                        }
                    }
                    else{
                        int index = -1;
                        int bushSN = findResSN(*p, RESOURCE_BUSH,index);
                        if(bushSN != 0){
                            HumanAction(p->SN, bushSN);
                            bushHumanNum +=1;
                        }
                    }

                }
                else{
                    if(shiList.isEmpty() && hasResource(RESOURCE_STONE) && AIGame.civilizationStage == CIVILIZATION_TOOLAGE){
                        int index = -1;
                        int bushSN = findResSN(*p, RESOURCE_STONE,index);
                        if(bushSN != 0){
                            HumanAction(p->SN, bushSN);
                            stoneHumanNum+=1;
                        }
                    }
                }
            }

        }
    }







    ProcessDataWork = 0;
    return;

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

//            if()
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



    ProcessDataWork = 0;
    return;

}
