#include "MainWidget.h"
#include "ui_MainWidget.h"
#include "AI_My.h"
#include <QThread>
#include <Windows.h>
#include <iostream>

ActWidget *acts[10] = {};
int currentSelectNum = -1;
int g_selectNum=1;
std::map<int,Coordinate*> g_Object;
std::map<int, std::string> actNames = {
    {ACT_CREATEFARMER, ACT_CREATEFARMER_NAME},
    {ACT_UPGRADE_AGE, ACT_UPGRADE_AGE_NAME},
    {ACT_UPGRADE_TOWERBUILD, ACT_UPGRADE_TOWERBUILD_NAME},
    {ACT_UPGRADE_WOOD, ACT_UPGRADE_WOOD_NAME},
    {ACT_UPGRADE_STONE, ACT_UPGRADE_STONE_NAME},
    {ACT_UPGRADE_FARM, ACT_UPGRADE_FARM_NAME},
    {ACT_STOP, ACT_STOP_NAME},
    {ACT_BUILD, ACT_BUILD_NAME},
    {ACT_BUILD_HOUSE, ACT_BUILD_HOUSE_NAME},
    {ACT_BUILD_GRANARY, ACT_BUILD_GRANARY_NAME},
    {ACT_BUILD_STOCK, ACT_BUILD_STOCK_NAME},
    {ACT_BUILD_CANCEL, ACT_BUILD_CANCEL_NAME},
    {ACT_BUILD_FARM, ACT_BUILD_FARM_NAME},
    {ACT_BUILD_MARKET, ACT_BUILD_MARKET_NAME},
    {ACT_BUILD_ARROWTOWER, ACT_BUILD_ARROWTOWER_NAME},
    {ACT_NULL, ACT_NULL_NAME}
};
QTextBrowser *DebugText;
QTextBrowser *g_DebugText;
QSound *bgm;
int elapsedSec = 0;
int elapsedFrame = 0;
MainWidget::MainWidget(bool MapJudge, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    //初始化资源
    InitImageResMap(RESPATH);
    InitSoundResMap(RESPATH);
    initFarmer();
    initAnimal();
    initBuilding();
    initCoores();
    initBlock();
    initMissile();
    initRuin();
    //设定调试框的全局指针
    DebugText=ui->DebugTextBrowser;
    g_DebugText = DebugText;

    this->setFixedSize(GAME_WIDTH,GAME_HEIGHT);
    this->setWindowTitle("Age Of Empires");
    this->setWindowIcon(QIcon());
    //设定游戏计时器
    timer=new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->start(40);
    showTimer=new QTimer(this);
    showTimer->setTimerType(Qt::PreciseTimer);
    showTimer->start(1000);
    connect(showTimer, &QTimer::timeout, this, &MainWidget::setShowTime);
    connect(timer,&QTimer::timeout,this,&MainWidget::setShowTimeFrame);
    //游戏帧数初始化
    gameframe=0;

    //新建map对象
    map=new Map;
    //map初始化
    map->init(MapJudge);
    //内存图开辟空间
    for(int i=0;i<MEMORYROW;i++)
    {
        memorymap[i]=new int[MEMORYCOLUMN];
    }
    //玩家开辟空间
    for(int i=0;i<MAXPLAYER;i++)
        player[i]=new Player();

    //向地图中添加资源
    initmap();

    pbuttonGroup = new QButtonGroup(this);
    pbuttonGroup->addButton(ui->radioButton,0);
    pbuttonGroup->addButton(ui->radioButton_2,1);
    pbuttonGroup->addButton(ui->radioButton_3,2);
    pbuttonGroup->addButton(ui->radioButton_4,3);
    connect(timer,SIGNAL(timeout()),this,SLOT(FrameUpdate()));
    connect(timer,SIGNAL(timeout()),this,SLOT(ShowFrame()));
    connect(ui->Game,SIGNAL(returnselectNum(int)),this,SLOT(receiveselectNum(int)));
    connect(ui->Game,SIGNAL(midpress()),this,SLOT(debuginformation()));
    connect(ui->Game,SIGNAL(rightpress(int,double,double)),this,SLOT(receiverightpress(int,double,double)));
    connect(ui->interact1,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact2,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact3,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact4,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact5,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact6,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact7,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact8,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact9,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->interact10,SIGNAL(actPress(int)),this,SLOT(widgetAct(int)));
    connect(ui->Game,SIGNAL(sendView(int,int,int)),this,SLOT(getView(int,int,int)));
    connect(ui->Game,SIGNAL(startJudgeStop()),this,SLOT(judgeStop()));
    connect(ui->Game,SIGNAL(startUpdatePosition()),this,SLOT(updatePosition()));
    connect(ui->radioButton, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_2, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_3, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(ui->radioButton_4, SIGNAL(clicked()), this, SLOT(onRadioClickSlot()));
    connect(this,SIGNAL(GameDataUpdate()),ui->Game,SLOT(UpdateData()));
    connect(ui->Game,SIGNAL(selectFarmerSignal(std::list<Farmer*> *)),this,SLOT(selectFarmerSlots(std::list<Farmer*>*)));
    connect(ui->Game,SIGNAL(sendClickPos(int,int)),this,SLOT(receiveClickPos(int,int)));
    ActWidget *act2[10] = {ui->interact1,ui->interact2,ui->interact3,ui->interact4,ui->interact5,ui->interact6,ui->interact7,ui->interact8,ui->interact9,ui->interact10};
    ui->Game->setMouseTracking(true);
    ui->Game->setAttribute(Qt::WA_MouseTracking,true);
    ui->Game->installEventFilter(this); //安装事件过滤器
    AboutDialog *about = new AboutDialog(this);
    Option *option = new Option(this);
    option->hide();
    about->hide();
    connect(option->btnHtml,&QPushButton::clicked,this,&MainWidget::exportDebugTextHtml);
    connect(option->btntxt,&QPushButton::clicked,this,&MainWidget::exportDebugTextTxt);
    connect(option->btnTextClear,&QPushButton::clicked,this,&MainWidget::clearDebugText);
    connect(option->btnFileClear,&QPushButton::clicked,this,&MainWidget::clearDebugTextFile);
    connect(option->btnMusic,&QPushButton::clicked,this,&MainWidget::toggleMusic);
    connect(option->btnSound,&QPushButton::clicked,this,&MainWidget::toggleSound);
    connect(option->btnSelect,&QPushButton::clicked,this,&MainWidget::toggleSelect);
    connect(option->btnLine,&QPushButton::clicked,this,&MainWidget::toggleLine);
    connect(option->btnPos,&QPushButton::clicked,this,&MainWidget::togglePos);
    connect(option->btnOverlap,&QPushButton::clicked,this,&MainWidget::toggleShowOverlap);
    option->btnSelect->hide();
    option->btnLine->hide();
    option->btnPos->hide();
    option->btnOverlap->hide();
    connect(ui->pause,&QPushButton::clicked,this,&MainWidget::toggleSelect);
    connect(ui->pause,&QPushButton::clicked,this,&MainWidget::toggleLine);
    connect(ui->pause,&QPushButton::clicked,this,&MainWidget::togglePos);
    //    connect(ui->pause,&QPushButton::clicked,this,&MainWidget::toggleShowOverlap);
    connect(ui->interact,&QPushButton::clicked,about,&QDialog::show);
    connect(ui->option,&QPushButton::clicked,option,&QDialog::show);

    for(int i = 0; i < 10; i++)
    {
        acts[i] = act2[i];
        acts[i]->setStatus(0);
        acts[i]->setNum(i);
        acts[i]->hide();
        acts[i]->setAttribute(Qt::WA_Hover, true);
        acts[i]->installEventFilter(this);
    }


    currentSelectNum = -1;
    ui->mapView->setBuildList(&(player[0]->build));
    ui->mapView->setFarmerList(&(player[0]->human));
    ui->mapView->setCooresList(&(map->coores));
    ui->mapView->setTreeList(&(map->animal));
    ui->mapView->setRuinList(&(map->ruin));
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::white);
    ui->objHp->setPalette(pe);
    ui->objName->setPalette(pe);
    ui->objText->setPalette(pe);
    pe.setColor(QPalette::WindowText,Qt::green);
    ui->tipLbl->setPalette(pe);
    //线程
    AIthread = new QThread;

//    worker = new AI;
    worker = new AI_MY;
    worker->moveToThread(AIthread);

    connect(this, &MainWidget::dataReady, worker, &AI::processData);
    connect(worker, &AI::cheatAge, this, &MainWidget::cheatEmpiresAge);
    connect(worker, &AI::cheatFarmerLimit, this, &MainWidget::cheatFarmerCountLimit);
    connect(worker, &AI::cheatRes, this, &MainWidget::cheatResource);
    connect(worker, &AI::AIDebugText, this, &MainWidget::receiveAIDebug);
    connect(worker, &AI::cheatPause, ui->pause, &QPushButton::click);

    inittagGame(Game);
    set_Game();
    get_Game();
    AIthread->start();
    bgm = SoundMap["BGM"];  // 创建 QSound 对象，指定音乐文件
    // 设置循环播放
    bgm->setLoops(QSound::Infinite);
    //    // 开始播放背景音乐
    //    bgm->play();
    //    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 游戏开始"));
    //    ui->DebugTextBrowser->insertPlainText("\n");
    debugText("blue"," 游戏开始");
}

MainWidget::~MainWidget()
{
    emit closed();
    AIthread->quit();
    delete ui;
    delete map;
    for(int i=0;i<MAXPLAYER;i++)
    {
        if(player[i]!=NULL)
        {
            delete player[i];
        }
    }
    for(int i=0;i<MEMORYROW;i++)
    {
        delete[] memorymap[i];
    }
    //    delete timer;
    // 释放 game->building 数组的内存
    delete[] Game->building;

    // 释放 game->human 数组的内存
    delete[] Game->human;

    // 释放 game->resource 数组的内存
    delete[] Game->resource;
    delete Game;
    delete pbuttonGroup;
    // 释放动态空间
    deleteFarmer();
    deleteAnimal();
    deleteBuilding();
    deleteBlock();
    deleteMissile();
    delete worker;
}

void MainWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pix;
    pix=resMap["Interface"].front();
    painter.drawPixmap(0,0,1440,45,pix);
    pix=resMap["Interface"].back();
    painter.drawPixmap(0,GAME_HEIGHT - 203.5,1440,203.5,pix);

}

//增加新的coores资源
void MainWidget::addCoores(int Num, double L, double U)
{
    Coores *newcoores=new Coores();
    newcoores->init(Num,L,U);
    map->coores.push_back(newcoores);
    return;
}

void MainWidget::addGameres(int Num, int BlockL, int BlockU)
{
    Coores *newgameres=new Coores();
    newgameres->init(Num,BlockL,BlockU);
    map->coores.push_back(newgameres);
    return ;
}

//增加新的animal资源
void MainWidget::addAnimal(int Num, double L, double U)
{
    Animal *newanimal=new Animal();
    newanimal->init(Num,L,U);
    map->animal.push_back(newanimal);

    return;
}

void MainWidget::addTree(int Num, int BlockL, int BlockU)
{
    Animal *newTree=new Animal();
    newTree->init(Num,BlockL,BlockU);
    map->animal.push_back(newTree);
    return ;
}

void MainWidget::addruin(int Num, int L, int U)
{
    Ruin *newruin=new Ruin();
    newruin->init(Num,L,U);
    map->ruin.push_back(newruin);

    return ;
}

