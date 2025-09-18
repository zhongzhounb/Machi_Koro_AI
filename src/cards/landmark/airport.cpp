#include "airport.h"

Airport::Airport(QObject* parent):
    Card("机场", 30, Color::Landmark, Type::Landmark, 10 , 0, 0,CommandType::None, parent
           ,""
           ,""
           ,State::Closing
           ,CommandType::GainCoinsIfNoBuy) {}

QString Airport::getDescription() const {
    return QString("若你在本回合没有建造任何建筑，你获得 %1 金币。").arg(m_value);
}
