#include "frenchrestaurant.h"
#include "player.h"

FrenchRestaurant::FrenchRestaurant(QObject* parent):
    Card("法国餐厅", 3, Color::Red, Type::Restaurant, 5 , 5, 5, "如果当前投掷玩家有≥2个已建成的地标建筑，所有玩家每有一家法国餐厅就可以从当前投掷玩家收取5个金币。",parent) {}

double FrenchRestaurant::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString FrenchRestaurant::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //收益(不能超过激活者所拥有的）
    int coins=qMin(num*this->getValue(),activePlayer->getCoins());
    if(coins>0){
        //赚钱
        owner->addCoins(coins);
        //扣钱
        activePlayer->delCoins(coins);
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

bool FrenchRestaurant::isActivate (Player* owner, Player* activePlayer, int rollSum)const {
    //激活者开了≥2个地表才能激活
    if(activePlayer->getTypeCardNum(Type::Landmark,State::Opening)>=2)
        return rollSum >= m_actLNum && rollSum <= m_actRNum;
    return false;
};
