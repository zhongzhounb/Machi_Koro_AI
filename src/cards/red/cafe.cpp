#include "cafe.h"

Cafe::Cafe(QObject* parent):
    Card("咖啡馆", 2, Color::Red, Type::Restaurant, 1, 3, 3,CommandType::StealCoins,parent) {}

QString Cafe::getDescription() const {
    return QString("收取投掷者 %1 金币。").arg(this->m_value);
}

