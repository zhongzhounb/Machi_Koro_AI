#include "stadium.h"
#include "player.h"
#include "gamestate.h"

Stadium::Stadium(QObject* parent):
    Card("体育馆", 6, Color::Purple, Type::Office, 2 , 6, 6, "如果你是投掷者，收取所有人 2 金币。",parent) {}

double Stadium::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString Stadium::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(activePlayer);
    //设置日志
    QString frontLog=QString("【%1】%2")
                      .arg(this->getName())
                    .arg(owner->getName());
    QString lastLog=QString(",没偷到");
    bool frontFlag=false,lastFlag=false;
    //收所有人
    for(Player* player : game->getAllPlayers())
        if(player!=owner){
            if(player->getCoins()==0){
                //日志
                if(lastFlag)
                    lastLog+="、";
                lastLog+=QString("%1").arg(player->getName());
                lastFlag=true;
            }
            else{
                //收益
                int coins=qMin(this->getValue(),player->getCoins());
                //赚钱
                owner->stealCoins(player,coins);
                //日志
                if(frontFlag)
                    frontLog+="，";
                frontLog+=QString("偷了%1的%2金币")
                           .arg(player->getName())
                           .arg(coins);
                frontFlag=true;
            }
        }
    //返回日志
    return frontLog+lastLog+"的金币";
}
