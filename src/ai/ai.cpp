#include "ai.h"
#include "gamestate.h"
#include "player.h"
AI:: AI(QObject* parent ): QObject(parent){}

void AI::update(GameState* state){
    QList<Player*>players=state->getPlayers();
    //先模拟一遍更新值

    //再做计算
    for(Player* player:players){

        QMap<int,double>value=m_PointToValue.value(player);

        bool hasHarbor=player->getCardNum("港口",State::Opening);
        bool hasAmuseMentPark=player->getCardNum("游乐园",State::Opening);

        double OneDiceEx=0.0;
        double TwoDiceEx=0.0;
        //抛一个
        for(int i=1;i<=6;i++)
            OneDiceEx+=value[i]/6.0;
        //抛两个
        for(int i=2;i<=12;i++)
            if(hasHarbor&&i>=10)//【港口】如果有港口则和+2的比大小
                TwoDiceEx+=qMax(value[i],value[i+2])*(6.0-fabs(7.0-i))/36.0;
            else
                TwoDiceEx+=value[i]*(6.0-fabs(7.0-i))/36.0;
        //【游乐园】如果有游乐园，期望收益多1/6，因为抛两个有1/6概率重抛
        if(hasAmuseMentPark)
            TwoDiceEx+=TwoDiceEx/6.0;

        //【广播塔】如果抛出的小于期望值且不相同，就重抛
        QMap<int,bool>isReRollForOneDice;
        QMap<int,bool>isReRollForTwoDice;
        for(int i=1;i<=6;i++)
            isReRollForOneDice[i]=(value[i]);
            else
        isReRollForOneDice[i]=true;

    }
}
