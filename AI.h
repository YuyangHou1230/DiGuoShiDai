#ifndef AI_H
#define AI_H

#include <QObject>
#include <QThread>
#include <QTextBrowser>
#include <GlobalVariate.h>

class AI : public QObject
{
    Q_OBJECT

public:
    explicit AI(QObject *parent = nullptr);
    int BuildingAction(int SN, int Action)
    {
        tagAction BuildAction;
        BuildAction.A = 0;

        // SN判断
        bool judgeBuildingSN = false;
        for(int i = 0; i < AIGame.building_n; i++)
        {
            if(AIGame.building[i].SN == SN)
            {
                judgeBuildingSN = true;
                break;
            }
        }
        if(judgeBuildingSN == false) return ACTION_INVALID_SN;
        BuildAction.SN = SN;

        // Action判断
        if(Action != 1 && Action != 2 && Action != 3 && Action != 4 && Action != 5 && Action != 6 && Action != 12) return ACTION_INVALID_ACTION;
        BuildAction.Action = Action;

        g_AiAction[SN] = BuildAction;
        return ACTION_SUCCESS;
    }

    int HumanMove(int SN, int L0, int U0)
    {
        tagAction MoveAction;
        MoveAction.A = 1;

        // SN判断
        bool judgeHumanSN = false;
        for(int i = 0; i < AIGame.human_n; i++)
        {
            if(AIGame.human[i].SN == SN)
            {
                judgeHumanSN = true;
                break;
            }
        }
        if(judgeHumanSN == false) return ACTION_INVALID_SN;
        MoveAction.SN = SN;

        // L0判断
        if(L0 > 2575 || L0 < 0) return ACTION_INVALID_LOCATION;
        MoveAction.L0 = L0;

        // U0判断
        if(U0 > 2575 || U0 < 0) return ACTION_INVALID_LOCATION;
        MoveAction.U0 = U0;

        g_AiAction[SN] = MoveAction;
        return ACTION_SUCCESS;
    }

    int HumanAction(int SN, int obSN)
    {
        tagAction HumanAction;
        HumanAction.A = 2;

        // SN判断
        bool judgeHumanSN = false;
        for(int i = 0; i < AIGame.human_n; i++)
        {
            if(AIGame.human[i].SN == SN)
            {
                judgeHumanSN = true;
                break;
            }
        }
        if(judgeHumanSN == false) return ACTION_INVALID_SN;
        HumanAction.SN = SN;

        // obSN判断
        bool judgeOBSN = false;
        for(int i = 0; i < AIGame.resource_n; i++)
        {
            if(AIGame.resource[i].SN == obSN)
            {
                judgeOBSN = true;
                break;
            }
        }
        for(int i = 0; i < AIGame.building_n; i++)
        {
            if(AIGame.building[i].SN == obSN)
            {
                judgeOBSN = true;
                break;
            }
        }
        if(judgeOBSN == false) return ACTION_INVALID_OBSN;
        HumanAction.obSN = obSN;

        g_AiAction[SN] = HumanAction;
        return ACTION_SUCCESS;
    }

    int HumanBuild(int SN, int BuildingNum, int BlockL, int BlockU)
    {
        tagAction HumanBuild;
        HumanBuild.A = 3;

        // SN判断
        bool judgeHumanSN = false;
        for(int i = 0; i < AIGame.human_n; i++)
        {
            if(AIGame.human[i].SN == SN)
            {
                judgeHumanSN = true;
                break;
            }
        }
        if(judgeHumanSN == false) return ACTION_INVALID_SN;
        HumanBuild.SN = SN;

        // BuildingNum判断
        bool judgeBuildingNum = false;
        if(BuildingNum != 0 && BuildingNum != 1 && BuildingNum != 3 && BuildingNum != 4 && BuildingNum != 5 && BuildingNum != 6)
            return ACTION_INVALID_BUILDINGNUM;
        HumanBuild.BuildingNum = BuildingNum;

        if(BlockL > 72 || BlockL < 0) return ACTION_INVALID_LOCATION;
        HumanBuild.BlockL = BlockL;
        if(BlockU > 72 || BlockU < 0) return ACTION_INVALID_LOCATION;
        HumanBuild.BlockU = BlockU;

        g_AiAction[SN] = HumanBuild;
        return ACTION_SUCCESS;
    }


    int tranBlockL(double L)
    {
        int BlockL;
        BlockL = L / 16 / gen5;
        return BlockL;
    }

    int tranBlockU(double U)
    {
        int BlockU;
        BlockU = U / 16 / gen5;
        return BlockU;
    }

    double tranL(double BlockL)
    {
        double L;
        L= BlockL * 16 * gen5;
        return L;
    }

    double tranU(double BlockU)
    {
        double U;
        U= BlockU * 16 * gen5;
        return U;
    }

    // 计算两点间距离的平方（坐标）
    double calDistance(double L0, double U0, double TL0, double TU0)
    {
        return pow(pow(TL0 - L0, 2) + pow(TU0 - U0, 2), 0.5);
    }
private:

public slots:
    virtual void processData();

signals:

    void cheatRes();
    void cheatAge();
    void cheatPause();
    void cheatFarmerLimit();
    void AIDebugText(QString String);
};


#endif // AI_H
