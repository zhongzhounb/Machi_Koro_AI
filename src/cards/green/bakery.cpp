#include "bakery.h"
#include "commandfactory.h"

Bakery::Bakery(QObject* parent):
    Card("面包房", 1, Color::Green, Type::Store, 1 , 2, 3,CommandType::GainCoins, parent) {}

QString Bakery::getDescription() const {
    return QString("若你是投掷者，获得 %1 金币。").arg(m_value);
}
