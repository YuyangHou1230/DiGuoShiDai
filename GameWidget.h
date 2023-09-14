#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <MainWidget.h>
#include <QDebug>
#include <Coordinate.h>
#include <QMouseEvent>
#include <unordered_set>


namespace Ui {
class GameWidget;
}

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void judgeExplored(Coordinate *object);
    void judgeVisible(Coordinate *object);
    //坐标转换函数使用同一套

    int tranX(int L, int U);
    int tranY(int L,int U);
    int tranL(int X,int Y);
    int tranU(int X, int Y);
    void openVision(int BlockL,int BlockU,int Vision);
    void openVision(int BlockL,int BlockU);
    void setExplored(int BlockL, int BlockU);
    void insert(Coordinate *p,std::list<Coordinate*> *drawlist);
    void drawmemory(int X, int Y, Resource res, int selectNum);
    void emptymemorymap();
    int getBlockL()
    {
        return this->BlockL;
    }
    int getBlockU()
    {
        return this->BlockU;
    }
    bool judgeinWindow(double x,double y);
    int buildMode = 0;
    Coordinate* nowselect=NULL;
    std::list<Farmer*> *selectFarmer=new std::list<Farmer *>;
private slots:
    void movemap();
    void UpdateData();
    void toggleLineReceive(bool line);
    void togglePosReceive(bool pos);
signals:
    void sendClickPos(int BlockL, int BlockU);
    void returnselectNum(int selectNum);
    void rightpress(int selectNum,double L,double U);
//    void rightselectNum(int selectNum);
    void midpress();
    void sendView(int BlockL, int BlockU, int num);
    void startJudgeStop();
    void startUpdatePosition();
    void selectFarmerSignal(std::list<Farmer*> *selectFarmer);
private:
    Ui::GameWidget *ui;


    int BlockL=13;//左上角对应地图的坐标
    int BlockU=34;//在建筑绘制中需要参考这个变量

    //由于左上角有半个block且上边这个blockl就是那半个块所在位置
    //于是对应到相应的L,U中坐标应该为

    double L=(BlockL+0.5)*16*gen5;
    double U=(BlockU+0.5)*16*gen5;
    //边长为16倍根号5

    MainWidget *mainwidget;
    Option *option;
    int flag=0;
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;

    QPoint selectionStartPos;
    QPoint selectionEndPos;
    bool showLine = false;
    bool pos = false;

};

#endif // GAMEWIDGET_H
