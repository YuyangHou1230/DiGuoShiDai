#ifndef MISSILE_H
#define MISSILE_H

#include <Coordinate.h>

class Missile:public Coordinate
{
public:
    Missile();
    ~Missile();
    void init(Coordinate *Attackeder,Coordinate *Attacker, int Num, int L, int U, int L0, int U0);
    void nextframe();
    bool isArrive();
    Coordinate *getAttackeder()
    {
        return this->Attackeder;
    }
    Coordinate *getAttacker()
    {
        return this->Attacker;
    }
    static std::list<Resource>* getmissile(int i) {
        return missile[i];
    }
    static void allocatemissile(int i)
    {
        missile[i]=new std::list<Resource>;
    }
    static void deallocatemissile(int i) {
        delete missile[i];
        missile[i] = nullptr;
    }
    static std::string getMissilename(int index)
    {
        return Missilename[index];
    }


private:

    int Num;

    int Angle=0;
    //方向 度数形式

    double speed=5;
    //速率
    //由速率来确定相应的方向的速度

//    double variationL=0;
//    double variationU=0;
//    //LU的变化量

    Coordinate *Attackeder=NULL;
    Coordinate *Attacker=NULL;

    int L0=0;
    int U0=0;
    //目的地 到地销毁

    static std::list<Resource> *missile[1];

    static std::string Missilename[1];

};

#endif // MISSILE_H