//向地图中添加资源
void MainWidget::initmap()
{
    int ArrowRuinFlag=0;
    player[0]->addfarmer(0,(MAP_L / 2 - 2), (MAP_U / 2 - 2));
    player[0]->addfarmer(0,(MAP_L / 2 + 2), (MAP_U / 2 - 2));
    player[0]->addfarmer(0,(MAP_L / 2 + 2), (MAP_U / 2 + 2));
//    addAnimal(3,tranL(30)+BLOCKSIDELENGTH/2, tranU(30)+BLOCKSIDELENGTH/2);
//    for(int i = 0; i < 30; i++)
//    {
//        player[0]->addfarmer(0, 40, 40);
//    }
    //        player[0]->addbuilding(BUILDING_FARM,30,30,100);
    //    addAnimal(3,tranL(30),tranU(30));
    //    addAnimal(3,tranL(31),tranU(31));
    //    addAnimal(3,tranL(32),tranU(32));

    for(int i = 0; i < MAP_U; i++)
    {
        for(int j = 0; j < MAP_L; j++)
        {
            if(Gamemap[i][j] == 9) player[0]->addbuilding(2, i, j, 100); //主营
            else if(Gamemap[i][j] == 7) addAnimal(2, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 大象
            else if(Gamemap[i][j] == 6) addAnimal(3, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 狮子
            else if(Gamemap[i][j] == 5) addGameres(2, i, j); // 金矿
            else if(Gamemap[i][j] == 4) addGameres(1, i, j); // 石头
            else if(Gamemap[i][j] == 3) addAnimal(1, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 瞪羚
            else if(Gamemap[i][j] == 2) addCoores(0, tranL(i)+BLOCKSIDELENGTH/2, tranU(j)+BLOCKSIDELENGTH/2); // 浆果
            else if(Gamemap[i][j] == 1) addTree(0, i, j); // 树
            else if(Gamemap[i][j] == 10)
            {
                addruin(0, tranL(i), tranU(j)); // 箭塔废墟
                //                qDebug() << "生成第" << ArrowRuinFlag + 1 << "个箭塔废墟在(" << i << ',' << j << ")；";
                ArrowTowerBlockL[ArrowRuinFlag]=i-1;
                ArrowTowerBlockU[ArrowRuinFlag]=j;
                //                qDebug()<<"ArrowTowerBlockL["<<ArrowRuinFlag<<"]="<<i-1;
                //                                qDebug()<<"ArrowTowerBlockU["<<ArrowRuinFlag<<"]="<<j-1;
                //                player[0]->addbuilding(6,i-1,j,100);
                ArrowRuinFlag++;
            }
            else if(Gamemap[i][j] == 11) addTree(0, i, j);
            /*
                种类：
                0为空地；
                1为树木；
                2为浆果；
                3为瞪羚；
                4为石头；
                5为金矿；
                6为狮子；
                7为大象；
                9为主营；
                10为箭塔废墟；
                11为树林。
            */
            Gamemap[i][j] = 0;  // :)
            mapFlag[i][j] = 0;
        }
    }
}

void MainWidget::judgestate()
{
    //玩家部分
    for(int i=0;i<8;i++)
    {
        if(!player[i]->human.empty())
        {
            std::list<Human *>::iterator iter=player[i]->human.begin();
            while(iter!=player[i]->human.end())
            {
                if((*iter)->isFarmer())//农民类
                {
                    Farmer *farmer=(Farmer *)(*iter);
                    if(!farmer->isDie())
                    {
                        //当前农民有工作对象且未开始流程
                        if(!farmer->nowobjectisNULL()&&farmer->isIdleing())
                        {
                            if(farmer->getnowobject()->isCoores())
                            {
                                Coores *coores=(Coores *)farmer->getnowobject();
                                if(coores->isBlockres())
                                {
                                    //                                    qDebug()<<"1";
                                    movetoGameres(farmer,farmer->getnowobject());
                                    farmer->setworkobject(farmer->getnowobject());
                                    if(farmer->reachDestination())
                                    {
                                        farmer->setMiner();
                                        farmer->setIsDiggingStone();
                                        farmer->setPreWork();
                                    }
                                    else
                                    {
                                        //设为前往当前对象状态
                                        farmer->setIsGoToObject();
                                        farmer->setPreWalk();
                                        farmer->setMiner();
                                    }
                                }
                                else
                                {
                                    movetoCoo(farmer,farmer->getnowobject());
                                    farmer->setPreWalk();
                                    farmer->setworkobject(farmer->getnowobject());
                                    if(farmer->reachDestination())
                                    {
                                        farmer->setGatherer();
                                        farmer->setIsGathering();
                                        farmer->setPreWork();
                                    }
                                    else
                                    {
                                        //设为前往当前对象状态
                                        farmer->setIsGoToObject();
                                        farmer->setPreWalk();
                                        farmer->setGatherer();
                                    }
                                }
                            }
                            else if(farmer->getnowobject()->isAnimal()||farmer->getnowobject()->isTreeForest())
                            {
                                Animal *animal=(Animal*)farmer->getnowobject();
                                //                                qDebug()<<"准备寻路";
                                if(animal->isBlockres())
                                {
                                    movetoGameres(farmer,farmer->getnowobject());
                                    farmer->setworkobject(farmer->getnowobject());
                                }
                                else
                                {
                                    movetoCoo(farmer,farmer->getnowobject());
                                    farmer->setPreWalk();
                                    farmer->setworkobject(farmer->getnowobject());
                                }
                                if(animal->isAlive())
                                {
                                    //                                    qDebug()<<"前往攻击";
                                    farmer->setIsGoToAttack();
                                    if(farmer->isinWindow()&&animal->isRealAnimal())
                                    {
                                        playSound("Beginning_Attack");
                                    }
                                    farmer->setPreWalk();
                                }
                                else
                                {
                                    farmer->setIsGoToObject();
                                    farmer->setPreWalk();
                                }
                                if(animal->isAnimal())
                                {
                                    farmer->setHunter();
                                }
                                else if(animal->isTreeForest())
                                {
                                    farmer->setLumber();
                                }
                            }
                            else if(farmer->getnowobject()->isFarm())
                            {
                                Building *farm=(Building *)farmer->getnowobject();
                                if(farm->getCnt()>0 && farm->isFinish())
                                {
                                    movetoGameres(farmer,farm);
                                    farmer->setworkobject(farmer->getnowobject());
                                    farmer->setIsGoToObject();
                                    farmer->setPreWalk();
                                    farmer->setFarmer();
                                }
                                else if(!farm->isFinish())
                                {
                                    movetoGameres(farmer,farm);
                                    farmer->setworkobject(farmer->getnowobject());
                                    farmer->setIsGoToObject();
                                    farmer->setPreWalk();
                                    farmer->setWorker();
                                }
                            }
                            else if(farmer->getnowobject()->isBuilding())
                            {
                                Building *build=(Building*)farmer->getnowobject();
                                if(!build->isFinish())
                                {
                                    movetoGameres(farmer,build);
                                    farmer->setworkobject(farmer->getnowobject());
                                    farmer->setIsGoToObject();
                                    farmer->setPreWalk();
                                    farmer->setWorker();
                                }
                                else if(farmer->haveresource())
                                {
                                    if(farmer->getresourceSort()==HUMAN_GRANARYFOOD)
                                    {
                                        if(build->isGranary()||build->isCenter())
                                        {
                                            movetoGameres(farmer,build);
                                            farmer->setPreWalk();
                                            farmer->setIsGoToCamp();
                                        }
                                    }
                                    else
                                    {
                                        if(build->isStock()||build->isCenter())
                                        {
                                            movetoGameres(farmer,build);
                                            farmer->setPreWalk();
                                            farmer->setIsGoToCamp();
                                        }
                                    }
                                }
                            }

                        }

                        if(!farmer->nowobjectisNULL()&&farmer->isAttacking())
                        {
                            if(farmer->getnowobject()->isAnimal()||farmer->getnowobject()->isTreeForest())
                            {
                                Animal *temp=(Animal *)farmer->getnowobject();
                                if(temp->isAlive())
                                {
                                    if(temp->isTreeForest())
                                    {
                                        if((gameframe-farmer->getActionOccurrenceGameframe())%farmer->getCuttingSpeed()==0)
                                        {
                                            if(farmer->isinWindow())
                                            {
                                                playSound("Cut");
                                            }
                                            temp->reduceBlood(farmer->getCuttingAttack());
                                            farmer->setNowRes();
                                        }
                                    }
                                    else
                                    {
                                        if((gameframe-farmer->getActionOccurrenceGameframe())==farmer->getLongRangeAttackFrame())
                                        {
                                            //产生矛
                                            player[farmer->getCamp()]->addmissile(farmer->getnowobject(),farmer,0,farmer->getL(),farmer->getU(),farmer->getnowobject()->getL(),farmer->getnowobject()->getU());
                                        }
                                        if((gameframe-farmer->getActionOccurrenceGameframe())==farmer->getLongRangeAttackSpeed())
                                        {
                                            if(judgementAttack(farmer,temp))
                                            {
                                                farmer->setActionOccurrenceGameframe(gameframe);
                                                farmer->setNowRes();
                                            }
                                            else
                                            {
                                                movetoCoo(farmer,farmer->getnowobject());
                                                farmer->setIsGoToAttack();
                                                farmer->setPreWalk();
                                            }
                                        }

                                    }
                                }
                                else
                                {
                                    farmer->setPreStand();
                                    farmer->setisIdle();
                                }
                            }
                        }

                        if((farmer->isCutting()||farmer->isDiggingGold()||farmer->isDiggingStone()||farmer->isGathering()))
                        {
                            if(!farmer->isFullResource()&&farmer->getworkobject()!=NULL)
                            {
                                if(farmer->isCutting())
                                {
                                    farmer->getworkobject()->addGatherSpeed(farmer->getWoodSpeed());
                                    if(gameframe%15==0&&farmer->isinWindow())
                                    {
                                        playSound("Cut");
                                    }
                                }
                                else if(farmer->isDiggingStone())
                                {
                                    farmer->getworkobject()->addGatherSpeed(farmer->getStoneSpeed());
                                    if(gameframe%15==0&&farmer->isinWindow())
                                    {
                                        playSound("Mine");
                                    }
                                }
                                else if(farmer->isDiggingGold())
                                {
                                    farmer->getworkobject()->addGatherSpeed(farmer->getGoldSpeed());
                                    if(gameframe%15==0&&farmer->isinWindow())
                                    {
                                        playSound("Mine");
                                    }
                                }
                                else if(farmer->isGathering())
                                {
                                    farmer->getworkobject()->addGatherSpeed(farmer->getMeatSpeed());
                                    if(gameframe%15==0&&farmer->isinWindow())
                                    {
                                        playSound("Gather");
                                    }
                                }
                                farmer->addResource(farmer->getworkobject()->getProductSort());
                            }
                            else
                            {
                                farmer->setIsGoToCamp();
                                if(farmer->isGatherer())
                                {

                                }
                                else
                                {
                                    farmer->setholdresource();
                                }
                                farmer->setPreWalk();
                                if(farmer->getresourceSort()==HUMAN_GRANARYFOOD)//食物
                                {
                                    farmer->setnowobject(findGranary(farmer));
                                }
                                else farmer->setnowobject(findStock(farmer));
                                int flag=findAngle(farmer);
                                if(flag==-1)
                                {

                                }
                                else
                                    farmer->setAngle(flag);
                            }
                        }
                        else if(farmer->isFarming())
                        {
                            if(!farmer->isFullResource()&&farmer->getworkobject()!=NULL)
                            {
                                Building *farm=(Building *)farmer->getnowobject();
                                if(farm->isNoFarmer())
                                {
                                    farm->setFarmer(farmer);
                                }
                            }
                            else
                            {
                                farmer->setIsGoToCamp();
                                if(farmer->haveresource())
                                {
                                    farmer->setholdresource();
                                }
                                farmer->setPreWalk();
                                if(farmer->getresourceSort()==HUMAN_GRANARYFOOD)
                                {
                                    farmer->setnowobject(findGranary(farmer));
                                }
                                else farmer->setnowobject(findStock(farmer));
                                int flag=findAngle(farmer);
                                if(flag==-1)
                                {

                                }
                                else
                                    farmer->setAngle(flag);
                                farmer->setNowRes();
                            }
                        }
                        else if(!farmer->nowobjectisNULL()&&farmer->isConstruct())
                        {
                            if(farmer->getnowobject()->isBuilding() || farmer->getnowobject()->isFarm())
                            {
                                Building * build=(Building *)farmer->getnowobject();
                                if(!build->isFinish())
                                {
                                    build->addConstructSpeed(farmer->getConstructSpeed());
                                    if((int)build->getBlood()%10==0&&farmer->isinWindow())
                                    {
                                        playSound("Build");
                                    }
                                }
                                else
                                {
                                    farmer->setisIdle();
                                    farmer->setPreStand();
                                }
                            }
                        }
                        //                    //当前农民需要转换动作
                        if(farmer->needTranState())
                        {
                            //                            DebugText->insertPlainText("转变动作\n");
                            farmer->setNowState(farmer->getPreState());
                            farmer->setPreStateIsIdle();
                            farmer->setNowRes();
                        }
                    }
                    if(farmer->isDestroy())
                    {
                        //删掉当前对象 产生相应的废墟
                        Human *p=(*iter);
                        for(auto it=drawlist.begin();it!=drawlist.end();it++)
                        {
                            Farmer *fa=(Farmer *)(*it);
                            if(p==fa)
                            {
                                drawlist.erase(it);
                                break;
                            }
                        }
                        g_Object.erase(p->getSelectNum());
                        replaceobject(p,NULL);
                        iter=player[i]->human.erase(iter);
                        delete p;
                        continue;
                    }
                }
                iter++;
            }
        }
        if(!player[i]->missile.empty())
        {
            std::list<Missile *>::iterator iter=player[i]->missile.begin();
            while(iter!=player[i]->missile.end())
            {
                if((*iter)->isArrive())
                {
                    //掉血
                    if((*iter)->getAttackeder()!=NULL&&(*iter)->getAttackeder()->isAnimal())
                    {
                        Animal *animal=(Animal *)(*iter)->getAttackeder();
                        if(animal->isGazelle()&&animal->isFleeing()&&gameframe-animal->getBeAttacked()>5)
                        {

                        }
                        else
                        {
                            (*iter)->getAttackeder()->reduceBlood(4);
                            animal->setBeAttacked(gameframe);
                        }
                    }
                    else if((*iter)->getAttackeder()!=NULL)
                    {
                        (*iter)->getAttackeder()->reduceBlood(4);
                    }


                    if((*iter)->getAttackeder()->isDie()&&(*iter)->getAttackeder()->needDieInformation()&&(*iter)->getAttackeder()->isinWindow())
                    {
                        playSound("Animal_Die");
                        Animal *animal=(Animal *)((*iter)->getAttackeder());
                        if(animal->isGazelle())
                        {
                            debugText("red"," ANIMAL_GAZELLE(编号"+QString::number(animal->getSelectNum())+")死亡");
                        }
                        else if(animal->isElephant())
                        {
                            debugText("red"," ANIMAL_ELEPHANT(编号"+QString::number(animal->getSelectNum())+")死亡");
                        }
                        else if(animal->isLion())
                        {
                            debugText("red"," ANIMAL_LION(编号"+QString::number(animal->getSelectNum())+")死亡");
                        }
                        animal->setDieInformation();
                    }
                    if((*iter)->getAttackeder()->isAnimal())
                    {
                        Animal *animal=(Animal *)((*iter)->getAttackeder());
                        if(animal->isElephant()&&animal->isFriendly())
                        {
                            animal->setIsHostile();
                        }
                        if(animal->isGazelle())
                        {
                            if(animal->isIdle())
                            {
                                animal->setL0(animal->getL()+animal->getL()-(*iter)->getAttacker()->getL());
                                animal->setU0(animal->getU()+animal->getU()-(*iter)->getAttacker()->getU());
                                Point start,destination;
                                start.x=tranBlockL(animal->getL());
                                start.y=tranBlockU(animal->getU());
                                destination.x=tranBlockL(animal->getL0());
                                destination.y=tranBlockU(animal->getU0());
                                loadfindPathMap();
                                animal->setPath(findPath(findPathMap,start,destination));
                                animal->setFleeding();
                                animal->setPreRun();
                            }
                        }
                        if(animal->isLion()&&animal->isIdle())
                        {
                            animal->setChasing();
                            animal->setNowobject((*iter)->getAttacker());
                            movetoCoo(animal,(*iter)->getAttacker());
                            animal->setPreRun();
                        }
                    }
                    Missile *missile=(*iter);
                    for(auto it=drawlist.begin();it!=drawlist.end();it++)
                    {
                        Missile *mi=(Missile *)(*it);
                        if(missile==mi)
                        {
                            drawlist.erase(it);
                            break;
                        }
                    }
                    iter=player[i]->missile.erase(iter);
                    delete missile;
                    continue;
                }
                else iter++;
                //判断速度前进方向上L0U0是否超前于LU，如果是则判断命中并销毁
            }
        }
        if(!player[i]->build.empty())
        {
            std::list<Building *>::iterator iter=player[i]->build.begin();
            while(iter!=player[i]->build.end())
            {
                if((*iter)->isArrowTower() && (*iter)->isFinish())
                {
                    judgeArrowTowerPosition((*iter)->getBlockL(),(*iter)->getBlockU());
                }
                if((*iter)->isFinish()&&(*iter)->isFarm())
                {
                    Building *farm=(*iter);
                    if(!farm->isNoFarmer())
                    {
                        Farmer *farmer=(Farmer *)farm->getnowfarmer();
                        farm->addGatherSpeed(farmer->getMeatSpeed());
                        (*iter)->reduceCnt();
                        farmer->addResource(farm->getProductSort());
                        if(farmer->isFullResource())
                        {
                            farm->setnowfarmer(NULL);
                        }
                    }
                }
                else if(!(*iter)->isFinish())
                {
                    (*iter)->addPercent();
                    if((*iter)->isFinish())
                    {
                        if((*iter)->isHome())
                        {
                            debugText("blue"," BUILDING_HOME(编号:"+QString::number((*iter)->getSelectNum())+")建造完毕");
                        }
                        else if((*iter)->isGranary())
                        {
                            debugText("blue"," BUILDING_GRANARY(编号:"+QString::number((*iter)->getSelectNum())+")建造完毕");
                        }
                        else if((*iter)->isStock())
                        {
                            debugText("blue"," BUILDING_STOCK(编号:"+QString::number((*iter)->getSelectNum())+")建造完毕");
                        }
                        else if((*iter)->isArrowTower())
                        {
                            debugText("blue"," BUILDING_ARROWTOWER(编号:"+QString::number((*iter)->getSelectNum())+")建造完毕");
                        }
                        else if((*iter)->isFarm())
                        {
                            debugText("blue"," BUILDING_FARM(编号:"+QString::number((*iter)->getSelectNum())+")建造完毕");
                        }
                        else if((*iter)->isMarket())
                        {
                            debugText("blue"," BUILDING_MARKET(编号:"+QString::number((*iter)->getSelectNum())+")建造完毕");
                        }
                    }
                }
                if((*iter)->isDestroy())
                {
                    //删掉当前对象 产生相应的废墟
                    Building *p=(*iter);
                    if(p->isFarm())
                    {
                        debugText("red"," BUILDING_FARM(编号:"+QString::number(p->getSelectNum())+")采集完成");
                    }
                    replaceobject(p,NULL);
                    for(auto it=drawlist.begin();it!=drawlist.end();it++)
                    {
                        Building *bu=(Building *)(*it);
                        if(p==bu)
                        {
                            drawlist.erase(it);
                            break;
                        }
                    }
                    g_Object.erase(p->getSelectNum());
                    iter=player[i]->build.erase(iter);
                    delete p;
                    continue;
                }
                iter++;
            }
        }
    }

    //地图部分
    if(!map->coores.empty())
    {
        std::list<Coores *>::iterator criter=map->coores.begin();
        while(criter!=map->coores.end())
        {
            (*criter)->reduceCnt();
            if((*criter)->isDestroy())
            {
                //删掉当前对象 产生相应的废墟
                Coores *p=(*criter);
                if(p->isBush())
                {
                    debugText("red"," RESOURCE_BUSH(编号:"+QString::number(p->getSelectNum())+")采集完成");
                }
                else if(p->isStone())
                {
                    debugText("red"," RESOURCE_STONE(编号:"+QString::number(p->getSelectNum())+")采集完成");
                }
                else if(p->isGoldore())
                {
                    debugText("red"," RESOURCE_GOLDORE(编号:"+QString::number(p->getSelectNum())+")采集完成");
                }
                replaceobject(p,NULL);
                for(auto it=drawlist.begin();it!=drawlist.end();it++)
                {
                    Coores *co=(Coores *)(*it);
                    if(p==co)
                    {
                        drawlist.erase(it);
                        break;
                    }
                }
                g_Object.erase(p->getSelectNum());
                criter=map->coores.erase(criter);
                delete p;
                continue;
            }
            criter++;

        }
    }
    if(!map->animal.empty())
    {
        std::list<Animal *>::iterator aniter=map->animal.begin();
        while(aniter!=map->animal.end())
        {
            Animal *animal=(Animal *)(*aniter);
            //遍历对象，无状态动物 其中人靠近到一定距离设定状态
            //逃跑逃到一定位置 再次设置为空闲并再次加入检测
            if(!animal->isDie())
            {
                if(animal->isGazelle())
                {
                    std::list<Coordinate *>::iterator iter=drawlist.begin();
                    while(iter!=drawlist.end())
                    {
                        if((*iter)->isHuman()||(*iter)->isFarmer())
                        {
                            if(animal->isIdle()&&calculateManhattanDistance((*iter)->getBlockL(),(*iter)->getBlockU(),animal->getBlockL(),animal->getBlockU())<=animal->getVision())
                            {
                                animal->setL0(animal->getL()+(animal->getL()-(*iter)->getL())*2);
                                animal->setU0(animal->getU()+(animal->getU()-(*iter)->getU())*2);
                                Point start,destination;
                                start.x=tranBlockL(animal->getL());
                                start.y=tranBlockU(animal->getU());
                                destination.x=tranBlockL(animal->getL0());
                                destination.y=tranBlockU(animal->getU0());
                                animal->setPath(findPath(map->intmap,start,destination));
                                animal->setFleeding();
                                //                                animal->setAngle(findAngle(animal));
                                animal->setPreRun();
                            }
                        }
                        iter++;
                    }
                }
                if(animal->isElephant()&&!animal->isFriendly()&&animal->getObject()==NULL)
                {
                    std::list<Coordinate *>::iterator iter=drawlist.begin();
                    while(iter!=drawlist.end())
                    {
                        if((*iter)->isHuman()||(*iter)->isFarmer())
                        {
                            if(!(*iter)->isDie()&&animal->isIdle()&&calculateManhattanDistance((*iter)->getBlockL(),(*iter)->getBlockU(),animal->getBlockL(),animal->getBlockU())<=animal->getVision())
                            {
                                if(calculateManhattanDistance((*iter)->getBlockL(),(*iter)->getBlockU(),animal->getBlockL(),animal->getBlockU())==animal->getVision())
                                {
                                    if((*iter)->getBlockL()==animal->getBlockL()||(*iter)->getBlockU()==animal->getBlockU())
                                    {
                                        iter++;
                                        continue;
                                    }
                                }
                                animal->setChasing();
                                animal->setNowobject((*iter));
                                movetoCoo(animal,(*iter));
                                animal->setPreWalk();
                            }
                        }
                        iter++;
                    }
                }
                if(animal->isLion()&&animal->getObject()==NULL)
                {
                    std::list<Coordinate *>::iterator iter=drawlist.begin();
                    while(iter!=drawlist.end())
                    {
                        if((*iter)->isHuman()||(*iter)->isFarmer())
                        {
                            if(!(*iter)->isDie()&&animal->isIdle()&&calculateManhattanDistance((*iter)->getBlockL(),(*iter)->getBlockU(),animal->getBlockL(),animal->getBlockU())<=animal->getVision())
                            {
                                if(calculateManhattanDistance((*iter)->getBlockL(),(*iter)->getBlockU(),animal->getBlockL(),animal->getBlockU())==animal->getVision())
                                {
                                    if((*iter)->getBlockL()==animal->getBlockL()||(*iter)->getBlockU()==animal->getBlockU())
                                    {
                                        iter++;
                                        continue;
                                    }
                                }
                                animal->setChasing();
                                animal->setNowobject((*iter));
                                movetoCoo(animal,(*iter));
                                animal->setPreRun();
                            }
                        }
                        iter++;
                    }
                }
                if(animal->getObject()!=NULL)
                {
                    if(animal->getObject()->isDie())
                    {
                        animal->setNowobject(NULL);
                        animal->setPreStand();
                        animal->setIdle();
                    }
                    else if(animal->isAttacking())
                    {
                        if((gameframe-animal->getActionOccurrenceGameframe())==animal->getAttackCaused())
                        {
                            //遍历一下所有对象 所有小于大象攻击范围的对象都将收到伤害 而不只是当前对象
                            if(animal->isElephant()&&animal->isinWindow())
                            {
                                playSound("Elephant_Attack");
                                debugText("red"," FARMER(编号:" + QString::number(animal->getObject()->getSelectNum()) + ")被ANIMAL_ELEPHANT(编号："+QString::number(animal->getSelectNum())+")攻击");
                                //                                playSound("Villager_BeAttacked");
                            }
                            else if(animal->isLion()&&animal->isinWindow())
                            {
                                playSound("Lion_Attack");
                                debugText("red"," FARMER(编号:" + QString::number(animal->getObject()->getSelectNum()) + ")被ANIMAL_LION(编号："+QString::number(animal->getSelectNum())+")攻击");

                                //                                playSound("Villager_BeAttacked");
                            }
                            animal->getObject()->reduceBlood(animal->getAttackValue());
                            if(animal->getObject()->isFarmer())
                            {
                                Farmer *farmer=(Farmer *)animal->getObject();
                                if(farmer->isDie()&&farmer->needDieInformation())
                                {
                                    playSound("Army_Die");
                                    debugText("red"," FARMER(编号:"+QString::number(farmer->getSelectNum())+")死亡");
                                    farmer->setDieInformation();
                                }
                                if(!farmer->isAttack())
                                {
                                    farmer->setHunter();
                                    farmer->setIsAttacking();
                                    farmer->setPreAttack();
                                    farmer->setNowRes();
                                    farmer->setnowobject(animal);
                                    farmer->setworkobject(farmer->getnowobject());
                                    farmer->setActionOccurrenceGameframe(gameframe);
                                }
                            }
                        }
                        if(gameframe-animal->getActionOccurrenceGameframe()==animal->getAttackSpeed())
                        {
                            if(judgementAttack(animal,animal->getObject()))
                            {
                                animal->setActionOccurrenceGameframe(gameframe);
                                animal->setNowRes();
                            }
                            else
                            {
                                movetoCoo(animal,animal->getObject());
                                animal->setChasing();
                                if(animal->isLion())
                                {
                                    animal->setPreRun();
                                }
                                else if(animal->isElephant())
                                    animal->setPreWalk();
                            }
                        }
                    }
                    else if(animal->isIdle())
                    {
                        movetoCoo(animal,animal->getObject());
                        animal->setChasing();
                        if(animal->isElephant())
                        {
                            animal->setPreWalk();
                        }
                        else if(animal->isLion())
                        {
                            animal->setPreRun();
                        }
                    }
                }
                if(animal->needTranState())
                {
                    animal->setNowState(animal->getPreState());
                    animal->setPreStateIsIdle();
                    animal->setNowRes();
                }
            }
            else
            {
                animal->reduceCnt();
            }
            if(animal->isDestroy())
            {

                //删掉当前对象 产生相应的废墟
                Animal *p=(*aniter);
                if(p->isTree())
                {
                    debugText("red"," RESOURCE_TREE(编号:"+QString::number(p->getSelectNum())+")采集完成");
                }
                else if(p->isLion())
                {
                    debugText("red"," RESOURCE_LION(编号:"+QString::number(p->getSelectNum())+")采集完成");
                }
                else if(p->isElephant())
                {
                    debugText("red"," RESOURCE_ELEPHANT(编号:"+QString::number(p->getSelectNum())+")采集完成");
                }
                else if(p->isGazelle())
                {
                    debugText("red"," RESOURCE_GAZELLE(编号:"+QString::number(p->getSelectNum())+")采集完成");
                }
                replaceobject(p,NULL);
                for(auto it=drawlist.begin();it!=drawlist.end();it++)
                {
                    Animal *an=(Animal *)(*it);
                    if(p==an)
                    {
                        drawlist.erase(it);
                        break;
                    }
                }
                g_Object.erase(p->getSelectNum());
                aniter=map->animal.erase(aniter);
                delete p;
                continue;
            }
            aniter++;
        }
    }
}

//添加到画框队列
void MainWidget::updateSelectobject()
{
    if(nowselectList->empty())
    {
        ui->Game->nowselect=nowobject;
    }
}

int MainWidget::findAngle(MoveObject *p)
{
    Point angle;
    angle.x=p->getnextBlockL()-p->getBlockL();
    angle.y=p->getnextBlockU()-p->getBlockU();
    if(angle.x==1&&angle.y==-1)
    {
        return 0;
    }
    else if(angle.x==0&&angle.y==-1)
    {
        return 1;
    }
    else if(angle.x==-1&&angle.y==-1)
    {
        return 2;
    }
    else if(angle.x==-1&&angle.y==0)
    {
        return 3;
    }
    else if(angle.x==-1&&angle.y==1)
    {
        return 4;
    }
    else if(angle.x==0&&angle.y==1)
    {
        return 5;
    }
    else if(angle.x==1&&angle.y==1)
    {
        return 6;
    }
    else if(angle.x==1&&angle.y==0)
    {
        return 7;
    }
    else
    {
        return -1;
    }
}

bool MainWidget::judgementAttack(Coordinate *Attacker, Coordinate *Attackered)
{

    if(Attackered==NULL)
    {
        return 0;
    }
    if(Attacker->isFarmer())
    {
        Farmer *farmer=(Farmer *)Attacker;
        if(Attackered->isTreeForest())
        {
            Animal *animal=(Animal *)Attackered;
            if(animal->isAlive())
            {
                if(animal->isTree())
                {
                    //                    qDebug()<<"L:"<<abs(Attackered->getL()-farmer->getL());
                    //                    qDebug()<<"U:"<<abs(Attackered->getU()-farmer->getU());
                    //                    qDebug()<<BLOCKSIDELENGTH+farmer->getLU()/2.0;
                    if(abs(Attackered->getL()-farmer->getL())<=BLOCKSIDELENGTH/2.0+farmer->getLU()/2.0+farmer->getCuttingRange()&&abs(Attackered->getU()-farmer->getU())<=BLOCKSIDELENGTH/2.0+farmer->getLU()/2.0+farmer->getCuttingRange())
                    {
                        return 1;
                    }
                    else return 0;
                }
            }
            else
            {
                //确定前往到位后再进行工作
                movetoCoo(farmer,Attackered);
                farmer->setPreWalk();
                farmer->setIsGoToObject();
            }
        }
        else
        {
            if(farmer->getHuntRange()>=calculateManhattanDistance(Attacker->getBlockL(),Attacker->getBlockU(),Attackered->getBlockL(),Attackered->getBlockU()))
            {
                return 1;
            }
            else return 0;
        }
    }
    if(Attacker->isAnimal())
    {
        Animal *animal=(Animal *)Attacker;
        if(animal->getAttackRange()+Attacker->getRadius()+Attackered->getRadius()>=countdistance(Attacker->getL(),Attacker->getU(),Attackered->getL(),Attackered->getU()))
        {
            return 1;
        }
        else return 0;
    }
    return 0;
}

void MainWidget::replaceobject(Coordinate *p,Coordinate *replaceobj)
{
    for(int i=0;i<MAXPLAYER;i++)
    {
        if(!player[i]->human.empty())
        {
            std::list<Human *>::iterator iter=player[i]->human.begin();
            while(iter!=player[i]->human.end())
            {
                if((*iter)->isFarmer())
                {
                    Farmer *farmer=(Farmer *)(*iter);
                    if(farmer->getnowobject()==p)
                    {
                        farmer->setnowobject(replaceobj);
                    }
                    if(farmer->getworkobject()==p)
                    {
                        farmer->setworkobject(replaceobj);
                    }
                }
                iter++;
            }
        }
    }

    if(!map->animal.empty())
    {
        std::list<Animal *>::iterator iter=map->animal.begin();
        while(iter!=map->animal.end())
        {
            Animal *animal=(Animal *)(*iter);
            if(animal->getObject()==p)
            {
                animal->setNowobject(replaceobj);
            }
            iter++;
        }
    }
    if(nowobject==p)
    {
        nowobject=replaceobj;
    }
    if(!nowselectList->empty())
    {
        auto iter=nowselectList->begin();
        while(iter!=nowselectList->end())
        {
            //            qDebug()<<"1";
            if((*iter)==p)
            {
                iter=nowselectList->erase(iter);
                break;
            }
            iter++;
        }
    }
}

//游戏帧更新
void MainWidget::FrameUpdate()
{
    judgeVictory();

    if(ProcessDataWork == 0)
    {
        handleAction();

        //        qDebug() << "在第" << gameframe << "帧时，主线程调用了AI线程";
    }
    else
    {
        //        qDebug() << "在第" << gameframe << "帧时，主线程失败地调用了AI";
    }

    judgemove();
    judgestate();
    actionUpdate();
    statusUpdate();

    if(ProcessDataWork == 0)
    {
        set_Game();
        get_Game();

        emit dataReady();
    }



    if(mapmoveFrequency==1||mapmoveFrequency==2)
    {
        ui->Game->update();
        emit mapmove();
        return;
    }
    else if(mapmoveFrequency==4)
    {
        if(gameframe%2==0)
        {
            emit mapmove();
            ui->Game->update();
        }
        else
        {
            emit GameDataUpdate();
        }
        return ;
    }
    else if(mapmoveFrequency==8)
    {
        if(gameframe%4==0)
        {
            emit mapmove();
            ui->Game->update();
        }
        else
        {
            emit GameDataUpdate();
        }
        return ;
    }
    else
    {
        qDebug()<<"速度设置错误";
    }
}

//帧数显示 右上角的小lcd
void MainWidget::ShowFrame()
{
    gameframe++;
}

//左键点击内容返回selectnum
void MainWidget::receiveselectNum(int selectNum)
{
    if(select && selectNum != 0) debugText("blue"," 当前选中目标编号为:"+QString::number(selectNum));
    //    DebugText->insertHtml(COLOR_BLUE("当前选中目标编号为:"+QString::number(selectNum)));
    //    DebugText->insertPlainText("\n");
    if(selectNum==0)
    {
        lastnowobject=nowobject;
        nowobject=NULL;
    }
    if(selectNum!=0)
    {
        nowobject=g_Object[selectNum];
        if(selectNum/10000==SORT_BUILDING)
        {
            Building *build=(Building *)nowobject;
            if(build->isCenter())
            {
                playSound("Click_Center");
            }
            if(build->isFarm())
            {
                playSound("Click_Farm");
            }
            if(build->isGranary())
            {
                playSound("Click_Granary");
            }
            if(build->isHome())
            {
                playSound("Click_House");
            }
            if(build->isMarket())
            {
                playSound("Click_Market");
            }
            if(build->isStock())
            {
                playSound("Click_Stock");
            }
        }
        if(selectNum/10000==SORT_FARMER)
        {
            playSound("Click_Villager");
        }
    }
    updateSelectobject();
    currentSelectNum=selectNum;
    refreshActs();
}

//右键点击的LU坐标位置 用来人物的点击移动
void MainWidget::receiverightpress(int num, double L, double U)
{
    auto enditer=g_Object.end();
    if(!nowselectList->empty())
    {
        if(num/10000==SORT_BUILDING&&g_Object.find(num)!=enditer)
        {
            Building *temp=(Building *)g_Object[num];
            auto iter=nowselectList->begin();
            while(iter!=nowselectList->end())
            {
                Farmer *farmer=(Farmer *)(*iter);
                if(temp->isCenter())//主营
                {
                    farmer->setisIdle();
                    if(farmer->getresource()>0)
                    {
                        movetoGameres(farmer,temp);
                        farmer->setIsGoToCamp();
                        farmer->setPreWalk();
                        farmer->setnowobject(temp);
                    }
                }
                else if(!temp->isFinish())//建筑未完成
                {
                    if(!farmer->isWorker()&&farmer->isinWindow())
                    {
                        playSound("Villager_Builder");
                    }
                    farmer->setWorker();
                    farmer->setnowobject(temp);
                    movetoGameres(farmer,temp);
                    farmer->setIsGoToObject();
                    farmer->setNowRes();
                    //变成工人 前往建造
                }
                else if(temp->isFarm())
                {
                    if(temp->isFinish())
                    {
                        farmer->setisIdle();
                        farmer->setFarmer();
                        farmer->setnowobject(temp);
                        farmer->setNowRes();
                    }
                    else
                    {
                        if(!farmer->isWorker()&&farmer->isinWindow())
                        {
                            playSound("Villager_Builder");
                        }
                        farmer->setisIdle();
                        farmer->setWorker();
                        farmer->setnowobject(temp);
                        farmer->setNowRes();
                    }
                }
                else if((temp->isGranary()&&temp->isFinish())||(temp->isStock()&&temp->isFinish()))
                {
                    if(farmer->getresource()>0)
                    {
                        movetoGameres(farmer,temp);
                        farmer->setIsGoToCamp();
                        farmer->setPreWalk();
                        farmer->setnowobject(temp);
                    }
                }
                iter++;
            }
        }
        else if((num/10000==SORT_ANIMAL||num/10000==SORT_TREEFOREST)&&g_Object.find(num)!=enditer)
        {
            Animal *animal=(Animal *)g_Object[num];
            auto iter=nowselectList->begin();
            while(iter!=nowselectList->end())
            {
                Farmer *farmer=(Farmer *)(*iter);
                farmer->setisIdle();
                if(animal->isTreeForest())
                {
                    if(!farmer->isLumber()&&farmer->isinWindow())
                    {
                        playSound("Villager_Lumber");
                    }
                    farmer->setLumber();
                }
                else
                {
                    if(!farmer->isHunter()&&farmer->isinWindow())
                    {
                        playSound("Villager_Hunter");
                    }
                    farmer->setHunter();
                }
                farmer->setnowobject(g_Object[num]);
                farmer->setNowRes();
                iter++;
            }
        }
        else if(num==0)
        {
            auto iter=nowselectList->begin();
            while(iter!=nowselectList->end())
            {
                Farmer *farmer=(Farmer *)(*iter);
                if(farmer->isinWindow())
                {
                    playSound("Villager_Move");
                }
                farmer->setisIdle();
                if(!farmer->isWalking())
                    farmer->setPreWalk();//设置为移动状态 用来重新设定攻击状态之后的状态 后续可加入判断来特判
                farmer->setdestination(L,U);
                Point start,destination;
                start.x=tranBlockL(farmer->getL());
                start.y=tranBlockU(farmer->getU());
                destination.x=tranBlockL(farmer->getL0());
                destination.y=tranBlockU(farmer->getU0());
                loadfindPathMap();
                farmer->setPath(findPath(findPathMap,start,destination));
                farmer->setnowobject(NULL);
                farmer->setworkobject(NULL);
                iter++;
            }
        }
        else if(num/10000==SORT_COORES&&g_Object.find(num)!=enditer)
        {
            auto iter=nowselectList->begin();
            while(iter!=nowselectList->end())
            {
                Farmer *farmer=(Farmer *)(*iter);
                farmer->setisIdle();
                farmer->setnowobject(g_Object[num]);
                if(farmer->getnowobject()->ProductIsWood())
                {
                    if(!farmer->isLumber()&&farmer->isinWindow())
                    {
                        playSound("Villager_Lumber");
                    }
                    farmer->setLumber();
                    farmer->setNowRes();
                }
                else if(farmer->getnowobject()->ProductIsFood())
                {
                    if(!farmer->isGatherer()&&farmer->isinWindow())
                    {
                        playSound("Villager_Gather");
                    }
                    farmer->setGatherer();
                    farmer->setNowRes();
                }
                else if(farmer->getnowobject()->ProductIsStone()||farmer->getnowobject()->ProductIsGold())
                {
                    if(!farmer->isMiner()&&farmer->isinWindow())
                    {
                        playSound("Villager_Miner");
                    }
                    farmer->setMiner();
                    farmer->setNowRes();
                }
                iter++;
            }
        }
    }
    if(nowobject!=NULL&&num/10000==SORT_BUILDING&&nowobject->isFarmer()&&g_Object.find(num)!=enditer)
    {
        Building *temp=(Building *)g_Object[num];
        if(temp->isCenter())//主营
        {
            Farmer *farmer=(Farmer *)nowobject;
            farmer->setisIdle();
            if(farmer->getresource()>0)
            {
                movetoGameres(farmer,temp);
                farmer->setIsGoToCamp();
                farmer->setPreWalk();
                farmer->setnowobject(temp);
            }
        }
        else if(!temp->isFinish())//建筑未完成
        {
            Farmer *farmer=(Farmer *)nowobject;
            if(!farmer->isWorker()&&farmer->isinWindow())
            {
                playSound("Villager_Builder");
            }
            farmer->setWorker();
            farmer->setnowobject(temp);
            movetoGameres(farmer,temp);
            farmer->setIsGoToObject();
            farmer->setNowRes();
            //变成工人 前往建造
        }
        else if(temp->isFarm())
        {
            Farmer *farmer=(Farmer *)nowobject;
            if(temp->isFinish())
            {
                farmer->setisIdle();
                farmer->setFarmer();
                farmer->setnowobject(temp);
                farmer->setNowRes();
            }
            else
            {
                if(!farmer->isWorker()&&farmer->isinWindow())
                {
                    playSound("Villager_Builder");
                }
                farmer->setisIdle();
                farmer->setWorker();
                farmer->setnowobject(temp);
                farmer->setNowRes();
            }
        }
        else if((temp->isGranary()&&temp->isFinish())||(temp->isStock()&&temp->isFinish()))
        {
            Farmer *farmer=(Farmer *)nowobject;
            if(farmer->getresource()>0)
            {
                movetoGameres(farmer,temp);
                farmer->setIsGoToCamp();
                farmer->setPreWalk();
                farmer->setnowobject(temp);
            }
        }
    }
    if(nowobject!=NULL&&(num/10000==SORT_ANIMAL||num/10000==SORT_TREEFOREST)&&nowobject->isFarmer()&&g_Object.find(num)!=enditer)
    {
        Farmer *farmer=(Farmer *)nowobject;
        Animal *animal=(Animal *)g_Object[num];
        farmer->setisIdle();
        if(animal->isTreeForest())
        {
            if(!farmer->isLumber()&&farmer->isinWindow())
            {
                playSound("Villager_Lumber");
            }
            farmer->setLumber();
            //            qDebug()<<"设定攻击";
        }
        else
        {
            if(!farmer->isHunter()&&farmer->isinWindow())
            {
                playSound("Villager_Hunter");
            }
            farmer->setHunter();
        }
        farmer->setnowobject(g_Object[num]);
        farmer->setNowRes();
    }
    if(nowobject!=NULL&&num==0&&nowobject->isFarmer())
    {
        Farmer *farmer=(Farmer *)nowobject;
        if(farmer->isinWindow())
        {
            playSound("Villager_Move");
        }
        farmer->setisIdle();
        if(!farmer->isWalking())
            farmer->setPreWalk();//设置为移动状态 用来重新设定攻击状态之后的状态 后续可加入判断来特判
        farmer->setdestination(L,U);
        Point start,destination;
        start.x=tranBlockL(farmer->getL());
        start.y=tranBlockU(farmer->getU());
        destination.x=tranBlockL(farmer->getL0());
        destination.y=tranBlockU(farmer->getU0());
        loadfindPathMap();
        farmer->setPath(findPath(findPathMap,start,destination));
        farmer->setnowobject(NULL);
        farmer->setworkobject(NULL);
    }
    if(nowobject!=NULL&&num/10000==SORT_COORES&&nowobject->isFarmer()&&g_Object.find(num)!=enditer)
    {
        Farmer *farmer=(Farmer *)nowobject;
        farmer->setisIdle();
        farmer->setnowobject(g_Object[num]);
        if(farmer->getnowobject()->ProductIsWood())
        {
            if(!farmer->isLumber()&&farmer->isinWindow())
            {
                playSound("Villager_Lumber");
            }
            farmer->setLumber();
            farmer->setNowRes();
            //            qDebug()<<"设定为樵夫";
        }
        else if(farmer->getnowobject()->ProductIsFood())
        {
            if(!farmer->isGatherer()&&farmer->isinWindow())
            {
                playSound("Villager_Gather");
            }
            farmer->setGatherer();
            farmer->setNowRes();
            //            qDebug()<<"设定为采集者";
        }
        else if(farmer->getnowobject()->ProductIsStone()||farmer->getnowobject()->ProductIsGold())
        {
            if(!farmer->isMiner()&&farmer->isinWindow())
            {
                playSound("Villager_Miner");
            }
            farmer->setMiner();
            farmer->setNowRes();
            //            qDebug()<<"设定为矿工";
        }
    }
}

//判断移动队列中的移动是否到位 以及方向是否需要调整
void MainWidget::judgemove()
{
    for(int i=0;i<MAXPLAYER;i++)
    {
        if(!player[i]->human.empty())
        {
            std::list<Human *>::iterator iter=player[i]->human.begin();
            while(iter!=player[i]->human.end())
            {
                if((*iter)->isFarmer())
                {
                    Farmer *farmer=(Farmer *)(*iter);
                    if(farmer->isGoToAttack())
                    {
                        if(judgementAttack(farmer,farmer->getnowobject()))
                        {
                            farmer->setIsAttacking();
                            farmer->setPreAttack();
                            farmer->setAngle(farmer->calculateAngle(farmer->getL0(),farmer->getU0()));
                            farmer->setActionOccurrenceGameframe(gameframe);
                        }
                        else if(farmer->reachDestination())
                        {
                            if(farmer->getnowobject()!=NULL)
                            {
                                movetoCoo(farmer,farmer->getnowobject());
                                farmer->setPreWalk();
                            }
                            else
                            {
                                farmer->setPreStand();
                                farmer->setisIdle();
                            }
                        }
                    }
                    else if(farmer->reachDestination())
                    {
                        if(!farmer->isStanding())//当前不属于站立状态
                        {
                            if(farmer->isIdleing())//没在进行流程
                            {
                                farmer->setPreStand();
                            }
                            else if(farmer->isGoToObjecting())//即前往到位
                            {
                                if(farmer->getnowobject()!=NULL)
                                {
                                    if(farmer->getnowobject()->isCoores())
                                    {
                                        Coores *temp=(Coores *)farmer->getnowobject();
                                        if(temp->ProductIsFood())
                                        {
                                            farmer->setIsGathering();
                                            farmer->setPreWork();
                                        }
                                        else if(temp->ProductIsStone())
                                        {
                                            farmer->setIsDiggingStone();
                                            farmer->setPreWork();
                                        }
                                        else if(temp->ProductIsGold())
                                        {
                                            farmer->setIsDiggingGold();
                                            farmer->setPreWork();
                                        }
                                    }
                                    else if(farmer->getnowobject()->isAnimal()||farmer->getnowobject()->isTreeForest())
                                    {
                                        Animal *animal=(Animal *)farmer->getnowobject();
                                        if(animal->isAlive())
                                        {
                                            farmer->setIsAttacking();
                                            farmer->setPreAttack();
                                            farmer->setActionOccurrenceGameframe(gameframe);
                                        }
                                        else
                                        {
                                            if(animal->isTreeForest())
                                            {
                                                farmer->setIsCutting();
                                                farmer->setPreWork();
                                            }
                                            else
                                            {
                                                farmer->setIsGathering();
                                                farmer->setPreWork();
                                            }
                                        }
                                    }
                                    else if(farmer->getnowobject()->isBuilding())
                                    {
                                        farmer->setIsConstruct();
                                        farmer->setPreWork();
                                    }
                                    else if(farmer->getnowobject()->isFarm())
                                    {
                                        Building *build=(Building *)farmer->getnowobject();
                                        if(build->isFinish())
                                        {
                                            farmer->setIsFarming();
                                            farmer->setPreWork();
                                        }
                                        else
                                        {
                                            farmer->setIsConstruct();
                                            farmer->setPreWork();
                                        }
                                    }
                                }
                                else
                                {
                                    farmer->setPreStand();
                                    farmer->setisIdle();
                                }
                            }
                            else if(farmer->isGoToCamp())
                            {
                                player[farmer->getCamp()]->addResource(farmer->getresourceSort(),farmer->getresource());
                                farmer->resetResource();
                                farmer->setisIdle();
                                farmer->setholdresourceIsNULL();
                                farmer->setNowRes();
                                if(farmer->getworkobject()!=NULL)
                                {
                                    farmer->setnowobject(farmer->getworkobject());
                                }
                                else
                                {

                                }
                            }
                        }
                    }
                }
                iter++;
            }
        }
    }


    if(!map->animal.empty())
    {
        std::list<Animal *>::iterator aniter=map->animal.begin();
        while(aniter!=map->animal.end())
        {
            Animal *animal=(Animal *)(*aniter);
            if(animal->isGazelle()||animal->isElephant()||animal->isLion())
            {
                if(animal->reachDestination())
                {
                    if(animal->isFleeing())
                    {
                        animal->setPreStand();
                        animal->setIdle();
                    }
                    if(animal->isChasing())
                    {
                        if(judgementAttack(animal,animal->getObject()))
                        {
                            animal->setPreAttack();
                            animal->setAttacking();
                            animal->setActionOccurrenceGameframe(gameframe);
                        }
                        else
                        {
                            movetoCoo(animal,animal->getObject());
                            if(animal->isLion())
                            {
                                animal->setPreRun();
                            }
                            else if(animal->isElephant())
                            {
                                animal->setPreWalk();
                            }
                        }
                    }

                }
            }
            aniter++;
        }
    }
}

void MainWidget::statusUpdate()
{
    ui->resFood->setText(QString::number(player[0]->getFood()));
    ui->resWood->setText(QString::number(player[0]->getWood()));
    ui->resStone->setText(QString::number(player[0]->getStone()));
    ui->resGold->setText(QString::number(player[0]->getGold()));

//    ui->mapView->screenL = ui->Game->getBlockL();
//    ui->mapView->screenU = ui->Game->getBlockU();
//    ui->mapView->update();
    ui->mapView->setSceenRect(ui->Game->getBlockL(), ui->Game->getBlockU());

    int newScore = 100;
    if(ui->selectwidget->getObj() != NULL && g_Object.find(currentSelectNum) != g_Object.end() && ui->selectwidget->getObj() == g_Object[currentSelectNum])
    {
        if(ui->selectwidget->getObj()->getActSpeed() > 0) ui->objText->setText(QString::number((int)(ui->selectwidget->getObj()->getActPercent())) + "%");
        else ui->objText->setText("");
    }//行动的进度
    if(ui->Game->buildMode == 0){
        QApplication::restoreOverrideCursor();
    }

    std::list<Building *>::iterator buildIt = player[0]->build.begin(), buildItTemp = buildIt;
    std::list<Human *>::iterator humanIt = player[0]->human.begin(), humanItTemp = humanIt;
    int human_num = 0, build_hold_human_num = 0;
    bool isGranaryBuilt = false, isStockBuilt = false;
    for(humanItTemp = player[0]->human.begin(); humanItTemp != player[0]->human.end(); humanItTemp++)
    {
        human_num++;
        newScore++;
    }
    for(buildItTemp = player[0]->build.begin(); buildItTemp != player[0]->build.end(); buildItTemp++)
    {
        if((*buildItTemp)->getNum() == BUILDING_HOME && (*buildItTemp)->isFinish()){
            build_hold_human_num+=HOUSE_HUMAN_NUM;
            newScore++;
        }
        else if((*buildItTemp)->getNum() == BUILDING_CENTER)
        {
            newScore += 5;
            build_hold_human_num+=HOUSE_HUMAN_NUM;
        }
        else if((*buildItTemp)->getNum() == BUILDING_ARROWTOWER && (*buildItTemp)->isFinish())
        {
            newScore += 3;
        }
        else if((*buildItTemp)->isFinish())
        {
            newScore += 2;
        }
        if((*buildItTemp)->getNum() == BUILDING_GRANARY && (*buildItTemp)->isFinish()) isGranaryBuilt = true;
        else if((*buildItTemp)->getNum() == BUILDING_STOCK && (*buildItTemp)->isFinish()) isStockBuilt = true;
    }
    if(player[0]->getCheatMaxHumanNum()) build_hold_human_num = MAX_HUMAN_NUM;
    player[0]->setMaxHumanNum(build_hold_human_num);
    newScore += (player[0]->getFood() / 100 + player[0]->getWood() / 100 + player[0]->getStone() / 100);
    if(player[0]->getCiv() == CIVILIZATION_TOOLAGE) newScore += 25;
    if(player[0]->getArrowTowerUnlocked()) newScore += 2;
    for(int i = 0; i < 3; i++)
    {
        if(player[0]->getMarketResearch(i)) newScore += 2;
    }
    int exploredBlocks = 0;
    for(int L = 0; L < MAP_L; L++)
    {
        for(int U = 0; U < MAP_U; U++)
        {
            if(map->cell[L][U].Explored) exploredBlocks++;
        }
    }
    newScore += exploredBlocks * 100 / (MAP_L * MAP_U);
    //计算得分
    player[0]->setScore(newScore);
    QString minute;
    if(elapsedSec / 60 < 10) minute = "0" + QString::number(elapsedSec / 60);
    else minute = QString::number(elapsedSec / 60);
    QString second;
    if(elapsedSec % 60 < 10) second = "0" + QString::number(elapsedSec % 60);
    else second = QString::number(elapsedSec % 60);
    ui->statusLbl->setText(getShowTime() + QString::fromStdString("\nPlayer1分数:") + QString::number(player[0]->getScore()));
    ui->version->setText(QString::fromStdString(GAME_VERSION));
    //先进行行动状态更新
    for(; buildIt != player[0]->build.end(); buildIt++)
    {
        if((*buildIt)->isFinish())
        {
            for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
            {
                (*buildIt)->setActNames(i, ACT_NULL);
                (*buildIt)->setActStatus(i, ACT_STATUS_ENABLED);
            }
            if((*buildIt)->getNum() == BUILDING_CENTER)
            {
                if((*buildIt)->getActSpeed() > 0)
                {
                    (*buildIt)->setActNames(0, ACT_STOP);
                    (*buildIt)->setActStatus(0, ACT_STATUS_ENABLED);
                }
                else
                {//创造村民判断
                    (*buildIt)->setActNames(0, ACT_CREATEFARMER);
                    if((player[0]->getFood() < BUILDING_CENTER_CREATEFARMER_FOOD && (*buildIt)->getActNames(0) == ACT_CREATEFARMER) && (*buildIt)->getActName() == ACT_NULL)
                    {
                        (*buildIt)->setActStatus(0, ACT_STATUS_DISABLED);
                    }//判断食物
                    else
                    {
                        if(human_num >= build_hold_human_num || human_num > MAX_HUMAN_NUM)
                        {
                            (*buildIt)->setActStatus(0, ACT_STATUS_DISABLED);
                        }
                        else
                        {
                            (*buildIt)->setActStatus(0, ACT_STATUS_ENABLED);
                        }
                    }//判断人口上限
                    //升级时代判断
                    if(player[0]->getCiv() == CIVILIZATION_STONEAGE)
                    {
                        (*buildIt)->setActNames(1, ACT_UPGRADE_AGE);
                        if((*buildIt)->getActName() == ACT_NULL && (player[0]->getFood() < BUILDING_CENTER_UPGRADE_FOOD || !(isGranaryBuilt && isStockBuilt)))
                        {
                            (*buildIt)->setActStatus(1, ACT_STATUS_DISABLED);
                        }
                        else
                        {
                            (*buildIt)->setActStatus(1, ACT_STATUS_ENABLED);
                        }
                    }
                }
            }
            else if((*buildIt)->getNum() == BUILDING_GRANARY)
            {
                if(player[0]->getCiv() == CIVILIZATION_TOOLAGE)
                {
                    if((*buildIt)->getActSpeed() > 0)
                    {
                        (*buildIt)->setActNames(0, ACT_STOP);
                    }
                    else
                    {
                        if(!player[0]->getArrowTowerUnlocked())
                        {
                            (*buildIt)->setActNames(0, ACT_UPGRADE_TOWERBUILD);
                            if(player[0]->getFood() < BUILDING_GRANARY_ARROWTOWER_FOOD)
                            {
                                (*buildIt)->setActStatus(0, ACT_STATUS_DISABLED);
                            }
                            else
                            {
                                (*buildIt)->setActStatus(0, ACT_STATUS_ENABLED);
                            }
                        }
                    }
                }
            }
            else if((*buildIt)->getNum() == BUILDING_MARKET)
            {
                if((*buildIt)->getActSpeed() > 0)
                {
                    (*buildIt)->setActNames(0, ACT_STOP);
                }
                else
                {
                    if(!player[0]->getMarketResearch(0))
                    {
                        (*buildIt)->setActNames(0, ACT_UPGRADE_WOOD);
                        if(player[0]->getFood() < BUILDING_MARKET_WOOD_UPGRADE_FOOD || player[0]->getWood() < BUILDING_MARKET_WOOD_UPGRADE_WOOD)
                        {
                            (*buildIt)->setActStatus(0, ACT_STATUS_DISABLED);
                        }
                        else
                        {
                            (*buildIt)->setActStatus(0, ACT_STATUS_ENABLED);
                        }
                    }
                    if(!player[0]->getMarketResearch(1))
                    {
                        (*buildIt)->setActNames(1, ACT_UPGRADE_STONE);
                        if(player[0]->getFood() < BUILDING_MARKET_STONE_UPGRADE_FOOD || player[0]->getStone() < BUILDING_MARKET_STONE_UPGRADE_STONE)
                        {
                            (*buildIt)->setActStatus(1, ACT_STATUS_DISABLED);
                        }
                        else
                        {
                            (*buildIt)->setActStatus(1, ACT_STATUS_ENABLED);
                        }
                    }
                    if(!player[0]->getMarketResearch(2))
                    {
                        (*buildIt)->setActNames(2, ACT_UPGRADE_FARM);
                        if(player[0]->getFood() < BUILDING_MARKET_FARM_UPGRADE_FOOD || player[0]->getWood() < BUILDING_MARKET_FARM_UPGRADE_WOOD)
                        {
                            (*buildIt)->setActStatus(2, ACT_STATUS_DISABLED);
                        }
                        else
                        {
                            (*buildIt)->setActStatus(2, ACT_STATUS_ENABLED);
                        }
                    }
                }
            }
        }
    }
    //再进行快捷栏和状态栏显示更新
    if(currentSelectNum <= 0)
    {
        ui->objHp->setText("");
        ui->objIcon->setPixmap(QPixmap());
        ui->objName->setText("");
        ui->objText->setText("");
        ui->objIconSmall->setPixmap(QPixmap());
        ui->selectwidget->hide();
        ui->selectwidget->update();
        ui->selectwidget->setObj(nullptr);
        ui->selectwidget->setObjType(0);
    }
    else
    {
        int type = currentSelectNum / 10000 % 10;
        if(type == 1)//建筑
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Building *objBuilding = (Building *)(g_Object[currentSelectNum]);
                int num = objBuilding->getNum();
                ui->objName->setText(QString::fromStdString(objBuilding->getDisplayName(num)));
                if(player[0]->getCiv() == CIVILIZATION_STONEAGE) ui->objIcon->setPixmap(resMap["Button_"+objBuilding->Builtname[player[0]->getCiv()][num]].front().scaled(110,110));
                else if(player[0]->getCiv() == CIVILIZATION_TOOLAGE) ui->objIcon->setPixmap(resMap["Button_"+objBuilding->Builtname[player[0]->getCiv()][num]].front().scaled(110,110));

                if(objBuilding->getActSpeed() != 0)
                {
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());
                }
                else
                {
                    ui->objIconSmall->setPixmap(QPixmap());
                }
                ui->objHp->setText(QString::number(objBuilding->getBlood()) + "/" + QString::number(objBuilding->getMaxBlood()));
                //同步行动状态至窗口
                if(objBuilding->isFinish())
                {
                    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
                    {
                        acts[i]->setActName(objBuilding->getActNames(i));
                        acts[i]->setStatus(objBuilding->getActStatus(i));
                        if(objBuilding->getActNames(i) != ACT_NULL)
                        {
                            if(objBuilding->getActNames(i) == ACT_STOP) acts[i]->setPix(resMap["Button_Stop"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_CREATEFARMER) acts[i]->setPix(resMap["Button_Village"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_AGE) acts[i]->setPix(resMap["ButtonTech_Center1"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_FARM) acts[i]->setPix(resMap["ButtonTech_Cow"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_STONE) acts[i]->setPix(resMap["ButtonTech_Stone"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_WOOD) acts[i]->setPix(resMap["ButtonTech_Lumber"].front().scaled(80,80));
                            else if(objBuilding->getActNames(i) == ACT_UPGRADE_TOWERBUILD) acts[i]->setPix(resMap["ButtonTech_ArrowTower"].front().scaled(80,80));
                            acts[i]->show();
                        }
                        else acts[i]->hide();
                    }
                    if(objBuilding->getNum() == BUILDING_HOME)
                    {
                        ui->objText->setText(QString::number(human_num) + "/" + QString::number(build_hold_human_num));
                        ui->objIconSmall->setPixmap(resMap["SmallIcon_People"].front());
                    }
                    else if(objBuilding->getNum() == BUILDING_FARM)
                    {
                        if(objBuilding->getCnt() > 0)
                        {
                            ui->objText->setText(QString::number((int)(objBuilding->getCnt())));
                            ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                        }
                        else
                        {
                            receiveselectNum(0);
                        }
                    }
                }
                else
                {
                    ui->objText->setText(QString::number((int)(objBuilding->getPercent())) + "%");
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Sandglass"].front());
                }
                ui->selectwidget->update();
                ui->selectwidget->show();
            }
            else
            {
                receiveselectNum(0);
            }
        }
        else if(type == 2)//块资源
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Coores *objCoores = (Coores *)(g_Object[currentSelectNum]);
                int num = objCoores->getNum();
                ui->selectwidget->setObj(g_Object[currentSelectNum]);
                ui->selectwidget->setObjType(type);
                ui->objName->setText(QString::fromStdString(objCoores->getCooresDisplayName(num)));
                ui->objHp->setText("");
                ui->objIcon->setPixmap(resMap["Button_"+objCoores->getCooresName(num)].front().scaled(80,80));
                ui->objIconSmall->setPixmap(QPixmap());
                if(objCoores->getNum() == 0) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                else if(objCoores->getNum() == 1) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
                ui->objText->setText(QString::number((int)(objCoores->getCnt())));
                ui->selectwidget->update();
                ui->selectwidget->show();
            }
            else
            {
                receiveselectNum(0);
            }
        }
        else if(type == 5)//村民
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Farmer *objFarmer = (Farmer *)(g_Object.find(currentSelectNum)->second);
                if(!objFarmer->isDie())
                {
                    int num = objFarmer->getstate();
                    ui->selectwidget->setObj((Coordinate *)objFarmer);
                    ui->selectwidget->setObjType(type);
                    QString name = QString::fromStdString(objFarmer->getDisplayName(num));
                    ui->objName->setText(name);
                    ui->objIcon->setPixmap(resMap["Button_Village"].front().scaled(110,110));
                    if(objFarmer->getresourceSort() == 1) ui->objIconSmall->setPixmap(resMap["Icon_Wood"].front());
                    else if(objFarmer->getresourceSort() == 2||objFarmer->getresourceSort() == 5) ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                    else if(objFarmer->getresourceSort() == 3) ui->objIconSmall->setPixmap(resMap["Icon_Stone"].front());
                    if(objFarmer->getresource() == 0)
                    {
                        ui->objIconSmall->setPixmap(QPixmap());
                        ui->objText->setText("");
                    }
                    else
                    {
                        ui->objText->setText(QString::number((int)(objFarmer->getresource())));
                    }
                    ui->objHp->setText(QString::number(objFarmer->getBlood()) + "/" + QString::number(objFarmer->getMaxBlood()));
                    ui->selectwidget->update();
                    ui->selectwidget->show();
                }
                else
                {
                    receiveselectNum(0);
                }
            }
        }
        else if(type == 6)//树
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Animal *objAnimal = (Animal *)(g_Object[currentSelectNum]);
                if(objAnimal->getCnt() <= 0) receiveselectNum(0);
                else
                {
                    ui->selectwidget->setObj((Coordinate *)objAnimal);
                    ui->selectwidget->setObjType(type);
                    ui->objName->setText("树");
                    ui->objIcon->setPixmap(resMap["Button_Tree"].front().scaled(110,110));
                    ui->objIconSmall->setPixmap(resMap["SmallIcon_Wood"].front());
                    ui->objText->setText(QString::number((int)(objAnimal->getCnt())));
                    if(objAnimal->isDie()) ui->objHp->setText("");
                    else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));
                    ui->selectwidget->update();
                    ui->selectwidget->show();
                }
            }
            else
            {
                receiveselectNum(0);
            }
        }
        else if(type == 7)//动物
        {
            if(g_Object.find(currentSelectNum)!=g_Object.end())
            {
                Animal *objAnimal = (Animal *)(g_Object[currentSelectNum]);
                if(objAnimal->getCnt() <= 0) receiveselectNum(0);
                else
                {
                    ui->selectwidget->setObj((Coordinate *)objAnimal);
                    ui->selectwidget->setObjType(type);
                    if(objAnimal->getNum() == 1)//瞪羚
                    {
                        ui->objName->setText("瞪羚");
                        ui->objIcon->setPixmap(resMap["Button_Gazelle"].front().scaled(110,110));

                    }
                    else if(objAnimal->getNum() == 2)//大象
                    {
                        ui->objName->setText("大象");
                        ui->objIcon->setPixmap(resMap["Button_Elephant"].front().scaled(110,110));
                    }
                    else if(objAnimal->getNum() == 3)
                    {
                        ui->objName->setText("狮子");
                        ui->objIcon->setPixmap(resMap["Button_Lion"].front().scaled(110,110));
                    }
                    ui->objIconSmall->setPixmap(resMap["Icon_Food"].front());
                    ui->objText->setText(QString::number((int)(objAnimal->getCnt())));
                    if(objAnimal->isDie()) ui->objHp->setText("");
                    else ui->objHp->setText(QString::number(objAnimal->getBlood()) + "/" + QString::number(objAnimal->getMaxBlood()));
                    ui->selectwidget->update();
                    ui->selectwidget->show();
                }
            }
            else
            {
                receiveselectNum(0);
            }
        }
    }
    for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
    {
        acts[i]->update();
    }
}

