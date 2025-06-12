#include "mackerelboat.h"
#include "player.h"

MackerelBoat::MackerelBoat(QObject* parent):
    Card("鲭鱼船", 2, Color::Blue, Type::Fishery,3, 8, 8, "如果你建成【港口】，获得 3 金币。",parent) {}

double MackerelBoat::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString MackerelBoat::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
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

bool MackerelBoat::isActivate (Player* owner, Player* activePlayer, int rollSum)const {
    //自己开了港口才能激活
    if(owner->getCardNum("港口",State::Opening))
        return rollSum >= m_actLNum && rollSum <= m_actRNum;
    return false;
};
