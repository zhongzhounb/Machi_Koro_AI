#include "flowerorchard.h"
#include "player.h"

FlowerOrchard::FlowerOrchard(QObject* parent):
    Card("花田", 2, Color::Blue, Type::Agriculture, 4, 4, "所有玩家每拥有一块花田，就从银行获得1个金币。",parent) {}

double FlowerOrchard::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString FlowerOrchard::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(activePlayer);Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //收益
    int coins=num*1;
    //赚钱
    owner->addCoins(coins);
    //返回日志
    return QString("【%1】%2%3获得%4金币")
        .arg(this->getName())
        .arg(num==1?"":QString("*%1").arg(num))
        .arg(owner->getName())
        .arg(coins);
}
