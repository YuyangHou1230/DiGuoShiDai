#include "Player.h"

Player::Player()
{

}

Player::~Player()
{
    for (auto it = build.begin(); it != build.end(); ++it)
    {
        delete *it;  // 删除指针指向的单个对象
    }
    build.clear();  // 清空列表
    for (auto it = missile.begin(); it != missile.end(); ++it)
    {
        delete *it;  // 删除指针指向的单个对象
    }
    missile.clear();  // 清空列表
    for (auto it = human.begin(); it != human.end(); ++it)
    {
        delete *it;  // 删除指针指向的单个对象
    }
    human.clear();  // 清空列表
}

//添加建筑
Building *Player::addbuilding(int Num, int BlockL, int BlockU, int Percent)
{
    Building *newbuild=new Building();
    newbuild->init(Num,civilization,BlockL,BlockU,Percent,marketResearch);
    build.push_back(newbuild);
    //    qDebug()<<"外部地址："<<newbuild;
    //    delete newbuild;
    return newbuild;
}


//添加人
void Player::addhuman(int Camp,int Num, int L, int U)
{
    Human *newhuman=new Human();
    newhuman->init(Camp,Num,L,U);
    //    qDebug()<<"before"<<human.size();
    human.push_back(newhuman);
    //    qDebug()<<"after"<<human.size();
    //    delete newhuman;
    return ;
}

//添加农民
int Player::addfarmer(int Camp, double L, double U)
{
    Farmer *newfarmer=new Farmer();
    newfarmer->init(Camp,L,U,marketResearch);
    human.push_back(newfarmer);
    return newfarmer->getSelectNum();
}

int Player::addfarmer(int Camp, int BlockL, int BlockU)
{
    Farmer *newfarmer=new Farmer();
    newfarmer->init(Camp,BlockL,BlockU,marketResearch);
    human.push_back(newfarmer);
    return newfarmer->getSelectNum();
}

void Player::addmissile(Coordinate *Attackeder, Coordinate *Attacker, int Num, double L, double U, int L0, int U0)
{
    Missile *newmissile=new Missile();
    newmissile->init(Attackeder,Attacker,Num,L-71*gen5/4.0,U+71*gen5/4.0,L0,U0);
//    qDebug()<<"L:"<<newmissile->getL();
//    qDebug()<<"U:"<<newmissile->getU();
//    qDebug()<<"U:"<<U;
    missile.push_back(newmissile);
    return;
}

