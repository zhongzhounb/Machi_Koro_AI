#include "station.h"
#include "commandfactory.h"

Station::Station(QObject* parent):
    Card("火车站", 4, Color::Landmark, Type::Landmark, -1 , 0, 0, parent,State::Closing) {}

QString Station::getDescription() const {
    return QString("你可以选择抛1个骰子还是2个骰子。");
}

double Station::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

//火车站作为检测是否开通，不产生命令
QList<GameCommand*> Station::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(owner);Q_UNUSED(activePlayer);
    return {};
}
