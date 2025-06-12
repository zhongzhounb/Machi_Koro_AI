#include "sushibar.h"
#include "player.h"

SushiBar::SushiBar(QObject* parent):
    Card("寿司店", 2, Color::Red, Type::Restaurant, 3 , 1, 1, "如果你建造了【港口】，收取投掷者 3 金币。",parent) {}

double SushiBar::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString SushiBar::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
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

bool SushiBar::isActivate (Player* owner, Player* activePlayer, int rollSum)const {
    //自己开了港口才能激活
    if(owner->getCardNum("港口",State::Opening))
        return rollSum >= m_actLNum && rollSum <= m_actRNum;
    return false;
};
