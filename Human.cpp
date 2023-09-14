#include "Human.h"

Human::Human()
{
    this->sort=SORT_HUMAN;
}

Human::~Human()
{

}


void Human::init(int Num, int Camp, int L, int U)
{
//    this->Angle=0;
//    this->Num=Num;
//    this->camp=Camp;
//    this->L=L;
//    this->U=U;

//    this->H=L-U+127*16*gen5;
////    Walk[0]=&resMap[humanname[this->Num]+"_down"];//0 向下
////    Walk[1]=&resMap[humanname[this->Num]+"_leftdown"];//1 左下
////    Walk[2]=&resMap[humanname[this->Num]+"_left"];//2 左
////    Walk[3]=&resMap[humanname[this->Num]+"_leftup"];//3 左上
////    Walk[4]=&resMap[humanname[this->Num]+"_up"];//4 上
////    for(int i=5;i<8;i++)
////    {
////        std::list<QPixmap>::iterator iter=Walk[8-i]->begin();
////        std::list<QPixmap> *a=new std::list<QPixmap>;
////        while(iter!=Walk[8-i]->end())
////        {
////            QImage image=(*iter).toImage();
////            image=image.mirrored(true,false);
////            QPixmap pix=QPixmap::fromImage(image);
////            iter++;
////            a->push_back(pix);
////            //            Move[i]->push_back(pix);//没有对象导致运行崩溃
////        }
////        Walk[i]=a;
////    }


//    //    nowres=Stand[Angle]->begin();
//    this->nowres=Walk[Angle]->begin();
//    this->LX=this->nowres->pix.width()/2.0;
//    this->LY=this->nowres->pix.width()/4.0;
//    this->selectNum=10000*sort+g_selectNum;
//    g_Object.insert({this->selectNum,this});
//    g_selectNum++;
}

void Human::nextframe()
{

}


