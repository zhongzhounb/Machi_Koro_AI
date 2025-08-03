#include "ranch.h"

Ranch::Ranch(QObject* parent):
    Card("农场", 1, Color::Blue, Type::Husbandry, 1 , 2, 2 ,CommandType::GainCoins,parent) {}

QString Ranch::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}
