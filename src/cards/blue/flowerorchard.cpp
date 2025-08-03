#include "flowerorchard.h"

FlowerOrchard::FlowerOrchard(QObject* parent):
    Card("花田", 2, Color::Blue, Type::Agriculture, 1, 4, 4,CommandType::GainCoins,parent) {}

QString FlowerOrchard::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}

