#ifndef RUIN_H
#define RUIN_H

#include <Coordinate.h>

class Ruin:public Coordinate
{
public:
    Ruin();
//    ~Ruin();

    void nextframe();

    void init(int Num, double L, double U);

    static std::list<Resource>* getruin(int i) {
        return ruin[i];
    }
    static void allocateruin(int i)
    {
        ruin[i]=new std::list<Resource>;
    }
    static void deallocateruin(int i) {
        delete ruin[i];
        ruin[i] = nullptr;
    }
    static std::string getRuinname(int index)
    {
        return Ruinname[index];
    }

    static std::list<Resource> *ruin[3];

    static std::string Ruinname[3];

    int Num;

    int time;
    //产生的时间->更改为存在的时间

};

#endif // RUIN_H
