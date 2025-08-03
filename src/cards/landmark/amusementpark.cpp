#include "amusementpark.h"

AmusementPark::AmusementPark(QObject* parent):
    Card("游乐园", 16, Color::Landmark, Type::Landmark, -1 , 0, 0,CommandType::None, parent
           ,""
           ,""
           ,State::Closing
           ,CommandType::GainNewTurn) {}

QString AmusementPark::getDescription() const {
    return QString("若你抛出的两个骰子点数相同，该回合结束后还是你的回合。").arg(m_value);
}

