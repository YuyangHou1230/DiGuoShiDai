#ifndef MOVEOBJECT_H
#define MOVEOBJECT_H

#include <Coordinate.h>
#include <stack>


class MoveObject:public Coordinate
{
public:
    MoveObject();

    void nextframe();

    int getPreState()
    {
        return this->prestate;
    }
    int getNowState()
    {
        return this->nowstate;
    }
    int getNum()
    {
        return this->Num;
    }
    double getL0()
    {
        return this->L0;
    }
    double getU0()
    {
        return this->U0;
    }
    int getnextBlockL()
    {
        return this->nextBlockL;
    }
    int getnextBlockU()
    {
        return this->nextBlockU;
    }
    int getAngle()
    {
        return this->Angle;
    }
    int getBlood()
    {
        return this->Blood;
    }
    int getAttackSpeed()
    {
        return this->AttackSpeed;
    }
    int getAttackValue()
    {
        return this->AttackValue;
    }
    int getAttackRange()
    {
        return this->AttackRange;
    }
    double getnextL()
    {
        return this->nextL;
    }
    double getnextU()
    {
        return this->nextU;
    }
    void calculateDiretionArray(stack<Point>& path);
    int calculateAngle(double L0,double U0);
    double getVL()
    {
        return VL;
    }
    double getVU()
    {
        return VU;
    }
    double getPredictedL()
    {
        return this->PredictedL;
    }
    double getPredictedU()
    {
        return this->PredictedU;
    }
    void setVariationLU(int SPEED);
    void setAngle(int Angle)
    {
        if(this->Angle!=Angle)
        {

        }
        this->Angle=Angle;
    }
    void setdestination(double L0,double U0)
    {
        setL0(L0);
        setU0(U0);
        this->BlockL=L/16/gen5;
        this->BlockU=U/16/gen5;
        //更新高度
        this->H=L-U;
    }
    void setPreStand()
    {
        this->prestate=0;
    }
    void setPreWalk()
    {
        this->prestate=1;
    }
    void setPreAttack()
    {
        this->prestate=2;
    }
    void setPreWork()
    {
        this->prestate=5;
    }
    void setPreRun()
    {
        this->prestate=6;
    }
    void setNowState(int PreState)
    {
        this->nowstate=PreState;
    }
    void setPreStateIsIdle()
    {
        this->prestate=-1;
    }
    void reduceBlood(int Blood)
    {
        this->Blood-=Blood;
    }
    void setL0(double L0)
    {
        if(L0<0)
        {
            this->L0=0;
        }
        else if(L0>MAP_L*BLOCKSIDELENGTH)
        {
            this->L0=MAP_L*BLOCKSIDELENGTH-1;
        }
        else
        {
            this->L0=L0;
        }
    }
    void setU0(double U0)
    {
        if(U0<0)
        {
            this->U0=0;
        }
        else if(U0>MAP_U*BLOCKSIDELENGTH)
        {
            this->U0=MAP_U*BLOCKSIDELENGTH-1;
        }
        else
        {
            this->U0=U0;
        }
    }
    void setPath(stack<Point> path)
    {
        this->path=path;
        resetpathIN();
        nextBlockL=BlockL;
        nextBlockU=BlockU;
        calculateDiretionArray(path);
        if(!path.empty())
        setNextBlock();
    }
    void setNextBlock()
    {
        Point p=path.top();
        int lastBlockL=nextBlockL;
        int lastBlockU=nextBlockU;
        this->nextBlockL=p.x;
        this->nextBlockU=p.y;
        this->nextL=L+(nextBlockL-lastBlockL)*BLOCKSIDELENGTH;
        this->nextU=U+(nextBlockU-lastBlockU)*BLOCKSIDELENGTH;
        path.pop();
    }
    void resetpathIN()
    {
        this->pathI=0;
        this->pathN=0;
    }
    void GoBackLU()
    {
        this->PredictedL=PreviousL;
        this->PredictedU=PreviousU;
    }
    void updateLU()
    {
        this->L=PredictedL;
        this->U=PredictedU;
    }
    bool CoordinateMovement(int SPEED);
    bool isWalking()
    {
        return this->nowstate==1;
    }
    bool isRunning()
    {
        return this->nowstate==6;
    }
    bool needTranState()
    {
        return this->prestate!=-1;
    }

    bool isDead()
    {
        return this->dead==1;
    }
    bool isStanding()
    {
        return this->nowstate==0;
    }
    bool isAttack()
    {
        return this->nowstate==2;
    }
    bool haveReachNextBlock()
    {
        if(this->nextBlockL==this->BlockL&&this->nextBlockU==this->BlockU)
        {
            //            qDebug()<<"格子相等";
            if(L==nextL&&U==nextU)
            {
                return 1;
            }
            else return 0;
        }
        else return 0;
    }
    bool reachDestination()
    {
        if((isWalking()||isRunning())&&L0==L&&U0==U)
        {
            return 1;
        }
        else return 0;
    }





protected:
    int Num;


    int Angle;
    //规定 从下顺时针分别为0 1 2 3 4 5 6 7

    int AttackValue;
    //攻击力

    int AttackSpeed;
    //攻速

    int AttackRange;
    //攻击距离

    double nextL;
    double nextU;
    //下一个格子中的具体位置

    int nextBlockL;
    int nextBlockU;
    //下一个格子的坐标位置

    double VL=0;
    double VU=0;
    //ΔL ΔU

    double PreviousL;
    double PreviousU;
    //上一位置

    double PredictedL;
    double PredictedU;
    //预判位置

    double L0;
    double U0;//目的地

    stack<Point> path;
    //记录路径

    int *d=new int[6400];
    //计算好的路径方向



    // 单位向量
    double VariationL[8] = {1 / sqrt(2), 0, -1 / sqrt(2), -1, -1 / sqrt(2), 0, 1 / sqrt(2), 1};
    double VariationU[8] = {-1 / sqrt(2), -1, -1 / sqrt(2), 0, 1 / sqrt(2), 1, 1 / sqrt(2), 0};



    int pathN=0;
    //路径长度

    int pathI=0;
    //当前进行到路径的哪一步

    int dead=0;
    //濒死阶段
    //1为已死 0为未死

    //stand walk attack die disappear work run
    int nowstate=0;//当前的状态
    int prestate=-1;//准备开始的状态 指示状态的切换

};

#endif // MOVEOBJECT_H
