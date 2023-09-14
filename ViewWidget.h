#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H
#include "Building.h"
#include "Farmer.h"
#include "Coores.h"
#include "Animal.h"
#include "Ruin.h"
#include <QWidget>
#include <QPainter>
#include <MainWidget.h>
class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(QWidget *parent = nullptr);
    void setBuildList(std::list<Building *> *list)
    {
        this->buildList = list;
    }
    void setFarmerList(std::list<Human *> *list)
    {
        this->farmerList = list;
    }
    void setCooresList(std::list<Coores *> *list)
    {
        this->cooresList = list;
    }
    void setTreeList(std::list<Animal *> *list)
    {
        this->treeList = list;
    }
    void setRuinList(std::list<Ruin *> *list)
    {
        this->ruinList = list;
    }
    void setSceenRect(int left, int right)
    {
        screenL = left;
        screenU = right;

        update();
    }
    void paintEvent(QPaintEvent *);
    int screenL;
    int screenU;
signals:
private:
    std::list<Building *> *buildList=NULL;
    std::list<Human *> *farmerList=NULL;
    std::list<Coores *> *cooresList=NULL;
    std::list<Animal *> *treeList=NULL;
    std::list<Ruin *> *ruinList=NULL;
    MainWidget *mainwidget;
public slots:
};

#endif // VIEWWIDGET_H
