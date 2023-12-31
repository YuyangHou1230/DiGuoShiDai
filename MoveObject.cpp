﻿#include "MoveObject.h"

MoveObject::MoveObject()
{
    this->canSpeed=1;
}

void MoveObject::nextframe()
{
    //还是应该在相应类下
    //根据当前nowres来进行下一帧
}

void MoveObject::calculateDiretionArray(stack<Point> &path)
{
    stack<Point> pathCopy = path;  // 复制一份路径
    this->pathN=0;
    Point prevPoint;  // 第一个点
    prevPoint.x=BlockL;
    prevPoint.y=BlockU;
//    qDebug()<<"prevPoint.x"<<prevPoint.x;
//    qDebug()<<"prevPoint.y"<<prevPoint.y;

    while (!pathCopy.empty()) {
        Point currentPoint = pathCopy.top();
//        qDebug()<<"currentPoint.x"<<currentPoint.x;
//        qDebug()<<"currentPoint.y"<<currentPoint.y;
        int dx = currentPoint.x - prevPoint.x;
        int dy = currentPoint.y - prevPoint.y;
        // 根据dx和dy计算方向值，并将其添加到方向数组d中
        if (dx == 1 && dy == -1) {
            d[pathN] = 0;  // 下
        } else if (dx == 0 && dy == -1) {
            d[pathN] = 1;  // 左下
        } else if (dx == -1 && dy == -1) {
            d[pathN] = 2;  // 左
        } else if (dx == -1 && dy == 0) {
            d[pathN] = 3;  // 左上
        }else if (dx == -1 && dy == 1) {
            d[pathN] = 4;  // 上
        }else if (dx == 0 && dy == 1) {
            d[pathN] = 5;  // 右上
        }else if (dx == 1 && dy == 1) {
            d[pathN] = 6;  // 右
        }else if (dx == 1 && dy == 0) {
            d[pathN] = 7;  // 右下
        }
//        qDebug()<<"pathN:"<<pathN;
//        qDebug()<<"dx:"<<dx;
//        qDebug()<<"dy:"<<dy;
//        qDebug()<<"d[pathN]:"<<d[pathN];
        prevPoint = currentPoint;
        pathCopy.pop();
        pathN++;
    }
}

int MoveObject::calculateAngle(double L0, double U0)
{
    int dL=L0-L;
    int dU=U0-U;
    int flag=Angle;
    if(abs(abs(dL)-abs(dU))>10)
    {
        if(abs(dL)>abs(dU))
        {
            if(dL>0)
            {
                flag=7;
            }
            else
            {
                flag=3;
            }
        }
        else
        {
            if(dU>0)
            {
                flag=5;
            }
            else
            {
                flag=1;
            }
        }
    }
    else
    {
        if(dL>0&&dU>0)
        {
            flag=6;
        }
        else if(dL>0&&dU<0)
        {
            flag=0;
        }
        else if(dL<0&&dU>0)
        {
            flag=4;
        }
        else
        {
            flag=2;
        }
    }
    return flag;
}