void MainWidget::refreshActs()
{
    ui->tipLbl->setText("");
    ui->objText->setText("");
    int type = currentSelectNum / 10000 % 10;
    if(g_Object.find(currentSelectNum) == g_Object.end())
    {
        for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            acts[i]->setActName(ACT_NULL);
            acts[i]->update();
            acts[i]->hide();
            acts[i]->setStatus(ACT_STATUS_ENABLED);
        }
        return;
    }
    ui->selectwidget->setObjType(type);
    ui->selectwidget->setObj(g_Object[currentSelectNum]);
    if(type == 5)
    {
        for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            acts[i]->setActName(ACT_NULL);
            acts[i]->setPix(QPixmap());
            acts[i]->setStatus(ACT_STATUS_ENABLED);
            acts[i]->hide();
            acts[i]->update();
        }
        acts[0]->setActName(ACT_BUILD);
        acts[0]->setPix(resMap["Button_Build"].front().scaled(80,80));
        acts[0]->show();
        acts[0]->update();
    }
    else
    {
        for(int i = 0; i < ACT_WINDOW_NUM_FREE; i++)
        {
            acts[i]->setActName(ACT_NULL);
            acts[i]->update();
            acts[i]->hide();
            acts[i]->setStatus(ACT_STATUS_ENABLED);
        }
    }
}

