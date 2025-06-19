#include "amusementpark.h"
#include "commandfactory.h"

AmusementPark::AmusementPark(QObject* parent):
    Card("游乐园", 16, Color::Landmark, Type::Office, -1 , 0, 0, parent) {}

QString AmusementPark::getDescription() const {
    return QString("若你抛出的两个骰子点数相同，该回合结束后还是你的回合。").arg(m_value);
}

double AmusementPark::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> AmusementPark::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {};
}

