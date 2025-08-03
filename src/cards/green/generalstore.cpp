#include "generalstore.h"
#include "player.h"

GeneralStore::GeneralStore(QObject* parent):
    Card("杂货店", 0, Color::Green, Type::Store, 2 , 2, 2,CommandType::GainCoins,parent
           ,""
           ,"建了太多地标了，没时间管理了") {}

QString GeneralStore::getDescription() const {
    return QString("若你是投掷者，且建成≤1地标，获得 %1 金币。").arg(this->m_value);
}

int GeneralStore::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getTypeCardNum(Type::Landmark,State::Opening);
    return num<=1;
}
