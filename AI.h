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
