#ifndef FARMER_H
#define FARMER_H

#include <Human.h>
#include <GlobalVariate.h>

class Farmer:public Human
{

public:
    Farmer();
    ~Farmer();

    void nextframe();
    void init(int Camp,double L, double U,bool *market);
    void init(int Camp,int BlockL,int BlockU,bool *market);

    int getstate()
    {
        return this->state;
    }
    int getworkstate()
    {
        return this->workstate;
    }
    Coordinate *getnowobject()
    {
        return this->nowobject;
    }
    Coordinate *getworkobject()
    {
        return this->workobject;
    }
    double getresource()
    {
        return this->resource;
    }
    int getresourceSort()
    {
        return this->resourceSort;
    }
    int getholdresource()
    {
        return this->holdResource;
    }
    int getActionOccurrenceGameframe()
    {
        return this->ActionOccurrenceGameframe;
    }
    int getCuttingSpeed()
    {
        //砍树的攻速
        return this->CuttingSpeed;
    }
    int getCuttingAttack()
    {
        return this->CuttingAttack;
    }
    int getHuntRange()
    {
        return this->HuntRange;
    }
    int getLongRangeAttackFrame()
    {
        return this->LongRangeAttackFrame;
    }
    int getLongRangeAttackSpeed()
    {
        return this->LongRangeAttackSpeed;
    }
    double getCuttingRange()
    {
        return this->CuttingRange;
    }
    //判断工作状态
    double getWoodSpeed()
    {
        return this->WoodSpeed;
    }
    double getStoneSpeed()
    {
        return this->StoneSpeed;
    }
    double getGoldSpeed()
    {
        return this->GoldSpeed;
    }
    double getMeatSpeed()
    {
        return this->FoodSpeed;
    }
    bool isIdleing()
    {
        return this->workstate==HUMAN_STATE_IDLE;
    }
    bool isGoToObjecting()
    {
        return this->workstate==HUMAN_STATE_GOTO_OBJECT;
    }
    bool isCutting()
    {
        return this->workstate==HUMAN_STATE_CUTTING;
    }
    bool isDiggingStone()
    {
        return this->workstate==HUMAN_STATE_DIGGING_STONE;
    }
    bool isDiggingGold()
    {
        return this->workstate==HUMAN_STATE_DIGGING_GOLD;
    }
    bool isGathering()
    {
        return this->workstate==HUMAN_STATE_GATHERING;
    }
    bool isConstruct()
    {
        return this->workstate==HUMAN_STATE_BUILDING;
    }
    bool isRepair()
    {
        return this->workstate==HUMAN_STATE_FIXING;
    }
    bool isAttacking()
    {
        return this->workstate==HUMAN_STATE_ATTACKING;
    }
    bool isGoToCamp()
    {
        return this->workstate==HUMAN_STATE_GOTO_RESOURCE;
    }
    bool isGoToAttack()
    {
        return this->workstate==HUMAN_STATE_GOTO_ATTACK;
    }
    bool isStop()
    {
        return this->workstate==HUMAN_STATE_STOP;
    }
    bool isFarming()
    {
        return this->workstate==HUMAN_STATE_FARMING;
    }
    bool nowobjectisNULL()
    {
        return this->nowobject==NULL;
    }
    bool isFullResource()
    {
        if(this->getresourceSort() == HUMAN_WOOD)
        {
            return this->resource>=WoodLimit;
        }
        else if(this->getresourceSort() == HUMAN_STONE)
        {
            return this->resource>=StoneLimit;
        }
        else
        {
            return this->resource>=10;
        }
    }
    bool isholdresource()
    {
        return this->holdResource==1;
    }
    bool haveresource()
    {
        return this->resource>=1;
    }
    bool isVillager()
    {
        return this->state==FARMER_VILLAGER;
    }
    bool isLumber()
    {
        return this->state==FARMER_LUMBER;
    }
    bool isWorker()
    {
        return this->state==FARMER_WORKER;
    }
    bool isHunter()
    {
        return this->state==FARMER_HUNTER;
    }
    bool isGatherer()
    {
        return this->state==FARMER_GATHERER;
    }
    bool isMiner()
    {
        return this->state==FARMER_MINER;
    }
    void setResourceSort(int resourceSort)
    {
        this->resourceSort=resourceSort;
    }
    void setstate(int state)
    {
        this->state=state;
    }
    void setnowobject(Coordinate *nowobject)
    {
        this->nowobject=nowobject;
    }
    void setworkobject(Coordinate *workobject)
    {
        this->workobject=workobject;
    }
    void setlastLU(int lastL,int lastU)
    {
        this->lastL=lastL;
        this->lastU=lastU;
    }

    //设置工作状态

