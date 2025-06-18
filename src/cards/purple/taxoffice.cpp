#include "taxoffice.h"
#include "player.h"
#include "gamestate.h"

TaxOffice::TaxOffice(QObject* parent):
    Card("税务所", 4, Color::Purple, Type::Office, 2 , 8, 9, "如果你是投掷者，其他人若有≥10金币，收取其一半（向下取整）金币。",parent) {}

double TaxOffice::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString TaxOffice::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
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
            if(player->getCoins()<10){
                //日志
                if(lastFlag)
                    lastLog+="、";
                lastLog+=QString("%1").arg(player->getName());
                lastFlag=true;
            }
            else{
                //收益
                int coins=player->getCoins()/this->getValue();
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
