#include "familyrestaurant.h"
#include "player.h"

FamilyRestaurant::FamilyRestaurant(QObject* parent):
    Card("西餐厅", 3, Color::Red, Type::Restaurant, 2 , 9, 10, "收取投掷者 2 金币。",parent) {}

double FamilyRestaurant::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString FamilyRestaurant::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
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
