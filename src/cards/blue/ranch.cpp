#include "ranch.h"
#include "player.h"

Ranch::Ranch(QObject* parent):
    Card("农场", 1, Color::Blue, Type::Husbandry, 1 , 2, 2, "获得 1 金币。",parent) {}

double Ranch::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString Ranch::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(activePlayer);Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //收益
    int coins=num*this->getValue();
    //赚钱
    owner->addCoins(coins*1);
    //返回日志
    return QString("【%1】%2%3获得%4金币")
        .arg(this->getName())
        .arg(num==1?"":QString("*%1").arg(num))
        .arg(owner->getName())
        .arg(coins);
}
