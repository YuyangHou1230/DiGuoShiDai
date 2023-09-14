#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPainter>
#include <Map.h>
#include <qtimer.h>
#include <Player.h>
#include <QTextBrowser>
#include <GlobalVariate.h>
#include <config.h>
#include <Animal.h>
#include <QElapsedTimer>
#include <Missile.h>
#include <stack>
#include <queue>
#include <QButtonGroup>
#include <QMessageBox>
#include "AI.h"
#include <windows.h>
#include <unordered_set>
#include <AboutDialog.h>
#include <Option.h>

extern QTextBrowser* g_DebugText;

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(bool MapJudge, QWidget *parent = 0);
    ~MainWidget();

    void paintEvent(QPaintEvent *);
    QString getShowTime();
    void addCoores(int Num,double L,double U);
    void addGameres(int Num,int BlockL,int BlockU);
    void addAnimal(int Num,double L,double U);
    void addTree(int Num,int BlockL,int BlockU);
    void addruin(int Num, int L, int U);//添加产生的废墟
    void initmap();
    void refreshActs();
    void statusUpdate();
    bool eventFilter(QObject *watched, QEvent *event);
    Building *findGranary(Farmer *farmer);
    Building *findStock(Farmer *farmer);
    void movetoCoo(MoveObject *moveobject, Coordinate *p);
    void movetoGameres(Human *human,Coordinate *p);
    void replaceobject(Coordinate *p, Coordinate *replaceobj);
    double tranL(double BlockL);
    double tranU(double BlockU);
    int tranBlockL(double L)
    {
        return L/16/gen5;
    }
    int tranBlockU(double U)
    {
        return U/16/gen5;
    }
    bool isValidPoint(const int (&map)[MAP_L][MAP_U], const Point& p);
    vector<Point> getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point& p);
    stack<Point> findPath(const int (&findPathMap)[MAP_L][MAP_U], const Point& start, const Point& destination);
    stack<Point> findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point& start, const Point& destination);
    void judgestate();
    void updateSelectobject();
    int findAngle(MoveObject *p);
    bool judgementAttack(Coordinate *Attacker,Coordinate *Attackered);
    bool isWinning()
    {
        return this->Winnning==1;
    }
    void setWinning()
    {
        this->Winnning=1;
    }
    void judgeArrowTowerPosition(int BlockL, int BlockU);
    bool isLose()
    {
        return this->Lose==1;
    }
    void setLose()
    {
        this->Lose=1;
    }

    void actionUpdate();
    void loadCollisionObject();
    void loadCollisionMap(int BlockL, int BlockU);
    void loadfindPathMap();
    int usefindPathMap(int BlockL,int BlockU);
    void clearCollisionMap();
    void clearfindPathMap();
    void clearCollisionObject();
    bool judgeCollision(int SN,double L,double U);
    void inittagGame(tagGame *game);
    void set_Game();
    void judgeVictory();
    void debugText(const QString& color, const QString& content);
    void initFarmer();
    void initAnimal();
    void initBuilding();
    void initCoores();
    void initBlock();
    void initMissile();
    void initRuin();
    void deleteFarmer();
    void deleteAnimal();
    void deleteBuilding();
    void deleteCoores();
    void deleteBlock();
    void deleteMissile();
    void deleteRuin();
    Map *map;
    Player* player[MAXPLAYER];
    int **memorymap=new int*[MEMORYROW];//动态
    Coordinate *nowobject=NULL;
    Coordinate *lastnowobject=NULL;
    std::list<Human*> nowmove;
    int mapmoveFrequency=1;
signals:
    void mapmove();
    void dataReady();
    void requestNewData();
    void GameDataUpdate();
    void closed();
    void toggleLineSend(bool line);
    void togglePosSend(bool pos);
private slots:
    void setShowTimeFrame();
    void receiveClickPos(int BlockL, int BlockU);
    void toggleShowOverlap();
    void togglePos();
    void toggleLine();
    void toggleSelect();
    void toggleMusic();
    void toggleSound();
    void exportDebugTextHtml();
    void exportDebugTextTxt();
    void clearDebugText();
    void clearDebugTextFile();
    void FrameUpdate();
    void ShowFrame();
    void receiveselectNum(int selectNum);
    void receiverightpress(int num, double L, double U);
    void judgemove();
    void widgetAct(int num);
    int doAct(int name);
    void debuginformation();
    Building *getView(int BlockL, int BlockU, int num);
    void get_Game();
    void judgeStop();
    void updatePosition();
    void onRadioClickSlot();
    void handleAction();
    void cheatResource();
    void cheatFarmerCountLimit();
    void cheatEmpiresAge();
    void receiveAIDebug(QString String);
    void selectFarmerSlots(std::list<Farmer *> *nowselectList);
    void playSound(const std::string &key);
    void setShowTime();
    void on_pause_clicked();

private:
    Ui::MainWidget *ui;
    QTimer *timer;
    QTimer *showTimer;
    int gameframe;
    tagGame *Game=new tagGame;
    int Winnning=0;
    int Lose = 0;
    int flag=0;
    QButtonGroup *pbuttonGroup;
    double *ArrowTowerBlockL=new double[3];
    double *ArrowTowerBlockU=new double[3];
    int ArrowTowerBuilt[3]={0,0,0};
    AI* worker;
    QThread* AIthread;
    time_t t,t0;
    int LastFrame = 0;
    int CollisionMap[72][72];
    int findPathMap[72][72];
    std::list<Coordinate *> CollisionObject;
    std::list<Farmer *> *nowselectList=new std::list<Farmer *>;
    bool music = false;
    bool sound = false;
    bool pause = false;
    bool select = false;
    bool line = false;
    bool pos = false;
    bool showOverlap = true;
};

#endif // MAINWIDGET_H
