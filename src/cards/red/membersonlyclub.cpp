#include "membersonlyclub.h"
#include "player.h"

MembersOnlyClub::MembersOnlyClub(QObject* parent):
    Card("会员俱乐部", 4, Color::Red, Type::Restaurant, -1 , 12, 14, "如果当前投掷玩家有≥3个已建成的地标建筑，所有玩家每有一家会员俱乐部就可以从当前投掷玩家收取所有金币。",parent) {}

double MembersOnlyClub::getBuyWight(Player* aiPlayer, Game* game) const {
    return 0.0;
}

QString MembersOnlyClub::activate(Player* owner, Player* activePlayer, Game* game, const QVariant& choiceData){
    Q_UNUSED(game);
    //卡牌数量
    int num=owner->getCardNum(this->getName(),State::Opening);
    //收益(收所有）
    int coins=activePlayer->getCoins();
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

bool MembersOnlyClub::isActivate (Player* owner, Player* activePlayer, int rollSum)const {
    //激活者开了≥3个地标才能激活
    if(activePlayer->getTypeCardNum(Type::Landmark,State::Opening)>=3)
        return rollSum >= m_actLNum && rollSum <= m_actRNum;
    return false;
};
