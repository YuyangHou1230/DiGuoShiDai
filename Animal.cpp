#include "Animal.h"

std::string Animal::Animalname[5]={"Tree","Gazelle","Elephant","Lion","Forest"};
std::string Animal::Animalcarcassname[5]={"Fallen_Tree","Gazelle","Elephant","Lion","Forest_Stool"};
std::string Animal::AnimalDisplayName[5]={"树","瞪羚","大象","狮子","树林"};
std::list<Resource>* Animal::Walk[5][8];
std::list<Resource>* Animal::Stand[5][8];
std::list<Resource>* Animal::Attack[5][8];
std::list<Resource>* Animal::Die[5][8];
std::list<Resource>* Animal::Run[5][8];


Animal::Animal()
{

}

Animal::~Animal()
{
    delete[] d;
}

void Animal::init(int Num, double L, double U)
{
    this->Num=Num;
    this->sort=SORT_ANIMAL;
    this->ProductSort=PRODUCTSORT_STOCKFOOD;
    this->L=L;
    this->U=U;
    this->L0=L;
    this->U0=U;
    this->BlockL=L/BLOCKSIDELENGTH;
    this->BlockU=U/BLOCKSIDELENGTH;
    this->nextBlockL=BlockL;
    this->nextBlockU=BlockU;
    this->PredictedL=L;
    this->PredictedU=U;
    this->PreviousL=L;
    this->PreviousU=U;
    this->H=L-U;
    this->real=1;
    this->GatherSpeed=0;
    this->object=NULL;
    if(Num==ANIMAL_GAZELLE)
    {
        this->Cnt=CNT_GAZELLE;
        this->MaxBlood=BLOOD_GAZELLE;
        this->vision=VISION_GAZELLE;
    }
    if(Num==ANIMAL_ELEPHANT)
    {
        MaxBlood=BLOOD_ELEPHANT;
        this->Cnt=CNT_ELEPHANT;
        this->AttackSpeed=10;
        this->AttackCaused=6;
        this->AttackValue=5;
        this->vision=VISION_ELEPHANT;
        this->AttackRange=ANIMAL_ATTACKRANGE_ELEPHANT;
    }
    else if(Num==ANIMAL_LION)
    {
        this->Friendly=0;
        MaxBlood=BLOOD_LION;
        this->Cnt=CNT_LION;
        this->AttackSpeed=20;
        this->AttackCaused=6;
        this->AttackValue=2;
        this->vision=VISION_LION;
        this->AttackRange=ANIMAL_ATTACKRANGE_LION;
    }
    this->Blood=this->MaxBlood;
    this->Angle=rand()%8;
    this->nowstate=ANIMAL_STATE_IDLE;
    setNowRes();
    this->selectNum=this->sort*10000+g_selectNum;
    g_Object.insert({this->selectNum,this});
    g_selectNum++;
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    this->LU=this->LY*gen5;
    this->radius=LX;

}

void Animal::init(int Num, int BlockL, int BlockU)
{
    this->Num=Num;
    if(Num==ANIMAL_TREE)//作为树来理解
    {
        this->sort=SORT_TREEFOREST;
        this->ProductSort=PRODUCTSORT_WOOD;
        this->objectType=OBJECTTYPE_BLOCK;
        this->real=0;
        this->type=FOUNDATION_BLOCK;
        this->BlockL=BlockL;
        this->BlockU=BlockU;
        this->L=(BlockL+0.5)*BLOCKSIDELENGTH;
        this->U=(BlockU+0.5)*BLOCKSIDELENGTH;
        this->L0=L;
        this->U0=U;
        this->PredictedL=L;
        this->PredictedU=U;
        this->PreviousL=L;
        this->PreviousU=U;
        this->H=L-U;
        this->GatherSpeed=0;
        this->MaxBlood=BLOOD_TREE;
        this->Blood=this->MaxBlood;
        this->Cnt=CNT_TREE;
        this->nowres=this->Stand[this->Num][0]->begin();
        int index=rand()%10;//因为有十张图 随机一张
        std::advance(nowres,index);
        this->selectNum=this->sort*10000+g_selectNum;
        g_Object.insert({this->selectNum,this});
        g_selectNum++;
        this->LX=this->nowres->pix.width()/2.0;
        this->LY=this->nowres->pix.width()/4.0;
        this->LU=BLOCKSIDELENGTH;
        this->radius=LX;
    }
}


