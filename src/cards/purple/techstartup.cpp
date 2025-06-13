#include "techstartup.h"
#include "player.h"
#include "game.h"

TechStartup::TechStartup(QObject* parent):
    Card("科技公司", 1, Color::Purple, Type::Office, 0 , 10, 10, "无论是否激活，你每回合选择是否投资 1 金币。激活后，收取所有人投资数量的金币。",parent) {}

double TechStartup::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString TechStartup::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
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
            if(player->getCoins()==0||this->getValue()==0){
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
