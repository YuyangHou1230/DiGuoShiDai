#ifndef ANIMAL_H
#define ANIMAL_H

#include <MoveObject.h>

class Animal:public MoveObject
{
public:
    Animal();
    ~Animal();

    bool isRealAnimal()
    {
        return this->real==1;
    }
    void init(int Num,double L,double U);
    void init(int Num,int BlockL,int BlockU);
    void nextframe();
    bool isBlockres()
    {
        return this->objectType==OBJECTTYPE_BLOCK;
    }
    bool isTree()
    {
        return this->Num==ANIMAL_TREE;
    }
    bool isForest()
    {
        return this->Num==ANIMAL_FOREST;
    }
    bool isGazelle()
    {
        return this->Num==ANIMAL_GAZELLE;
    }
    bool isElephant()
    {
        return this->Num==ANIMAL_ELEPHANT;
    }
    bool isLion()
    {
        return this->Num==ANIMAL_LION;
    }
    int getState()
    {
        return this->state;
    }
    int getActionOccurrenceGameframe()
    {
        return this->ActionOccurrenceGameframe;
    }
    int getAttackCaused()
    {
        return this->AttackCaused;
    }
    int getAttackFinished()
    {
        return this->AttackFinished;
    }
    int getfriendly()
    {
        return this->Friendly;
    }
    int getBeAttacked()
    {
        return this->BeAttacked;
    }
    double getCnt()
    {
        return this->Cnt;
    }
    Coordinate *getObject()
    {
        return this->object;
    }
    void setIdle()
    {
        this->state=ANIMAL_STATE_IDLE;
    }
    void setRoaming()
    {
        this->state=ANIMAL_STATE_ROAMING;
    }
    void setFleeding()
    {
        this->state=ANIMAL_STATE_FLEEING;
    }
    void setChasing()
    {
        this->state=ANIMAL_STATE_CHASING;
    }
    void setAttacking()
    {
        this->state=ANIMAL_STATE_ATTACKING;
    }
    void setNowobject(Coordinate *p)
    {
        this->object=p;
    }
    void setNowRes();
    void setIsHostile()
    {
        this->Friendly=0;
    }
    void setActionOccurrenceGameframe(int gameframe)
    {
        this->ActionOccurrenceGameframe=gameframe;
    }
    void reduceCnt()
    {
        this->Cnt-=this->getGatherSpeed();
        this->Cnt-=this->DecayRate;
        this->resetGatherSpeed();
    }
    void setBeAttacked(int frame)
    {
        this->BeAttacked=frame;
    }
    bool isFriendly()
    {
        return this->Friendly==1;
    }
    bool isIdle()
    {
        return this->state==ANIMAL_STATE_IDLE;
    }
    bool isRoaming()
    {
        return this->state==ANIMAL_STATE_ROAMING;
    }
    bool isFleeing()
    {
        return this->state==ANIMAL_STATE_FLEEING;
    }
    bool isChasing()
    {
        return this->state==ANIMAL_STATE_CHASING;
    }
    bool isAttacking()
    {
        return this->state==ANIMAL_STATE_ATTACKING;
    }
    bool isAlive()
    {
        return this->aliveStatus==1;
    }
    static std::string getAnimalName(int index)
    {
            return Animalname[index];
    }
    static std::string getAnimalcarcassname(int index)
    {
            return Animalcarcassname[index];
    }
    static std::string getAnimalDisplayName(int index)
    {
        return AnimalDisplayName[index];
    }
    static std::list<Resource>* getRun(int i, int j) {
        return Run[i][j];
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
    static void setRun(int i, int j, std::list<Resource>* newValue) {
        Run[i][j] = newValue;
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
    static void allocateRun(int i, int j) {
        Run[i][j] = new std::list<Resource>;
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
    static void deallocateRun(int i, int j) {
        delete Run[i][j];
        Run[i][j] = nullptr;
    }

private:
    int real;
    //指示是否为真正意义上的动物，0为树，1为动物

    int Friendly=1;
    //友好度 1为友好 2为敌对

    int aliveStatus=1;
    //是否存活 1为不可采集 0为可以采集且不再具有其他功能

    double DecayRate=0;
    //腐烂速度

    double Cnt;
    //资源量

    int objectType=OBJECTTYPE_COORDINATE;
    //1为坐标 0为块

    int state=ANIMAL_STATE_IDLE;
    //当前动物的状态
    /*
     * 0 Idle       闲置状态 遍历人准备攻击或逃跑
     * 1 Roaming    随机移动
     * 2 Fleeing    逃跑
     * 3 Chasing    找到人追人
     * 4 Attacking  攻击中
     */

    int ActionOccurrenceGameframe;
    //记录动作开始的帧数

    int AttackCaused;
    //攻击造成帧

    int AttackFinished;
    //攻击完成帧

    int BeAttacked=0;
    //被攻击记录帧

    Coordinate *object=NULL;
    //当前攻击的对象

    static std::list<Resource> *Walk[5][8];

    static std::list<Resource> *Stand[5][8];

    static std::list<Resource> *Attack[5][8];

    static std::list<Resource> *Die[5][8];

    static std::list<Resource> *Run[5][8];

    static std::string Animalname[5];

    static std::string Animalcarcassname[5];

    static std::string AnimalDisplayName[5];
};

#endif // ANIMAL_H
