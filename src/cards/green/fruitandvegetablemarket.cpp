#include "fruitandvegetablemarket.h"
#include "player.h"

FruitAndVegetableMarket::FruitAndVegetableMarket(QObject* parent):
    Card("果蔬超市", 2, Color::Green, Type::Factory, 2 , 11, 12, "如果你是投掷者，你每拥有一个🌾，获得 2 金币。",parent) {}

double FruitAndVegetableMarket::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString FruitAndVegetableMarket::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(activePlayer);Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //组合数量
    int combo=owner->getTypeCardNum(Type::Agriculture,State::None);
    //收益
    int coins=num*combo*this->getValue();
    //赚钱
    owner->addCoins(coins);
    //返回日志
    return QString("【%1】%2%3拥有%4🌾获得%5金币")
        .arg(this->getName())
        .arg(num==1?"":QString("*%1").arg(num))
        .arg(owner->getName())
        .arg(combo)
        .arg(coins);
}
