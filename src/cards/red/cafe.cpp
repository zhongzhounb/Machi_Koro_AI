#include "cafe.h"
#include "commandfactory.h"
#include "player.h"

Cafe::Cafe(QObject* parent):
    Card("咖啡馆", 2, Color::Red, Type::Restaurant, 1, 3, 3, parent) {}

QString Cafe::getDescription() const {
    return QString("收取投掷者 %1 金币。").arg(this->m_value);
}

double Cafe::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> Cafe::createCommands(Player* owner, Player* activePlayer){
    return { CommandFactory::instance().createStealCoinsCommand(owner,this,activePlayer,this) };
}

