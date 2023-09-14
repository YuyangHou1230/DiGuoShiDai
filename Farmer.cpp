#include "Farmer.h"

std::list<Resource>* Farmer::Carry[5][8];
std::list<Resource>* Farmer::Walk[7][8];
std::list<Resource>* Farmer::Work[7][8];
std::list<Resource>* Farmer::Stand[7][8];
std::list<Resource>* Farmer::Attack[7][8];
std::list<Resource>* Farmer::Die[7][8];


std::string Farmer::FarmerName[7]={"Villager","Lumber","Gatherer","Miner","Hunter","Farmer","Worker"};
std::string Farmer::FarmerCarry[5]={"","CarryWood","CarryMeat","CarryStone","CarryGold"};

Farmer::Farmer()
{
    this->sort=SORT_FARMER;
}

Farmer::~Farmer()
{
    delete[] d;
}



void Farmer::nextframe()
{
    std::list<Resource> *nowlist=NULL;

    if(!isDie())
    {
        if(this->isholdresource()&&(this->isGoToCamp()||this->isIdleing()))
        {
            nowlist=this->Carry[this->resourceSort][this->Angle];
        }
        else switch (this->nowstate) {
        case 0:
            nowlist=this->Stand[this->state][this->Angle];
            break;
        case 1:
            //            qDebug()<<"1";
            nowlist=this->Walk[this->state][this->Angle];
            break;
        case 2:
            nowlist=this->Attack[this->state][this->Angle];
            break;
        case 5:
            nowlist=this->Work[this->state][this->Angle];
            break;
            //    default:
            //        break;
        }
    }
    else if(!isDead())
    {
        nowres=this->Die[this->state][this->Angle]->begin();
        dead=1;
        nowlist=this->Die[this->state][this->Angle];
    }
    else
    {
        nowlist=this->Die[this->state][this->Angle];
    }


    if(destroy==0)
    {
        if(this->isholdresource()&&this->isStanding()&&!isDie())
        {
            //空的 即不切换动作
            //有点问题 没加入死后的判断
        }
        else
        {
            nowres++;
            if(nowres==nowlist->end())
            {
                if(isDead())
                {
//                    qDebug()<<"si";
                    destroy=1;
                    nowres--;
                    return;
                }
                nowres=nowlist->begin();
                //读到最后回到最初
            }
        }



        if(!isDie()&&isWalking())
        {
            if(L!=L0||U!=U0)
            {
                PreviousL=L;
                PreviousU=U;
                if(pathN==0)
                {
//                    qDebug()<<"pathN==0";
                    nextL=L0;
                    nextU=U0;
                    double dL=nextL-L;
                    double dU=nextU-U;
                    double dis = round(sqrt(dL*dL + dU*dU));  // 计算与目标之间的距离
                    // 根据速率计算每次的坐标变化量
                    double ratio = HUMAN_SPEED / static_cast<double>(dis);
                    VL = round(dL * ratio);
                    VU = round(dU * ratio);
                    if(countdistance(PreviousL,PreviousU,nextL,nextU)<HUMAN_SPEED)
                    {
                        PredictedL=nextL;
                        PredictedU=nextU;
                    }
                    else
                    {
                        PredictedL=PreviousL+VL;
                        PredictedU=PreviousU+VU;
                    }
//                    qDebug()<<"predictedL:"<<PredictedL;
//                    qDebug()<<"predictedU:"<<PredictedU;
                    int tempAngle=calculateAngle(nextL,nextU);
                    if(tempAngle!=Angle)
                    {
                        Angle=tempAngle;
                        setNowRes();
                    }
                }
                else if(pathI<pathN-1)
                {
//                    qDebug()<<"pathI<pathN-1";
//                    qDebug()<<pathI;
                    if(Angle!=d[pathI])
                    {
                        Angle=d[pathI];
                        setNowRes();
                    }
                    VL=VariationL[Angle]*HUMAN_SPEED;
                    VU=VariationU[Angle]*HUMAN_SPEED;
                    if(countdistance(PreviousL,PreviousU,nextL,nextU)<HUMAN_SPEED)
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
                    double ratio = HUMAN_SPEED / static_cast<double>(dis);
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
        else
        {
            VL=0;
            VU=0;
        }
    }
    this->BlockL=L/BLOCKSIDELENGTH;
    this->BlockU=U/BLOCKSIDELENGTH;
    //更新高度
    this->H=L-U;
}

void Farmer::init(int Camp, double L, double U, bool *market)
{
    this->camp=Camp;
    this->Angle=rand()%8;
    this->AttackValue=ATTACKVALUE_FARMER;
    this->MaxBlood=BLOOD_FARMER;
    this->Blood=MaxBlood;
    this->state=HUMAN_STATE_IDLE;
    this->resource=0;
    this->L=L;
    this->U=U;
    this->BlockL=L/BLOCKSIDELENGTH;
    this->BlockU=U/BLOCKSIDELENGTH;
    this->nextBlockL=BlockL;
    this->nextBlockU=BlockU;
    this->PredictedL=L;
    this->PredictedU=U;
    this->PreviousL=L;
    this->PreviousU=U;
    this->WoodSpeed=FARMER_GATHERSPEED_WOOD;
    this->StoneSpeed=FARMER_GATHERSPEED_STONE;
    this->GoldSpeed=FARMER_GATHERSPEED_GOLD;
    this->FoodSpeed=FARMER_GATHERSPEED_WOOD;
    this->WoodLimit=FARMER_CARRYLIMIT_WOOD;
    this->StoneLimit=FARMER_CARRYLIMIT_STONE;
    if(market[0]==true)
    {
        this->HuntRange=FARMER_UPGRADEHUNTRANGE;
        this->WoodLimit=FARMER_CARRYLIMIT_UPGRADEWOOD;
    }
    if(market[1]==true)
    {
        this->StoneLimit=FARMER_CARRYLIMIT_UPGRADESTONE;
    }
    this->ConstructSpeed=FARMER_CONSTRUCTSPEED;
    this->sort=SORT_FARMER;
    this->L0=L;
    this->U0=U;
    this->vision=VISION_FARMER;
    this->nowstate=MOVEOBJECT_STATE_STAND;
    this->Angle=0;
    setNowRes();
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    this->LU=this->LY*gen5;
    this->radius=LX;
    this->H=L-U;
    //    this->nowstate=2;
    this->selectNum=10000*sort+g_selectNum;
    g_Object.insert({this->selectNum,this});
    g_selectNum++;
}

void Farmer::init(int Camp,int BlockL,int BlockU,bool *market)
{
    this->camp=Camp;
    this->Angle=rand()%8;
    this->AttackValue=ATTACKVALUE_FARMER;
    this->MaxBlood=BLOOD_FARMER;
    this->Blood=MaxBlood;
    this->state=HUMAN_STATE_IDLE;
    this->resource=0;
    this->BlockL=BlockL;
    this->BlockU=BlockU;
    this->L=(BlockL+0.5)*BLOCKSIDELENGTH;
    this->U=(BlockU+0.5)*BLOCKSIDELENGTH;
    this->nextBlockL=BlockL;
    this->nextBlockU=BlockU;
    this->PredictedL=L;
    this->PredictedU=U;
    this->PreviousL=L;
    this->PreviousU=U;
    this->WoodSpeed=FARMER_GATHERSPEED_WOOD;
    this->StoneSpeed=FARMER_GATHERSPEED_STONE;
    this->GoldSpeed=FARMER_GATHERSPEED_GOLD;
    this->FoodSpeed=FARMER_GATHERSPEED_WOOD;
    this->WoodLimit=FARMER_CARRYLIMIT_WOOD;
    this->StoneLimit=FARMER_CARRYLIMIT_STONE;
    if(market[0]==true)
    {
        this->HuntRange=FARMER_UPGRADEHUNTRANGE;
        this->WoodLimit=FARMER_CARRYLIMIT_UPGRADEWOOD;
    }
    if(market[1]==true)
    {
        this->StoneLimit=FARMER_CARRYLIMIT_UPGRADESTONE;
    }
    this->ConstructSpeed=FARMER_CONSTRUCTSPEED;
    this->sort=SORT_FARMER;
    this->L0=L;
    this->U0=U;
    this->vision=VISION_FARMER;
    this->nowstate=MOVEOBJECT_STATE_STAND;
    this->Angle=0;
    setNowRes();
    this->LX=this->nowres->pix.width()/2.0;
    this->LY=this->nowres->pix.width()/4.0;
    this->LU=this->LY*gen5;
    this->radius=LX;
    this->H=L-U;
    //    this->nowstate=2;
    this->selectNum=10000*sort+g_selectNum;
    g_Object.insert({this->selectNum,this});
    g_selectNum++;
}

void Farmer::setNowRes()
{
    if(!isDie())
    {
        if(this->isholdresource()&&(this->isGoToCamp()||this->isIdleing()||this->isStop()))
        {
//            qDebug()<<this->resourceSort;
            this->nowres=this->Carry[this->resourceSort][this->Angle]->begin();
        }
        else
            switch (this->nowstate) {
            case MOVEOBJECT_STATE_STAND:
                this->nowres=this->Stand[this->state][this->Angle]->begin();
                break;
            case MOVEOBJECT_STATE_WALK:
                this->nowres=this->Walk[this->state][this->Angle]->begin();
                break;
            case MOVEOBJECT_STATE_ATTACK:
                this->nowres=this->Attack[this->state][this->Angle]->begin();
                break;
            case MOVEOBJECT_STATE_WORK:
                this->nowres=this->Work[this->state][this->Angle]->begin();
            default:
                break;
            }
    }

}
