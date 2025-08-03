#include "sushibar.h"
#include "player.h"

SushiBar::SushiBar(QObject* parent):
    Card("寿司店", 2, Color::Red, Type::Restaurant, 3 , 1, 1,CommandType::StealCoins,parent
           ,""
           ,"没建【港口】，无法获取海产品。") {}

QString SushiBar::getDescription() const {
    return QString("若你建成【港口】，收取投掷者 %1 金币。").arg(this->m_value);
}

int SushiBar::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getCardNum("港口",State::Opening);
    return num>0;
}
