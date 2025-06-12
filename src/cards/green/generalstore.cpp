#include "generalstore.h"
#include "player.h"

GeneralStore::GeneralStore(QObject* parent):
    Card("杂货店", 0, Color::Green, Type::Store, 2 , 2, 2, "如果你是投掷者，且建造了≤1个地标，获得 2 金币。",parent) {}

double GeneralStore::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString GeneralStore::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
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

bool GeneralStore::isActivate (Player* owner, Player* activePlayer, int rollSum)const {
    //拥有者开了≤1个地标才能激活
    if(activePlayer->getTypeCardNum(Type::Landmark,State::Opening)<=1)
        return rollSum >= m_actLNum && rollSum <= m_actRNum;
    return false;
};
