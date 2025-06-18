#include "publisher.h"
#include "player.h"
#include "gamestate.h"

Publisher::Publisher(QObject* parent):
    Card("出版社", 5, Color::Purple, Type::Office, 1 , 7, 7, "如果你是投掷者，其他人每拥有一个☕/🏪，收取其 1 金币。",parent) {}

double Publisher::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString Publisher::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
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
            //计算共有多少类
            int num1=player->getTypeCardNum(Type::Restaurant,State::None);
            int num2=player->getTypeCardNum(Type::Store,State::None);
            if(player->getCoins()==0||num1+num2==0){
                //日志
                if(lastFlag)
                    lastLog+="、";
                lastLog+=QString("%1").arg(player->getName());
                lastFlag=true;
            }
            else{
                //收益
                int coins=qMin((num1+num2)*this->getValue(),player->getCoins());
                //赚钱
                owner->stealCoins(player,coins);
                //日志
                if(frontFlag)
                    frontLog+="，";
                frontLog+=QString("偷了%1(%2☕+%3🏪)的%4金币")
                                .arg(player->getName())
                                .arg(num1)
                                .arg(num2)
                                .arg(coins);
                frontFlag=true;
            }
        }
    //返回日志
    return frontLog+lastLog+"的金币";
}
