#ifndef COORDINATE_H
#define COORDINATE_H

#include <QPixmap>
#include <QDebug>
#include <config.h>
#include <QImage>
#include <GlobalVariate.h>

class MainWidget;

class Coordinate:public QObject
{
public:
    Coordinate();
    ~Coordinate();

    virtual void nextframe();

    double getL()
    {
        return this->L;
    }
    double getU()
    {
        return this->U;
    }
    double getLX()
    {
        return this->LX;
    }
    double getLY()
    {
        return this->LY;
    }
    double getLU()
    {
        return this->LU;
    }
    double getRadius()
    {
        return this->radius;
    }
    int getH()
    {
        return this->H;
    }
    double getGatherSpeed()
    {
        return this->GatherSpeed;
    }
    double getConstructSpeed()
    {
        return this->ConstructSpeed;
    }
    int getCamp()
    {
        return this->camp;
    }
    int getSort()
    {
        return this->sort;
    }
    int getProductSort()
    {
        return this->ProductSort;
    }
    int getVision()
    {
        return this->vision;
    }
    int getBlockL()
    {
        return this->BlockL;
    }
    int getBlockU()
    {
        return this->BlockU;
    }
    int getType()
    {
        return this->type;
    }
    std::list<Resource>::iterator getNowRes()
    {
        return this->nowres;
    }
    int getSelectNum()
    {
        return this->selectNum;
    }
    bool isBuilding()
    {
        if(this!=NULL)
        return this->sort==SORT_BUILDING;
    }
    bool isCoores()
    {
        if(this!=NULL)
        return this->sort==SORT_COORES;
    }
    bool isFarmer()
    {
        if(this!=NULL)
        return this->sort==SORT_FARMER;
    }
    bool isHuman()
    {
        if(this!=NULL)
        return this->sort==SORT_HUMAN;
    }
    bool isAnimal()
    {
        if(this!=NULL)
        return this->sort==SORT_ANIMAL;
    }
    bool isMoveObject()
    {
        if(this!=NULL)
        if(this->sort==SORT_FARMER||this->sort==SORT_HUMAN||this->sort==SORT_ANIMAL)
        {
            return 1;
        }
        else return 0;
    }
    bool isTreeForest()
    {
        if(this!=NULL)
        return this->sort==SORT_TREEFOREST;
    }
    bool isMissile()
    {
        if(this!=NULL)
        return this->sort==SORT_MISSILE;
    }
    bool isFarm()
    {
        if(this!=NULL)
        return this->sort==SORT_FARM;
    }
    bool ProductIsWood()
    {
        return this->ProductSort==PRODUCTSORT_WOOD;
    }
    bool ProductIsFood()
    {
        return this->ProductSort==PRODUCTSORT_STOCKFOOD||this->ProductSort==PRODUCTSORT_GRANARYFOOD;
    }
    bool ProductIsStone()
    {
        return this->ProductSort==PRODUCTSORT_STONE;
    }
    bool ProductIsGold()
    {
        return this->ProductSort==PRODUCTSORT_GOLD;
    }
    bool isVisible()
    {
        return this->visible==1;
    }
    bool isExplored()
    {
        return this->explored==1;
    }
    bool isDie()
    {
        return this->Blood<=0;
    }
    bool isBlockFoundation()
    {
        return this->type==4;
    }
    bool isMiddleFoundation()
    {
        return this->type==1;
    }
    bool isSmallFoundation()
    {
        return (this->type==0||this->type==3);
    }
    bool isBigFoundation()
    {
        return this->type==2;
    }
    bool isDestroy()
    {
        return this->destroy==1;
    }
    bool isinWindow()
    {
        return this->inWindow==1;
    }
    bool needDieInformation()
    {
        return this->DieInformation==0;
    }
    void setIsExplored()
    {
        this->explored=1;
    }
    void setIsVisible()
    {
        this->visible=1;
    }
    void setIsUnVisible()
    {
        this->visible=0;
    }
    void resetGatherSpeed()
    {
        this->GatherSpeed=0;
    }
    void resetConstructSpeed()
    {
        this->ConstructSpeed=0;
    }
    void addGatherSpeed(double speed)
    {
        this->GatherSpeed+=speed;
    }
    void addConstructSpeed(double speed)
    {
        this->ConstructSpeed+=speed;
    }
    void reduceBlood(int Blood)
    {
        this->Blood-=Blood;
    }
    void setActSpeed(double actSpeed)
    {
        this->actSpeed = actSpeed;
    }
    void setActPercent(double actPercent)
    {
        this->actPercent = actPercent;
    }
    void updatePercent()
    {
        this->setActPercent(actPercent + actSpeed);
    }
    void setisinWindow()
    {
        this->inWindow=1;
    }
    void setnotinWindow()
    {
        this->inWindow=0;
    }
    double getActPercent()
    {
        return this->actPercent;
    }
    double getActSpeed()
    {
        return this->actSpeed;
    }
    void setActName(int actName)
    {
        this->actName = actName;
    }
    int getActName()
    {
        return this->actName;
    }
    int getBlood()
    {
        return this->Blood;
    }
    int getMaxBlood()
    {
        return this->MaxBlood;
    }
    int getActNum()
    {
        return this->actNum;
    }
    void setActNum(int actNum)
    {
        this->actNum = actNum;
    }
    void setDieInformation()
    {
        this->DieInformation=1;
    }
    //转移到Coordinate类下
    double tranX(double L,double U);
    double tranY(double L,double U);
    double tranBlockL(double L);
    double tranBlockU(double U);

protected:

    double L;//当前物体中心所在的坐标位置
    double U;//在块类中该坐标即为正中心
    //此LU所指游戏中坐标

    int BlockL;
    int BlockU;
    //对象所在的区块

    double LX;//该物体的长宽（即占地面积）
    double LY;//需要根据占地大小来就算确切的绘制偏移量

    double LU;
    //占地大小转换成游戏内坐标 边长

    double radius=0;
    //半径 对于坐标类对象需要采集时的参数

    int type;
    //为1时为大建筑3*3，为0时为小建筑2*2 通过Num查找

    int Blood=100;
    int MaxBlood=100;

//    int pixX;
//    int pixY;
//    //用作记录相应资源在pixmap中的偏移量（即中心相对于左上角的距离）

    int H;//该物体在平面中的上下位置


    double GatherSpeed=0;
    //资源的减少速度

    double ConstructSpeed=0;
    //建筑的建造速度

    int selectNum;
    //每个对象所拥有的独一无二的num，用来确定具体对象

    int camp;
    //当前对象所属阵营 0~7

    int vision;
    //视野

    int explored=0;
    //0为未探索 1为探索

    int visible=0;
    //0为不可见 1为可见

    int inWindow=0;
    //在游戏窗口内

    int sort=0;
    //所属类别，用来确定当前指针具体的类别
    //0为基类 1为建筑 2为资源 4为human 5为farmer

    int ProductSort;
    //产出资源类型

    int destroy=0;
    //销毁信号
    //0为正常状态 可以进行操作
    //1为产生废墟指示

    int DieInformation=0;
    //死亡信息提示 0为未提示 1为已提示

    int actNum=0;
    //动作类型的编号
    MainWidget *mainwidget;
    //游戏进程窗口，用来获取和修正相关信息

    int canSpeed=0;//是否能由玩家操纵移动，能为1
    double actPercent = 0;
    double actSpeed = 0;
    int actName = 0;

    std::list<Resource>::iterator nowres;
//    QPixmap *nowres=NULL;
//    Resource *nowres=new Resource;

};

#endif // COORDINATE_H
