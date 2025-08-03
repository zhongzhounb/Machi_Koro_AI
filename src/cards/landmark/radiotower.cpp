#include "radiotower.h"

RadioTower::RadioTower(QObject* parent):
    Card("广播塔", 22, Color::Landmark, Type::Landmark, -1 , 0, 0,CommandType::None, parent
           ,""
           ,""
           ,State::Closing
           ,CommandType::RerollDice) {}

QString RadioTower::getDescription() const {
    return QString("你可以选择重新抛一次骰子。");
}
