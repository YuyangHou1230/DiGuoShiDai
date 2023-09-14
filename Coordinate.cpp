#include "Coordinate.h"
#include <MainWidget.h>

Coordinate::Coordinate()
{

}

Coordinate::~Coordinate()
{

}

void Coordinate::nextframe()
{

}

//根据LU转化为X
double Coordinate::tranX(double L, double U)
{
    double X;
    //    X=L*2.0/gen5+U*2.0/gen5;
    X=(L+U)*2/gen5;
    return X;
}

//根据LU转化为Y
double Coordinate::tranY(double L, double U)
{
    double Y;
    //    Y=L/gen5-U/gen5;
    Y=(L-U)/gen5;
    return Y;
}

double Coordinate::tranBlockL(double L)
{
    return L/(16*gen5);
}

double Coordinate::tranBlockU(double U)
{
    return U/(16*gen5);
}
