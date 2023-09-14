#ifndef COORES_H
#define COORES_H

#include <Coordinate.h>

class Coores:public Coordinate
{
public:
    Coores();

    void nextframe();

    void init(int Num, double L, double U);
    void init(int Num, int BlockL, int BlockU);
    bool isBush()
    {
        return this->Num==COORES_BUSH;
    }
    bool isStone()
    {
        return this->Num==COORES_STONE;
    }
    bool isGoldore()
    {
        return this->Num==COORES_GOLDORE;
    }
    void reduceCnt()
    {
        this->Cnt-=this->getGatherSpeed();
        this->resetGatherSpeed();
    }
    double getCnt()
    {
        return this->Cnt;
    }
    int getNum()
    {
        return this->Num;
    }
    bool isBlockres()
    {
        return this->objectType==OBJECTTYPE_BLOCK;
    }
    static std::list<Resource>* getcoores(int i) {
        return coores[i];
    }

    static std::list<Resource>* getgraycoores(int i)
    {
        return graycoores[i];
    }

    static void setcoores(int i, std::list<Resource>* newValue) {
        coores[i] = newValue;
    }

    static void setgraycoores(int i,std::list<Resource>* newValue)
    {
        graycoores[i]=newValue;
    }

    static void allocatecoores(int i)
    {
        coores[i]=new std::list<Resource>;
    }
    static void allocategraycoores(int i)
    {
        graycoores[i]=new std::list<Resource>;
    }
    static void deallocatecoores(int i) {
        delete coores[i];
        coores[i] = nullptr;
    }
    static void deallocategraycoores(int i) {
        delete graycoores[i];
        graycoores[i] = nullptr;
    }
    static std::string getCooresDisplayName(int num)
    {
        return CooresDisplayName[num];
    }
    static std::string getCooresName(int num)
    {
        return Cooresname[num];
    }

private:
    int Num;

    double Cnt;

    double MaxCnt;

    int Percent;

    int objectType=OBJECTTYPE_COORDINATE;
    //1为坐标 0为块


    static std::list<Resource> *coores[3];
    static std::list<Resource> *graycoores[3];

    static std::string Cooresname[3];
    static std::string CooresDisplayName[3];

};

#endif // COORES_H
