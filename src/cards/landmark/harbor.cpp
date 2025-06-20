#include "harbor.h"
#include "commandfactory.h"

Harbor::Harbor(QObject* parent):
    Card("港口", 2, Color::Landmark, Type::Landmark, -1 , 0, 0, parent) {}

QString Harbor::getDescription() const {
    return QString("若你抛出的骰子点数之和≥10，可以让点数+2。").arg(m_value);
}

double Harbor::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> Harbor::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {};
}