void MainWidget::widgetAct(int num)
{
    if(acts[num]->getStatus() == ACT_STATUS_DISABLED) return;
    int actName = acts[num]->getActName();
    doAct(actName);
}

int MainWidget::doAct(int actName)
{
    if(ui->selectwidget->getObj() == NULL) return ACTION_INVALID_ACTION;
    if(actName == ACT_BUILD){
        acts[0]->setPix(resMap["Button_House1"].front().scaled(80,80));
        acts[0]->show();
        acts[0]->setActName(ACT_BUILD_HOUSE);
        acts[1]->setPix(resMap["Button_Granary"].front().scaled(80,80));
        acts[1]->show();
        acts[1]->setActName(ACT_BUILD_GRANARY);
        acts[2]->setPix(resMap["Button_Stock"].front().scaled(80,80));
        acts[2]->show();
        acts[2]->setActName(ACT_BUILD_STOCK);
        acts[3]->setPix(resMap["Exit"].front().scaled(80,80));
        acts[3]->show();
        acts[3]->setActName(ACT_BUILD_CANCEL);
        if(player[0]->getWood() < BUILD_HOUSE_WOOD)
        {
            acts[0]->setStatus(ACT_STATUS_DISABLED);
        }
        if(player[0]->getWood() < BUILD_GRANARY_WOOD)
        {
            acts[1]->setStatus(ACT_STATUS_DISABLED);
            acts[2]->setStatus(ACT_STATUS_DISABLED);
        }
        if(player[0]->getCiv() == CIVILIZATION_TOOLAGE)
        {
            acts[3]->setPix(resMap["Button_Farm"].front().scaled(80,80));
            acts[3]->show();
            acts[3]->setActName(ACT_BUILD_FARM);
            acts[4]->setPix(resMap["Button_Market"].front().scaled(80,80));
            acts[4]->show();
            acts[4]->setActName(ACT_BUILD_MARKET);
            acts[5]->setPix(resMap["Button_ArrowTower"].front().scaled(80,80));
            acts[5]->show();
            acts[5]->setActName(ACT_BUILD_ARROWTOWER);
            acts[6]->setPix(resMap["Exit"].front().scaled(80,80));
            acts[6]->show();
            acts[6]->setActName(ACT_BUILD_CANCEL);
            std::list<Building *>::iterator buildIt = player[0]->build.begin();
            bool isGranaryBuilt = false, isMarketBuilt = false;
            for(; buildIt != player[0]->build.end(); buildIt++)
            {
                if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish())//建成谷仓
                {
                    isGranaryBuilt = true;
                }
                if((*buildIt)->getNum() == BUILDING_MARKET && (*buildIt)->isFinish())//建成市场
                {
                    isMarketBuilt = true;
                }
            }
            if(!isGranaryBuilt || player[0]->getWood() < BUILD_MARKET_WOOD)
            {
                acts[4]->setStatus(ACT_STATUS_DISABLED);
            }
            if(!isMarketBuilt || player[0]->getWood() < BUILD_FARM_WOOD)
            {
                acts[3]->setStatus(ACT_STATUS_DISABLED);
            }
            if(!player[0]->getArrowTowerUnlocked() || player[0]->getStone() < BUILD_ARROWTOWER_STONE)
            {
                acts[5]->setStatus(ACT_STATUS_DISABLED);
            }
        }

    }
    else if(actName == ACT_BUILD_HOUSE){
        ui->Game->buildMode = BUILDING_HOME + 1;
        QCursor my(resMap["House1"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_GRANARY){
        ui->Game->buildMode = BUILDING_GRANARY + 1;
        QCursor my(resMap["Granary"].front());
        QApplication::setOverrideCursor(my);
    }
    else if(actName == ACT_BUILD_STOCK){
        QCursor my(resMap["Stock"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_STOCK + 1;
    }
    else if(actName == ACT_BUILD_FARM){
        QCursor my(resMap["Farm"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_FARM + 1;
    }
    else if(actName == ACT_BUILD_MARKET){
        QCursor my(resMap["Market"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_MARKET + 1;
    }
    else if(actName == ACT_BUILD_ARROWTOWER){
        QCursor my(resMap["ArrowTower"].front());
        QApplication::setOverrideCursor(my);
        ui->Game->buildMode = BUILDING_ARROWTOWER + 1;
    }
    else if(actName == ACT_BUILD_CANCEL)
    {
        QApplication::restoreOverrideCursor();
        ui->Game->buildMode = 0;
    }
    else if(actName == ACT_CREATEFARMER){
        if(player[0]->getFood() < BUILDING_CENTER_CREATEFARMER_FOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_CENTER_CREATEFARMER_FOOD);
        ui->selectwidget->getObj()->setActPercent(0);
        ui->selectwidget->getObj()->setActSpeed(0.25);
        ui->selectwidget->getObj()->setActNum(BUILDING_CENTER_CREATEFARMER);
        ui->selectwidget->getObj()->setActName(ACT_CREATEFARMER);
    }
    else if(actName == ACT_UPGRADE_AGE)
    {
        std::list<Building *>::iterator buildIt = player[0]->build.begin();
        bool isGranaryBuilt = false, isStockBuilt = false;
        for(; buildIt != player[0]->build.end(); buildIt++)
        {
            if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish()) isGranaryBuilt = true;
            else if((*buildIt)->getNum() == BUILDING_STOCK && (*buildIt)->isFinish()) isStockBuilt = true;
        }
        if(!isGranaryBuilt || !isStockBuilt) return ACTION_INVALID_ACTION;

        if(player[0]->getFood() < BUILDING_CENTER_UPGRADE_FOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_CENTER_UPGRADE_FOOD);
        ui->selectwidget->getObj()->setActPercent(0);
        ui->selectwidget->getObj()->setActSpeed(0.05);
        ui->selectwidget->getObj()->setActNum(BUILDING_CENTER_UPGRADE);
        ui->selectwidget->getObj()->setActName(ACT_UPGRADE_AGE);
    }
    else if(actName == ACT_UPGRADE_TOWERBUILD)
    {
        if(player[0]->getFood() < BUILDING_GRANARY_ARROWTOWER_FOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_GRANARY_ARROWTOWER_FOOD);
        ui->selectwidget->getObj()->setActPercent(0);
        ui->selectwidget->getObj()->setActSpeed(0.25);
        ui->selectwidget->getObj()->setActNum(BUILDING_GRANARY_ARROWTOWER);
        ui->selectwidget->getObj()->setActName(ACT_UPGRADE_TOWERBUILD);
    }
    else if(actName == ACT_UPGRADE_WOOD)
    {
        if(player[0]->getFood() < BUILDING_MARKET_WOOD_UPGRADE_FOOD || player[0]->getWood() < BUILDING_MARKET_WOOD_UPGRADE_WOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_MARKET_WOOD_UPGRADE_FOOD);
        player[0]->setWood(player[0]->getWood() - BUILDING_MARKET_WOOD_UPGRADE_WOOD);
        ui->selectwidget->getObj()->setActPercent(0);
        ui->selectwidget->getObj()->setActSpeed(0.25);
        ui->selectwidget->getObj()->setActNum(BUILDING_MARKET_WOOD_UPGRADE);
        ui->selectwidget->getObj()->setActName(ACT_UPGRADE_WOOD);
    }
    else if(actName == ACT_UPGRADE_STONE)
    {
        if(player[0]->getFood() < BUILDING_MARKET_STONE_UPGRADE_FOOD || player[0]->getStone() < BUILDING_MARKET_STONE_UPGRADE_STONE) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_MARKET_STONE_UPGRADE_FOOD);
        player[0]->setStone(player[0]->getStone() - BUILDING_MARKET_STONE_UPGRADE_STONE);
        ui->selectwidget->getObj()->setActPercent(0);
        ui->selectwidget->getObj()->setActSpeed(0.25);
        ui->selectwidget->getObj()->setActNum(BUILDING_MARKET_STONE_UPGRADE);
        ui->selectwidget->getObj()->setActName(ACT_UPGRADE_STONE);
    }
    else if(actName == ACT_UPGRADE_FARM)
    {
        if(player[0]->getFood() < BUILDING_MARKET_FARM_UPGRADE_FOOD || player[0]->getWood() < BUILDING_MARKET_FARM_UPGRADE_WOOD) return ACTION_INVALID_RESOURCE;
        player[0]->setFood(player[0]->getFood() - BUILDING_MARKET_FARM_UPGRADE_FOOD);
        player[0]->setWood(player[0]->getWood() - BUILDING_MARKET_FARM_UPGRADE_WOOD);
        ui->selectwidget->getObj()->setActPercent(0);
        ui->selectwidget->getObj()->setActSpeed(0.25);
        ui->selectwidget->getObj()->setActNum(BUILDING_MARKET_FARM_UPGRADE);
        ui->selectwidget->getObj()->setActName(ACT_UPGRADE_FARM);
    }
    else if(actName == ACT_STOP)
    {
        ui->selectwidget->getObj()->setActNum(0);
        if(ui->selectwidget->getObj()->getActName() == ACT_CREATEFARMER) player[0]->setFood(player[0]->getFood() + BUILDING_CENTER_CREATEFARMER_FOOD);
        else if(ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_AGE) player[0]->setFood(player[0]->getFood() + BUILDING_CENTER_UPGRADE_FOOD);
        else if(ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_TOWERBUILD) player[0]->setFood(player[0]->getFood() + BUILDING_GRANARY_ARROWTOWER_FOOD);
        else if(ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_WOOD)
        {
            player[0]->setFood(player[0]->getFood() + BUILDING_MARKET_WOOD_UPGRADE_FOOD);
            player[0]->setWood(player[0]->getWood() + BUILDING_MARKET_WOOD_UPGRADE_WOOD);
        }
        else if(ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_STONE)
        {
            player[0]->setFood(player[0]->getFood() + BUILDING_MARKET_STONE_UPGRADE_FOOD);
            player[0]->setStone(player[0]->getStone() + BUILDING_MARKET_STONE_UPGRADE_STONE);
        }
        else if(ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_FARM)
        {
            player[0]->setFood(player[0]->getFood() + BUILDING_MARKET_FARM_UPGRADE_FOOD);
            player[0]->setWood(player[0]->getWood() + BUILDING_MARKET_FARM_UPGRADE_WOOD);
        }
        if(ui->selectwidget->getObj()->getActName() == ACT_CREATEFARMER || ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_AGE)
        {
            ui->objText->setText("");
            ui->selectwidget->getObj()->setActSpeed(0);
            ui->selectwidget->getObj()->setActPercent(0);
            ui->selectwidget->getObj()->setActName(ACT_NULL);
        }
        else if(ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_TOWERBUILD)
        {
            ui->objText->setText("");
            ui->selectwidget->getObj()->setActSpeed(0);
            ui->selectwidget->getObj()->setActPercent(0);
            ui->selectwidget->getObj()->setActName(ACT_NULL);
        }
        else if(ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_WOOD || ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_STONE || ui->selectwidget->getObj()->getActName() == ACT_UPGRADE_FARM)
        {
            ui->objText->setText("");
            ui->selectwidget->getObj()->setActSpeed(0);
            ui->selectwidget->getObj()->setActPercent(0);
            ui->selectwidget->getObj()->setActName(ACT_NULL);
        }
        else
        {
            return ACTION_INVALID_ACTION;
        }
    }
    for(int i = 0; i < 10; i++)
    {
        acts[i]->update();
    }
    return ACTION_SUCCESS;
}

void MainWidget::debuginformation()
{
    //    Farmer *farmer=(Farmer *)player[0]->human.front();
    //    debugText("red","farmer L:"+QString::number(farmer->getL()));
    //    debugText("red","farmer U:"+QString::number(farmer->getU()));
    //    debugText("red","farmer L0:"+QString::number(farmer->getL0()));
    //    debugText("red","farmer U0:"+QString::number(farmer->getU0()));
    //    debugText("red","farmer nowstate:"+QString::number(farmer->getNowState()));
    //    debugText("red","farmer workstate:"+QString::number(farmer->getworkstate()));
    for(int i=0;i<72;i++)
    {
        for(int j=0;j<72;j++)
        {
            std::cout<<findPathMap[i][j]<<" ";
        }
        std::cout <<endl;
    }
}

Building *MainWidget::findGranary(Farmer *farmer)
{
    std::list<Building *>::iterator iter=player[farmer->getCamp()]->build.begin();
    Building *tempbuild=NULL;
    int mindistance;
    while(iter!=player[farmer->getCamp()]->build.end())
    {
        if((*iter)->isCenter()||((*iter)->isGranary()&&(*iter)->isFinish()))
        {
            if(tempbuild==NULL)
            {
                tempbuild=(*iter);
                mindistance=countdistance(farmer->getL(),farmer->getU(),(*iter)->getL(),(*iter)->getU());
            }
            else if(mindistance>=countdistance(farmer->getL(),farmer->getU(),(*iter)->getL(),(*iter)->getU()))
            {
                tempbuild=(*iter);
                mindistance=countdistance(farmer->getL(),farmer->getU(),(*iter)->getL(),(*iter)->getU());
            }
        }
        iter++;
    }
    movetoGameres(farmer,tempbuild);
    return tempbuild;
}

Building *MainWidget::findStock(Farmer *farmer)
{
    std::list<Building *>::iterator iter=player[farmer->getCamp()]->build.begin();
    Building *tempbuild=NULL;
    int mindistance;
    while(iter!=player[farmer->getCamp()]->build.end())
    {
        if((*iter)->isCenter()||((*iter)->isStock()&&(*iter)->isFinish()))
        {
            if(tempbuild==NULL)
            {
                tempbuild=(*iter);
                mindistance=countdistance(farmer->getL(),farmer->getU(),(*iter)->getL(),(*iter)->getU());
            }
            else if(mindistance>=countdistance(farmer->getL(),farmer->getU(),(*iter)->getL(),(*iter)->getU()))
            {
                tempbuild=(*iter);
                mindistance=countdistance(farmer->getL(),farmer->getU(),(*iter)->getL(),(*iter)->getU());
            }
        }
        iter++;
    }
    movetoGameres(farmer,tempbuild);
    return tempbuild;
}

void MainWidget::movetoCoo(MoveObject *moveobject, Coordinate *p)
{

    double x,y;
    double radius, distPH;
    radius=p->getRadius()+moveobject->getRadius();
    //LU不应该是半径

    // 计算 p 点与 human 点的距离
    distPH = countdistance(p->getL(),p->getU(), moveobject->getL(),moveobject->getU());

    // 判断 p 点与 human 点是否相交
    if (distPH <= radius)
    {
        // 两点相交，交点即为 human 点
        x = moveobject->getL();
        y = moveobject->getU();
    }
    else
    {
        int BlockL=p->getBlockL();
        int BlockU=p->getBlockU();
        double rd=radius/2.0;
        double distances[8];
        int coMap[8];
        coMap[0]=usefindPathMap(BlockL-1,BlockU-1);
        coMap[1]=usefindPathMap(BlockL-1,BlockU);
        coMap[2]=usefindPathMap(BlockL-1,BlockU+1);
        coMap[3]=usefindPathMap(BlockL,BlockU+1);
        coMap[4]=usefindPathMap(BlockL+1,BlockU+1);
        coMap[5]=usefindPathMap(BlockL+1,BlockU);
        coMap[6]=usefindPathMap(BlockL+1,BlockU-1);
        coMap[7]=usefindPathMap(BlockL,BlockU-1);

        distances[0]=countdistance(moveobject->getL(),moveobject->getU(),p->getL()-rd,p->getU()-rd);
        distances[1]=countdistance(moveobject->getL(),moveobject->getU(),p->getL()-radius,p->getU());
        distances[2]=countdistance(moveobject->getL(),moveobject->getU(),p->getL()-rd,p->getU()+rd);
        distances[3]=countdistance(moveobject->getL(),moveobject->getU(),p->getL(),p->getU()+radius);
        distances[4]=countdistance(moveobject->getL(),moveobject->getU(),p->getL()+rd,p->getU()+rd);
        distances[5]=countdistance(moveobject->getL(),moveobject->getU(),p->getL()+radius,p->getU());
        distances[6]=countdistance(moveobject->getL(),moveobject->getU(),p->getL()+rd,p->getU()-rd);
        distances[7]=countdistance(moveobject->getL(),moveobject->getU(),p->getL(),p->getU()-radius);

        double minDistance = distances[0];
        int minIndex = 0;

        for (int i = 1; i < 12; i++) {
            if (coMap[i] == 0 && distances[i] < minDistance) {
                minDistance = distances[i];
                minIndex = i;
            }
        }

        if(minIndex==0)
        {
            x=p->getL()-rd;
            y=p->getU()-rd;
        }
        else if(minIndex==1)
        {
            x=p->getL()-radius;
            y=p->getU();
        }
        else if(minIndex==2)
        {
            x=p->getL()-rd;
            y=p->getU()+rd;
        }
        else if(minIndex==3)
        {
            x=p->getL();
            y=p->getU()+radius;
        }
        else if(minIndex==4)
        {
            x=p->getL()+rd;
            y=p->getU()+rd;
        }
        else if(minIndex==5)
        {
            x=p->getL()+radius;
            y=p->getU();
        }
        else if(minIndex==6)
        {
            x=p->getL()+rd;
            y=p->getU()-rd;
        }
        else
        {
            x=p->getL();
            y=p->getU()-radius;
        }

    }
    moveobject->setdestination(x,y);
    Point start,destination;
    start.x=tranBlockL(moveobject->getL());
    start.y=tranBlockU(moveobject->getU());
    destination.x=tranBlockL(moveobject->getL0());
    destination.y=tranBlockU(moveobject->getU0());
    loadfindPathMap();
    moveobject->setPath(findPath(findPathMap,start,destination));
    //    moveobject->setPreWalk();
}

void MainWidget::movetoGameres(Human *human, Coordinate *p)
{
    loadfindPathMap();
    int BlockL=p->getBlockL();
    int BlockU=p->getBlockU();
    //    qDebug()<<"BlockL:"<<BlockL;
    //    qDebug()<<"BlockU:"<<BlockU;
    double L=human->getL();
    double U=human->getU();
    double LU=BLOCKSIDELENGTH/2.0;
    double xMin, yMin; // 最近的点的坐标
    if(p->isSmallFoundation())
    {
        double distances[12];
        int coMap[12];
        coMap[0]=usefindPathMap(BlockL-1,BlockU-1);
        coMap[1]=usefindPathMap(BlockL-1,BlockU);
        coMap[2]=usefindPathMap(BlockL-1,BlockU+1);
        coMap[3]=usefindPathMap(BlockL-1,BlockU+2);
        coMap[4]=usefindPathMap(BlockL,BlockU+2);
        coMap[5]=usefindPathMap(BlockL+1,BlockU+2);
        coMap[6]=usefindPathMap(BlockL+2,BlockU+2);
        coMap[7]=usefindPathMap(BlockL+2,BlockU+1);
        coMap[8]=usefindPathMap(BlockL+2,BlockU);
        coMap[9]=usefindPathMap(BlockL+2,BlockU-1);
        coMap[10]=usefindPathMap(BlockL+1,BlockU-1);
        coMap[11]=usefindPathMap(BlockL,BlockU-1);

        distances[0] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * BlockU - LU);
        distances[1] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 0.5));
        distances[2] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 1.5));
        distances[3] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 2) + LU);
        distances[4] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 0.5), BLOCKSIDELENGTH * (BlockU + 2) + LU);
        distances[5] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 1.5), BLOCKSIDELENGTH * (BlockU + 2) + LU);
        distances[6] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 2) + LU, BLOCKSIDELENGTH * (BlockU + 2) + LU);
        distances[7] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 2) + LU, BLOCKSIDELENGTH * (BlockU + 1.5));
        distances[8] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 2) + LU, BLOCKSIDELENGTH * (BlockU + 0.5));
        distances[9] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 2) + LU, BLOCKSIDELENGTH * BlockU - LU);
        distances[10] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 1.5), BLOCKSIDELENGTH * BlockU - LU);
        distances[11] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 0.5), BLOCKSIDELENGTH * BlockU - LU);
        double minDistance = INT_MAX;
        int minIndex = -1;

        for (int i = 0; i < 12; i++) {
            if (coMap[i] == 0 && distances[i] < minDistance) {
                minDistance = distances[i];
                minIndex = i;
            }
        }
        if (minIndex == 0)
        {
            xMin=BLOCKSIDELENGTH * BlockL - LU;
            yMin=BLOCKSIDELENGTH * BlockU - LU;
        }
        else if (minIndex == 1)
        {
            xMin=BLOCKSIDELENGTH * BlockL - LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 0.5);
        }
        else if (minIndex == 2)
        {
            xMin=BLOCKSIDELENGTH * BlockL - LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 1.5);
        }
        else if (minIndex == 3)
        {
            xMin=BLOCKSIDELENGTH * BlockL - LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 2) + LU;
        }
        else if (minIndex == 4)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 0.5);
            yMin=BLOCKSIDELENGTH * (BlockU + 2) + LU;
        }
        else if (minIndex == 5)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 1.5);
            yMin=BLOCKSIDELENGTH * (BlockU + 2) + LU;
        }
        else if (minIndex == 6)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 2) + LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 2) + LU;
        }
        else if (minIndex == 7)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 2) + LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 1.5);
        }
        else if (minIndex == 8)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 2) + LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 0.5);
        }
        else if (minIndex == 9)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 2) + LU;
            yMin=BLOCKSIDELENGTH * BlockU - LU;
        }
        else if (minIndex == 10)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 1.5);
            yMin=BLOCKSIDELENGTH * BlockU - LU;
        }
        else
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 0.5);
            yMin=BLOCKSIDELENGTH * BlockU - LU;
        }

    }
    else if(p->isMiddleFoundation())
    {
        double distances[16];
        int coMap[16];

        coMap[0] = usefindPathMap(BlockL - 1, BlockU - 1);
        coMap[1] = usefindPathMap(BlockL - 1, BlockU);
        coMap[2] = usefindPathMap(BlockL - 1, BlockU + 1);
        coMap[3] = usefindPathMap(BlockL - 1, BlockU + 2);
        coMap[4] = usefindPathMap(BlockL - 1, BlockU + 3);
        coMap[5] = usefindPathMap(BlockL, BlockU + 3);
        coMap[6] = usefindPathMap(BlockL + 1, BlockU + 3);
        coMap[7] = usefindPathMap(BlockL + 2, BlockU + 3);
        coMap[8] = usefindPathMap(BlockL + 3, BlockU + 3);
        coMap[9] = usefindPathMap(BlockL + 3, BlockU + 2);
        coMap[10] = usefindPathMap(BlockL + 3, BlockU + 1);
        coMap[11] = usefindPathMap(BlockL + 3, BlockU);
        coMap[12] = usefindPathMap(BlockL + 3, BlockU - 1);
        coMap[13] = usefindPathMap(BlockL + 2, BlockU - 1);
        coMap[14] = usefindPathMap(BlockL + 1, BlockU - 1);
        coMap[15] = usefindPathMap(BlockL, BlockU - 1);

        distances[0] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * BlockU - LU);
        distances[1] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 0.5));
        distances[2] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 1.5));
        distances[3] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 2.5));
        distances[4] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 3) + LU);
        distances[5] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 0.5), BLOCKSIDELENGTH * (BlockU + 3) + LU);
        distances[6] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 1.5), BLOCKSIDELENGTH * (BlockU + 3) + LU);
        distances[7] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 2.5), BLOCKSIDELENGTH * (BlockU + 3) + LU);
        distances[8] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 3) + LU, BLOCKSIDELENGTH * (BlockU + 3) + LU);
        distances[9] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 3) + LU, BLOCKSIDELENGTH * (BlockU + 2.5));
        distances[10] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 3) + LU, BLOCKSIDELENGTH * (BlockU + 1.5));
        distances[11] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 3) + LU, BLOCKSIDELENGTH * (BlockU + 0.5));
        distances[12] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 3) + LU, BLOCKSIDELENGTH * BlockU - LU);
        distances[13] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 2.5), BLOCKSIDELENGTH * BlockU - LU);
        distances[14] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 1.5), BLOCKSIDELENGTH * BlockU - LU);
        distances[15] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 0.5), BLOCKSIDELENGTH * BlockU - LU);

        double minDistance = INT_MAX;
        int minIndex = -1;

        for (int i = 0; i < 16; i++) {
            if (coMap[i] == 0 && distances[i] < minDistance) {
                minDistance = distances[i];
                minIndex = i;
            }
        }

        if (minIndex == 0)
        {
            xMin = BLOCKSIDELENGTH * BlockL - LU;
            yMin = BLOCKSIDELENGTH * BlockU - LU;
        }
        else if (minIndex == 1)
        {
            xMin = BLOCKSIDELENGTH * BlockL - LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 0.5);
        }
        else if (minIndex == 2)
        {
            xMin = BLOCKSIDELENGTH * BlockL - LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 1.5);
        }
        else if (minIndex == 3)
        {
            xMin = BLOCKSIDELENGTH * BlockL - LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 2.5);
        }
        else if (minIndex == 4)
        {
            xMin = BLOCKSIDELENGTH * BlockL - LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 3) + LU;
        }
        else if (minIndex == 5)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 0.5);
            yMin = BLOCKSIDELENGTH * (BlockU + 3) + LU;
        }
        else if (minIndex == 6)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 1.5);
            yMin = BLOCKSIDELENGTH * (BlockU + 3) + LU;
        }
        else if (minIndex == 7)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 2.5);
            yMin = BLOCKSIDELENGTH * (BlockU + 3) + LU;
        }
        else if (minIndex == 8)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 3) + LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 3) + LU;
        }
        else if (minIndex == 9)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 3) + LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 2.5);
        }
        else if (minIndex == 10)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 3) + LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 1.5);
        }
        else if (minIndex == 11)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 3) + LU;
            yMin = BLOCKSIDELENGTH * (BlockU + 0.5);
        }
        else if (minIndex == 12)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 3) + LU;
            yMin = BLOCKSIDELENGTH * BlockU - LU;
        }
        else if (minIndex == 13)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 2.5);
            yMin = BLOCKSIDELENGTH * BlockU - LU;
        }
        else if (minIndex == 14)
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 1.5);
            yMin = BLOCKSIDELENGTH * BlockU - LU;
        }
        else
        {
            xMin = BLOCKSIDELENGTH * (BlockL + 0.5);
            yMin = BLOCKSIDELENGTH * BlockU - LU;
        }
        //        else
        //        {
        //            qDebug()<<"无路可走";
        //            return;
        //        }

    }
    else if(p->isBlockFoundation())
    {
        double distances[8];
        int coMap[8];
        coMap[0]=usefindPathMap(BlockL-1,BlockU-1);
        coMap[1]=usefindPathMap(BlockL-1,BlockU);
        coMap[2]=usefindPathMap(BlockL-1,BlockU+1);
        coMap[3]=usefindPathMap(BlockL,BlockU+1);
        coMap[4]=usefindPathMap(BlockL+1,BlockU+1);
        coMap[5]=usefindPathMap(BlockL+1,BlockU);
        coMap[6]=usefindPathMap(BlockL+1,BlockU-1);
        coMap[7]=usefindPathMap(BlockL,BlockU-1);

        distances[0] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * BlockU - LU);
        distances[1] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 0.5));
        distances[2] = countdistance(L, U, BLOCKSIDELENGTH * BlockL - LU, BLOCKSIDELENGTH * (BlockU + 1) + LU);
        distances[3] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 0.5), BLOCKSIDELENGTH * (BlockU + 1) + LU);
        distances[4] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 1) + LU, BLOCKSIDELENGTH * (BlockU + 1) + LU);
        distances[5] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 1) + LU, BLOCKSIDELENGTH * (BlockU + 0.5));
        distances[6] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 1) + LU, BLOCKSIDELENGTH * BlockU - LU);
        distances[7] = countdistance(L, U, BLOCKSIDELENGTH * (BlockL + 0.5), BLOCKSIDELENGTH * BlockU - LU);

        double minDistance = INT_MAX;
        int minIndex = -1;

        for (int i = 0; i < 8; i++) {
            if (coMap[i] == 0 && distances[i] < minDistance) {
                minDistance = distances[i];
                minIndex = i;
            }
        }
        if (minIndex == 0)
        {
            xMin=BLOCKSIDELENGTH * BlockL - LU;
            yMin=BLOCKSIDELENGTH * BlockU - LU;
        }
        else if (minIndex == 1)
        {
            xMin=BLOCKSIDELENGTH * BlockL - LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 0.5);
        }
        else if (minIndex == 2)
        {
            xMin=BLOCKSIDELENGTH * BlockL - LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 1) + LU;
        }
        else if (minIndex == 3)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 0.5);
            yMin=BLOCKSIDELENGTH * (BlockU + 1) + LU;
        }
        else if (minIndex == 4)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 1) + LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 1) + LU;
        }
        else if (minIndex == 5)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 1) + LU;
            yMin=BLOCKSIDELENGTH * (BlockU + 0.5);
        }
        else if (minIndex == 6)
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 1) + LU;
            yMin=BLOCKSIDELENGTH * BlockU - LU;
        }
        else
        {
            xMin=BLOCKSIDELENGTH * (BlockL + 0.5);
            yMin=BLOCKSIDELENGTH * BlockU - LU;
        }
        //        else
        //        {
        //            qDebug()<<"无路可走";
        //            return;
        //        }
    }




    //    double LU=p->getLU()+human->getLU();
    //    double cx=p->getL();
    //    double cy=p->getU();



    //    // 平移坐标系
    //    x -= cx;
    //    y -= cy;



    //    if((x<=LU/2.0&&x>=-LU/2.0)&&(y<=LU/2.0&&y>=-LU/2.0))
    //    {
    //        xMin = x;
    //        yMin = y;
    //        qDebug()<<"在建筑内";
    //    }
    //    else if((x<=LU/2.0&&x>=-LU/2.0)&&(y>LU/2.0||y<-LU/2.0))
    //    {
    //        double d1 = abs(y - LU/2.0);
    //        double d2 = abs(y + LU/2.0);

    //        // 找到最小距离对应的边中点
    //        if (d1 == min({ d1, d2 }))
    //        {
    //            xMin = x;
    //            yMin = LU/2.0;
    //        }
    //        else if (d2 == min({ d1, d2}))
    //        {
    //            xMin = x;
    //            yMin = -LU/2.0;
    //        }
    //    }
    //    else if((x>LU/2.0||x<-LU/2.0)&&(y<=LU/2.0&&y>=-LU/2.0))
    //    {
    //        double d3 = abs(x + LU/2.0);
    //        double d4 = abs(x - LU/2.0);

    //        // 找到最小距离对应的边中点
    //        if (d3 == min({d3, d4 }))
    //        {
    //            xMin = -LU/2.0;
    //            yMin = y;
    //        }
    //        else
    //        {
    //            xMin = LU/2.0;
    //            yMin = y;
    //        }
    //    }
    //    else
    //    {
    //        // 计算到四个顶点距离
    //        double d1 = countdistance(x,y,LU/2.0,LU/2.0);
    //        double d2 = countdistance(x,y,-LU/2.0,LU/2.0);
    //        double d3 = countdistance(x,y,-LU/2.0,-LU/2.0);
    //        double d4 = countdistance(x,y,LU/2.0,-LU/2.0);

    //        // 找到最小距离对应的顶点
    //        if (d1 == min({ d1, d2, d3, d4 }))
    //        {
    //            xMin = LU/2.0;
    //            yMin = LU/2.0;
    //        }
    //        else if (d2 == min({ d1, d2, d3, d4 }))
    //        {
    //            xMin = -LU/2.0;
    //            yMin = LU/2.0;
    //        }
    //        else if (d3 == min({ d1, d2, d3, d4 }))
    //        {
    //            xMin = -LU/2.0;
    //            yMin = -LU/2.0;
    //        }
    //        else
    //        {
    //            xMin = LU/2.0;
    //            yMin = -LU/2.0;
    //        }
    //    }

    //    // 平移回原坐标系
    //    xMin += cx;
    //    yMin += cy;

    //    //    qDebug()<<"moveL"<<xMin;
    //    //    qDebug()<<"moveU"<<yMin;
    //    //    qDebug()<<"moveBlockL"<<xMin/BLOCKSIDELENGTH;
    //    //    qDebug()<<"moveBlockU"<<yMin/BLOCKSIDELENGTH;

    //    qDebug()<<"xMin:"<<xMin;
    //    qDebug()<<"yMin:"<<yMin;

    human->setdestination(xMin,yMin);
    Point start,destination;
    start.x=tranBlockL(human->getL());
    start.y=tranBlockU(human->getU());
    destination.x=tranBlockL(human->getL0());
    destination.y=tranBlockU(human->getU0());
    human->setPath(findPath(findPathMap,start,destination));
    human->setPreWalk();
}