void Animal::nextframe()
{

    if(this->isAnimal())
    {
        std::list<Resource> *nowlist=NULL;

        if(!isDie())
        {
            switch (this->nowstate) {
            case 0:
                nowlist=this->Stand[this->Num][this->Angle];
                break;
            case 1:
                nowlist=this->Walk[this->Num][this->Angle];
                break;
            case 2:
                nowlist=this->Attack[this->Num][this->Angle];
                break;
            case 3:
                nowlist=this->Die[this->Num][this->Angle];
                break;
            case 6:
                nowlist=this->Run[this->Num][this->Angle];
                break;
            }
        }
        else if(!isDead())
        {
//            qDebug()<<"设置死亡动画队列";
            this->nowstate=3;
            nowres=this->Die[this->Num][this->Angle]->begin();
            dead=1;
            nowlist=this->Die[this->Num][this->Angle];
        }
        else
        {
            nowlist=this->Die[this->Num][this->Angle];
        }
        if(destroy==0)//没被销毁正常状态
        {
            if(aliveStatus==0)//可供采集的情况下
            {
                if(Cnt<=0)//资源量为0了
                {
                    destroy=1;//设置待销毁
                    return;
                }
            }
            else
            {
                if(aliveStatus==1)
                nowres++;
                if(nowres==nowlist->end())
                {
                    if(this->Blood<this->MaxBlood)
//                    qDebug()<<"当前进行到最后了";
                    if(isDie())
                    {
//                        qDebug()<<"进行到最后的同时动物也死了";
                        this->aliveStatus=0;
                        nowres--;
                        return;
                    }
                    nowres=nowlist->begin();
                    //读到最后回到最初
                }
            }

        }
        //设置Δx Δy部分
        if(!isDie()&&(isWalking()||isRunning()))
        {
            if(L!=L0||U!=U0)
            {
                double VL;
                double VU;
                PreviousL=L;
                PreviousU=U;
                if(pathN==0)
                {
//                    qDebug()<<"pathN==0";
                    nextL=L0;
                    nextU=U0;
                    double dL=nextL-PreviousL;
                    double dU=nextU-PreviousU;
                    double dis = round(sqrt(dL*dL + dU*dU));  // 计算与目标之间的距离
                    // 根据速率计算每次的坐标变化量
                    double ratio;
                    if(isRunning())
                    {
                        ratio = ANIMAL_SPEED / static_cast<double>(dis);
                    }
                    else if(isWalking())
                    {
                        ratio = ANIMAL_SPEED / static_cast<double>(dis);
                    }

                    VL = round(dL * ratio);
                    VU = round(dU * ratio);
                    if(countdistance(L,U,nextL,nextU)<HUMAN_SPEED)
                    {
                        PredictedL=nextL;
                        PredictedU=nextU;
                    }
                    else
                    {
                        PredictedL=PreviousL+VL;
                        PredictedU=PreviousU+VU;
                    }
                    int tempAngle=calculateAngle(nextL,nextU);
                    if(tempAngle!=Angle)
                    {
                        Angle=tempAngle;
                        setNowRes();
                    }
                }
                else if(pathI<pathN-1)
                {

                    if(Angle!=d[pathI])
                    {
                        Angle=d[pathI];
                        setNowRes();
                    }
                    VL=VariationL[Angle]*ANIMAL_SPEED;
                    VU=VariationU[Angle]*ANIMAL_SPEED;
                    if(countdistance(L,U,nextL,nextU)<HUMAN_SPEED)
                    {
                        PredictedL=nextL;
                        PredictedU=nextU;
                    }
                    else
                    {
                        PredictedL=PreviousL+VL;
                        PredictedU=PreviousU+VU;
                    }
                    if(fabs(nextL-L)<1&&fabs(nextU-U)<1)
                    {
//                        qDebug()<<"setNextBlock";
                        setNextBlock();
                        pathI++;
                    }
                }
                else if(pathI==pathN-1)
                {
//                    qDebug()<<"pathI==pathN-1";
                    if(Angle!=d[pathI])
                    {
                        Angle=d[pathI];
                        setNowRes();
                    }
                    nextL=L0;
                    nextU=U0;
                    double dL=nextL-L;
                    double dU=nextU-U;
                    double dis = round(sqrt(dL*dL + dU*dU));  // 计算与目标之间的距离
                    // 根据速率计算每次的坐标变化量
                    double ratio;
                    if(isRunning())
                    {
                        ratio = ANIMAL_SPEED / static_cast<double>(dis);
                    }
                    else if(isWalking())
                    {
                        ratio = ANIMAL_SPEED / static_cast<double>(dis);
                    }
                    VL = round(dL * ratio);
                    VU = round(dU * ratio);
                    if(countdistance(L,U,nextL,nextU)<HUMAN_SPEED)
                    {
                        PredictedL=nextL;
                        PredictedU=nextU;
                    }
                    else
                    {
                        PredictedL=PreviousL+VL;
                        PredictedU=PreviousU+VU;
                    }
                    if(fabs(nextL-L)<1&&fabs(nextU-U)<1)
                    {
                        pathI=0;
                        pathN=0;
                    }
                }
            }

        }
        this->BlockL=L/BLOCKSIDELENGTH;
        this->BlockU=U/BLOCKSIDELENGTH;
    }
    if(this->isTree())
    {
        if(isDie()&&!isDead())
        {
            aliveStatus=0;
            nowres=this->Die[this->Num][0]->begin();
            dead=1;
            int index=rand()%2;
            std::advance(nowres,index);
        }
        if(destroy==0)
        {
            if(aliveStatus==0&&Cnt<=0)
            {
                destroy=1;
            }
        }
    }
    if(this->isForest())
    {
        if(isDie()&&!isDead())
        {
            aliveStatus=0;
            nowres=this->Die[this->Num][0]->begin();
            dead=1;
            int index=rand()%5;
            std::advance(nowres,index);
        }
        if(destroy==0)
        {
            if(aliveStatus==0&&Cnt<=0)
            {
                destroy=1;
            }
        }
    }
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    //更新高度
    this->H=L-U;
}

void Animal::setNowRes()
{

    switch (this->nowstate) {
    case 0:
        this->nowres=this->Stand[this->Num][this->Angle]->begin();
        break;
    case 1:
        this->nowres=this->Walk[this->Num][this->Angle]->begin();
        break;
    case 2:
        this->nowres=this->Attack[this->Num][this->Angle]->begin();
        break;
    case 3:
        this->nowres=this->Die[this->Num][this->Angle]->begin();
        break;
    case 6:
        this->nowres=this->Run[this->Num][this->Angle]->begin();
    default:
        break;
    }
}
