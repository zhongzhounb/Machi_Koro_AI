#include "familyrestaurant.h"
#include "player.h"

FamilyRestaurant::FamilyRestaurant(QObject* parent):
    Card("西餐厅", 3, Color::Red, Type::Restaurant, 2 , 9, 10,CommandType::StealCoins, parent) {}

QString FamilyRestaurant::getDescription() const {
    return QString("收取投掷者 %1 金币。").arg(this->m_value);
}


