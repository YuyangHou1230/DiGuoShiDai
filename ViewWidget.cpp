#include "ViewWidget.h"

ViewWidget::ViewWidget(QWidget *parent) : QWidget(parent)
{

    mainwidget=(MainWidget*)this->parentWidget();
}

void ViewWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for(int L = 0; L < MAP_L; L++)
    {
        for(int U = 0; U < MAP_U; U++)
        {
            int paintY = 1.23 * (MAP_L + L - U);
            int paintX = 3.55 * ((double)L + (double)U);
            if(mainwidget->map->cell[L][U].Explored)
            {
                painter.fillRect(QRect(paintX, paintY, 7, 4), QBrush(QColor(99, 123, 47)));
//                else painter.fillRect(QRect(paintX, paintY, 7, 4), QBrush(QColor(255,222,173)));
            }
        }
    }
    int outlineX = 3.55 * ((double)screenL + (double)screenU);
    int outlineY = 1.23 * (MAP_L + screenL - screenU);
    //遍历各list对象标注在地图上
    std::list<Building *>::iterator buildIter = buildList->begin();
    std::list<Human *>::iterator farmerIter = farmerList->begin();
    std::list<Coores *>::iterator cooresIter = cooresList->begin();
    std::list<Animal *>::iterator treeIter = treeList->begin();
    std::list<Ruin *>::iterator ruinIter = ruinList->begin();
    for(; treeIter != treeList->end(); treeIter++)
    {
            int treeL = (*treeIter)->getL() / 16 / gen5;
            int treeU = (*treeIter)->getU() / 16 / gen5;
            int treeY = 1.23 * (MAP_L + treeL - treeU);
            int treeX = 3.55 * (treeL + treeU);
            if((*treeIter)->isTreeForest()) painter.fillRect(QRect(treeX, treeY, 6, 6), QBrush(Qt::green));
            else painter.fillRect(QRect(treeX, treeY, 6, 6), QBrush(Qt::yellow));
    }
    for(; cooresIter != cooresList->end(); cooresIter++)
    {
            int cooresL = (*cooresIter)->getL() / 16 / gen5;
            int cooresU = (*cooresIter)->getU() / 16 / gen5;
            int cooresY = 1.23 * (MAP_L + cooresL - cooresU);
            int cooresX = 3.55 * (cooresL + cooresU);
            if((*cooresIter)->ProductIsFood()) painter.fillRect(QRect(cooresX, cooresY, 6, 6), QBrush(QColor(178,255,0)));
            else if((*cooresIter)->ProductIsStone()) painter.fillRect(QRect(cooresX, cooresY, 6, 6), QBrush(Qt::gray));
    }
    for(; buildIter != buildList->end(); buildIter++)
    {
        int buildL = (*buildIter)->getL() / 16 / gen5;
        int buildU = (*buildIter)->getU() / 16 / gen5;
        int buildY = 1.23 * (MAP_L + buildL - buildU);
        int buildX = 3.55 * (buildL + buildU);

//        qDebug() << buildL << " " << buildU << " " << buildX << " " << buildY;
        painter.fillRect(QRect(buildX, buildY, 8, 8), QBrush(Qt::red));
    }
    for(; farmerIter != farmerList->end(); farmerIter++)
    {
        int farmerL = (*farmerIter)->getL() / 16 / gen5;
        int farmerU = (*farmerIter)->getU() / 16 / gen5;
        int farmerY = 1.23 * (MAP_L + farmerL - farmerU);
        int farmerX = 3.55 * (farmerL + farmerU);
        painter.fillRect(QRect(farmerX, farmerY, 8, 8), QBrush(Qt::blue));
    }
    for(; ruinIter != ruinList->end(); ruinIter++)
    {
        int ruinL = (*ruinIter)->getL() / 16 / gen5;
        int ruinU = (*ruinIter)->getU() / 16 / gen5;
        int ruinY = 1.23 * (MAP_L + ruinL - ruinU);
        int ruinX = 3.55 * (ruinL + ruinU);
        painter.fillRect(QRect(ruinX, ruinY, 8, 8), QBrush(QColor(128,0,128)));
    }
    for(int L = 0; L < MAP_L; L++)
    {
        for(int U = 0; U < MAP_U; U++)
        {
            int paintY = 1.23 * (MAP_L + L - U);
            int paintX = 3.55 * (L + U);
            if(!(mainwidget->map->cell[L][U].Explored))
            {
                painter.fillRect(QRect(paintX, paintY, 7, 4), QBrush(Qt::black));
            }

        }
    }
    //绘制当前区域边框
    painter.fillRect(QRect(outlineX, outlineY, 160, 1), QBrush(Qt::white));
    painter.fillRect(QRect(outlineX, outlineY + 61, 160, 1), QBrush(Qt::white));
    painter.fillRect(QRect(outlineX, outlineY, 1, 61), QBrush(Qt::white));
    painter.fillRect(QRect(outlineX + 160, outlineY, 1, 61), QBrush(Qt::white));
}