    void setisIdle()
    {
        this->workstate=HUMAN_STATE_IDLE;
    }
    void setIsGoToObject()
    {
        this->workstate=HUMAN_STATE_GOTO_OBJECT;
    }
    void setIsCutting()
    {
        this->workstate=HUMAN_STATE_CUTTING;
    }
    void setIsDiggingStone()
    {
        this->workstate=HUMAN_STATE_DIGGING_STONE;
    }
    void setIsDiggingGold()
    {
        this->workstate=HUMAN_STATE_DIGGING_GOLD;
    }
    void setIsGathering()
    {
        this->workstate=HUMAN_STATE_GATHERING;
    }
    void setIsConstruct()
    {
        this->workstate=HUMAN_STATE_BUILDING;
    }
    void setIsRepair()
    {
        this->workstate=HUMAN_STATE_FIXING;
    }
    void setIsAttacking()
    {
        this->workstate=HUMAN_STATE_ATTACKING;
    }
    void setIsGoToCamp()
    {
        this->workstate=HUMAN_STATE_GOTO_RESOURCE;
    }
    void setIsGoToAttack()
    {
        this->workstate=HUMAN_STATE_GOTO_ATTACK;
    }
    void setIsStop()
    {
        this->workstate=HUMAN_STATE_STOP;
    }
    void setIsFarming()
    {
        this->workstate=HUMAN_STATE_FARMING;
    }
    void addResource(int resourceSort)
    {
        if(resource==0)
        {
            setResourceSort(resourceSort);
        }
        else if(this->resourceSort!=resourceSort)
        {
            setResourceSort(resourceSort);
            resource=0;
        }
        if(this->resourceSort==HUMAN_GRANARYFOOD||this->resourceSort==HUMAN_STOCKFOOD)
        {
            this->resource+=this->getMeatSpeed();
        }
        else if(this->resourceSort==HUMAN_GOLD)
        {
            this->resource+=this->getGoldSpeed();
        }
        else if(this->resourceSort==HUMAN_STONE)
        {
            this->resource+=this->getStoneSpeed();
        }
        else if(this->resourceSort==HUMAN_WOOD)
        {
            this->resource+=this->getWoodSpeed();
        }
    }
    void resetResource()
    {
        this->resource=0;
        setResourceSort(0);
    }
    void setVillager()
    {
        this->state=FARMER_VILLAGER;
    }
    void setLumber()
    {
        this->state=FARMER_LUMBER;
    }
    void setGatherer()
    {
        this->state=FARMER_GATHERER;
    }
    void setMiner()
    {
        this->state=FARMER_MINER;
    }
    void setHunter()
    {
        this->state=FARMER_HUNTER;
    }
    void setFarmer()
    {
        this->state=FARMER_FARMER;
    }
    void setWorker()
    {
        this->state=FARMER_WORKER;
    }
    void setholdresource()
    {
        this->holdResource=1;//改为判断持有什么资源来调整变量
    }
    void setholdresourceIsNULL()
    {
        this->holdResource=0;
    }
    void upgradeCapablity(bool *market)
    {
        if(market[0])
        {
            isRangeUpgraded=true;
            isWoodUpgraded=true;
        }
        if(market[1])
        {
            isStoneUpgraded=true;
        }
        if(isRangeUpgraded==true)
        {
            this->HuntRange=FARMER_UPGRADEHUNTRANGE;
        }
        if(isWoodUpgraded==true)
        {
            this->WoodLimit=FARMER_CARRYLIMIT_UPGRADEWOOD;
        }
        if(isStoneUpgraded==true)
        {
            this->StoneLimit=FARMER_CARRYLIMIT_UPGRADESTONE;
        }
    }
    void setNowRes();
    void setActionOccurrenceGameframe(int gameframe)
    {
        this->ActionOccurrenceGameframe=gameframe;
    }
    std::string getDisplayName(int num)
    {
        return FarmerDisplayName[num];
    }
    bool stackIsNull()
    {
        return path.size()==0;
    }
    bool getRangeUpgraded()
    {
        return this->isRangeUpgraded;
    }
    bool getWoodUpgraded()
    {
        return this->isWoodUpgraded;
    }
    bool getStoneUpgraded()
    {
        return this->isStoneUpgraded;
    }
    void setRangeUpgraded(bool flag)
    {
        isRangeUpgraded = flag;
    }
    void setWoodUpgraded(bool flag)
    {
        isWoodUpgraded = flag;
    }
    void setStoneUpgraded(bool flag)
    {
        isStoneUpgraded = flag;
    }
    static std::string getFarmerName(int index) {
        if (index >= 0 && index < 7) {
            return FarmerName[index];
        }
        return "";
    }
    static std::string getFarmerCarry(int index) {
        if (index >= 0 && index < 5) {
            return FarmerCarry[index];
        }
        return "";
    }
    static std::list<Resource>* getCarry(int i, int j) {
        return Carry[i][j];
    }
    static std::list<Resource>* getWork(int i, int j) {
        return Work[i][j];
    }

