#include "Ruin.h"

std::list<Resource>* Ruin::ruin[3];
std::string Ruin::Ruinname[3]={"Small_Ruin","Stool","Villager_Disappear_LeftUp"};

Ruin::Ruin()
{

}

//Ruin::~Ruin()
//{
//    if(!ruin.empty())
//    {
//        std::list<QPixmap>::iterator Riter=ruin.begin();
//        for(;Riter!=ruin.end();Riter++)
//        {
//            delete &(*Riter);
//        }
//        ruin.clear();
//    }
//}

void Ruin::nextframe()
{
    if(destroy==0)
    {
        time++;
        if(time%1000==0)
        {
            nowres++;
            if(nowres==ruin[this->Num]->end())
            {
                destroy=1;
                nowres--;
            }
        }
    }
}

void Ruin::init(int Num, double L, double U)
{
    this->L=L;
    this->U=U;
    this->H=-10000;
    this->BlockL=L/BLOCKSIDELENGTH;
    this->BlockU=U/BLOCKSIDELENGTH;
//    qDebug()<<"BlockL:"<<BlockL;
//    qDebug()<<"BlockU:"<<BlockU;
    this->LU=2*BLOCKSIDELENGTH;
    this->time=0;
    this->Num=Num;
    this->selectNum = 0;
    this->nowres=ruin[Num]->begin();

}
