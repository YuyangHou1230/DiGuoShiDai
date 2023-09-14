#include "Missile.h"

std::list<Resource>* Missile::missile[1];
std::string Missile::Missilename[1]={"Spear"};

Missile::Missile()
{
    this->sort=SORT_MISSILE;


}

Missile::~Missile()
{

}

void Missile::init(Coordinate *Attackeder, Coordinate *Attacker, int Num, int L, int U, int L0, int U0)
{
    this->L=L;
    this->U=U;
    //  坐标位置微调
    this->Num=Num;
    this->Attackeder=Attackeder;
    this->Attacker=Attacker;
    this->L0=L0;
    this->U0=U0;
    this->nowres=missile[this->Num]->begin();
    //计算方向用于贴图

    double dx = L0 - L;
    double dy = U0 - U;
    Angle = round(atan2(dy, dx) * 180.0 / M_PI);
    if (Angle < 0)
    {
        Angle += 360;
    }
    int index = (Angle + 5.625) / 11.25;
    if (index >= 32)
    {
        index = 0;
    }
    //当前为逆时针

    //更改为顺时针
    if(index!=0)
    {
        index=32-index;
    }

    //坐标系与atan2对应的坐标系不同
    index+=2;
    if(index>=32)
    {
        index-=32;
    }


    // 将 nowres 指向需要绘制的贴图资源
    std::advance(nowres, index);


    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    this->LU=this->LY*gen5;
}

void Missile::nextframe()
{
    int dx = L0 - L;
    int dy = U0 - U;
    int d = round(sqrt(dx*dx + dy*dy));  // 计算矛与目标之间的距离

    if (d <= speed)
    {
        // 矛已到达目标位置，停止更新位置
        L = L0;
        U = U0;
    }
    else
    {
        // 根据速率计算每次的坐标变化量
        double ratio = speed / static_cast<double>(d);
        L += round(dx * ratio);
        U += round(dy * ratio);
    }

}

bool Missile::isArrive()
{
    if(this->L==this->L0&&this->U==this->U0)
    {
        return true;
    }
    else
        return false;
}


