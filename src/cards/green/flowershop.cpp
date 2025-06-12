#include "flowershop.h"
#include "player.h"

FlowerShop::FlowerShop(QObject* parent):
    Card("花店", 1, Color::Green, Type::Store, 1 , 6, 6, "如果你是投掷者，你每拥有一个【花田】，获得 1 金币。",parent) {}

double FlowerShop::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString FlowerShop::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(activePlayer);Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //组合数量
    int combo=owner->getCardNum("花田",State::Opening);
    //收益
    int coins=num*combo*this->getValue();
    //赚钱
    owner->addCoins(coins);
    //返回日志
    return QString("【%1】%2%3拥有%4【花田】获得%5金币")
        .arg(this->getName())
        .arg(num==1?"":QString("*%1").arg(num))
        .arg(owner->getName())
        .arg(combo)
        .arg(coins);
}
