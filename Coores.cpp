#include "Coores.h"

std::list<Resource>* Coores::coores[3];
std::list<Resource>* Coores::graycoores[3];
std::string Coores::Cooresname[3]={"Bush","Stone","GoldOre"};
std::string Coores::CooresDisplayName[3] = {"浆果丛","石头","金矿"};

Coores::Coores()
{
    this->sort=SORT_COORES;
    //    SoundMap["BGM"]->play();
}

void Coores::nextframe()
{
    if(visible==1)
    {
        nowres=coores[this->Num]->begin();
    }
    else
    {
        nowres=graycoores[this->Num]->begin();
    }

    if(destroy==0)
    {
        if(Cnt<=0)
        {
            destroy=1;
        }
    }

}

void Coores::init(int Num, double L, double U)
{
    this->Num=Num;
    if(Num==COORES_BUSH)//树莓丛
    {
        this->ProductSort=PRODUCTSORT_GRANARYFOOD;
        this->MaxCnt=CNT_BUSH;
    }
    this->L=L;
    this->U=U;
    this->BlockL=L/BLOCKSIDELENGTH;
    this->BlockU=U/BLOCKSIDELENGTH;
    this->H=L-U;
    this->nowres=coores[this->Num]->begin();
    this->Cnt=this->MaxCnt;
    this->GatherSpeed=0;
    this->Percent=100;
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    this->LU=this->LY*gen5;
    this->radius=LX;
    this->selectNum=sort*10000+g_selectNum;
    g_Object.insert({this->selectNum,this});
    g_selectNum++;
}

void Coores::init(int Num, int BlockL, int BlockU)
{
    this->Num=Num;
    if(Num==COORES_STONE)//石头
    {
        this->ProductSort=PRODUCTSORT_STONE;
        this->objectType=OBJECTTYPE_BLOCK;
        this->MaxCnt=CNT_STONE;
        this->type=FOUNDATION_SMALL;
    }
    else if(Num==COORES_GOLDORE)//金矿
    {
        this->ProductSort=PRODUCTSORT_GOLD;
        this->objectType=OBJECTTYPE_BLOCK;
        this->MaxCnt=CNT_GOLDORE;
        this->type=FOUNDATION_SMALL;
    }
    this->BlockL=BlockL;
    this->BlockU=BlockU;
    if(type==FOUNDATION_SMALL)
    {
        this->L=(BlockL+1)*BLOCKSIDELENGTH;
        this->U=(BlockU+1)*BLOCKSIDELENGTH;
    }
    this->H=L-U;
    this->nowres=coores[this->Num]->begin();
    this->Cnt=this->MaxCnt;
    this->GatherSpeed=0;
    this->Percent=100;
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    this->LU=2*BLOCKSIDELENGTH;
    this->radius=LX;
    this->selectNum=sort*10000+g_selectNum;
    g_Object.insert({this->selectNum,this});
    g_selectNum++;
}


