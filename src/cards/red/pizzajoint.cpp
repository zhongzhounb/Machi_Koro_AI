#include "pizzajoint.h"
#include "player.h"

PizzaJoint::PizzaJoint(QObject* parent):
    Card("披萨店", 1, Color::Red, Type::Restaurant, 1 , 7, 7,CommandType::StealCoins, parent) {}

QString PizzaJoint::getDescription() const {
    return QString("收取投掷者 %1 金币。").arg(this->m_value);
}