    static std::list<Resource>* getWalk(int i, int j) {
        return Walk[i][j];
    }
    static std::list<Resource>* getStand(int i, int j) {
        return Stand[i][j];
    }
    static std::list<Resource>* getAttack(int i, int j) {
        return Attack[i][j];
    }
    static std::list<Resource>* getDie(int i, int j) {
        return Die[i][j];
    }
    static void setCarry(int i, int j, std::list<Resource>* newValue) {
        Carry[i][j] = newValue;
    }
    static void setWork(int i, int j, std::list<Resource>* newValue) {
        Work[i][j] = newValue;
    }
    static void setWalk(int i, int j, std::list<Resource>* newValue) {
        Walk[i][j] = newValue;
    }
    static void setStand(int i, int j, std::list<Resource>* newValue) {
        Stand[i][j] = newValue;
    }
    static void setAttack(int i, int j, std::list<Resource>* newValue) {
        Attack[i][j] = newValue;
    }
    static void setDie(int i, int j, std::list<Resource>* newValue) {
        Die[i][j] = newValue;
    }
    static void allocateCarry(int i, int j) {
        Carry[i][j] = new std::list<Resource>;
    }
    static void allocateWork(int i, int j) {
        Work[i][j] = new std::list<Resource>;
    }
    static void allocateWalk(int i, int j) {
        Walk[i][j] = new std::list<Resource>;
    }
    static void allocateStand(int i, int j) {
        Stand[i][j] = new std::list<Resource>;
    }
    static void allocateAttack(int i, int j) {
        Attack[i][j] = new std::list<Resource>;
    }
    static void allocateDie(int i, int j) {
        Die[i][j] = new std::list<Resource>;
    }
    static void deallocateCarry(int i, int j) {
        delete Carry[i][j];
        Carry[i][j] = nullptr;
    }
    static void deallocateWork(int i, int j) {
        delete Work[i][j];
        Work[i][j] = nullptr;
    }
    static void deallocateWalk(int i, int j) {
        delete Walk[i][j];
        Walk[i][j] = nullptr;
    }
    static void deallocateStand(int i, int j) {
        delete Stand[i][j];
        Stand[i][j] = nullptr;
    }
    static void deallocateAttack(int i, int j) {
        delete Attack[i][j];
        Attack[i][j] = nullptr;
    }

    static void deallocateDie(int i, int j) {
        delete Die[i][j];
        Die[i][j] = nullptr;
    }


private:

    int state;
    //指示状态 指示具体动作
    //暂定 	0为无职 1为樵夫 2为矿工 3为采集者 4为猎人 5为种地农民 6为工人
    int workstate=HUMAN_STATE_IDLE;
    //现在的工作流程

    /* 0代表为空闲状态
     * 1代表为正在前往当前对象的状态
     * 2代表为正在砍树的状态（收集木头）
     * 3代表为正在挖石头的状态
     * 4代表为正在挖金子的状态
     * 5代表正在采集果子的状态
     * 6代表正在建造建筑的状态
     * 7代表正在修理建筑的状态
     * 8代表为进行攻击的状态 在攻击状态的下边进行特判 采用不同的方式计算
     * 9代表为正在返回资源建筑的状态，即放置资源
     * 10代表正在前往攻击的状态
     * 11代表人物遇到障碍物停止移动的状态
     * 后续补充
     */

    int holdResource=0;
    //0表示当前不抱有状态

    Coordinate *workobject=NULL;
    //工作对象（资源）

    Coordinate *nowobject=NULL;
    //当前工作对象

    double resource;
    //资源携带量

    int resourceSort;
    //指示所携带资源的类型
    //1指代木头 2指代肉 3指代石头 4指代金子

    int LongRangeAttackFrame=6;
    //第六帧产生矛
    int LongRangeAttackSpeed=30;
    //打猎时扔茅攻速
    int CloseCombatAttackSpeed;
    //近战攻速
    int CuttingSpeed=11;
    //砍树动作的攻速

    int CuttingAttack=4;
    //攻击树的伤害

    int HuntRange=FARMER_HUNTRANGE;
    //打猎时的攻击范围 街区距离

    double CuttingRange=5;
    //砍树范围

    int ActionOccurrenceGameframe;
    //记录动作开始的帧数

    double WoodSpeed;
    double StoneSpeed;
    double FoodSpeed;
    double GoldSpeed;

    double WoodLimit;
    double StoneLimit;
    double FoodLimit;
    double GoldLimit;

    bool isRangeUpgraded = false;
    bool isWoodUpgraded = false;
    bool isStoneUpgraded = false;

    int lastL;
    int lastU;
    //工作对象消失后需要返回消失处

    static std::list<Resource> *Walk[7][8];

    static std::list<Resource> *Work[7][8];

    static std::list<Resource> *Stand[7][8];

    static std::list<Resource> *Attack[7][8];

    static std::list<Resource> *Die[7][8];

    static std::list<Resource> *Carry[5][8];

    static std::string FarmerName[7];
    static std::string FarmerCarry[5];
    std::string FarmerDisplayName[7]={"村民","樵夫","浆果采集者","矿工","猎人","农民","工人"};
    //    std::string actName[1]={};
    //窗口按钮资源的字符串名称 用来查找对应的qpixmap资源



};

#endif // FARMER_H
