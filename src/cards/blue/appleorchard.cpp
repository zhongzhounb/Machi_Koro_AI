#include "appleorchard.h"

AppleOrchard::AppleOrchard(QObject* parent):
    Card("果园", 3, Color::Blue, Type::Agriculture, 3, 10, 10, CommandType::GainCoins,parent) {}

QString AppleOrchard::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}
