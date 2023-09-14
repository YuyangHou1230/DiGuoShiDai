#ifndef HUMAN_H
#define HUMAN_H

#include <MoveObject.h>

class Human:public MoveObject
{
public:
    Human();
    ~Human();


    void init(int Num,int Camp,int L,int U);
    void nextframe();

private:

    std::string humanname[1]={"Clubman"};

    int Range;
    //攻击范围 周围一圈

    int AttackSpeed;
    //攻速

};

#endif // HUMAN_H
