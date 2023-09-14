#ifndef BUILDING_H
#define BUILDING_H

#include <Coordinate.h>

class Building:public Coordinate
{
public:
    Building();
    ~Building();
    void init(int num,int civ,int BlockL,int BlockU,int Percent,bool *market);

    void nextframe();
    bool isFinish()
    {
        return this->Percent>=100;
    }
    int getNum()
    {
        return this->Num;
    }
    double getCnt()
    {
        return this->Cnt;
    }
    void setCiv(int civ)
    {
        this->civ = civ;
    }
    void setFarmer(Coordinate* farmer)
    {
        this->nowfarmer=farmer;
    }
    int getCiv()
    {
        return this->civ;
    }
    bool isCenter()
    {
        return this->Num==BUILDING_CENTER;
    }
    bool isGranary()
    {
        return this->Num==BUILDING_GRANARY;
    }
    bool isStock()
    {
        return this->Num==BUILDING_STOCK;
    }
    bool isFarm()
    {
        return this->Num==BUILDING_FARM;
    }
    bool isHome()
    {
        return this->Num==BUILDING_HOME;
    }
    bool isMarket()
    {
        return this->Num==BUILDING_MARKET;
    }
    bool isNoFarmer()
    {
        return this->nowfarmer==NULL;
    }
    int getBlood()
    {
        return this->Blood;
    }
    int getMaxBlood()
    {
        return this->MaxBlood;
    }
    double getPercent()
    {
        return this->Percent;
    }
    void updateCivilization()
    {
        if(civ==CIVILIZATION_TOOLAGE)
        {
            this->nowres=built[civ][this->Num]->begin();
            this->LX=this->nowres->pix.width()/2.0;
            this->LY=this->nowres->pix.width()/4.0;
        }
    }
    void addPercent()
    {
        if(this->Num==BUILDING_HOME)
        {
            this->Percent+=10*ConstructSpeed;
        }
        else
        this->Percent+=5*ConstructSpeed;
        ConstructSpeed=0;
        if(Percent>=100&&Finish==0)
        {
            Percent=100;
            Finish=1;
            nowres=built[civ][this->Num]->begin();
        }
        this->Blood=Percent*MaxBlood/100;
    }
    void reduceCnt()
    {
        this->Cnt-=this->getGatherSpeed();
        this->resetGatherSpeed();
    }
    void setnowfarmer(Coordinate *farmer)
    {
        this->nowfarmer=farmer;
    }
    Coordinate *getnowfarmer()
    {
        return this->nowfarmer;
    }
    std::string getDisplayName(int num)
    {
        return this->BuildDisplayName[num];
    }
    bool isArrowTower()
    {
        return this->Num==BUILDING_ARROWTOWER;
    }

    static std::list<Resource>* getBuild(int i) {
        return build[i];
    }
    static std::list<Resource>* getBuilt(int i,int j) {
        return built[i][j];
    }

    static void allocatebuild(int i)
    {
        build[i]=new std::list<Resource>;
    }
    static void allocatebuilt(int i,int j)
    {
        built[i][j]=new std::list<Resource>;
    }
    static void deallocatebuild(int i) {
        delete build[i];
        build[i] = nullptr;
    }
    static void deallocatebuilt(int i,int j) {
        delete built[i][j];
        built[i][j] = nullptr;
    }

    static std::string getBuildingname(int index)
    {
        return Buildingname[index];
    }
    static std::string getBuiltname(int index1,int index2)
    {
        return Builtname[index1][index2];
    }
    int getActNames(int num)
    {
        return actNames[num];
    }
    void setActNames(int num, int name)
    {
        this->actNames[num] = name;
    }
    int getActStatus(int num)
    {
        return actStatus[num];
    }
    void setActStatus(int num, int status)
    {
        this->actStatus[num] = status;
    }

    static std::list<Resource> *build[4];
    //建设list

    static std::list<Resource> *built[3][7];
    //建设完成的list

    int Num;
    //建筑编号

    double Cnt;
    //仅用于农场

    Coordinate *nowfarmer=NULL;
    //仅用于农场 当前实际耕作的农民

    double CntSpeed;

    double Percent = 0;
    //完成百分比 100时表示建筑已经被建造完成 根据完成度有不同的贴图

    int Finish=0;//0为未完成 1为完成

    int civ;
    //建筑所处时代 来确定不同时代建筑有何变化 ？时代要不要用player类下的
    double hpPercent;

    static std::string Buildingname[4];
    static std::string Builtname[3][7];

    std::string BuildDisplayName[7]={"房屋","谷仓","市镇中心","仓库","农场","市场","箭塔"};
    int BuildingMaxBlood[7]={600,600,600,600,600,600,600};
    int actNames[10] = {0};
    int actStatus[10] = {0};

};

#endif // BUILDING_H
