#include "Building.h"

std::list<Resource>* Building::build[4];
std::list<Resource>* Building::built[3][7];

std::string Building::Buildingname[4]={"Small_Foundation","Foundation","Big_Foundation","Building_House1"};
std::string Building::Builtname[3][7]={
                                        {},
                                       {"House1","Granary","Center1","Stock","Farm","Market","ArrowTower"},
                                       {"House2","Granary","Center2","Stock","Farm","Market","ArrowTower"}
                                      };


Building::Building()
{
    this->sort=SORT_BUILDING;
}

Building::~Building()
{

}

void Building::init(int num, int civ, int BlockL, int BlockU, int Percent, bool *market)
{
//    qDebug()<<"1";
    this->Num=num;
    this->civ=civ;
    this->BlockL=BlockL;
    this->BlockU=BlockU;
    this->visible=1;
    this->H=(BlockL-BlockU)*16*gen5;
    if(num==BUILDING_HOME)
    {
        type=FOUNDATION_SMALL;
        this->vision=VISION_HOME;
    }
    else if(num==BUILDING_ARROWTOWER)
    {
        type=FOUNDATION_SMALL;
        vision=VISION_ARROWTOWER;
    }
    else if(num==BUILDING_FARM)
    {
        type=FOUNDATION_MIDDLE;
        this->sort=SORT_FARM;
        this->ProductSort=PRODUCTSORT_GRANARYFOOD;
        this->vision=VISION_FARM;
        this->Cnt=CNT_FARM;
        if(market[2]==true)
        {
            this->Cnt=CNT_UPGRADEFARM;
        }
    }
    else
    {
        type=FOUNDATION_MIDDLE;
        this->vision=VISION_CENTER;
    }
    if(type==FOUNDATION_SMALL)
    {
        this->L=(BlockL+1)*BLOCKSIDELENGTH;
        this->U=(BlockU+1)*BLOCKSIDELENGTH;
        this->LU=2*BLOCKSIDELENGTH;
    }
    if(type==FOUNDATION_MIDDLE)
    {
        this->L=(BlockL+1.5)*BLOCKSIDELENGTH;
        this->U=(BlockU+1.5)*BLOCKSIDELENGTH;
        this->LU=3*BLOCKSIDELENGTH;
    }
    if(type==FOUNDATION_BIG)
    {
        this->L=(BlockL+2)*BLOCKSIDELENGTH;
        this->U=(BlockU+2)*BLOCKSIDELENGTH;
        this->LU=4*BLOCKSIDELENGTH;
    }
    this->Percent=Percent;
    this->ConstructSpeed=0;
    this->GatherSpeed=0;
    this->MaxBlood=BuildingMaxBlood[Num];
    this->Blood=MaxBlood*Percent/100;
    //最大血量的设置是不是应该通过一个已经初始化的数组来确定
    if(Percent!=100)
    {
        this->nowres=build[this->type]->begin();
    }
    else this->nowres=built[civ][this->Num]->begin();
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    this->selectNum=10000+g_selectNum;
    g_Object.insert({this->selectNum,this});
    g_selectNum++;
}

void Building::nextframe()
{
    if(Percent<100)
    {
        nowres=build[type]->begin();
        int n=Percent/25;
        std::advance(nowres,n);
    }
    else
    {
        if(isFarm())
        {
            if(Cnt<=0)
            {
                this->destroy=1;
            }
        }
        nowres++;
        if(nowres==built[civ][this->Num]->end())
        {
            nowres=built[civ][this->Num]->begin();
        }
    }
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
}
