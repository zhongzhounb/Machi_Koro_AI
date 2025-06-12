#include "sodabottlingplant.h"
#include "player.h"

SodaBottlingPlant::SodaBottlingPlant(QObject* parent):
    Card("饮料工厂", 5, Color::Green, Type::Factory, 1 , 11, 11, "如果你是投掷者，所有人每拥有一张☕，你获得 1 金币。",parent) {}

double SodaBottlingPlant::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString SodaBottlingPlant::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(activePlayer);Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //组合数量
    int combo=owner->getTypeCardNum(Type::Husbandry,State::None);
    //收益
    int coins=num*combo*this->getValue();
    //赚钱
    owner->addCoins(coins);
    //返回日志
    return QString("【%1】%2%3场上共%4☕获得%5金币")
        .arg(this->getName())
        .arg(num==1?"":QString("*%1").arg(num))
        .arg(owner->getName())
        .arg(combo)
        .arg(coins);
}
