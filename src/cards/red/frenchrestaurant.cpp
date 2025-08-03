#include "frenchrestaurant.h"
#include "player.h"

FrenchRestaurant::FrenchRestaurant(QObject* parent):
    Card("法国餐厅", 3, Color::Red, Type::Restaurant, 5 , 5, 5,CommandType::StealCoins, parent
           ,""
           ,"对方未建成≥2个地标。") {}

QString FrenchRestaurant::getDescription() const {
    return QString("若投掷者建成≥2个地标，收取投掷者 %1 金币。").arg(this->m_value);
}

int FrenchRestaurant::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=activePlayer->getTypeCardNum(Type::Landmark,State::Opening);
    return num>=2;
}
