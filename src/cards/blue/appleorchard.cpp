#include "appleorchard.h"
#include "player.h"

AppleOrchard::AppleOrchard(QObject* parent):
    Card("果园", 3, Color::Blue, Type::Agriculture, 3, 10, 10, "获得 3 金币。",parent) {}

double AppleOrchard::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString AppleOrchard::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(activePlayer);Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //收益
    int coins=num*this->getValue();
    //赚钱
    owner->addCoins(coins);
    //返回日志
    return QString("【%1】%2%3获得%4金币")
        .arg(this->getName())
        .arg(num==1?"":QString("*%1").arg(num))
        .arg(owner->getName())
        .arg(coins);
}