double MainWidget::tranL(double BlockL)
{
    double L;
    L= BlockL * 16 * gen5;
    return L;
}

double MainWidget::tranU(double BlockU)
{
    double U;
    U= BlockU * 16 * gen5;
    return U;
}

bool MainWidget::isValidPoint(const int (&map)[MAP_L][MAP_U], const Point &p)
{
    int rows = MAP_L;
    int cols = MAP_U;
    return (p.x >= 0 && p.x < rows && p.y >= 0 && p.y < cols);
}

vector<Point> MainWidget::getAdjacentPoints(const int (&map)[MAP_L][MAP_U], const Point &p)
{
    vector<Point> adjacentPoints;

    // 八个相邻正方向的偏移量
    int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
    int dy[] = { 0, 0, -1, 1, -1, 1, -1, 1 };

    for (int i = 0; i < 8; i++) {
        int newX = p.x + dx[i];
        int newY = p.y + dy[i];
        Point newPoint = { newX, newY };
        if (isValidPoint(map, newPoint) && map[newX][newY] != 1) {
            if(abs(dx[i])+abs(dy[i])==2)
            {
                if(map[newX][p.y]!=1&&map[p.x][newY]!=1)
                {
                    adjacentPoints.push_back(newPoint);
                }
            }
            else adjacentPoints.push_back(newPoint);
        }
    }
    return adjacentPoints;
}

stack<Point> MainWidget::findPath(const int (&findPathMap)[MAP_L][MAP_U], const Point &start, const Point &destination)
{
    int rows = 72;
    int cols = 72;

    // 记录已访问的点
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    // 使用map记录每个点的前驱点，以便回溯路径
    vector<vector<Point>> prev(rows, vector<Point>(cols));

    // 使用queue保存路径
    queue<Point> q;

    // 广度优先搜索
    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        // 找到目标点，回溯路径
        if (current.x == destination.x && current.y == destination.y) {
            stack<Point> pathStack;
            while (!(current.x == start.x && current.y == start.y)) {
                pathStack.push(current);
                current = prev[current.x][current.y];
            }
            return pathStack;
        }

        vector<Point> adjacentPoints = getAdjacentPoints(findPathMap, current);
        for (const Point& next : adjacentPoints) {
            if (!visited[next.x][next.y]) {
                visited[next.x][next.y] = true;
                q.push(next);
                prev[next.x][next.y] = current;
            }
        }
    }

    return findPathAlternative(map->intmap,start,destination);
}

stack<Point> MainWidget::findPathAlternative(const int (&map)[MAP_L][MAP_U], const Point &start, const Point &destination)
{
    int rows = 72;
    int cols = 72;

    // 记录已访问的点
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    // 使用map记录每个点的前驱点，以便回溯路径
    vector<vector<Point>> prev(rows, vector<Point>(cols));

    // 使用queue保存路径
    queue<Point> q;

    // 广度优先搜索
    q.push(start);
    visited[start.x][start.y] = true;

    while (!q.empty()) {
        Point current = q.front();
        q.pop();

        // 找到目标点，回溯路径
        if (current.x == destination.x && current.y == destination.y) {
            stack<Point> pathStack;
            while (!(current.x == start.x && current.y == start.y)) {
                pathStack.push(current);
                current = prev[current.x][current.y];
            }
            return pathStack;
        }

        vector<Point> adjacentPoints = getAdjacentPoints(map, current);
        for (const Point& next : adjacentPoints) {
            if (!visited[next.x][next.y]) {
                visited[next.x][next.y] = true;
                q.push(next);
                prev[next.x][next.y] = current;
            }
        }
    }

    return stack<Point>();
}

bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
    for(int i = 0; i < 10; i++)
    {
        if(watched == acts[i] && !acts[i]->isHidden()) {
            if(event->type() == QEvent::HoverEnter) {
                ui->tipLbl->setText(QString::fromStdString(actNames[acts[i]->getActName()]));
                if(acts[i]->getStatus() != 2)
                {
                    acts[i]->setStatus(0);
                    acts[i]->update();
                }
            }
            else if(event->type() == QEvent::MouseButtonRelease && acts[i]->getStatus() != 2){
                acts[i]->setStatus(0);
            }
            else if(event->type() == QEvent::MouseButtonPress && acts[i]->getStatus() != 2){
                acts[i]->setStatus(1);
            }
            else if(event->type() == QEvent::HoverLeave && acts[i]->getStatus() != 2)
            {
                ui->tipLbl->setText("");
            }
        }
    }
    return QWidget::eventFilter(watched,event);
}

Building *MainWidget::getView(int BlockL, int BlockU, int num)
{
    if(BlockL < 0 || BlockL > 71 || BlockU < 0 || BlockU > 71)
    {
        debugText("red", " 建造失败,选中位置越界");
        return nullptr;//位置越界
    }
    if(!((num >= 1 && num <= 2) || (num >= 4 && num <= 7)))
    {
        debugText("red", " 建造失败,建筑类型不存在");
        return nullptr;//BuildingNum不存在
    }
    if(!map->cell[BlockL][BlockU].Explored)
    {
        debugText("red", " 建造失败,选中位置未被探索");
        return nullptr;//没有被探索过
    }

    Building *newBuild;
    if(nowobject==NULL&&lastnowobject!=NULL)
    {
        if(lastnowobject->isFarmer())
        {
            Farmer *farmer=(Farmer *)lastnowobject;
            //            player[0]->setWood(player[0]->getWood()-50);
            int LU0;
            bool isOverlap = false;
            std::list<Building *>::iterator buildIt = player[0]->build.begin();
            std::list<Human *>::iterator humanIt = player[0]->human.begin();
            std::list<Animal *>::iterator animalIt = map->animal.begin();
            std::list<Coores *>::iterator cooresIt = map->coores.begin();
            if(num == 1 || num == 7) LU0 = 2;
            else if(num == 2 || num == 4 || num == 6 || num == 5) LU0 = 3;
            int blockP2L = BlockL + LU0 - 1, blockP2U = BlockU + LU0 - 1;//最大L,U点
            if(num == 1 || num == 7)
            {
                if(blockP2L > 71 || blockP2U > 71)
                {
                    debugText("red", " 建造失败,选中位置越界");
                    return nullptr;//位置越界
                }
            }
            else if(num == 2 || num == 4 || num == 5 || num == 6)
            {
                if(BlockL < 1 || BlockU < 1 || blockP2L > 71 || blockP2U > 71)
                {
                    debugText("red", " 建造失败,选中位置越界");
                    return nullptr;//位置越界
                }
            }
            for(; buildIt != player[0]->build.end(); buildIt++)
            {
                int buildLU = 0;
                if((*buildIt)->getNum() == BUILDING_HOME || (*buildIt)->getNum() == BUILDING_ARROWTOWER)
                {
                    buildLU = 2;
                }
                else if((*buildIt)->getNum() == BUILDING_GRANARY || (*buildIt)->getNum() == BUILDING_CENTER || (*buildIt)->getNum() == BUILDING_STOCK || (*buildIt)->getNum() == BUILDING_MARKET || (*buildIt)->getNum() == BUILDING_FARM)
                {
                    buildLU = 3;
                }
                int buildP1L = (*buildIt)->getBlockL(), buildP1U = (*buildIt)->getBlockU(), buildP2L = buildP1L + buildLU - 1, buildP2U = buildP1U + buildLU - 1;

                for(int tryL = BlockL; tryL <= blockP2L; tryL++)
                {
                    for(int tryU = BlockU; tryU <= blockP2U; tryU++)
                    {
                        if(tryL >= buildP1L && tryL <= buildP2L && tryU >= buildP1U && tryU <= buildP2U)
                        {
                            isOverlap = true;
                            //                            qDebug() << "buildOverlap BL:" << tryL << " BU:" << tryU;
                            if(showOverlap) debugText("red", " 放置位置与编号为" + QString::number((*buildIt)->getSelectNum()) + "的对象在块坐标(" + QString::number(tryL) + "," + QString::number(tryU) + ")上重叠");
                        }
                    }
                }
            }
            for(; humanIt != player[0]->human.end(); humanIt++)
            {
                int humanL = (*humanIt)->getBlockL(), humanU = (*humanIt)->getBlockU();

                for(int tryL = BlockL; tryL <= blockP2L; tryL++)
                {
                    for(int tryU = BlockU; tryU <= blockP2U; tryU++)
                    {
                        if(tryL == humanL && tryU == humanU)
                        {
                            isOverlap = true;
                            //                            qDebug() << "humanOverlap BL:" << tryL << " BU:" << tryU;
                            if(showOverlap) debugText("red", " 放置位置与编号为" + QString::number((*humanIt)->getSelectNum()) + "的对象在块坐标(" + QString::number(tryL) + "," + QString::number(tryU) + ")上重叠");
                        }
                    }
                }
            }
            for(; cooresIt != map->coores.end(); cooresIt++)
            {
                int cooresLU = 2;
                if((*cooresIt)->isBush()) cooresLU = 1;
                int cooresP1L = (*cooresIt)->getBlockL(), cooresP1U = (*cooresIt)->getBlockU(), cooresP2L = cooresP1L + cooresLU - 1, cooresP2U = cooresP1U + cooresLU - 1;
                for(int tryL = BlockL; tryL <= blockP2L; tryL++)
                {
                    for(int tryU = BlockU; tryU <= blockP2U; tryU++)
                    {
                        if(tryL >= cooresP1L && tryL <= cooresP2L && tryU >= cooresP1U && tryU <= cooresP2U)
                        {
                            isOverlap = true;
                            //                            qDebug() << "cooresOverlap BL:" << tryL << " BU:" << tryU;
                            if(showOverlap) debugText("red", " 放置的位置与编号为" + QString::number((*cooresIt)->getSelectNum()) + "的对象在块坐标(" + QString::number(tryL) + "," + QString::number(tryU) + ")上重叠");
                        }
                    }
                }
            }
            for(; animalIt != map->animal.end(); animalIt++)
            {
                double animalLU = (*animalIt)->getLU();
                int animalBL1 = ((*animalIt)->getL() - animalLU / 2) / BLOCKSIDELENGTH, animalBU1 = ((*animalIt)->getU() - animalLU / 2) / BLOCKSIDELENGTH, animalBL2 = ((*animalIt)->getL() + animalLU / 2) / BLOCKSIDELENGTH, animalBU2 = ((*animalIt)->getU() + animalLU / 2) / BLOCKSIDELENGTH;
                if((*animalIt)->getNum() == ANIMAL_FOREST)
                {
                    animalBL1 = (*animalIt)->getBlockL();
                    animalBU1 = (*animalIt)->getBlockU();
                    animalBL2 = animalBL1 + 1;
                    animalBU2 = animalBU1 + 1;
                }
                else if((*animalIt)->getNum() == ANIMAL_TREE)
                {
                    animalBL1 = (*animalIt)->getBlockL();
                    animalBU1 = (*animalIt)->getBlockU();
                    animalBL2 = animalBL1;
                    animalBU2 = animalBU1;
                }
                for(int tryL = BlockL; tryL <= blockP2L; tryL++)
                {
                    for(int tryU = BlockU; tryU <= blockP2U; tryU++)
                    {
                        if(tryL >= animalBL1 && tryL <= animalBL2 && tryU >= animalBU1 && tryU <= animalBU2)
                        {
                            isOverlap = true;
                            //                            qDebug() << "animalOverlap BL:" << tryL << " BU:" << tryU;
                            if(showOverlap) debugText("red", " 放置位置与编号为" + QString::number((*animalIt)->getSelectNum()) + "的对象在块坐标(" + QString::number(tryL) + "," + QString::number(tryU) + ")上重叠");
                        }
                    }
                }
            }
            if(isOverlap)//如果重叠则不建造
            {
                debugText("red"," 建造失败:放置位置非空地");
                //                        farmer->setisIdle();
                //                farmer->setIsStop();
                //                farmer->setPreStand();
                return nullptr;
            }
            else
            {
                if(num == 1)
                {
                    if(player[0]->getWood() >= 30) player[0]->setWood(player[0]->getWood() - 30);
                    else
                    {
                        //                        farmer->setisIdle();
                        //                        farmer->setIsStop();
                        //                        farmer->setPreStand();
                        return nullptr;//资源不足
                    }
                }
                if(num == 2)
                {
                    if(player[0]->getWood() >= 120) player[0]->setWood(player[0]->getWood() - 120);
                    else
                    {
                        //                        qDebug()<<"3";
                        //                        farmer->setisIdle();
                        //                        farmer->setIsStop();
                        //                        farmer->setPreStand();
                        return nullptr;//资源不足
                    }
                }
                if(num == 4)
                {
                    if(player[0]->getWood() >= 120) player[0]->setWood(player[0]->getWood() - 120);
                    else
                    {
                        //                        qDebug()<<"4";
                        //                        farmer->setisIdle();
                        //                        farmer->setIsStop();
                        //                        farmer->setPreStand();
                        return nullptr;//资源不足
                    }
                }
                if(num == 5)
                {
                    if(player[0]->getWood() >= 75) player[0]->setWood(player[0]->getWood() - 75);
                    else
                    {
                        //                        qDebug()<<"5";
                        //                        farmer->setisIdle();
                        //                        farmer->setIsStop();
                        //                        farmer->setPreStand();
                        return nullptr;//资源不足
                    }
                }
                if(num == 6)
                {
                    if(player[0]->getWood() >= 150) player[0]->setWood(player[0]->getWood() - 150);
                    else
                    {
                        //                        qDebug()<<"6";
                        //                        farmer->setisIdle();
                        //                        farmer->setIsStop();
                        //                        farmer->setPreStand();
                        return nullptr;//资源不足
                    }
                }
                if(num == 7)
                {
                    if(player[0]->getStone() >= 150) player[0]->setStone(player[0]->getStone() - 150);
                    else
                    {
                        //                        qDebug()<<"7";
                        //                        farmer->setisIdle();
                        //                        farmer->setIsStop();
                        //                        farmer->setPreStand();
                        return nullptr;//资源不足
                    }
                }
                Building *temp=player[0]->addbuilding(num - 1, BlockL, BlockU, 0);
                newBuild = temp;
                farmer->setWorker();
                farmer->setPreStand();
                farmer->setNowState(farmer->getPreState());
                farmer->setPreStateIsIdle();
                farmer->setnowobject(temp);
                farmer->setworkobject(farmer->getnowobject());
                movetoGameres(farmer,temp);
                farmer->setIsGoToObject();
                farmer->setNowRes();
            }
        }
    }
    return newBuild;
}

void MainWidget::actionUpdate()
{
    std::list<Building *>::iterator it = player[0]->build.begin();

    for(;it != player[0]->build.end(); it++)
    {
        if((*it)->getActSpeed() > 0)
        {
            (*it)->updatePercent();
            if((*it)->getActPercent() >= 100)
            {
                if((*it)->getActName() == ACT_CREATEFARMER)
                {
                    int farmerSN = 0;
                    //                    int tryL = rand() % (6 * BLOCKSIDELENGTH) + 33.5 * BLOCKSIDELENGTH;
                    //                    int tryU = rand() % (6 * BLOCKSIDELENGTH) + 33.5 * BLOCKSIDELENGTH;
                    int tryCreate[5][5] = {0};
                    std::list<Human *>::iterator humanIt = player[0]->human.begin();
                    std::list<Animal *>::iterator animalIt = map->animal.begin();
                    std::list<Coores *>::iterator cooresIt = map->coores.begin();
                    std::list<Building *>::iterator buildIt = player[0]->build.begin();
                    for(; buildIt != player[0]->build.end(); buildIt++)
                    {
                        int buildLU = 0;
                        if((*buildIt)->getNum() == BUILDING_HOME || (*buildIt)->getNum() == BUILDING_ARROWTOWER)
                        {
                            buildLU = 2;
                        }
                        else if((*buildIt)->getNum() == BUILDING_GRANARY || (*buildIt)->getNum() == BUILDING_CENTER || (*buildIt)->getNum() == BUILDING_STOCK || (*buildIt)->getNum() == BUILDING_MARKET || (*buildIt)->getNum() == BUILDING_FARM)
                        {
                            buildLU = 3;
                        }
                        for(int i = (*buildIt)->getBlockL(); i < (*buildIt)->getBlockL() + buildLU; i++)
                        {
                            for(int j = (*buildIt)->getBlockU(); j < (*buildIt)->getBlockU() + buildLU; j++)
                            {
                                if(i >= 34 && i <= 38 && j >= 34 && j <= 38)
                                {
                                    tryCreate[i-34][j-34] = 1;
                                }
                            }
                        }
                    }
                    for(; humanIt != player[0]->human.end(); humanIt++)
                    {
                        if((*humanIt)->getBlockL() >= 34 && (*humanIt)->getBlockL() <= 38 && (*humanIt)->getBlockU() >= 34 && (*humanIt)->getBlockU() <= 38)
                        {
                            tryCreate[(*humanIt)->getBlockL()-34][(*humanIt)->getBlockU()-34] = 1;
                        }
                    }
                    int availablePos = 0, desPos = -1;
                    for(int i = 0; i < 5; i++)
                    {
                        for(int j = 0; j < 5; j++)
                        {
                            if(tryCreate[i][j] == 0) availablePos++;
                        }
                    }
                    if(availablePos != 0)
                    {
                        desPos = rand() % availablePos + 1;
                    }
                    else
                    {
                        player[0]->setFood(player[0]->getFood() + BUILDING_CENTER_CREATEFARMER_FOOD);
                    }
                    if(desPos > 0)
                    {
                        int count = 0;
                        bool flag = false;
                        while(count < desPos)
                        {
                            for(int i = 0; i < 5; i++)
                            {
                                for(int j = 0; j < 5; j++)
                                {
                                    //                                                                        ui->DebugTextBrowser->insertPlainText(QString::number(tryCreate[i][j]) + " ");
                                    if(tryCreate[i][j] == 0) count++;
                                    if(count == desPos && !flag)
                                    {
                                        farmerSN = player[0]->addfarmer(0, i+34, j+34);
                                        flag = true;
                                    }

                                }
                                //                                                                ui->DebugTextBrowser->insertPlainText("\n");
                            }
                        }
                    }
                    playSound("Villager_Born");
                    if(farmerSN != 0) debugText("blue"," 产生了新的农民(编号:" + QString::number(farmerSN) + ")");
                    else debugText("red", " 市镇中心旁无空间生成村民");
                    //上面是动作完成时操作，下面是重置操作窗口等信息
                    (*it)->setActName(ACT_NULL);
                    (*it)->setActNum(0);
                    (*it)->setActSpeed(0);
                    (*it)->setActPercent(0);
                    if((*it) == ui->selectwidget->getObj())//如果选中对象改变，不改变当前行动框
                    {
                        acts[0]->setActName(ACT_CREATEFARMER);
                        acts[0]->setPix(resMap["Button_Village"].front().scaled(80,80));
                        acts[0]->update();
                        if(player[0]->getCiv() == CIVILIZATION_STONEAGE)
                        {
                            acts[1]->setActName(ACT_UPGRADE_AGE);
                            acts[1]->setPix(resMap["ButtonTech_Center1"].front().scaled(80,80));
                            acts[1]->show();
                            acts[1]->update();
                        }
                    }
                    ui->objText->setText("");
                }
                else if((*it)->getActName() == ACT_UPGRADE_AGE)
                {
                    debugText("blue"," 已完成技术升级:演进至工具时代");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 已完成技术升级:演进至工具时代"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    player[0]->setCiv(CIVILIZATION_TOOLAGE);
                    std::list<Building *>::iterator buildIt = player[0]->build.begin();
                    for(; buildIt != player[0]->build.end(); buildIt++)
                    {
                        (*buildIt)->setCiv(CIVILIZATION_TOOLAGE);
                        if((*buildIt)->isFinish())
                        {
                            (*buildIt)->updateCivilization();
                        }
                    }
                    (*it)->setActName(ACT_NULL);
                    (*it)->setActNum(0);
                    (*it)->setActSpeed(0);
                    (*it)->setActPercent(0);
                    if((*it) == ui->selectwidget->getObj())
                    {
                        acts[0]->setActName(ACT_CREATEFARMER);
                        acts[0]->setPix(resMap["Button_Village"].front().scaled(80,80));
                        acts[0]->update();
                    }
                    ui->objText->setText("");
                }
                else if((*it)->getActName() == ACT_UPGRADE_TOWERBUILD)
                {
                    debugText("blue"," 已完成技术升级:箭塔建造");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 已完成技术升级:箭塔建造"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    player[0]->setArrowTowerUnlocked(true);
                    (*it)->setActName(ACT_NULL);
                    (*it)->setActNum(0);
                    (*it)->setActSpeed(0);
                    (*it)->setActPercent(0);
                    ui->objText->setText("");
                    if((*it) == ui->selectwidget->getObj())
                    {
                        acts[0]->setActName(ACT_NULL);
                        acts[0]->hide();
                        acts[0]->update();
                    }
                }
                else if((*it)->getActName() == ACT_UPGRADE_WOOD)
                {
                    debugText("blue"," 已完成技术升级:猎人攻击距离+50,樵夫木头最大持有量+2");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 已完成技术升级:猎人攻击距离+50,樵夫木头最大持有量+2"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    player[0]->setMarketResearch(0, true);
                    std::list<Human *>::iterator humanIt = player[0]->human.begin();
                    for(; humanIt != player[0]->human.end(); humanIt++)
                    {
                        Farmer *farmer = (Farmer *)(*humanIt);
                        farmer->upgradeCapablity(player[0]->getmarketResearch());
                    }
                    (*it)->setActName(ACT_NULL);
                    (*it)->setActNum(0);
                    (*it)->setActSpeed(0);
                    (*it)->setActPercent(0);
                    ui->objText->setText("");
                    if((*it) == ui->selectwidget->getObj())
                    {
                        acts[0]->setActName(ACT_NULL);
                        acts[0]->hide();
                        acts[0]->update();
                        acts[1]->setActName(ACT_UPGRADE_STONE);
                        acts[2]->setActName(ACT_UPGRADE_FARM);
                        acts[1]->setPix(resMap["ButtonTech_Stone"].front().scaled(80,80));
                        acts[2]->setPix(resMap["ButtonTech_Cow"].front().scaled(80,80));
                        acts[1]->show();
                        acts[2]->show();
                        acts[1]->update();
                        acts[2]->update();
                    }
                }
                else if((*it)->getActName() == ACT_UPGRADE_STONE)
                {
                    debugText("blue"," 已完成技术升级:矿工石头最大持有量+3");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 已完成技术升级:矿工石头最大持有量+3"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    player[0]->setMarketResearch(1, true);
                    std::list<Human *>::iterator humanIt = player[0]->human.begin();
                    for(; humanIt != player[0]->human.end(); humanIt++)
                    {
                        Farmer *farmer = (Farmer *)(*humanIt);
                        farmer->upgradeCapablity(player[0]->getmarketResearch());
                    }
                    (*it)->setActName(ACT_NULL);
                    (*it)->setActNum(0);
                    (*it)->setActSpeed(0);
                    (*it)->setActPercent(0);
                    ui->objText->setText("");
                    if((*it) == ui->selectwidget->getObj())
                    {
                        acts[1]->setActName(ACT_NULL);
                        acts[1]->hide();
                        acts[1]->update();
                        acts[0]->setActName(ACT_UPGRADE_WOOD);
                        acts[2]->setActName(ACT_UPGRADE_FARM);
                        acts[0]->setPix(resMap["ButtonTech_Lumber"].front().scaled(80,80));
                        acts[2]->setPix(resMap["ButtonTech_Cow"].front().scaled(80,80));
                        acts[0]->show();
                        acts[2]->show();
                        acts[0]->update();
                        acts[2]->update();
                    }
                }
                else if((*it)->getActName() == ACT_UPGRADE_FARM)
                {
                    debugText("blue"," 已完成技术升级:农场初始资源持有量+75");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 已完成技术升级:农场初始资源持有量+75"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    player[0]->setMarketResearch(2, true);
                    (*it)->setActName(ACT_NULL);
                    (*it)->setActNum(0);
                    (*it)->setActSpeed(0);
                    (*it)->setActPercent(0);
                    ui->objText->setText("");
                    if((*it) == ui->selectwidget->getObj())
                    {
                        acts[2]->setActName(ACT_NULL);
                        acts[2]->hide();
                        acts[2]->update();
                        acts[1]->setActName(ACT_UPGRADE_STONE);
                        acts[1]->setPix(resMap["ButtonTech_Stone"].front().scaled(80,80));
                        acts[1]->show();
                        acts[1]->update();
                        acts[0]->setActName(ACT_UPGRADE_WOOD);
                        acts[0]->setPix(resMap["ButtonTech_Lumber"].front().scaled(80,80));
                        acts[0]->show();
                        acts[0]->update();
                    }
                }
            }

        }

    }
}

void MainWidget::loadCollisionObject()
{
    clearCollisionMap();
    for(int i=0;i<MAXPLAYER;i++)
    {
        if(!player[i]->build.empty())
        {
            std::list<Building *>::iterator iter=player[i]->build.begin();
            while(iter!=player[i]->build.end())
            {
                if((*iter)->isMiddleFoundation())
                {
                    loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU());
                    loadCollisionMap((*iter)->getBlockL()+1,(*iter)->getBlockU());
                    loadCollisionMap((*iter)->getBlockL()+2,(*iter)->getBlockU());
                    loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU()+1);
                    loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU()+2);
                    loadCollisionMap((*iter)->getBlockL()+1,(*iter)->getBlockU()+1);
                    loadCollisionMap((*iter)->getBlockL()+1,(*iter)->getBlockU()+2);
                    loadCollisionMap((*iter)->getBlockL()+2,(*iter)->getBlockU()+1);
                    loadCollisionMap((*iter)->getBlockL()+2,(*iter)->getBlockU()+2);
                }
                else if((*iter)->isSmallFoundation())
                {
                    loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU());
                    loadCollisionMap((*iter)->getBlockL()+1,(*iter)->getBlockU());
                    loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU()+1);
                    loadCollisionMap((*iter)->getBlockL()+1,(*iter)->getBlockU()+1);
                }
                iter++;
            }
        }
        //        if(!player[i]->human.empty())
        //        {
        //            std::list<Human *>::iterator iter=player[i]->human.begin();
        //            while(iter!=player[i]->human.end())
        //            {
        //                CollisionObject.push_back((*ite));
        //                iter++;
        //            }
        //        }
    }
    if(!map->coores.empty())
    {
        std::list<Coores *>::iterator iter=map->coores.begin();
        while(iter!=map->coores.end())
        {
            if((*iter)->isBlockres())
            {
                loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU());
                loadCollisionMap((*iter)->getBlockL()+1,(*iter)->getBlockU());
                loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU()+1);
                loadCollisionMap((*iter)->getBlockL()+1,(*iter)->getBlockU()+1);
            }
            else
            {
                CollisionObject.push_back((*iter));
            }
            iter++;
        }
    }
    if(!map->animal.empty())
    {
        std::list<Animal *>::iterator iter=map->animal.begin();
        while(iter!=map->animal.end())
        {
            Animal *animal=(Animal *)(*iter);
            if(animal->isTree())
            {
                loadCollisionMap((*iter)->getBlockL(),(*iter)->getBlockU());
            }
            else
            {
                CollisionObject.push_back((*iter));
            }
            iter++;
        }
    }
}

