#include "cafe.h"
#include "player.h"

Cafe::Cafe(QObject* parent):
    Card("咖啡馆", 2, Color::Red, Type::Restaurant, 1, 3, 3, "收取投掷者 1 金币。",parent) {}

double Cafe::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString Cafe::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //收益(不能超过激活者所拥有的）
    int coins=qMin(num*this->getValue(),activePlayer->getCoins());
    if(coins>0){
        //赚钱
        owner->stealCoins(activePlayer,coins);
        //返回日志
        return QString("【%1】%2%3偷了%4的%5金币")
            .arg(this->getName())
            .arg(num==1?"":QString("*%1").arg(num))
            .arg(owner->getName())
            .arg(activePlayer->getName())
            .arg(coins);
    }
    else
        return QString("【%1】%2%3没偷到%4的金币")
            .arg(this->getName())
            .arg(num==1?"":QString("*%1").arg(num))
            .arg(owner->getName())
            .arg(activePlayer->getName());
}

