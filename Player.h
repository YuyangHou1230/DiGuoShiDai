#ifndef PLAYER_H
#define PLAYER_H

#include <Building.h>
#include <Ruin.h>
#include <Farmer.h>
#include <Missile.h>

class Building;
class Human;
class Ruin;
class Farmer;
class Missile;

class Player
{
public:
    Player();
    ~Player();

    Building *addbuilding(int Num,int BlockL,int BlockU,int Percent);//添加准备要建的建筑
    void addhuman(int Camp, int Num, int L, int U);
    int addfarmer(int Camp, double L, double U);
    int addfarmer(int Camp,int BlockL,int BlockU);
    void addmissile(Coordinate *Attackeder,Coordinate *Attacker, int Num, double L, double U, int L0, int U0);
    void addResource(int resourceSort,int resource)
    {
        switch (resourceSort) {
        case 1:
            this->wood+=resource;
            break;
        case 2:
            this->food+=resource;
            break;
        case 3:
            this->stone+=resource;
            break;
        case 4:
            this->gold+=resource;
            break;
        case 5:
            this->food+=resource;
            break;
        default:
            break;
        }

    }
    int getWood()
    {
        return this->wood;
    }
    int getFood()
    {
        return this->food;
    }
    int getStone()
    {
        return this->stone;
    }
    int getGold()
    {
        return this->gold;
    }
    bool *getmarketResearch()
    {
        return this->marketResearch;
    }
    void setWood(int wood)
    {
        this->wood = wood;
    }
    void setFood(int food)
    {
        this->food = food;
    }
    void setStone(int stone)
    {
        this->stone = stone;
    }
    void setGold(int gold)
    {
        this->gold = gold;
    }
    void setCiv(int civ)
    {
        this->civilization = civ;
    }
    int getCiv()
    {
        return this->civilization;
    }
    bool getArrowTowerUnlocked()
    {
        return this->isArrowTowerUnlocked;
    }
    void setArrowTowerUnlocked(bool flag)
    {
        this->isArrowTowerUnlocked = flag;
    }
    bool getMarketResearch(int num)
    {
        return marketResearch[num];
    }
    void setMarketResearch(int num, bool flag)
    {
        this->marketResearch[num] = flag;
    }
    int getScore()
    {
        return this->score;
    }
    void setScore(int score)
    {
        this->score = score;
    }
    int getMaxHumanNum()
    {
        return this->maxHumanNum;
    }
    void setMaxHumanNum(int num)
    {
        this->maxHumanNum = num;
    }
    void setCheatMaxHumanNum(bool flag)
    {
        this->cheatMaxHumanNum = flag;
    }
    bool getCheatMaxHumanNum()
    {
        return this->cheatMaxHumanNum;
    }
    //建筑池子
    std::list<Building *> build;

    //投掷物池子
    std::list<Missile *> missile;

    //人口池子
    std::list<Human *> human;
private:
    //当前文明
    int civilization=1;

    //人口容量

    //所拥有的四个资源
    int wood=200;
    int food=200;
    int stone=150;
    int gold=0;
    bool isArrowTowerUnlocked = false;
    bool marketResearch[3] = {false};
    int score=0;
    int maxHumanNum=0;
    bool cheatMaxHumanNum=false;
};

#endif // PLAYER_H