void MainWidget::loadCollisionMap(int BlockL,int BlockU)
{
    if (BlockL >= 0 && BlockL < 72 && BlockU >= 0 && BlockU < 72)
    {
        CollisionMap[BlockL][BlockU] = 1;
    }
}

void MainWidget::loadfindPathMap()
{
    clearfindPathMap();
    for (int i = 0; i < MAP_L; ++i)
    {
        for (int j = 0; j < MAP_U; ++j)
        {
            if(map->cell[i][j].Explored==false)
            {
                findPathMap[i][j]=1;
            }
        }
    }
    for(int i=0;i<MAXPLAYER;i++)
    {
        if(!player[i]->build.empty())
        {
            std::list<Building *>::iterator iter=player[i]->build.begin();
            while(iter!=player[i]->build.end())
            {
                if((*iter)->isMiddleFoundation())
                {
                    //                    qDebug()<<"loadfindpathmapL:"<<(*iter)->getBlockL();
                    //                    qDebug()<<"loadfindpathmapU:"<<(*iter)->getBlockU();
                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+1]=1;
                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+2]=1;
                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()]=1;
                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+1]=1;
                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+2]=1;
                    findPathMap[(*iter)->getBlockL()+2][(*iter)->getBlockU()]=1;
                    findPathMap[(*iter)->getBlockL()+2][(*iter)->getBlockU()+1]=1;
                    findPathMap[(*iter)->getBlockL()+2][(*iter)->getBlockU()+2]=1;
                }
                else if((*iter)->isSmallFoundation())
                {
                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
                    findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+1]=1;
                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()]=1;
                    findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+1]=1;
                }
                iter++;
            }
        }
        //        if(!player[i]->human.empty())
        //        {
        //            std::list<Human *>::iterator iter=player[i]->human.begin();
        //            while(iter!=player[i]->human.end())
        //            {
        //                CollisionObject.push_back((*ite));
        //                iter++;
        //            }
        //        }
    }
    if(!map->coores.empty())
    {
        std::list<Coores *>::iterator iter=map->coores.begin();
        while(iter!=map->coores.end())
        {
            if((*iter)->isBlockres())
            {
                findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
                findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()+1]=1;
                findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()]=1;
                findPathMap[(*iter)->getBlockL()+1][(*iter)->getBlockU()+1]=1;
            }
            iter++;
        }
    }
    if(!map->animal.empty())
    {
        std::list<Animal *>::iterator iter=map->animal.begin();
        while(iter!=map->animal.end())
        {
            Animal *animal=(Animal *)(*iter);
            if(animal->isTree())
            {
                findPathMap[(*iter)->getBlockL()][(*iter)->getBlockU()]=1;
            }
            iter++;
        }
    }
}

int MainWidget::usefindPathMap(int BlockL, int BlockU)
{
    if(BlockL>=0&&BlockU>=0&&BlockL<72&&BlockU<72)
    {
        return findPathMap[BlockL][BlockU];
    }
    return 1;
}

void MainWidget::clearCollisionMap()
{
    for (int i = 0; i < MAP_L; ++i)
    {
        for (int j = 0; j < MAP_U; ++j)
        {
            CollisionMap[i][j] = 0;
        }
    }
}

void MainWidget::clearfindPathMap()
{
    for (int i = 0; i < MAP_L; ++i)
    {
        for (int j = 0; j < MAP_U; ++j)
        {
            findPathMap[i][j] = 0;
        }
    }
}

void MainWidget::clearCollisionObject()
{
    CollisionObject.clear();
}

void MainWidget::judgeStop()
{
    loadCollisionObject();
    for(int i=0;i<MAXPLAYER;i++)
    {
        if(!player[i]->human.empty())
        {
            std::list<Human *>::iterator iter=player[i]->human.begin();
            while(iter!=player[i]->human.end())
            {
                Farmer *farmer=(Farmer *)(*iter);
                if(farmer->isWalking())
                {
                    if(judgeCollision(farmer->getSelectNum(),farmer->getPredictedL(),farmer->getPredictedU()))
                    {
                        debugText("red"," FARMER(编号:" + QString::number(farmer->getSelectNum()) + ")碰撞停止");
                        farmer->GoBackLU();
                        farmer->setIsStop();
                        farmer->setPreStand();
                    }
                }
                iter++;
            }
        }
    }
    if(!map->animal.empty())
    {
        std::list<Animal *>::iterator iter=map->animal.begin();
        while(iter!=map->animal.end())
        {
            Animal *animal=(Animal *)(*iter);
            if(animal->isWalking()||animal->isRunning())
            {
                if(judgeCollision(animal->getSelectNum(),animal->getPredictedL(),animal->getPredictedU()))
                {
//                                        qDebug()<<"1";
                    animal->GoBackLU();
                    animal->setIdle();
                    animal->setPreStand();
                }
            }
            iter++;
        }
    }
    clearCollisionObject();
}

bool MainWidget::judgeCollision(int SN, double L, double U)
{
    int BlockL,BlockU;
    BlockL=L/BLOCKSIDELENGTH;
    BlockU=U/BLOCKSIDELENGTH;
    if(CollisionMap[BlockL][BlockU]==1)
    {
        return 1;
    }

    //    //    for(int i=0;i<MAXPLAYER;i++)
    //    //    {
    //    //        if(!player[i]->build.empty())
    //    //        {
    //    //            std::list<Building *>::iterator iter=player[i]->build.begin();
    //    //            while(iter!=player[i]->build.end())
    //    //            {
    //    //                if((*iter)->isMiddleFoundation())
    //    //                {
    //    //                    if(L>tranL((*iter)->getBlockL())&&L<tranL((*iter)->getBlockL()+3)&&U>tranU((*iter)->getBlockU())&&U<tranU((*iter)->getBlockU()+3))
    //    //                    {
    //    //                        //                        qDebug()<<(*iter)->getSelectNum();
    //    //                        return 1;
    //    //                    }
    //    //                }
    //    //                else if((*iter)->isSmallFoundation())
    //    //                {
    //    //                    if(L>tranL((*iter)->getBlockL())&&L<tranL((*iter)->getBlockL()+2)&&U>tranU((*iter)->getBlockU())&&U<tranU((*iter)->getBlockU()+2))
    //    //                    {
    //    //                        //                        qDebug()<<(*iter)->getSelectNum();
    //    //                        return 1;
    //    //                    }
    //    //                }
    //    //                iter++;
    //    //            }
    //    //        }
    //    //    qDebug()<<CollisionObject.size();


    //    if(!CollisionObject.empty())
    //    {
    //        std::list<Coordinate *>::iterator iter=CollisionObject.begin();
    //        Coordinate *ob=g_Object[SN];
    //        while(iter!=CollisionObject.end())
    //        {
    //            if((*iter)->isFarmer())
    //            {
    //                Farmer *farmer=(Farmer *)(*iter);
    //                if(L>(farmer->getPredictedL()-(farmer->getLU()+ob->getLU())/2.0)&&L<(farmer->getPredictedL()+(farmer->getLU()+ob->getLU())/2.0)&&U>(farmer->getPredictedU()-(farmer->getLU()+ob->getLU())/2.0)&&U<(farmer->getPredictedU()+(farmer->getLU()+ob->getLU())/2.0))
    //                {
    //                    if(SN!=(*iter)->getSelectNum())
    //                    {
    //                        return 1;
    //                    }
    //                }
    //            }
    //            else if((*iter)->isCoores())
    //            {
    //                if(L>((*iter)->getL()-((*iter)->getLU()+ob->getLU())/2.0+1)&&L<((*iter)->getL()+((*iter)->getLU()+ob->getLU())/2.0-1)&&U>((*iter)->getU()-((*iter)->getLU()+ob->getLU())/2.0+1)&&U<((*iter)->getU()+((*iter)->getLU()+ob->getLU())/2.0-1))
    //                {
    //                    return 1;
    //                }
    //            }
    //            else if((*iter)->isAnimal()||(*iter)->isTreeForest())
    //            {
    //                Animal *animal=(Animal *)(*iter);
    //                if(L>(animal->getPredictedL()-(animal->getLU()+ob->getLU())/2.0+1)&&L<(animal->getPredictedL()+(animal->getLU()+ob->getLU())/2.0-1)&&U>(animal->getPredictedU()-(animal->getLU()+ob->getLU())/2.0+1)&&U<(animal->getPredictedU()+(animal->getLU()+ob->getLU())/2.0-1))
    //                {
    //                    if(SN!=(*iter)->getSelectNum())
    //                    {
    //                        return 1;
    //                    }

    //                }
    //            }
    //            iter++;
    //        }
    //    }


    //    //        if(!player[i]->human.empty())
    //    //        {
    //    //            std::list<Human *>::iterator iter=player[i]->human.begin();
    //    //            while(iter!=player[i]->human.end())
    //    //            {
    //    //                Coordinate *ob=g_Object[SN];
    //    //                if(L>((*iter)->getPredictedL()-((*iter)->getLU()+ob->getLU())/2.0)&&L<((*iter)->getPredictedL()+((*iter)->getLU()+ob->getLU())/2.0)&&U>((*iter)->getPredictedU()-((*iter)->getLU()+ob->getLU())/2.0)&&U<((*iter)->getPredictedU()+((*iter)->getLU()+ob->getLU())/2.0))
    //    //                {
    //    //                    if(SN!=(*iter)->getSelectNum())
    //    //                    {
    //    //                        //                        qDebug()<<(*iter)->getSelectNum();
    //    //                        return 1;
    //    //                    }
    //    //                }
    //    //                iter++;
    //    //            }
    //    //        }
    //    //    }
    //    //    if(!map->coores.empty())
    //    //    {
    //    //        std::list<Coores *>::iterator iter=map->coores.begin();
    //    //        while(iter!=map->coores.end())
    //    //        {
    //    //            Coordinate *ob=g_Object[SN];
    //    //            if(L>((*iter)->getL()-((*iter)->getLU()+ob->getLU())/2.0+1)&&L<((*iter)->getL()+((*iter)->getLU()+ob->getLU())/2.0-1)&&U>((*iter)->getU()-((*iter)->getLU()+ob->getLU())/2.0+1)&&U<((*iter)->getU()+((*iter)->getLU()+ob->getLU())/2.0-1))
    //    //            {
    //    //                return 1;
    //    //            }
    //    //            iter++;
    //    //        }
    //    //    }
    //    //    if(!map->animal.empty())
    //    //    {
    //    //        std::list<Animal *>::iterator iter=map->animal.begin();
    //    //        while(iter!=map->animal.end())
    //    //        {
    //    //            Coordinate *ob=g_Object[SN];
    //    //            if(L>((*iter)->getPredictedL()-((*iter)->getLU()+ob->getLU())/2.0+1)&&L<((*iter)->getPredictedL()+((*iter)->getLU()+ob->getLU())/2.0-1)&&U>((*iter)->getPredictedU()-((*iter)->getLU()+ob->getLU())/2.0+1)&&U<((*iter)->getPredictedU()+((*iter)->getLU()+ob->getLU())/2.0-1))
    //    //            {
    //    ////                qDebug()<<"L>"<<((*iter)->getPredictedL()-((*iter)->getLU()+ob->getLU())/2.0);
    //    ////                qDebug()<<"L<"<<((*iter)->getPredictedL()+((*iter)->getLU()+ob->getLU())/2.0);
    //    ////                qDebug()<<"U>"<<((*iter)->getPredictedU()-((*iter)->getLU()+ob->getLU())/2.0);
    //    ////                qDebug()<<"U<"<<((*iter)->getPredictedU()+((*iter)->getLU()+ob->getLU())/2.0);
    //    ////                qDebug()<<"L:"<<L;
    //    ////                qDebug()<<"U:"<<U;
    //    //                if(SN!=(*iter)->getSelectNum())
    //    //                {
    //    //                    return 1;
    //    //                }

    //    //            }
    //    //            iter++;
    //    //        }
    //    //    }

    return 0;
}

void MainWidget::inittagGame(tagGame *game)
{
    game->building=new tagBuilding[50];
    game->human=new tagHuman[200];
    game->resource=new tagResource[2000];
}

void MainWidget::set_Game()
{
    Game->building_n=0;
    Game->human_n=0;
    Game->resource_n=0;
    for(int i=0;i<MAXPLAYER;i++)
    {
        if(!player[i]->human.empty())
        {
            std::list<Human *>::iterator iter=player[i]->human.begin();
            while(iter!=player[i]->human.end())
            {
                Farmer *farmer=(Farmer *)(*iter);
                Game->human[Game->human_n].L=farmer->getL();
                Game->human[Game->human_n].U=farmer->getU();
                Game->human[Game->human_n].BlockL=farmer->getBlockL();
                Game->human[Game->human_n].BlockU=farmer->getBlockU();
                Game->human[Game->human_n].L0=farmer->getL0();
                Game->human[Game->human_n].U0=farmer->getU0();
                Game->human[Game->human_n].SN=farmer->getSelectNum();
                Game->human[Game->human_n].NowState=farmer->getworkstate();
                Game->human[Game->human_n].Blood=farmer->getBlood();
                if(farmer->getworkobject()==NULL)
                {
                    Game->human[Game->human_n].WorkObjectSN=0;
                }
                else
                {
                    Game->human[Game->human_n].WorkObjectSN=farmer->getworkobject()->getSelectNum();
                }

                Game->human[Game->human_n].Resource=farmer->getresource();
                Game->human[Game->human_n].ResourceSort=farmer->getresourceSort();
                Game->Human_MaxNum=player[i]->getMaxHumanNum();
                Game->human_n++;
                iter++;
            }
        }
        if(!player[i]->build.empty())
        {
            std::list<Building *>::iterator iter=player[i]->build.begin();
            while(iter!=player[i]->build.end())
            {
                Building *building=(Building *)(*iter);
                Game->building[Game->building_n].BlockL=building->getBlockL();
                Game->building[Game->building_n].BlockU=building->getBlockU();
                Game->building[Game->building_n].Blood=building->getBlood();
                Game->building[Game->building_n].MaxBlood=building->getMaxBlood();
                Game->building[Game->building_n].Type=building->getNum();
                Game->building[Game->building_n].Percent=building->getPercent();
                Game->building[Game->building_n].Project=building->getActNum();
                Game->building[Game->building_n].ProjectPercent=building->getActPercent();
                Game->building[Game->building_n].SN=building->getSelectNum();
                Game->building[Game->building_n].Foundation=building->getType();
                Game->building[Game->building_n].Cnt=building->getCnt();
                Game->building_n++;
                iter++;
            }
        }
    }
    if(!map->coores.empty())
    {
        std::list<Coores *>::iterator iter=map->coores.begin();
        while(iter!=map->coores.end())
        {
            if((*iter)->isExplored())
            {
                Coores *coores=(Coores *)(*iter);
                Game->resource[Game->resource_n].L=coores->getL();
                Game->resource[Game->resource_n].U=coores->getU();
                Game->resource[Game->resource_n].BlockL=coores->getBlockL();
                Game->resource[Game->resource_n].BlockU=coores->getBlockU();
                Game->resource[Game->resource_n].Blood=coores->getBlood();
                Game->resource[Game->resource_n].Cnt=coores->getCnt();
                Game->resource[Game->resource_n].Type=coores->getSort()*10+coores->getNum();
                Game->resource[Game->resource_n].ProductSort=coores->getProductSort();
                Game->resource[Game->resource_n].SN=coores->getSelectNum();
                Game->resource_n++;
            }
            iter++;
        }
    }
    if(!map->animal.empty())
    {
        std::list<Animal *>::iterator iter=map->animal.begin();
        while(iter!=map->animal.end())
        {
            if(map->cell[(*iter)->getBlockL()][(*iter)->getBlockU()].Explored==true)
            {
                Animal *animal=(Animal *)(*iter);
                Game->resource[Game->resource_n].L=animal->getL();
                Game->resource[Game->resource_n].U=animal->getU();
                Game->resource[Game->resource_n].BlockL=animal->getBlockL();
                Game->resource[Game->resource_n].BlockU=animal->getBlockU();
                Game->resource[Game->resource_n].Blood=animal->getBlood();
                Game->resource[Game->resource_n].Cnt=animal->getCnt();
                Game->resource[Game->resource_n].Type=animal->getSort()*10+animal->getNum();
                Game->resource[Game->resource_n].ProductSort=animal->getProductSort();
                Game->resource[Game->resource_n].SN=animal->getSelectNum();
                Game->resource_n++;
            }
            iter++;
        }
    }
    if(!map->ruin.empty())
    {
        std::list<Ruin *>::iterator iter=map->ruin.begin();
        while(iter!=map->ruin.end())
        {
            if((*iter)->isExplored())//后续改动
            {
                Game->building[Game->building_n].BlockL=(*iter)->getBlockL();
                Game->building[Game->building_n].BlockU=(*iter)->getBlockU();
                Game->building[Game->building_n].Type=-1;
                Game->building[Game->building_n].Blood=-1;
                Game->building[Game->building_n].Cnt=-1;
                Game->building[Game->building_n].Foundation=-1;
                Game->building[Game->building_n].MaxBlood=-1;
                Game->building[Game->building_n].Percent=-1;
                Game->building[Game->building_n].Project=-1;
                Game->building[Game->building_n].ProjectPercent=-1;
                Game->building[Game->building_n].SN=-1;
                Game->building_n++;
            }
            iter++;
        }
    }
    Game->GameFrame = gameframe;
    Game->civilizationStage = player[0]->getCiv();
    Game->Meat = player[0]->getFood();
    Game->Wood = player[0]->getWood();
    Game->Stone = player[0]->getStone();
    Game->Gold = player[0]->getGold();
}

void MainWidget::judgeVictory()
{
    if(!isLose())
    {
        if(elapsedSec > GAME_LOSE_SEC)
        {
            //停止当前动作
            timer->stop();
            showTimer->stop();
            playSound("Lose");
            debugText("blue"," 游戏失败，未达成目标。最终得分为:" + QString::number(player[0]->getScore()));
            //            ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 游戏失败，未达成目标。最终得分为:" + QString::number(player[0]->getScore())));
            //            ui->DebugTextBrowser->insertPlainText("\n");
            //弹出胜利提示
            if(QMessageBox::information(this, QStringLiteral("失败"), "很遗憾你没能振兴部落。", QMessageBox::Ok))
            {
                setLose();
            }
        }
    }
    else return;

    if(!isWinning())
    {
        if(player[0]->getCiv()==2)
        {
            if(ArrowTowerBuilt[0]==1&&ArrowTowerBuilt[1]==1&&ArrowTowerBuilt[2]==1)
            {
                if(player[0]->getWood()>=1000&&player[0]->getStone()>=1000&&player[0]->getFood()>=2000)
                {
                    //停止当前动作
                    timer->stop();

                    playSound("Win");
                    debugText("blue"," 游戏胜利");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + " 游戏胜利"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    //弹出胜利提示
                    if(QMessageBox::information(this, QStringLiteral("胜利"), "恭喜你取得了游戏的胜利，成功振兴了部落！", QMessageBox::Ok))
                    {
                        setWinning();
                    }
                }
            }
        }
    }
    else return;
}

