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



//    qDebug() << "Move:" << QPoint(man.BlockL, man.BlockU) + targetPos;
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
void createBuliding(tagHuman* human, int BuildingNum,int BuildingRes, int BlockL, int BlockU){
    static int StockFrame = 0; // 记录建造房屋时的帧数
    static int BuildingStockX1 = AIGame.building[0].BlockL+QRandomGenerator::global()->bounded(-8, 8), BuildingStockY1 = AIGame.building[0].BlockU+QRandomGenerator::global()->bounded(-8, 8); // 记录准备建造仓库的位置
    if(AIGame.Wood > BuildingRes && human->NowState != HUMAN_STATE_BUILDING)
    {

        HumanBuild1(human->SN, BuildingNum, BuildingStockX1, BuildingStockY1);
        StockFrame = AIGame.GameFrame + 5;
    }
    // 如果5帧以后，人还没有进入到工作状态，则换一个位置建造
    if(StockFrame == AIGame.GameFrame && (human->NowState == HUMAN_STATE_IDLE || human->NowState == HUMAN_STATE_STOP|| human->NowState == HUMAN_STATE_STOP))
    {
        BuildingStockX1 ++;
        BuildingStockY1 ++;
        // 在坐标不超过地图的情况下，继续尝试在其他位置建造
        if(BuildingStockX1 >= 72 || BuildingStockY1 >= 72) {
            StockFrame = 0;
            BuildingStockX1 = AIGame.building[0].BlockL+QRandomGenerator::global()->bounded(-8, 8);
            BuildingStockY1 = AIGame.building[0].BlockU+QRandomGenerator::global()->bounded(-8, 8);
        }else {
            //            emit AIDebugText("AI获取到建造失败，尝试在(" + QString::number(BuildingStockX) + "," + QString::number(BuildingStockY) + ")处再次建立仓库");
            StockFrame = AIGame.GameFrame + 5;
            HumanBuild1(human->SN, BuildingNum, BuildingStockX1, BuildingStockY1);
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
//    QList<tagBuilding*> feixuList ;
    QList<tagHuman*> m_humansList;
    static int findRoadHumanNum = 0;
    int homeNumber =AIGame.Human_MaxNum/4-1;


    QList<tagHuman*> emptyMans;
    QList<tagHuman*> shuList;//树
    QList<tagHuman*> guoList;//果子
    QList<tagHuman*> shiList;//石头
    QList<tagHuman*> bulidList;//建造

    QList<tagResource*> m_bushList;
    QList<tagResource*> m_woodList;
    QList<tagResource*> m_stoneList;
    QList<tagResource*> m_animalList;
    static bool jiantaTech = false;
    static int gucangSN = 0;
    static int shichangSN = 0;
    bool isFinishedMarket = false;
    static int cangkuSN = 0;
    QList<tagBuilding*> m_zhuzhaiList;
    QList<tagBuilding*> buildingList;
    QList<tagBuilding*> farmList ;
    tagBuilding* gucangBuilding = nullptr ;
    QList<tagBuilding*> jiantaList ;
    //    static QList<tagBuilding*> m_gucangList;
    //    static QList<tagBuilding*> m_List;

    static QPointF humanGoTo;

    if(AIGame.GameFrame == 0){
        //前俩个人去寻路

        humanGoTo = QPointF(-3*BLOCKSIDELENGTH,0*BLOCKSIDELENGTH) + QPointF(AIGame.human[0].L, AIGame.human[0].U);
        HumanMove(AIGame.human[0].SN,humanGoTo.rx(),humanGoTo.ry());
        FoundRoad f{AIGame.human[0].SN, 0, &AIGame.human[0], humanGoTo};
        xunluMap.insert(AIGame.human[0].SN, f);
        findRoadHumanNum++;

        humanGoTo = QPointF(3*BLOCKSIDELENGTH,0*BLOCKSIDELENGTH) + QPointF(AIGame.human[1].L, AIGame.human[1].U);
        HumanMove(AIGame.human[1].SN,humanGoTo.rx(),humanGoTo.ry());
        FoundRoad f2{AIGame.human[1].SN, 0, &AIGame.human[1], humanGoTo};
        xunluMap.insert(AIGame.human[1].SN, f2);
        findRoadHumanNum++;

        // 生产农民
        if(AIGame.human_n < AIGame.Human_MaxNum && AIGame.human_n <= 24 && AIGame.Meat >= 50){
            if(AIGame.building[0].Project == BUILDING_FREE){
                BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_CREATEFARMER);
            }
        }

        // 第三个人尝试去(38,32)的位置建立一个房屋

        bool notFind = true;
        int x = 38, y = 32;
        while(notFind){
//            x = QRandomGenerator::global()->bounded(25, 55);
//            y = QRandomGenerator::global()->bounded(25, 55);

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

        HumanBuild(AIGame.human[2].SN, BUILDING_HOME, x, y);

//        int StockFrame = 0; // 记录建造房屋时的帧数
//        int BuildingStockX = 38, BuildingStockY = 32; // 记录准备建造仓库的位置
//        if(AIGame.Wood > BUILD_HOUSE_WOOD && AIGame.human[2].NowState != HUMAN_STATE_BUILDING)
//        {
//            HumanBuild(AIGame.human[2].SN, BUILDING_HOME, BuildingStockX, BuildingStockY);
//            StockFrame = AIGame.GameFrame + 10;
//        }
//        // 如果10帧以后，人还没有进入到工作状态，则换一个位置建造
//        if(StockFrame == AIGame.GameFrame && (AIGame.human[2].NowState == HUMAN_STATE_IDLE || AIGame.human[2].NowState == HUMAN_STATE_STOP || AIGame.human[2].NowState == HUMAN_STATE_STOP))
//        {
//            BuildingStockX ++;
//            BuildingStockY ++;
//            // 在坐标不超过地图的情况下，继续尝试在其他位置建造
//            if(BuildingStockX >= 72 || BuildingStockY >= 72) StockFrame = 0;
//            else {
//                emit AIDebugText("AI获取到建造失败，尝试在(" + QString::number(BuildingStockX) + "," + QString::number(BuildingStockY) + ")处再次建立仓库");
//                StockFrame = AIGame.GameFrame + 10;
//                HumanBuild(AIGame.human[2].SN, BUILDING_HOME, BuildingStockX, BuildingStockY);
//            }
//        }
        ProcessDataWork = 0;
        return ;
    }


    //取资源
    for(int a=0;a < AIGame.resource_n;a++){
        myMap[AIGame.resource[a].BlockL][AIGame.resource[a].BlockU] = AIGame.resource[a];
        myMap[AIGame.resource[a].BlockL][AIGame.resource[a].BlockU].IsEmpty = false;
        switch (AIGame.resource[a].Type) {
        case RESOURCE_BUSH:m_bushList.append(&AIGame.resource[a]);break;
        case RESOURCE_TREE:m_woodList.append(&AIGame.resource[a]);break;
        case RESOURCE_STONE:m_stoneList.append(&AIGame.resource[a]);break;
        case RESOURCE_GAZELLE:m_animalList.append(&AIGame.resource[a]);break;
        case RESOURCE_ELEPHANT:m_animalList.append(&AIGame.resource[a]);break;
        case RESOURCE_LION:m_animalList.append(&AIGame.resource[a]);break;
        }
    }

    //取建筑
    for (int b=0; b < AIGame.building_n; b++) {

        buildingList << &AIGame.building[b];
        mapInfo bulidInfo = AIGame.building[b];
        int  foundTmp = bulidInfo.Foundation;
        if(AIGame.building[b].Type == BUILDING_ARROWTOWERPOSITION ){ //标记废墟
            if(!feixuList.contains(AIGame.building[b].BlockL)){
                feixuList[AIGame.building[b].BlockL] =AIGame.building[b].BlockU;
            }
//            feixuList.append(&AIGame.building[b]);
        }

        if(AIGame.building[b].Type == BUILDING_HOME){
            m_zhuzhaiList.append(&AIGame.building[b]);
//            qDebug()<<"append ..m_zhuzhaiList1111   "<<homeNumber;
//            qDebug()<<"append ..m_zhuzhaiList2222   "<<m_zhuzhaiList.size();
//            qDebug()<<"     AIGame.building_n3333   "<<AIGame.building_n;
        }else if (AIGame.building[b].Type == BUILDING_GRANARY) {
            gucangSN = AIGame.building[b].SN;
            gucangBuilding = &AIGame.building[b];
        }else if (AIGame.building[b].Type == BUILDING_MARKET ) {
            shichangSN = AIGame.building[b].SN;
            if(AIGame.building[b].Percent == 100){
                isFinishedMarket = true;
            }
        }else if (AIGame.building[b].Type == BUILDING_STOCK ) {
            cangkuSN = AIGame.building[b].SN;

        }else if (AIGame.building[b].Type == BUILDING_FARM) {
            farmList.append(&AIGame.building[b]);
        }
        else if(AIGame.building[b].Type ==BUILDING_ARROWTOWER ){
            if(feixuList.contains(AIGame.building[b].BlockL) && AIGame.building[b].Percent == 100){
                feixuList.remove(AIGame.building[b].BlockL);
            }
            jiantaList.append(&AIGame.building[b]);
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
        if(!xunluMap.contains(AIGame.human[c].SN) && AIGame.human[c].Blood > 0){ // 没在探路中且血量大于0
            m_humansList.append(&AIGame.human[c]);
        }

        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU].IsEmpty = false;
        myMap[AIGame.human[c].BlockL][AIGame.human[c].BlockU].IsArrived = true;
    }
    //除开探路与血量为零的人后，农民分类
    for(auto i:m_humansList){
        switch (i->NowState) {
        case HUMAN_STATE_IDLE:emptyMans.append(i);break;
        case HUMAN_STATE_CUTTING:shuList.append(i);break;
        case HUMAN_STATE_DIGGING_STONE:shiList.append(i);break;
        case HUMAN_STATE_GATHERING:guoList.append(i);break;
        case HUMAN_STATE_BUILDING:bulidList.append(i);break;
        case HUMAN_STATE_FARMING:break;

        }
    }

    ///第一阶段
    //    GameFirstStep()
    //    mapInfo humanPathMap[5][5];



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
    int maxNum;
    if(AIGame.GameFrame  <= (24*60*5)){
        maxNum = 3;
    }else if(AIGame.GameFrame  <= (24*60*5) && AIGame.GameFrame  <= (24*60*7)){
        maxNum = 2;
    }else {
        maxNum = 1;
    }
//    int maxNum =(m_zhuzhaiList.isEmpty() && AIGame.human_n < 8)?2:3;
    if(feixuList.size() == 3){
        //废墟找到完了一个人探路
        xunluMap.clear();
        int maxNum = 1;

    }
    if(AIGame.GameFrame %(24*60*2) == 0){
        xunluMap.clear();
        findRoadHumanNum = 0;
        QList<int> varyXunluPeople;
        int xunluIndex = 0;
        for(auto p:m_humansList){
            if(findRoadHumanNum == 2){
                break;
            }

            if( p->NowState != HUMAN_STATE_BUILDING){
                if(xunluIndex == 0 ){
                    humanGoTo = QPointF(-3*BLOCKSIDELENGTH,2*BLOCKSIDELENGTH) + QPointF(p->L, p->U);
                }else if(xunluIndex == 1){
                    humanGoTo = QPointF(3*BLOCKSIDELENGTH,-2*BLOCKSIDELENGTH) + QPointF(p->L, p->U);
                }else {
                    humanGoTo = QPointF(0*BLOCKSIDELENGTH,-3*BLOCKSIDELENGTH) + QPointF(p->L, p->U);
                }
                HumanMove(p->SN,humanGoTo.rx(),humanGoTo.ry());
                FoundRoad f{p->SN, 0, p, humanGoTo};
                xunluMap.insert(p->SN, f);
                findRoadHumanNum++;
                xunluIndex++;
            }
        }

    }else {
        for(auto p : wuxiaoSN){
            xunluMap.remove(p);
        }


        if(xunluMap.size() < maxNum ){
            for(int i=0; i<maxNum - xunluMap.size(); i++){
                for(auto p : m_humansList){
                    if(!xunluMap.contains(p->SN) && p->NowState != HUMAN_STATE_BUILDING){
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
    }




    //是否到达目标点
    //    QMap<int, FoundRoad>::iterator it;
    for(it = xunluMap.begin(); it != xunluMap.end(); ++it){
        if(it.value().man->Blood == 0){
            continue;
        }
        FoundRoad& p = it.value();

        int blockHumanX = p.man->BlockL;
        int blockHumanY = p.man->BlockU;
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
    //生产谷仓
    if(!hasGucang && AIGame.human_n > 4 && AIGame.human[4].NowState != HUMAN_STATE_BUILDING){

        if(!m_bushList.isEmpty()){

            QPolygon points;
            static int index = 0;
            // 尝试在这五个点建造
            points << QPoint(-4, -3) << QPoint(-4, -2) << QPoint(-4, -1) << QPoint(-4, 0)
                   << QPoint(-4, 1) << QPoint(-4, 2) << QPoint(-4, 3) << QPoint(-4, 4);


            int buildingX = m_bushList.first()->BlockL +  points[index].x();
            int buildingY = m_bushList.first()->BlockU +  points[index].y(); ;
            HumanBuild(AIGame.human[AIGame.human_n - 1].SN, BUILDING_GRANARY, buildingX, buildingY);
            isBuildingGucang = true;
            index++;
            if(index > 4){
                index = 0;
            }

            emptyMans.removeOne(&AIGame.human[AIGame.human_n - 1]);
        }
        else if(AIGame.GameFrame > 24*60*4){
            int index = ((Cangkutimes %2) == 0) ? 1 : -1;
            int buildingX = 38 +  -1 *times ;
            int buildingY = 37 ;
            HumanBuild(AIGame.human[AIGame.human_n - 1].SN, BUILDING_GRANARY, buildingX, buildingY);
            isBuildingGucang = true;
            times += 1;
            if(times > 10){
                times = 0;
            }

            emptyMans.removeOne(&AIGame.human[AIGame.human_n - 1]);
        }

    }
//    if(!hasGucang && hasResource(RESOURCE_BUSH) && AIGame.human[AIGame.human_n - 1].NowState != HUMAN_STATE_BUILDING && AIGame.human_n > 4){ // 并且已经找到果子, 在果子旁边
//        // 计算果子的位置
//        int index = ((times %2) == 0) ? 1 : -1;
//        int buildingX = m_bushList[0]->BlockL + index * ( 2 * (times /2 +1));
//        int buildingY = m_bushList[0]->BlockU + index * ( 2 * (times /2 +1));

//        HumanBuild(AIGame.human[AIGame.human_n - 1].SN, BUILDING_GRANARY, buildingX, buildingY);
//        isBuildingGucang = true;
//        times += 1;
//        if(times > 10){
//            times = 0;
//        }

//        emptyMans.removeOne(&AIGame.human[AIGame.human_n - 1]);

//    }

    // 生产农民

    if((xunluMap.size() + m_humansList.size()) < AIGame.Human_MaxNum && AIGame.human_n <= 24 &&AIGame.Meat >= 50){
        if(AIGame.building[0].Project == BUILDING_FREE){
            BuildingAction(AIGame.building[0].SN, BUILDING_CENTER_CREATEFARMER);
        }
    }

    // 建造房屋

    static bool isBuildingHome = false;
    static int size = 0;
    static int frameSTOC = AIGame.GameFrame;
    if(isBuildingHome && frameSTOC+25 == AIGame.GameFrame && AIGame.human[2].NowState!= HUMAN_STATE_BUILDING){
        isBuildingHome = false;
        size --;
    }

    if(isBuildingHome && size == homeNumber){
        isBuildingHome = false;
    }
    //        if(isBuildingHome && m_zhuzhaiList.last()->Percent == 100){
    //            isBuildingHome = false;
    //        }

    int homeMaxNum = 2;
    if(AIGame.civilizationStage == CIVILIZATION_STONEAGE){
        homeMaxNum = 1;
    }
    //检查是否有建筑未建成，如有继续建
    //每5帧检测一次

    if(m_zhuzhaiList.size() > homeNumber){
        for(auto i:m_zhuzhaiList){
            if(i->Percent < 100 && AIGame.human[2].NowState != HUMAN_STATE_BUILDING && AIGame.GameFrame % 5 == 0){
                HumanAction(AIGame.human[2].SN,i->SN);
            }
        }
    }
//    qDebug()<<"bulid ..m_zhuzhaiList   "<<homeNumber;
    if( !isBuildingHome && homeNumber < homeMaxNum  && AIGame.human[2].NowState != HUMAN_STATE_BUILDING && m_zhuzhaiList.size() < homeMaxNum){

        bool notFind = true;
        int x, y;
        while(notFind){
            x = QRandomGenerator::global()->bounded(25, 55);
            y = QRandomGenerator::global()->bounded(25, 55);

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

        HumanBuild(AIGame.human[2].SN, BUILDING_HOME, x, y);
        emptyMans.removeOne(&AIGame.human[2]);
        isBuildingHome = true;
        size++;
        frameSTOC = AIGame.GameFrame;
    }

    //建造仓库
    if(!hasCangku && AIGame.human_n > 4 && AIGame.human[4].NowState != HUMAN_STATE_BUILDING){
        int index = ((Cangkutimes %2) == 0) ? 1 : -1;
        int buildingX = 33 +  -1 *Cangkutimes ;
        int buildingY = 34 ;

        HumanBuild(AIGame.human[4].SN, BUILDING_STOCK, buildingX, buildingY);
        Cangkutimes += 1;
        isBuildingCangku = true;

        if(Cangkutimes > 10){
            Cangkutimes = 0;
        }

        emptyMans.removeOne(&AIGame.human[4]);

    }



    if(AIGame.civilizationStage == CIVILIZATION_TOOLAGE){ //工具时代
        int buildSN = 4;
        int SN = AIGame.human[buildSN].SN;
        while(AIGame.human[buildSN].Blood < 1 && buildSN < AIGame.human_n){
            buildSN ++;
        }

        //谷仓升级箭塔技术
        static bool updateJianTa = false;
        if(updateJianTa && gucangBuilding && gucangBuilding->Project == BUILDING_FREE){
            jiantaTech = true;
            updateJianTa = false;
        }
        if(!updateJianTa && !jiantaTech && gucangBuilding && gucangBuilding->Project == BUILDING_FREE){
             BuildingAction(gucangSN,BUILDING_GRANARY_ARROWTOWER);
             updateJianTa = true;
        }

        static int jiantaNum = 0;
        static bool building = 0;
        static int buidlKey = 0;
        if(jiantaList.size() < jiantaNum ){
            building = false;
            jiantaNum--;
            buidlKey = 0;
        }


        if(building && jiantaList.size() > 0 && jiantaList.last()->Percent == 100){
            building = false;
            feixuList.remove(buidlKey);
        }

        if(!building){
            if(!feixuList.isEmpty()){
                if(jiantaTech){
                    int l = feixuList.begin().key();
                    int u =  feixuList.begin().value();

                    buidlKey = l;
                    HumanBuild(AIGame.human[buildSN].SN,BUILDING_ARROWTOWER,l,u);
                    jiantaNum++;
                    building = true;

                    emptyMans.removeOne(&AIGame.human[buildSN]);
                }
            }
            else if(shichangSN == 0){ //第一步先造市场 提升资源采集效率
                HumanBuild(buildSN,BUILDING_MARKET, 32,40);
            }
            else{
                if(farmList.size() < 3 && isFinishedMarket){ // 市场完成后建造农田
                    createBuliding(&AIGame.human[buildSN],BUILDING_FARM,BUILD_FARM_WOOD,0,0);
                }
            }
        }


    }

    // 收拾残局
    if(emptyMans.size() > 0){
        //检查是否有建筑未建成，如有继续建
        for(auto bul:buildingList){
            if((bul->SN == gucangSN &&bul->Percent != 100) || (bul->SN == cangkuSN &&bul->Percent != 100)){
                HumanAction(emptyMans.first()->SN,bul->SN);
                emptyMans.removeFirst();
            }
        }
    }
    if(emptyMans.size() > 0 && !hasResource(RESOURCE_BUSH)){
        for(auto ani:m_animalList){
            if(ani->Blood <= 5){
                HumanAction(emptyMans.first()->SN,ani->SN);
                emptyMans.removeFirst();
                break;
            }
        }
    }


    // 空闲分配
    if(emptyMans.size() > 0){
        for(auto p : emptyMans){
            if(guoList.empty() && hasResource(RESOURCE_BUSH)){
                int index = -1;
                int bushSN = findResSN(*p, RESOURCE_BUSH,index);
                if(bushSN != 0){
                    HumanAction(p->SN, bushSN);
                }
                else{
                    for(auto ani:m_animalList){
                        if(ani->Blood <= 5 || ani->Type == RESOURCE_GAZELLE){
                            HumanAction(p->SN,ani->SN);
                            break;
                        }
                    }
                }
            }
            else if(shuList.empty()){
                int index = -1;
                int bushSN = findResSN(*p, RESOURCE_TREE,index);
                if(bushSN != 0){
                    HumanAction(p->SN, bushSN);
                }

            }
            else{
                //第一阶段

                if(AIGame.civilizationStage == CIVILIZATION_STONEAGE){
                    if(shuList.size() < guoList.size() && guoList.size() > 2 ){
                        int index = -1;
                        int bushSN = findResSN(*p, RESOURCE_TREE,index);
                        if(bushSN != 0){
                            HumanAction(p->SN, bushSN);

                        }else if(AIGame.Wood < 500){
                            int bushSN = findResSN(*p, RESOURCE_BUSH,index);
                            HumanAction(p->SN, bushSN);
                        }
                    }
                    else{
                        int index = -1;
                        int bushSN = findResSN(*p, RESOURCE_BUSH,index);
                        if(bushSN != 0){
                            HumanAction(p->SN, bushSN);
                        }else if(AIGame.Wood < 300){
                            int bushSN = findResSN(*p, RESOURCE_TREE,index);
                            HumanAction(p->SN, bushSN);
                        }else if(AIGame.Meat < 500){
                            //食物不够，就去打猎
                            //将地图上的已杀死食物取回
                            for(auto ani:m_animalList){
                                if(ani->Blood <= 5 || ani->Type == RESOURCE_GAZELLE){
                                    HumanAction(p->SN,ani->SN);
                                }
                            }
                        }else {
                            //如果木头超过500了，就去采石
                            int bushSN = findResSN(*p, RESOURCE_STONE,index);
                            if(bushSN != 0){
                                HumanAction(p->SN, bushSN);
                            }
                        }

                    }

                }
                else{
                    //第二阶段


                    //找食物
                    //建农场
                    for(auto f: farmList){
                        if(f->Percent == 100){
                            HumanAction(p->SN,f->SN);
                        }else {

                        }
                    }

                    if(guoList.size()< 5){
                        int index = -1;
                        int bushSN = findResSN(*p, RESOURCE_BUSH,index);
                        if(bushSN != 0){
                            HumanAction(p->SN, bushSN);
                        }else if(AIGame.Wood < 320){
                            int bushSN = findResSN(*p, RESOURCE_TREE,index);
                            HumanAction(p->SN, bushSN);
                        }else if(AIGame.Meat < 250){
                            //食物不够，就去打猎
                            //将地图上的已杀死食物取回
                            for(auto ani:m_animalList){
                                if(ani->Blood <= 5 || ani->Type == RESOURCE_GAZELLE ){
                                    HumanAction(p->SN,ani->SN);
                                }
                            }
                        }else {
                            //如果木头超过400了，就去采石
                            int bushSN = findResSN(*p, RESOURCE_STONE,index);
                            if(bushSN != 0){
                                HumanAction(p->SN, bushSN);
                            }
                        }
                    }
                    //采石
                    if(shiList.isEmpty() && hasResource(RESOURCE_STONE)){
                        int index = -1;
                        int bushSN = findResSN(*p, RESOURCE_STONE,index);
                        if(bushSN != 0){
                            HumanAction(p->SN, bushSN);
                        }
                    }



                }
            }

        }
    }







    ProcessDataWork = 0;
    return;

    //人物策略




    ProcessDataWork = 0;
    return;

    ///第二阶段
    // 采集浆果 采浆果3个人

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
