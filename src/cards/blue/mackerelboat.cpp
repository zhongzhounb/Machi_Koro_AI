#include "mackerelboat.h"
#include "player.h"

MackerelBoat::MackerelBoat(QObject* parent):
    Card("鲭鱼船", 2, Color::Blue, Type::Fishery,3, 8, 8,CommandType::GainCoins, parent
           ,""
           ,"没建【港口】，船无法停靠") {}

QString MackerelBoat::getDescription() const {
    return QString("若你建成【港口】，获得 %1 金币。").arg(this->m_value);
}

int MackerelBoat::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getCardNum("港口",State::Opening);
    return num>0;
}

