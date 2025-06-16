#include "forest.h"
#include "commandfactory.h"

Forest::Forest(QObject* parent):
    Card("林场", 3, Color::Blue, Type::Industry, 1, 5, 5,parent) {}

QString Forest::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}

double Forest::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> Forest::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}

