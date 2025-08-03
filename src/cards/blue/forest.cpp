#include "forest.h"

Forest::Forest(QObject* parent):
    Card("林场", 3, Color::Blue, Type::Industry, 1, 5, 5,CommandType::GainCoins,parent) {}

QString Forest::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}