void MainWidget::debugText(const QString& color, const QString& content)
{
    if (color == "blue")
    {
        ui->DebugTextBrowser->insertHtml(COLOR_BLUE(getShowTime() + content));
    }
    else if (color == "red")
    {
        ui->DebugTextBrowser->insertHtml(COLOR_RED(getShowTime() + content));
    }
    else if (color == "green")
    {
        ui->DebugTextBrowser->insertHtml(COLOR_GREEN(getShowTime() + content));
    }
    ui->DebugTextBrowser->insertPlainText("\n");
    QScrollBar *bar = DebugText->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWidget::judgeArrowTowerPosition(int BlockL, int BlockU)
{
    for(int i=0;i<3;i++)
        if(BlockL>=ArrowTowerBlockL[i]-1&&BlockL<=ArrowTowerBlockL[i]+1&&BlockU>=ArrowTowerBlockU[i]-1&&BlockU<=ArrowTowerBlockU[i]+1)
        {
            //            qDebug()<<i;
            ArrowTowerBuilt[i]=1;
        }
}

void MainWidget::initFarmer()
{
    //加载素材
    //"Villager","Lumber","Gatherer","Miner","Hunter","Farmer","Worker"

    for(int statei=0;statei<7;statei++)
    {
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateWalk(statei,i);
            Farmer::allocateStand(statei,i);
            Farmer::allocateDie(statei,i);
            loadResource(Farmer::getFarmerName(statei)+"_Stand_"+direction[i],Farmer::getStand(statei,i));
            loadResource(Farmer::getFarmerName(statei)+"_Walk_"+direction[i],Farmer::getWalk(statei,i));
            loadResource(Farmer::getFarmerName(statei)+"_Die_"+direction[i],Farmer::getDie(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateWalk(statei,i);
            Farmer::allocateStand(statei,i);
            Farmer::allocateDie(statei,i);
            flipResource(Farmer::getWalk(statei,8-i),Farmer::getWalk(statei,i));
            flipResource(Farmer::getStand(statei,8-i),Farmer::getStand(statei,i));
            flipResource(Farmer::getDie(statei,8-i),Farmer::getDie(statei,i));
        }
    }
    //Work
    for(int statei=0;statei<7;statei++)
    {
        if(statei==0)
        {
            continue;
        }
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateWork(statei,i);
            loadResource(Farmer::getFarmerName(statei)+"_Work_"+direction[i],Farmer::getWork(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateWork(statei,i);
            flipResource(Farmer::getWork(statei,8-i),Farmer::getWork(statei,i));
        }
    }
    //Attack
    for(int statei=0;statei<7;statei++)
    {
        if(statei==2||statei==3||statei==5||statei==6)
        {
            continue;
        }
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateAttack(statei,i);
            loadResource(Farmer::getFarmerName(statei)+"_Attack_"+direction[i],Farmer::getAttack(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateAttack(statei,i);
            flipResource(Farmer::getAttack(statei,8-i),Farmer::getAttack(statei,i));
        }
    }
    //Carry 考虑如何对接
    for(int statei=0;statei<=4;statei++)
    {
        if(statei==0)
        {
            continue;
        }
        for(int i=0;i<=4;i++)
        {
            Farmer::allocateCarry(statei,i);
            loadResource(Farmer::getFarmerCarry(statei)+"_"+direction[i],Farmer::getCarry(statei,i));
        }
        for(int i=5;i<8;i++)
        {
            Farmer::allocateCarry(statei,i);
            flipResource(Farmer::getCarry(statei,8-i),Farmer::getCarry(statei,i));
        }
    }

}

void MainWidget::initAnimal()
{
    for(int num=0;num<5;num++)
    {
        if(num==ANIMAL_TREE)
        {
            Animal::allocateStand(num,0);
            Animal::allocateDie(num,0);
            loadResource(Animal::getAnimalName(num),Animal::getStand(num,0));
            loadResource(Animal::getAnimalcarcassname(num),Animal::getDie(num,0));
            continue;
        }
        if(num==ANIMAL_GAZELLE||num==ANIMAL_LION)
        {
            for(int i=0;i<=4;i++)
            {
                Animal::allocateRun(num,i);
                loadResource(Animal::getAnimalName(num)+"_Run_"+direction[i],Animal::getRun(num,i));
            }
            for(int i=5;i<8;i++)
            {
                Animal::allocateRun(num,i);
                flipResource(Animal::getRun(num,8-i),Animal::getRun(num,i));
            }
        }
        for(int i=0;i<=4;i++)
        {
            Animal::allocateAttack(num,i);
            Animal::allocateWalk(num,i);
            Animal::allocateStand(num,i);
            Animal::allocateDie(num,i);
            loadResource(Animal::getAnimalName(num)+"_Stand_"+direction[i],Animal::getStand(num,i));
            loadResource(Animal::getAnimalName(num)+"_Walk_"+direction[i],Animal::getWalk(num,i));
            loadResource(Animal::getAnimalName(num)+"_Attack_"+direction[i],Animal::getAttack(num,i));
            loadResource(Animal::getAnimalName(num)+"_Die_"+direction[i],Animal::getDie(num,i));
        }
        for(int i=5;i<8;i++)
        {
            Animal::allocateAttack(num,i);
            Animal::allocateWalk(num,i);
            Animal::allocateStand(num,i);
            Animal::allocateDie(num,i);
            flipResource(Animal::getWalk(num,8-i),Animal::getWalk(num,i));
            flipResource(Animal::getStand(num,8-i),Animal::getStand(num,i));
            flipResource(Animal::getAttack(num,8-i),Animal::getAttack(num,i));
            flipResource(Animal::getDie(num,8-i),Animal::getDie(num,i));
        }
    }
}

void MainWidget::initBuilding()
{
    for (int i = 0; i < 4; i++)
    {
        Building::allocatebuild(i);
        loadResource(Building::getBuildingname(i),Building::getBuild(i));
    }
    for (int i = 1; i < 3; i++)
    {
        for(int j=0;j<7;j++)
        {
            Building::allocatebuilt(i,j);
            loadResource(Building::getBuiltname(i,j),Building::getBuilt(i,j));
        }
    }
}

void MainWidget::initCoores()
{
    for(int num=0;num<3;num++)
    {
        Coores::allocatecoores(num);
        loadResource(Coores::getCooresName(num),Coores::getcoores(num));
        Coores::allocategraycoores(num);
        loadGrayRes(Coores::getcoores(num), Coores::getgraycoores(num));
    }
}

void MainWidget::initBlock()
{
    for(int num=0;num<17;num++)
    {
        Block::allocateblock(num);
        loadResource(Block::getBlockname(num),Block::getblock(num));
        Block::allocategrayblock(num);
        loadGrayRes(Block::getblock(num), Block::getgrayblock(num));
        Block::allocateblackblock(num);
        loadBlackRes(Block::getblock(num),Block::getblackblock(num));
    }
}

void MainWidget::initMissile()
{
    for(int num=0;num<1;num++)
    {
        Missile::allocatemissile(num);
        loadResource(Missile::getMissilename(num),Missile::getmissile(num));
    }
}

void MainWidget::initRuin()
{
    for(int num=0;num<3;num++)
    {
        Ruin::allocateruin(num);
        loadResource(Ruin::getRuinname(num),Ruin::getruin(num));
    }
}

void MainWidget::deleteFarmer()
{
    // 清理素材资源
    for(int statei = 0; statei < 7; statei++)
    {
        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateWalk(statei, i);
            Farmer::deallocateStand(statei, i);
            Farmer::deallocateDie(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateWalk(statei, i);
            Farmer::deallocateStand(statei, i);
            Farmer::deallocateDie(statei, i);
        }
    }

    // 清理 Work 资源
    for(int statei = 0; statei < 7; statei++)
    {
        if(statei == 0)
        {
            continue;
        }

        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateWork(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateWork(statei, i);
        }
    }

    // 清理 Attack 资源
    for(int statei = 0; statei < 7; statei++)
    {
        if(statei == 2 || statei == 3 || statei == 5 || statei == 6)
        {
            continue;
        }

        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateAttack(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateAttack(statei, i);
        }
    }

    // 清理 Carry 资源
    for(int statei = 0; statei <= 4; statei++)
    {
        if(statei == 0)
        {
            continue;
        }

        for(int i = 0; i <= 4; i++)
        {
            Farmer::deallocateCarry(statei, i);
        }
        for(int i = 5; i < 8; i++)
        {
            Farmer::deallocateCarry(statei, i);
        }
    }

}

void MainWidget::deleteAnimal()
{
    for (int num = 0; num < 3; num++)
    {
        if (num == 0)
        {
            Animal::deallocateStand(num, 0);
            Animal::deallocateDie(num, 0);
            continue;
        }
        if (num == 1|| num == 3)
        {
            for (int i = 0; i <= 4; i++)
            {
                Animal::deallocateRun(num, i);
            }
            for (int i = 5; i < 8; i++)
            {
                Animal::deallocateRun(num, i);
            }
        }
        for (int i = 0; i <= 4; i++)
        {
            Animal::deallocateAttack(num, i);
            Animal::deallocateWalk(num, i);
            Animal::deallocateStand(num, i);
            Animal::deallocateDie(num, i);
        }
        for (int i = 5; i < 8; i++)
        {
            Animal::deallocateAttack(num, i);
            Animal::deallocateWalk(num, i);
            Animal::deallocateStand(num, i);
            Animal::deallocateDie(num, i);
        }
    }
}

void MainWidget::deleteBuilding()
{
    for (int i = 0; i < 4; i++)
    {
        Building::deallocatebuild(i);
    }
    for (int i = 1; i < 3; i++)
    {
        for(int j=0;j<7;j++)
        {
            Building::deallocatebuilt(i,j);
        }
    }
}

void MainWidget::deleteCoores()
{
    for(int i=0;i<3;i++)
    {
        Coores::deallocatecoores(i);
        Coores::deallocategraycoores(i);
    }
}

void MainWidget::deleteBlock()
{
    for(int i=0;i<1;i++)
    {
        Block::deallocateblock(i);
        Block::deallocateblackblock(i);
        Block::deallocategrayblock(i);
    }
}

void MainWidget::deleteMissile()
{
    for(int i=0;i<1;i++)
    {
        Missile::deallocatemissile(i);
    }
}

void MainWidget::deleteRuin()
{
    for(int i=0;i<3;i++)
    {
        Ruin::deallocateruin(i);
    }
}

void MainWidget::playSound(const string &key)
{
    if(!sound) return;
    auto soundIter = SoundMap.find(key);
    if (soundIter != SoundMap.end())
    {
        QSound* sound = soundIter->second;
        if (sound != nullptr)
        {
            sound->play();
        }
    }
}

void MainWidget::cheatResource()
{
    player[0]->setFood(player[0]->getFood() + 500);
    player[0]->setWood(player[0]->getWood() + 500);
    player[0]->setStone(player[0]->getStone() + 500);
    return ;
}

void MainWidget::cheatFarmerCountLimit()
{
    player[0]->setCheatMaxHumanNum(true);
}

void MainWidget::cheatEmpiresAge()
{
    player[0]->setCiv(2);
    return ;
}

void MainWidget::receiveAIDebug(QString String)
{
    DebugText->insertPlainText(String);
    DebugText->insertPlainText("\n");
}

void MainWidget::selectFarmerSlots(std::list<Farmer *> *nowselectList)
{
    this->nowselectList=nowselectList;
    //    qDebug()<<this->nowselectList->size();
    if(!this->nowselectList->empty())
    {
        nowobject=nowselectList->front();
    }
}

void MainWidget::updatePosition()
{
    if(!drawlist.empty())
    {
        //        qDebug()<<"1";
        std::list<Coordinate *>::iterator iter=drawlist.begin();
        while(iter!=drawlist.end())
        {
            if((*iter)->isMoveObject())
            {
                MoveObject *mo=(MoveObject *)(*iter);
                mo->updateLU();
            }
            iter++;
        }
    }
}

void MainWidget::onRadioClickSlot()
{

    switch(pbuttonGroup->checkedId())
    {
    case 0:
        timer->setInterval(40);
        showTimer->setInterval(1000);
        mapmoveFrequency=1;
        break;
    case 1:
        timer->setInterval(20);
        showTimer->setInterval(500);
        mapmoveFrequency=2;

        break;
    case 2:
        timer->setInterval(10);
        showTimer->setInterval(250);
        mapmoveFrequency=4;
        break;
    case 3:
        timer->setInterval(5);
        showTimer->setInterval(125);
        mapmoveFrequency=8;
        nowobject=NULL;
        lastnowobject=NULL;
        break;
    }
}

void MainWidget::handleAction()
{
    //        qDebug() << "handleAction：";
    if(!g_AiAction.empty())
    {
        //                qDebug()<<"AiAction非空";
        std::map<int, tagAction>::iterator iter=g_AiAction.begin();
        while(iter!=g_AiAction.end())
        {
            if((iter->second).A==0)
            {
                bool canAct = true;
                int i = 0;
                for(; i < ACT_WINDOW_NUM_FREE; i++)
                {
                    if((iter->second).Action == ((Building *)(g_Object[(iter->second).SN]))->getActNames(i))
                    {
                        if(((Building *)(g_Object[(iter->second).SN]))->getActStatus(i) == ACT_STATUS_DISABLED)
                        {
                            canAct = false;
                            break;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                if(i == ACT_WINDOW_NUM_FREE)
                {
                    canAct = false;
                }
                if(canAct)
                {
                    Coordinate *lastBuild = ui->selectwidget->getObj();
                    ui->selectwidget->setObj((Building *)g_Object[(iter->second).SN]);
                    int actName = (iter->second).Action;
                    doAct(actName);
                    debugText("green"," <下达指令>" + QString::fromStdString(((Building *)g_Object[(iter->second).SN])->getDisplayName(((Building *)g_Object[(iter->second).SN])->getNum())) + "(编号:" + QString::number(((Building *)g_Object[(iter->second).SN])->getSelectNum()) + ")执行了行动:" + QString::fromStdString(actNames[(iter->second).Action]));
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_GREEN(getShowTime() + " <下达指令>" + QString::fromStdString(((Building *)g_Object[(iter->second).SN])->getDisplayName(((Building *)g_Object[(iter->second).SN])->getNum())) + "(编号:" + QString::number(((Building *)g_Object[(iter->second).SN])->getSelectNum()) + ")执行了行动:" + QString::fromStdString((iter->second).Action)));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    if(lastBuild != NULL)
                    {
                        ui->selectwidget->setObj(lastBuild);
                    }
                }
                else
                {
                    iter++;
                    continue;
                }
            }
            else if((iter->second).A==1)
            {
                //                                qDebug() << "移动";
                Farmer *farmer=(Farmer *)g_Object[(iter->second).SN];
                farmer->setnowobject(NULL);
                farmer->setworkobject(NULL);
                farmer->setdestination((iter->second).L0,(iter->second).U0);
                Point start, destination;
                start.x=tranBlockL(farmer->getL());
                start.y=tranBlockU(farmer->getU());
                destination.x=tranBlockL(farmer->getL0());
                destination.y=tranBlockU(farmer->getU0());
                loadfindPathMap();
                farmer->setPath(findPath(findPathMap,start,destination));
                farmer->setisIdle();
                if(!farmer->isWalking())
                {
                    farmer->setPreWalk();
                }
                debugText("green"," <下达指令>FARMER(编号:" + QString::number(farmer->getSelectNum()) + ")移动至:(" + QString::number((iter->second).L0) + "," + QString::number((iter->second).U0) + ")");
                //                ui->DebugTextBrowser->insertHtml(COLOR_GREEN(getShowTime() + " <下达指令>农民(编号:" + QString::number(farmer->getSelectNum()) + ")移动至:(" + QString::number((iter->second).L0) + "," + QString::number((iter->second).U0) + ")"));
                //                ui->DebugTextBrowser->insertPlainText("\n");
            }
            else if((iter->second).A==2)
            {
                //                                qDebug() << "互动";
                //                qDebug() << "A = " << (iter->second).A;
                //                qDebug() << "Action = " << (iter->second).Action;
                //                qDebug() << "BlockL = " << (iter->second).BlockL;
                //                qDebug() << "BlockU = " << (iter->second).BlockU;
                //                qDebug() << "BuildingNum = " << (iter->second).BuildingNum;
                //                qDebug() << "L0 = " << (iter->second).L0;
                //                qDebug() << "U0 = " << (iter->second).U0;
                //                qDebug() << "obSN = " << (iter->second).obSN;
                //                qDebug() << "SN = " << (iter->second).SN;
                Farmer *farmer=(Farmer *)g_Object[(iter->second).SN];
                farmer->setisIdle();
                farmer->setnowobject(g_Object[(iter->second).obSN]);
                farmer->setworkobject(farmer->getnowobject());
                int type = (iter->second).obSN / 10000 % 10;
                QString obType = "";
                if(g_Object.find((iter->second).obSN) == g_Object.end() || g_Object[(iter->second).obSN] == NULL)
                {
                    iter++;
                    continue;
                }
                if(type == SORT_BUILDING)
                {
                    obType = QString::fromStdString(((Building *)(g_Object[(iter->second).obSN]))->getDisplayName(((Building *)(g_Object[(iter->second).obSN]))->getNum()));
                }
                else if(type == SORT_COORES)
                {
                    obType = QString::fromStdString(((Coores *)(g_Object[(iter->second).obSN]))->getCooresDisplayName(((Coores *)(g_Object[(iter->second).obSN]))->getNum()));
                }
                else if(type == SORT_ANIMAL || type == SORT_TREEFOREST)
                {
                    obType = QString::fromStdString(((Animal *)(g_Object[(iter->second).obSN]))->getAnimalDisplayName(((Animal *)(g_Object[(iter->second).obSN]))->getNum()));
                }
                debugText("green"," <下达指令>FARMER(编号:" + QString::number(farmer->getSelectNum()) + ")设置工作目标为:" + obType + "(编号" + QString::number(((iter)->second).obSN) + ")");
                //                ui->DebugTextBrowser->insertHtml(COLOR_GREEN(getShowTime() + " <下达指令>农民(编号:" + QString::number(farmer->getSelectNum()) + ")设置工作目标为:" + obType + "(编号" + QString::number(((iter)->second).obSN) + ")"));
                //                ui->DebugTextBrowser->insertPlainText("\n");
            }
            else if((iter->second).A==3)
            {
                //                qDebug() << "建造";
                if(AIGame.civilizationStage == CIVILIZATION_STONEAGE && ((iter->second).BuildingNum == BUILDING_ARROWTOWER || (iter->second).BuildingNum == BUILDING_FARM || (iter->second).BuildingNum == BUILDING_MARKET))
                {
                    debugText("red"," 请先升级到工具时代再建造该建筑");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_RED(getShowTime() + " 请先升级到工具时代再建造该建筑"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    iter++;
                    continue;
                }
                std::list<Building *>::iterator buildIt = player[0]->build.begin();
                bool isGranaryBuilt = false, isMarketBuilt = false;
                for(; buildIt != player[0]->build.end(); buildIt++)
                {
                    if((*buildIt)->getNum() == BUILDING_GRANARY && (*buildIt)->isFinish())//建成谷仓
                    {
                        isGranaryBuilt = true;
                    }
                    if((*buildIt)->getNum() == BUILDING_MARKET && (*buildIt)->isFinish())//建成市场
                    {
                        isMarketBuilt = true;
                    }
                }
                if((iter->second).BuildingNum == BUILDING_MARKET && !isGranaryBuilt)
                {
                    debugText("red"," 请先建造谷仓");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_RED(getShowTime() + " 请先建造谷仓"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    iter++;
                    continue;
                }
                if((iter->second).BuildingNum == BUILDING_FARM && !isMarketBuilt)
                {
                    debugText("red"," 请先建造市场");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_RED(getShowTime() + " 请先建造市场"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    iter++;
                    continue;
                }
                if((iter->second).BuildingNum == BUILDING_ARROWTOWER && !player[0]->getArrowTowerUnlocked())
                {
                    debugText("red"," 请先在谷仓中研究箭塔建造的科技");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_RED(getShowTime() + " 请先在谷仓中研究箭塔建造的科技"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                    iter++;
                    continue;
                }
                nowobject = NULL;
                lastnowobject = g_Object[(iter->second).SN];
                Building *newBuild = getView((iter->second).BlockL,(iter->second).BlockU,(iter->second).BuildingNum + 1);
//                qDebug()<<"BlockL:"<<(iter->second).BlockL;
//                qDebug()<<"BlockU:"<<(iter->second).BlockU;
                if(newBuild != nullptr)
                {
                    debugText("green"," <下达指令>FARMER(编号:" + QString::number(((Farmer *)g_Object[(iter->second).SN])->getSelectNum()) + ")开始建造:" + QString::fromStdString(newBuild->getDisplayName((iter->second).BuildingNum)) + " 位于格子:(" + QString::number((iter->second).BlockL) + "," + QString::number((iter->second).BlockU) + ")");
                    //                    ui->DebugTextBrowser->insertHtml(COLOR_GREEN(getShowTime() + " <下达指令>农民(编号:" + QString::number(((Farmer *)g_Object[(iter->second).SN])->getSelectNum()) + ")开始建造:" + QString::fromStdString(newBuild->getDisplayName((iter->second).BuildingNum)) + " 位于格子:(" + QString::number((iter->second).BlockL) + "," + QString::number((iter->second).BlockU) + ")"));
                    //                    ui->DebugTextBrowser->insertPlainText("\n");
                }

            }
            iter++;
        }
        g_AiAction.clear();
    }
}

void MainWidget::get_Game()
{
    // 发送数据给 AI 线程
    AIGame = *Game;
}

void MainWidget::setShowTime()
{
    elapsedSec++;
}

void MainWidget::setShowTimeFrame()
{
    elapsedFrame += 4;
    if(elapsedFrame >= 100) elapsedFrame = 0;
}

QString MainWidget::getShowTime()
{
    QString minute;
    if(elapsedSec / 60 < 10) minute = "0" + QString::number(elapsedSec / 60);
    else minute = QString::number(elapsedSec / 60);
    QString second;
    if(elapsedSec % 60 < 10) second = "0" + QString::number(elapsedSec % 60);
    else second = QString::number(elapsedSec % 60);
    QString millSecond;
    if(elapsedFrame < 10) millSecond = "0" + QString::number(elapsedFrame);
    else millSecond = QString::number(elapsedFrame);
    return (minute + ":" + second + ":" + millSecond);
}

void MainWidget::exportDebugTextHtml()
{
    QString debugInfo = DebugText->toHtml();

    // 获取当前系统时间，用于命名文件
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

    // 获取项目文件夹路径
    QString projectPath = QDir::currentPath();

    // 构建输出文件夹路径
    QString outputPath = QDir::cleanPath(projectPath + QDir::separator() + "output");

    // 创建输出文件夹（如果不存在）
    QDir outputDir(outputPath);
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    // 构建文件名
    QString fileName = QString("%1/debug_info_%2.html").arg(outputPath).arg(currentTime);

    // 打开文件以写入文本
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << debugInfo;
        file.close();
        qDebug() << "已保存调试信息至:" << fileName;
    } else {
        qDebug() << "保存失败";
    }
}

void MainWidget::exportDebugTextTxt()
{
    QString debugInfo = DebugText->toPlainText();

    // 获取当前系统时间，用于命名文件
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

    // 获取项目文件夹路径
    QString projectPath = QDir::currentPath();

    // 构建输出文件夹路径
    QString outputPath = QDir::cleanPath(projectPath + QDir::separator() + "output");

    // 创建输出文件夹（如果不存在）
    QDir outputDir(outputPath);
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    // 构建文件名
    QString fileName = QString("%1/debug_info_%2.txt").arg(outputPath).arg(currentTime);

    // 打开文件以写入文本
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << debugInfo;
        file.close();
        qDebug() << "已保存调试信息至:" << fileName;
    } else {
        qDebug() << "保存失败";
    }
}

void MainWidget::clearDebugTextFile()
{
    // 获取项目文件夹路径
    QString projectPath = QDir::currentPath();

    // 构建输出文件夹路径
    QString outputPath = QDir::cleanPath(projectPath + QDir::separator() + "output");

    // 打开输出文件夹
    QDir outputDir(outputPath);

    // 遍历目录并删除文件
    QStringList fileList = outputDir.entryList(QDir::Files);
    foreach (QString fileName, fileList) {
        if (outputDir.remove(fileName)) {
            qDebug() << "已删除:" << fileName;
        } else {
            qDebug() << "删除失败";
        }
    }
}

void MainWidget::clearDebugText()
{
    DebugText->clear();
}

void MainWidget::toggleMusic()
{
    if(music)
    {
        music = false;
        bgm->stop();
    }
    else
    {
        music = true;
        bgm->play();
    }
}

void MainWidget::toggleSound()
{
    if(sound)
    {
        sound = false;
    }
    else
    {
        sound = true;
    }
}

void MainWidget::toggleSelect()
{
    if(select)
    {
        select = false;
    }
    else
    {
        select = true;
    }
}

void MainWidget::on_pause_clicked()
{
    if(pause)
    {
        pause = false;
        ui->pause->setText("暂停");
        timer->start();
        showTimer->start();
        debugText("blue"," 游戏继续");
    }
    else
    {
        pause = true;
        ui->pause->setText("继续");
        timer->stop();
        showTimer->stop();
        debugText("blue"," 游戏暂停");
    }
}

void MainWidget::toggleLine()
{
    if(line) line = false;
    else line = true;
    emit toggleLineSend(line);
}

void MainWidget::togglePos()
{
    if(pos) pos = false;
    else pos = true;
    emit togglePosSend(pos);
}

void MainWidget::receiveClickPos(int BlockL, int BlockU)
{
    debugText("blue", " 鼠标点击的块坐标为(" + QString::number(BlockL) + "," + QString::number(BlockU) + ")");
}

void MainWidget::toggleShowOverlap()
{
    if(showOverlap)
    {
        showOverlap = false;
    }
    else
    {
        showOverlap = true;
    }
}
