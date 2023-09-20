#include "GameWidget.h"
#include "ui_GameWidget.h"

GameWidget::GameWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameWidget)
{
    ui->setupUi(this);
    mainwidget=(MainWidget*)this->parentWidget();
    connect(mainwidget,SIGNAL(mapmove()),this,SLOT(movemap()));
    connect(mainwidget,SIGNAL(toggleLineSend(bool)),this,SLOT(toggleLineReceive(bool)));
    connect(mainwidget,SIGNAL(togglePosSend(bool)),this,SLOT(togglePosReceive(bool)));
    setFocusPolicy(Qt::StrongFocus);
}

GameWidget::~GameWidget()
{
    delete ui;
    drawlist.clear();
}

void GameWidget::paintEvent(QPaintEvent *)
{
    //        QElapsedTimer timer;
    //        timer.start();

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
    QPainter painter(this);

    //    QPixmap pix;
    painter.setPen(Qt::black);

    // 使用 fillRect 方法填充整个窗口区域为黑色
    painter.fillRect(rect(), Qt::black);

    //地图绘制部分
    int x1=BlockL;//x1，y1作为参考坐标，用来每次循环初始化x2，y2的值
    int y1=BlockU;//x2，y2则是引导绘制，用来实际判断cell中的内容
    int x2;
    int y2;

    for(int i = 0; i < MAP_L; i++)
    {
        for(int j = 0; j < MAP_U; j++)
        {
            mainwidget->map->cell[i][j].Visible = false;
        }
    }

    //    //遍历所有建筑和人物，并将视野内block转为可见状态
    if(!mainwidget->player[0]->build.empty())
    {
        std::list<Building*>::iterator tmpBuild = mainwidget->player[0]->build.begin();

        while(tmpBuild != mainwidget->player[0]->build.end())
        {
            if((*tmpBuild)->isFinish())
            {
                if((*tmpBuild)->isMiddleFoundation())
                {
    //                openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+2,(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+2,(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+2,(*tmpBuild)->getVision());

                    //左角
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()-3);

                    //左上边
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+2);

                    //上角
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+5);

                    //右上边
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+5);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+5);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+5);

                    //右角
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+5);
                    openVision((*tmpBuild)->getBlockL()+5,(*tmpBuild)->getBlockU()+3);


                    //右下边
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+5,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+5,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()+5,(*tmpBuild)->getBlockU()+2);

                    //下角
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+5,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()-3);

                    //左下边
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()-3);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()-3);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()-3);

                    //中心
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+2);

                }
                else if((*tmpBuild)->isSmallFoundation())
                {
    //                openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
    //                openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());

                    //左角
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()-3);

                    //左上边
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+1);

                    //上角
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()-2,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()-3,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()-1,(*tmpBuild)->getBlockU()+4);

                    //右上边
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+4);

                    //右角
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+2);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+3);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+4);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()+2);

                    //右下边
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()+1);

                    //下角
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()-3);
                    openVision((*tmpBuild)->getBlockL()+4,(*tmpBuild)->getBlockU()-1);

                    //左下边
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()-3);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()-1);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()-2);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()-3);

                    //中心
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU());
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+1);
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+1);

                }
                else if((*tmpBuild)->isBigFoundation())
                {
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+2,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+2,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+3,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+2,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+3,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+2,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+3,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+3,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU()+1,(*tmpBuild)->getVision());
                    openVision((*tmpBuild)->getBlockL()+3,(*tmpBuild)->getBlockU(),(*tmpBuild)->getVision());

                }

            }
            else
            {
                if((*tmpBuild)->isMiddleFoundation())
                {
                    //中心
                    setExplored((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU());
                    setExplored((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU());
                    setExplored((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU());
                    setExplored((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+1);
                    setExplored((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+2);
                    setExplored((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+1);
                    setExplored((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+2);
                    setExplored((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+1);
                    setExplored((*tmpBuild)->getBlockL()+2,(*tmpBuild)->getBlockU()+2);

                }
                else if((*tmpBuild)->isSmallFoundation())
                {
                    //中心
                    setExplored((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU());
                    setExplored((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU());
                    setExplored((*tmpBuild)->getBlockL(),(*tmpBuild)->getBlockU()+1);
                    setExplored((*tmpBuild)->getBlockL()+1,(*tmpBuild)->getBlockU()+1);
                }

            }

            //            for(int i = (*tmpBuild)->getBlockL() - 4; i <= (*tmpBuild)->getBlockL() + 4; i++)
            //            {
            //                if(i < 0 || i >= MAP_L) continue;
            //                for(int j = (*tmpBuild)->getBlockU() - 4; j <= (*tmpBuild)->getBlockU() + 4; j++)
            //                {
            //                    if(j < 0 || j >= MAP_U) continue;

            //                    // 计算当前单元格与中心位置的距离
            //                    int distanceX = abs((*tmpBuild)->getBlockL() - i);
            //                    int distanceY = abs((*tmpBuild)->getBlockU() - j);
            //                    double distance = sqrt(distanceX * distanceX + distanceY * distanceY);

            //                    // 如果距离小于等于4（近似圆的半径），则设置Visible和Explored为true
            //                    if(distance <= 4&& !(distanceX == 4 || distanceY == 4))
            //                    {
            //                        mainwidget->map->cell[i][j].Visible = true;
            //                        mainwidget->map->cell[i][j].Explored = true;
            //                    }
            //                }
            //            }
            tmpBuild++;
        }
    }

    if (!mainwidget->player[0]->human.empty())
    {
        std::list<Human*>::iterator tmpHuman = mainwidget->player[0]->human.begin();

        while (tmpHuman != mainwidget->player[0]->human.end())
        {
            int centerX = ((*tmpHuman)->getL()) / 16 / gen5; // 圆心横坐标
            int centerY = ((*tmpHuman)->getU()) / 16 / gen5; // 圆心纵坐标
            openVision(centerX,centerY,(*tmpHuman)->getVision());
            tmpHuman++;
        }
    }


    for(int i=0;i<GAMEWIDGET_HEIGHT/(mainwidget->map->cell[0][0].block[0]->front().pix.height()/2.0)+2;i++)
    {
        x2=x1;
        y2=y1;
        if(i%2==1)
        {
            y1--;
        }else if(i%2==0)
        {
            x1++;
        }
        //此处改动不采用nowres来显示图片
        for(int j=0;j<GAMEWIDGET_WIDTH/mainwidget->map->cell[0][0].block[0]->front().pix.width()+1;j++) // 行绘制
        {
            if(x2>=MAP_L||y2>=MAP_U||x2<0||y2<0)
            {
                x2++;
                y2++;
                continue;
            }
            if(i%2==0)
            {
                if(mainwidget->map->cell[x2][y2].Visible == true && mainwidget->map->cell[x2][y2].Explored == true)
                    painter.drawPixmap(-32+64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::block[mainwidget->map->cell[x2][y2].Num]->front().pix);//先采用64*32，之后改为65*33
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == true)
                {
                    //                    testimage = applyTransparencyEffect(resMap["Grass"].front(), 0.5);
                    painter.drawPixmap(-32+64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::grayblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                    //                    painter.drawPixmap(-32+64*j,-16+16*i,64,32,resMap["Grass"].front());
                }
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == false)
                {
                    //                    testimage = applyTransparencyEffect(resMap["Grass"].front(), 1);
                    painter.drawPixmap(-32+64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::blackblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                }
            }
            if(i%2==1)
            {
                if(mainwidget->map->cell[x2][y2].Visible == true && mainwidget->map->cell[x2][y2].Explored == true)
                    painter.drawPixmap(64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::block[mainwidget->map->cell[x2][y2].Num]->front().pix);
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == true)
                {
                    //                    testimage = applyTransparencyEffect(resMap["Grass"].front(), 0.5);
                    painter.drawPixmap(64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::grayblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                }
                else if(mainwidget->map->cell[x2][y2].Visible == false && mainwidget->map->cell[x2][y2].Explored == false)
                {
                    //                    testimage = applyTransparencyEffect(resMap["Grass"].front(), 1);
                    painter.drawPixmap(64*j,-16+16*i,BLOCKPIXEL_X,BLOCKPIXEL_Y,Block::blackblock[mainwidget->map->cell[x2][y2].Num]->front().pix);
                }
            }

            x2++;
            y2++;
        }
    }

    //清除内存图内容
    emptymemorymap();

    //绘制列表清空
    drawlist.clear();
    //    qDebug()<<"begin"<<drawlist.size();
    //玩家的建筑部分 人物部分
    for(int i=0;i<MAXPLAYER;i++)
    {
        std::list<Building *> *b=&(mainwidget->player[i]->build);
        std::list<Building *>::iterator biter=b->begin();
        while(!b->empty()&&biter!=b->end())
        {
            Coordinate *p=*biter;
            insert(p,&drawlist);
            biter++;
        }


        //        flag=0;
        std::list<Missile *> *m=&(mainwidget->player[i]->missile);
        std::list<Missile *>::iterator miter=m->begin();
        while(!m->empty()&&miter!=m->end())
        {
            Coordinate *p=*miter;
            insert(p,&drawlist);
            //            flag++;
            miter++;
        }

        std::list<Human *> *h=&(mainwidget->player[i]->human);
        std::list<Human *>::iterator hiter=h->begin();
//        qDebug()<<h->size();
        while(!h->empty()&&hiter!=h->end())
        {
                Coordinate *p=*hiter;
                insert(p,&drawlist);
            hiter++;
        }

        //        std::list<Farmer *> *f=&(mainwidget->player[i]->farmer);
        //        std::list<Farmer *>::iterator fiter=f->begin();
        //        //        qDebug()<<"before"<<drawlist.size();
        //        while(!f->empty()&&fiter!=f->end())
        //        {

        //            Coordinate *p=*fiter;
        //            insert(p,&drawlist);
        //            fiter++;
        //        }
        //        qDebug()<<"after"<<drawlist.size();
    }

    //地图资源相关 树木石块等
    std::list<Coores*> *cr=&mainwidget->map->coores;
    if(!cr->empty())
    {
        std::list<Coores*>::iterator criter=cr->begin();
        while(criter!=cr->end())
        {
            if((*criter)->isExplored())
            {
                judgeVisible((*criter));
                insert((*criter),&drawlist);
            }
            else
            {
                judgeExplored((*criter));
            }
            criter++;
        }
    }

    std::list<Ruin *> *r=&(mainwidget->map->ruin);
    if(!r->empty())
    {
        std::list<Ruin *>::iterator riter=r->begin();
        while(riter!=r->end())
        {
            if((*riter)->isExplored())
            {
                judgeVisible((*riter));
                insert((*riter),&drawlist);
            }
            else
            {
                judgeExplored((*riter));
            }
            riter++;
        }
    }

    //    std::list<Ruin *>::iterator riter=r->begin();
    //    while(!r->empty()&&riter!=r->end())
    //    {
    //        Coordinate *p=*riter;
    //        insert(p,&drawlist);
    //        riter++;
    //    }

    std::list<Animal *> *ar=&mainwidget->map->animal;
    if(!ar->empty())
    {
        std::list<Animal *>::iterator ariter=ar->begin();
        while(ariter!=ar->end())
        {
            if(mainwidget->map->cell[(*ariter)->getBlockL()][(*ariter)->getBlockU()].Explored==true)
            {
                insert((*ariter),&drawlist);
            }
//            if((*ariter)->isExplored())
//            {
//                judgeVisible((*ariter));
//                insert((*ariter),&drawlist);
//            }
//            else
//            {
//                judgeExplored((*ariter));
//            }
            ariter++;
        }
    }


    //当前选中的脚下画框
    if(nowselect!=NULL)
    {
        painter.setPen(Qt::white);
        int width=nowselect->getLX()*2;
        int height=nowselect->getLY()*2;
        int X=tranX(nowselect->getL()-L,nowselect->getU()-U)-nowselect->getLX();
        int Y=tranY(nowselect->getL()-L,nowselect->getU()-U) - height / 2;
        QPolygonF diamond;
        diamond << QPointF(X+width/2, Y);
        diamond << QPointF(X+width, Y+height/2);
        diamond << QPointF(X+width/2, Y+height);
        diamond << QPointF(X, Y+height/2);
        painter.drawPolygon(diamond);
    }
    if(!selectFarmer->empty())
    {
        auto iter=selectFarmer->begin();
        while(iter!=selectFarmer->end())
        {
            painter.setPen(Qt::white);
            int width=(*iter)->getLX()*2;
            int height=(*iter)->getLY()*2;
            int X=tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX();
            int Y=tranY((*iter)->getL()-L,(*iter)->getU()-U) - height / 2;
            QPolygonF diamond;
            diamond << QPointF(X+width/2, Y);
            diamond << QPointF(X+width, Y+height/2);
            diamond << QPointF(X+width/2, Y+height);
            diamond << QPointF(X, Y+height/2);
            painter.drawPolygon(diamond);
            iter++;
        }
    }


    //drawlist正常绘制
    if(!drawlist.empty())
    {
        //qDebug()<<drawlist.size();
        std::list<Coordinate *>::iterator iter=drawlist.begin();
        while(iter!=drawlist.end())
        {
            (*iter)->nextframe();
            (*iter)->setnotinWindow();
            iter++;
        }
        emit startJudgeStop();
        emit startUpdatePosition();
        iter=drawlist.begin();
        if(mainwidget->mapmoveFrequency>4)
        {
            while(iter!=drawlist.end())
            {

                painter.drawPixmap(tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX(),(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U),(*iter)->getNowRes()->pix.width(),(*iter)->getNowRes()->pix.height(),(*iter)->getNowRes()->pix);
                //                drawmemory(tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX(),(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U),(*(*iter)->getNowRes()),(*iter)->getSelectNum());
                if(judgeinWindow(tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX(),(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U)))
                {
                    (*iter)->setisinWindow();
                }
                iter++;
            }
        }
        else
            while(iter!=drawlist.end())
            {
//                qDebug()<<(*iter)->getSort();
//                if((*iter)->getSort()==SORT_FARMER)
//                {
//                    Farmer *farmer=(Farmer *)(*iter);
//                    qDebug()<<"sn:"<<farmer->getSelectNum();
//                    qDebug()<<"nowstate:"<<farmer->getNowState();
//                    qDebug()<<"prestate:"<<farmer->getPreState();
//                    qDebug()<<"state:"<<farmer->getstate();
//                    qDebug()<<"angle:"<<farmer->getAngle();
////                    qDebug()<<"tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX():"<<tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX();
//                    if((*iter)->getNowRes()->pix.isNull())
//                    {
//                        qDebug()<<"1";
//                    }
////                    qDebug()<<"(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U):"<<(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U);
//                    qDebug()<<"(*iter)->getNowRes()->pix.width():"<<(*iter)->getNowRes()->pix.width();
////                    qDebug()<<""
//                }
//                else if((*iter)->getSort()==SORT_ANIMAL)
//                {
//                    Animal *animal=(Animal *)(*iter);
//                    if(animal->isLion())
//                    {
//                        qDebug()<<"sn:"<<animal->getSelectNum();
//                        qDebug()<<"state:"<<animal->getNowState();
//                    }
//                }
                painter.drawPixmap(tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX(),(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U),(*iter)->getNowRes()->pix.width(),(*iter)->getNowRes()->pix.height(),(*iter)->getNowRes()->pix);
                drawmemory(tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX(),(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U),(*(*iter)->getNowRes()),(*iter)->getSelectNum());
                if(judgeinWindow(tranX((*iter)->getL()-L,(*iter)->getU()-U)-(*iter)->getLX(),(*iter)->getLY()-(*iter)->getNowRes()->pix.height()+tranY((*iter)->getL()-L,(*iter)->getU()-U)))
                {
                    (*iter)->setisinWindow();
                }
                iter++;
            }

        //        qDebug()<<drawlist.size();
    }
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    //    g_DebugText->insertPlainText("当前执行次数："+QString::number(++flag)+"  所需执行时间(微秒为单位):"+QString::number(ElapsedMicroseconds.QuadPart)+"\n");
    //    qDebug()<<"flag:"<<++flag<<"   "<<ElapsedMicroseconds.QuadPart;
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton)
    {
        emit rightpress(mainwidget->memorymap[event->x()/4][event->y()/4],tranL(event->x(),event->y())+L,tranU(event->x(),event->y())+U);
    }
    if(event->button()==Qt::LeftButton)
    {
        emit returnselectNum(mainwidget->memorymap[event->x()/4][event->y()/4]);
//        qDebug()<<mainwidget->memorymap[event->x()/4][event->y()/4];
        int hoverL = (tranL(event->x(), event->y()) + L) / BLOCKSIDELENGTH;
        int hoverU = (tranU(event->x(), event->y()) + U) / BLOCKSIDELENGTH;
        if(pos) emit sendClickPos(hoverL, hoverU);
        if(buildMode > 0){

            if(buildMode == 2 || buildMode == 4 || buildMode == 5 || buildMode == 6)
            {
                hoverL--;
                hoverU--;
            }
            emit sendView(hoverL, hoverU, buildMode);
            buildMode = 0;
        }
        // 记录框选区域的起始位置
        selectionStartPos = event->pos();
    }
    if(event->button()==Qt::MidButton)
    {
        emit midpress();
    }
}

void GameWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 记录框选区域的结束位置

        selectionEndPos = event->pos();
//        qDebug()<<"记录终止";
//        qDebug()<<"selectionStartPos.x():"<<selectionStartPos.x();
//        qDebug()<<"selectionStartPos.y():"<<selectionStartPos.y();
//        qDebug()<<"selectionEndPos.x():"<<selectionEndPos.x();
//        qDebug()<<"selectionEndPos.y():"<<selectionEndPos.y();
        // 确定被选择的对象
        selectFarmer->clear();
        std::unordered_set<int> selectedObjects;
        for (int i = selectionStartPos.x()/4; i < selectionEndPos.x()/4; ++i)
        {
            for (int j = selectionStartPos.y()/4; j < selectionEndPos.y()/4; ++j)
            {
                if(i >= MEMORYROW || j >= MEMORYCOLUMN) continue;
                if(mainwidget->memorymap[i][j]!=0)
                {
                    selectedObjects.insert(mainwidget->memorymap[i][j]);
                }
            }
        }
        for (auto it = selectedObjects.begin(); it != selectedObjects.end(); ++it)
        {
            if((*it)/10000==SORT_FARMER)
            {
                Farmer *farmer = (Farmer*)g_Object[(*it)];
                selectFarmer->push_back(farmer);
            }
        }
        emit selectFarmerSignal(selectFarmer);
    }
}

void GameWidget::judgeExplored(Coordinate *object)
{
    if(mainwidget->map->cell[object->getBlockL()][object->getBlockU()].Explored==true)
    {
        object->setIsExplored();
    }
}

void GameWidget::judgeVisible(Coordinate *object)
{
    //    qDebug()<<"L"<<object->getBlockL();
    //    qDebug()<<"U"<<object->getBlockU();
    if(mainwidget->map->cell[object->getBlockL()][object->getBlockU()].Visible==true)
    {
        object->setIsVisible();
    }
    else
    {
        object->setIsUnVisible();
    }
}

//坐标间的相互转化
int GameWidget::tranX(int L, int U)
{
    int X;
    //    X=L*2.0/gen5+U*2.0/gen5;
    X=(L+U)*2/gen5;
    return X;
}

int GameWidget::tranY(int L, int U)
{
    int Y;
    //    Y=L/gen5-U/gen5;
    Y=(L-U)/gen5;
    return Y;
}

int GameWidget::tranL(int X, int Y)
{
    int L;
    L=X*gen5/4.0+Y*gen5/2.0;
    return L;
}

int GameWidget::tranU(int X, int Y)
{
    int U;
    U=X*gen5/4.0-Y*gen5/2.0;
    return U;
}

void GameWidget::openVision(int BlockL, int BlockU, int Vision)
{
    for (int i = BlockL - Vision; i <= BlockL + Vision; i++)
    {
        if (i < 0 || i >= MAP_L) continue;

        for (int j = BlockU - Vision; j <= BlockU + Vision; j++)
        {
            if (j < 0 || j >= MAP_U) continue;

            // 计算当前单元格与圆心的距离
            int distanceX = abs(BlockL - i);
            int distanceY = abs(BlockU - j);
            double distance = sqrt(distanceX * distanceX + distanceY * distanceY);

            // 如果距离小于等于4（近似圆的半径），则设置Visible和Explored为true
            if (distance <= Vision&& !(distanceX == Vision || distanceY == Vision))
            {
                mainwidget->map->cell[i][j].Visible = true;
                mainwidget->map->cell[i][j].Explored = true;
            }
        }
    }
}

void GameWidget::openVision(int BlockL, int BlockU)
{
    if(BlockL<72&&BlockL>=0&&BlockU<72&&BlockU>=0)
    {
        mainwidget->map->cell[BlockL][BlockU].Visible = true;
        mainwidget->map->cell[BlockL][BlockU].Explored = true;
    }
}

void GameWidget::setExplored(int BlockL, int BlockU)
{
    if(BlockL<72&&BlockL>=0&&BlockU<72&&BlockU>=0)
    {
        mainwidget->map->cell[BlockL][BlockU].Explored = true;
    }
}

//根据当前对象高度插入drawlist
void GameWidget::insert(Coordinate *p, std::list<Coordinate *> *drawlist)
{
    if(drawlist->empty())
    {
        drawlist->push_back(p);
    }
    else
    {
        std::list<Coordinate *>::iterator iter=drawlist->begin();
        while(1)//H越大说明越靠下，应该先打印H小的，所以H小的在前边
        {
            //            qDebug()<<p->L;
            //            qDebug()<<(*iter)->H;
            if(p->getH()<(*iter)->getH())
            {
                drawlist->insert(iter,p);
                break;
            }
            if((*iter)==drawlist->back())
            {
                drawlist->push_back(p);
                break;
            }
            iter++;
        }
    }

}

//绘制内存图
void GameWidget::drawmemory(int X, int Y, Resource res, int selectNum)
{
    for(int i=0;i<res.pix.width();i++)
    {
        for(int j=0;j<res.pix.height();j++)
        {
            int mx,my;
            mx=i+X;
            my=j+Y;
            if(mx>=0&&my>=0&&mx<GAMEWIDGET_WIDTH&&my<GAMEWIDGET_HEIGHT)//
            {
                if(res.memorymap.getMemoryMap(i,j)!=0)
                {
                    mainwidget->memorymap[mx/4][my/4]=selectNum;
                }
            }
        }
    }
}

bool GameWidget::judgeinWindow(double x, double y)
{
    if(x>0&&x<GAMEWIDGET_WIDTH&&y>0&&y<GAMEWIDGET_HEIGHT)
    {
        return 1;
    }
    return 0;
}

//清除内存图
void GameWidget::emptymemorymap()
{
    for(int i=0;i<MEMORYROW;i++)
    {
        for(int j=0;j<MEMORYCOLUMN;j++)
        {
            mainwidget->memorymap[i][j]=0;
        }
    }
}

//地图移动
void GameWidget::movemap()
{
//    return;
    //此处采用相对坐标，只相对于当前窗口 所以在纵向数据判断处 应该加上的是下窗口的大小
    int x=this->mapFromGlobal(QCursor().pos()).x();
    int y=this->mapFromGlobal(QCursor().pos()).y();
    //    qDebug()<<"x:"<<x;
    //    qDebug()<<"y:"<<y;
    if(BlockL+22<0)
    {
        BlockL++;
    }
    if(BlockU<0)
    {
        BlockU++;
    }
    if(BlockL+GAMEWIDGET_MIDBLOCKL>MAP_L-1)
    {
        BlockL--;
    }
    if(BlockU+GAMEWIDGET_MIDBLOCKU>MAP_U-1)
    {
        BlockU--;
    }
    if(x<2)
    {
        BlockL--;
        BlockU--;
    }
    if(x>GAME_WIDTH-20)
    {
        BlockL++;
        BlockU++;
    }
    if(y<-44)
    {
        BlockU++;
        BlockL--;
    }
    if(y>GAME_HEIGHT-50-45)//此处先用常数 其中45代表上边框的宽
    {
        BlockU--;
        BlockL++;
    }
    if(x<2&&y<-44)
    {
        BlockL++;
    }
    if(x<-44&&y>GAME_HEIGHT-50-45)
    {
        BlockU++;
    }
    if(x>GAME_WIDTH-20&&y<-44)
    {
        BlockU--;
    }
    if(x>GAME_WIDTH-20&&y>GAME_HEIGHT-50-45)
    {
        BlockL--;
    }
    L=(BlockL+0.5)*16*gen5;
    U=(BlockU+0.5)*16*gen5;

}

void GameWidget::UpdateData()
{
    if(!drawlist.empty())
    {
        std::list<Coordinate *>::iterator iter=drawlist.begin();
        while(iter!=drawlist.end())
        {
            (*iter)->nextframe();
            iter++;
        }
        emit startJudgeStop();
        emit startUpdatePosition();
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == KEY_UP)
    {
        BlockU++;
        BlockL--;
    }
    if(event->key() == KEY_DOWN)
    {
        BlockU--;
        BlockL++;
    }
    if(event->key() == KEY_LEFT)
    {
        BlockL--;
        BlockU--;
    }
    if(event->key() == KEY_RIGHT)
    {
        BlockL++;
        BlockU++;
    }
}

void GameWidget::toggleLineReceive(bool line)
{
    showLine = line;
    this->update();
}

void GameWidget::togglePosReceive(bool pos)
{
    this->pos = pos;

}
