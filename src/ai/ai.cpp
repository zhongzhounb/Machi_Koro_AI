#include "ai.h"

AI:: AI(QObject* parent ): QObject(parent){}

double AI::oneDiceEx(){
    double sum=0.0;
    for(int i=1;i<=6;i++)
        sum+=m_PointNumToValue[i]/6.0;
    return sum;
}

double AI::twoDiceEx(bool hasHarbor){
    double sum=0.0;
    for(int i=1;i<=12;i++)
        if(i>=10&&hasHarbor)
            sum+=qMax(m_PointNumToValue[i],m_PointNumToValue[i+2])*(6.0-fabs(i-7.0))/36.0;
        else
            sum+=m_PointNumToValue[i]*(6.0-fabs(i-7.0))/36.0;
    return sum;
}
