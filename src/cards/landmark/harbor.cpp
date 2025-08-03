#include "harbor.h"

Harbor::Harbor(QObject* parent):
    Card("港口", 2, Color::Landmark, Type::Landmark, -1 , 0, 0,CommandType::None, parent
           ,""
           ,""
           ,State::Closing
           ,CommandType::AddDiceNum) {}

QString Harbor::getDescription() const {
    return QString("若你抛出的骰子点数之和≥10，可以让点数+2。").arg(m_value);
}
