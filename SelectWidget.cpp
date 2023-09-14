#include "SelectWidget.h"
#include "ui_SelectWidget.h"

SelectWidget::SelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectWidget)
{
    ui->setupUi(this);
    MainWidget* c=(MainWidget*)this->parentWidget();
//    connect()
}

SelectWidget::~SelectWidget()
{
    delete ui;
}

void SelectWidget::paintEvent(QPaintEvent *event)
{
    if(obj != g_Object.end()->second && obj != NULL && (objType == 1 || objType == 6 || objType == 7 || !obj->isDie()))
    {
        QPainter painter(this);
        painter.drawRect(0, 0, 290, 170);
        painter.fillRect(QRect(0, 0, 290, 170), QBrush(QColor(Qt::black)));
        if(objType == 2 || (objType != 2 && !obj->isDie())){
            painter.drawRect(130, 90, 120, 20);
            if(objType != 2)
            {
                int StartX = 130, StartY = 110;
                int percent = obj->getBlood() * 100 / obj->getMaxBlood();
                if(percent > 100) percent = 100;
                painter.fillRect(QRect(117 + StartX, StartY, 3, 20), QBrush(QColor(0, 255, 0)));
                painter.fillRect(QRect(StartX, StartY, 120, 4), QBrush(QColor(0, 242, 11)));
                painter.fillRect(QRect(StartX, StartY + 4, 120, 12), QBrush(QColor(0, 103, 99)));
                painter.fillRect(QRect(StartX, StartY + 16, 120, 4), QBrush(QColor(0, 143, 71)));
                painter.fillRect(QRect(StartX, StartY + 3, 3, 3), QBrush(QColor(0, 103, 99)));
                painter.fillRect(QRect(StartX, StartY + 14, 3, 3), QBrush(QColor(0, 103, 99)));
                painter.fillRect(QRect(StartX + 120 * percent / 100, StartY, 120 - 120 * percent / 100, 20), QBrush(Qt::red));
            }

        }
    }
    else
    {
        this->hide();
    }
}
