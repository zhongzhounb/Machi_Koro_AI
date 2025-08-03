#include "station.h"

Station::Station(QObject* parent):
    Card("火车站", 4, Color::Landmark, Type::Landmark, -1 , 0, 0,CommandType::None, parent
           ,""
           ,""
           ,State::Closing
           ,CommandType::None) {}

QString Station::getDescription() const {
    return QString("你可以选择抛1个骰子还是2个骰子。");
}
